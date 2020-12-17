
#include <LiquidCrystal.h>

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

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
int score=0;
int bestScore=0;

int k=0;

int menuButtonVal;
int menuButtonOldVal=1;

int startButtonVal;

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
  //debounce pentru butonul de navigare prin meniu
  menuButtonVal=digitalRead(menuButton);
  if(menuButtonVal==0&&menuButtonOldVal==1){
  	k++;
  }
  menuButtonOldVal=menuButtonVal;
  //in functie de valoarea lui k, vom plasa sub cifra corespunzatoare
  //nivelului caracterul _ pentru a ilustra selectia curenta
  if(k%3==0){
  	lcd.setCursor(10,1);
    lcd.print("_");
    lcd.setCursor(12,1);
    lcd.print(" ");
    lcd.setCursor(14,1);
    lcd.print(" ");
  }
  else{
    if(k%3==1){
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
}

bool showScore=false;

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
    lcd.print(bestScore);
    startButtonVal=digitalRead(startButton);
    if(startButtonVal==0){
      playing=false;
      lcd.clear(); 
      delay(200); 
    }
  }  
}

int obstaclesCounter=0;
int noObstaclesCounter=0;
int currentManPos=0;

int terr[16];

void initialize(){
  
  for(int i=0;i<16;i++){
    terr[i]=3;
  } 
  obstaclesCounter=0;
  noObstaclesCounter=0;
  jumping=false;
  showScore=false;
  score=0;
   
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


void setup() {
  
  lcd.begin(16, 2);
  lcd.createChar(1,jumpingMan);
  lcd.createChar(2,obstacle);
  lcd.createChar(3,noObstacle);
  pinMode(menuButton,INPUT_PULLUP);
  pinMode(startButton,INPUT_PULLUP);
  pinMode(jumpButton,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(jumpButton),jump,CHANGE);
  Serial.begin(9600);

}


void loop() {
  
  if(!playing){
    startPage();
  }
  else{
    if(preStart==true){
      initialize();
      preStart=false;
      endGame=false;
    }
    else{
      if(endGame==false){
        animation();
      }
      else{
        endPage();
      }
    }
  }
 delay(20);
  
}

void jump(){
  
  jumping=true;
  
}


 
