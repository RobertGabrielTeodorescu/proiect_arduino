
#include <LiquidCrystal.h>


LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

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

const int menuButton=4; //butonul cu care se va naviga prin pagina de start
const int startButton=5; //dupa apasarea acestui buton jocul va incepe
const int jumpButton=3; //dupa aparea acetui buton, personajul va sari

bool playing=false;

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
    lcd.clear();
  }
}

void animation(){
  lcd.setCursor(0,1);
  lcd.write(byte(0));
  lcd.setCursor(15,1);
  lcd.write(byte(1));
}

void setup() {
  
  lcd.begin(16, 2);
  lcd.createChar(0, jumpingMan);
  lcd.createChar(1,obstacle);
  pinMode(menuButton,INPUT_PULLUP);
  pinMode(startButton,INPUT_PULLUP);
  
}


void loop() {
  
  while(!playing){
    startPage();
    delay(20);
  }
  animation();
 delay(20);
  
}

void jump(){
  
}


 