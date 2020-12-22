
#include <LiquidCrystal.h>

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

struct level{
  int bestScore;
  String unlocked;  
};

level levels[3];

byte jumpingMan[8] = {
	0b00000,
	0b00000,
	0b00101,
	0b01110,
	0b10100,
	0b00100,
	0b01010,
	0b10001
};

byte obstacle[8] = {
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111
};

byte noObstacle[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

const int menuButton=13; //butonul cu care se va naviga prin pagina de start
const int startButton=12; //dupa apasarea acestui buton jocul va incepe
const int jumpButton=21; //dupa aparea acetui buton, personajul va sari

bool playing=false;
bool jumping=false;
bool preStart=false;
bool endGame=false;
bool updated=false;
int score=0;
int bestScore=0;

int pickLevel=1;

int menuButtonVal;
int menuButtonOldVal=1;

int startButtonVal;

bool readingFromFile=true;

int obstaclesCounter=0;
int noObstaclesCounter=0;
int currentManPos=0;

int terr[16];

char incomingChar=0;
String info="";


void startPage(){
 lcd.setCursor(0,0);
 lcd.print("Select");
  lcd.setCursor(0,1);
  lcd.print("a level");
  lcd.setCursor(10,0);
  lcd.print("1");
  lcd.setCursor(12,0);
  lcd.print("2");
  lcd.setCursor(14,0);
  lcd.print("3");
  //in functie de valoarea lui k, vom plasa sub cifra corespunzatoare
  //nivelului caracterul _ pentru a ilustra selectia curenta
  if(pickLevel%3==1){
  	lcd.setCursor(10,1);
    lcd.print("_");
    lcd.setCursor(12,1);
    lcd.print(" ");
    lcd.setCursor(14,1);
    lcd.print(" ");
  }
  else{
    if(pickLevel%3==2){
    	lcd.setCursor(10,1);
    	lcd.print(" ");
    	lcd.setCursor(12,1);
    	lcd.print("_");
    	lcd.setCursor(14,1);
    	lcd.print(" ");
    }
    else{
    	lcd.setCursor(10,1);
    	lcd.print(" ");
    	lcd.setCursor(12,1);
    	lcd.print(" ");
    	lcd.setCursor(14,1);
    	lcd.print("_");
    }
  }
  startButtonVal=digitalRead(startButton);
  if(startButtonVal==0){
  	playing=true;
    preStart=true;
    lcd.clear();
  }
  //debounce pentru butonul de navigare prin meniu
  menuButtonVal=digitalRead(menuButton);
  if(menuButtonVal==0&&menuButtonOldVal==1){
    pickLevel=pickLevel%3+1;
  }
  menuButtonOldVal=menuButtonVal;
}

bool showScore=false;

void updateLevelInfo(){
  if(score>levels[pickLevel-1].bestScore){
      levels[pickLevel-1].bestScore=score;
  }
  info="";
  for(int i=0;i<3;i++){
      info=info+levels[i].bestScore+" "+levels[i].unlocked+" ";
  }
  delay(500);
  Serial.println(info);
  delay(500);
  Serial.flush();
}

void endPage(){
  if(showScore==false){
    lcd.setCursor(0,0);
    lcd.print("You lost!");
    lcd.setCursor(0,1);
    lcd.print("Try again!");
    delay(2000);
    lcd.clear(); 
    showScore=true;
  }
  else{
    lcd.setCursor(0,0);
    lcd.print("Score:");
    lcd.setCursor(7,0);
    lcd.print(score);
    lcd.setCursor(0,1);
    lcd.print("Best:");
    lcd.setCursor(7,1);
    lcd.print(levels[pickLevel-1].bestScore);
    startButtonVal=digitalRead(startButton);
    if(startButtonVal==0){
      playing=false;
      pickLevel=1;
      lcd.clear(); 
      delay(200); 
    }
  }  
}


void readInfo(){
  while(Serial.available()>0){
    incomingChar=Serial.read();
    info.concat(incomingChar);  
  }
  char infoCharArray[30];
  info.toCharArray(infoCharArray,30);
  char* separator=strtok(infoCharArray," ");
  int index=0;
  while(separator!=0){
      levels[index].bestScore=atoi(separator);
      separator=strtok(NULL," ");
      levels[index].unlocked=separator;
      index++;
      separator=strtok(NULL," ");
  }
}

void initialize(){
  
  for(int i=0;i<16;i++){
    terr[i]=3;
  } 
  obstaclesCounter=0;
  noObstaclesCounter=0;
  jumping=false;
  showScore=false;
  score=0;
  endGame=false;
  updated=false;
}

void shiftLeftOnePos(int x){
  
  for(int i=0;i<15;i++){
    terr[i]=terr[i+1];
  }
  terr[15]=x;
  
}


void drawJumpingMan(){
  
  if(jumping==false){
    lcd.setCursor(0,1);
    lcd.write(byte(1));
    lcd.setCursor(0,0);
    lcd.print(" ");
  }
  else{
    lcd.setCursor(0,0);
    lcd.write(byte(1));
    lcd.setCursor(0,1);
    lcd.write(byte(terr[0]));
  }

}

void checkCollision(){
  if(jumping==true&&terr[0]==3){
    jumping=false;  
  }
  else{
    if(jumping==false&&terr[0]==2){
      endGame=true;
      lcd.clear();
    }  
  } 
}

void animation(){
  
  drawJumpingMan();
  for(int i=1;i<16;i++){
    lcd.setCursor(i,1);
    lcd.write(byte(terr[i]));  
  }
  checkCollision();
  if(obstaclesCounter!=0){
    shiftLeftOnePos(2);
    obstaclesCounter--;  
  }
  else{
    if(noObstaclesCounter!=0){
      shiftLeftOnePos(3); 
      noObstaclesCounter--; 
    }
    else{
      if(random(2)==0){
         if(terr[15]==2){
            shiftLeftOnePos(3);
            noObstaclesCounter=random(3)+3;
         }
         else{
            shiftLeftOnePos(2);
            obstaclesCounter=random(3)+3; 
         }
      }
      else{
         if(terr[15]==3){
            shiftLeftOnePos(2);
            obstaclesCounter=random(3)+3; 
         }
         else{
            shiftLeftOnePos(3);
            noObstaclesCounter=random(3)+3; 
         }
      }  
    }  
  }
  score++;
  delay(250);
    
}

void initLevels(){
  levels[0].bestScore=0;
  levels[0].unlocked="true";
  levels[1].bestScore=0;
  levels[1].unlocked="false";
  levels[2].bestScore=0;
  levels[2].unlocked="false";  
}

void setup() {
  
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.createChar(1,jumpingMan);
  lcd.createChar(2,obstacle);
  lcd.createChar(3,noObstacle);
  pinMode(menuButton,INPUT_PULLUP);
  pinMode(startButton,INPUT_PULLUP);
  pinMode(jumpButton,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(jumpButton),jump,CHANGE);
  initLevels();

}


void loop() {

      if(!playing){
        startPage();
        if(Serial.available()>0&&readingFromFile){
          readInfo();
          readingFromFile=false;
        }
      }
      else{
        if(preStart){
          initialize();
          preStart=false;
        }
        else{
          if(!endGame){
            animation();
          }
          else{
            if(!updated){
              updateLevelInfo();
              updated=true;
            }
            else{
              endPage();
            }
          }
        }
      }     
 delay(20);
  
}

void jump(){
  
  jumping=true;
  
}


 
