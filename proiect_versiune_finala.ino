#include <LiquidCrystal.h>

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

//structura pentru stocarea informatiilor despre nivele
struct level{
  int bestScore;
  String unlocked;
  int unlockScore;
  int generateBlocksConstant;
  int gameSpeed;  
};

level levels[3];

byte jumpingMan1[8] = {
  0b00000,
  0b00000,
  0b00101,
  0b01110,
  0b10100,
  0b00100,
  0b01110,
  0b10010
};

byte jumpingMan2[8] = {
  0b00000,
  0b00000,
  0b00101,
  0b01110,
  0b10100,
  0b00100,
  0b01110,
  0b01001
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
bool playing=false; //varaibila de control prin care este aratata starea aplicatiei; este setata pe true atunci cand utiliztorul este in fereastra de joc si controleaza personajul
bool jumping=false;//variabila prin care este controlata saritura personajului; este setata pe true atunci cand personajul este in aer
bool preStartPlaying=false;//variabila de control cu ajutorul caruia se initializeaza datele jocului, dupa care utilizatorul va intra in fereastra de joc
bool endGame=false;//variabila de control prin care se arata daca utilizatorul a pierdut si se iese din fereastra de joc
bool updated=false;//variabila de control prin care se arata daca s-au actualizat informatiile informatiile despre nivelul jucat de utilizator
int score=0;//variabila in care se retine scorul curent obtinut de utilizator
int pickLevel=1;//variabila in care se retine nivelul selectat de utilizator pentru a fi jucat
//urmatoarele doua valori vor fi folosite pentru a crea debounce-ul pentru butonul de navigare prin meniul de start
int menuButtonVal;//valoarea curenta a butonului
int menuButtonOldVal=1;//vechea valoare a butonului
int startButtonVal;//valoarea butonului de start
bool readingFromFile=true;//variabila de control care arata daca in momentul actual se citeste din fisier
int obstaclesCounter=0;//variabila pentru contorizarea obstacolelor de pe traseu
int noObstaclesCounter=0;//variabila pentru contorizarea spatiilor de pe traseu
int terr[16];//variabila in care se vor retine informatiile despre traseu; obstacolele si spatiile goale vor fi codificate in acest vector de intregi
char incomingChar=0;//variabila pentru citirea urmatorului caracter vare vine pe comunicarea seriala cu programul java, program ce citeste informatii dintr-un fisier stocat in memoria calculatorului
String info="";//sir in care se vor cumula caracterele primite pe comunicarea seariala
bool showScore=false;//variabila de control prin care este aratat daca scorul a fost afisat; variabila este setata pe true dupa ce scorul a fost afisat
int jumpingManCarachterContor=0;//variabial de contor cu ajutorul careia se alterneaza cele 2 caractere definite pentru personaj

//functie care afiseaza meniul de start
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
  //in functie de valoarea lui pickLevel, vom plasa sub cifra corespunzatoare
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
  //se citeste starea butonului de start
  startButtonVal=digitalRead(startButton);
  if(startButtonVal==0){
    //daca butonul este apasat se verifica daca nivelul este deblocat
    if(levels[pickLevel-1].unlocked.equals("false")){
      //daca nivelul este blocat, se va afisa un mesaj pe ecran care sa confirme acest lucru
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Level");
      lcd.setCursor(0,1);
      lcd.print("locked");
      delay(2000);
      lcd.clear();
    }
    else{
      //daca nivelul este deblocat, se poate trece mai departe
      playing=true;
      preStartPlaying=true;
      lcd.clear();   
    }
  }
  //debounce pentru butonul de navigare prin meniu
  menuButtonVal=digitalRead(menuButton);
  if(menuButtonVal==0&&menuButtonOldVal==1){
    pickLevel=pickLevel%3+1;
  }
  menuButtonOldVal=menuButtonVal;
  
}

//functie care actualizeaza informatiile despre nivele in functie de rezultatul obtinut de utilizator 
void updateLevelInfo(){
  
  //daca scorul obtinut la nivelul curent este mai bun decat cel mai bun
  //scor obtinut pana in momentul respectiv, atunci se va actualiza cel mai bun scor obtinui
  if(score>levels[pickLevel-1].bestScore){
      levels[pickLevel-1].bestScore=score;
  }
  //se verifica daca scorul obtinut la nivelul curent va debloca nivelul urmator
  //intrucat nivelul 3 este ulitmul, nu are rost sa faca aceasta verificare si pentru acest nivel
  if(pickLevel!=3){
    if(score>=levels[pickLevel].unlockScore){
      levels[pickLevel].unlocked="true";
    }  
  }
  //se salveaza informatiile actualizate despre nivele intr-un sir
  info="";
  for(int i=0;i<3;i++){
      info=info+levels[i].bestScore+" "+levels[i].unlocked+" ";
  }
  delay(500);
  //se trimite sirul serial, fiind preluat de programul java care va suprascrie in fisier
  Serial.println(info);
  delay(750);
  Serial.flush();
  
}

//functie care va afisa 2 mesaje dupa terminarea rundei de joc
void endPage(){
  
  if(showScore==false){
    //primul mesaj reprezibta o confirmare a faptului ca jocul a fost pierdut
    lcd.setCursor(0,0);
    lcd.print("You lost!");
    lcd.setCursor(0,1);
    lcd.print("Try again!");
    delay(2000);
    lcd.clear();
    //acest mesaj va fi afisat doar o data, timp de 2 secunde, dupa care se trece la urmatorul mesaj
    //odata ce variabila showScore devine true, inseamna ca acest msaj a fost afisat si se poate trece la urmatoru; 
    showScore=true;
  }
  else{
    //al doilea mesaj va afisa scorul obtinui, dar si cel mai bun scor obtinul pentru nivelul jucat
    lcd.setCursor(0,0);
    lcd.print("Score:");
    lcd.setCursor(7,0);
    lcd.print(score);
    lcd.setCursor(0,1);
    lcd.print("Best:");
    lcd.setCursor(7,1);
    lcd.print(levels[pickLevel-1].bestScore);
    startButtonVal=digitalRead(startButton);
    //acest mesaj va ramane pe ecran pana in momentul in care butonul de start este apasat
    if(startButtonVal==0){
      playing=false;
      pickLevel=1;
      lcd.clear(); 
      delay(200); 
    }
  } 
   
}

//functie care citeste caracterele venite pe cale seriala de la programul java
void readInfo(){

  //cat timp sunt caractere pe calea seriala, se vor concatena la un sir
  while(Serial.available()>0){
    incomingChar=Serial.read();
    info.concat(incomingChar);  
  }
  char infoCharArray[30];
  //se converteste sirul intr-un vector de caractere pentru a putea folosi functia strtok, ce separa sirul in functie de un separator
  info.toCharArray(infoCharArray,30);
  //intrucat informatiile din sir sunt despartite de un spatiu, se face separarea dupa spatiu
  char* separator=strtok(infoCharArray," ");
  //variabila de contor cu ajutorul careia se salveaza informatiile in structura definita pentru nivele
  int index=0;
  while(separator!=0){
      //pentru fiecare nivel e salvat cel mai bun scor obitnut si starea de blocare/deblocare
      levels[index].bestScore=atoi(separator);
      separator=strtok(NULL," ");
      levels[index].unlocked=separator;
      index++;
      separator=strtok(NULL," ");
  }
  
}

//functie de initializare pentru variabilele de control si pentru traseu
void initialize(){
  
  //traseul este initializat cu un bloc de spatii goale
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

//functie care asigura deplasarea caracterelor din traseu cu cate o pozitie spre stanga
void shiftLeftOnePos(int x){
  
  for(int i=0;i<15;i++){
    terr[i]=terr[i+1];
  }
  //se umple caracterul lasat liber cu urmatorul caracter generat
  terr[15]=x;
  
}

//functie pentru desenarea personajului
void drawJumpingMan(){
  
  if(jumping==false){
    //daca personajul nu se afla in aer, va fi afisat pe al doilea rand
    //totodata, nu se mai afiseaza si primul caracter din teren
    lcd.setCursor(0,1);
    lcd.write(byte(jumpingManCarachterContor%2+4));
    lcd.setCursor(0,0);
    lcd.print(" ");
  }
  else{
    //daca personajul se afla in aer, va fi afisat pe primul rand
    //totodata, se afiseaza si primul caracter din teren
    lcd.setCursor(0,0);
    lcd.write(byte(jumpingManCarachterContor%2+4));
    lcd.setCursor(0,1);
    lcd.write(byte(terr[0]));
  }

}

//functie pentru verificarea starii de coliziune dintre personaj si un obstacol
void checkCollision(){
  
  if(jumping==true&&terr[0]==3){
    //daca personajul se afla in aer si sub el se afla un spatiu gol, atunci va cobori
    jumping=false;  
  }
  else{
    //daca personul se afla jos si se loveste de un obstacol, jocul se termina, prin setarea variabilei endGame pe true
    if(jumping==false&&terr[0]==2){
      endGame=true;
      lcd.clear();
    }  
  } 
}

//functie care asigura animatia jocului
//are 2 parametri: primul reprezinta viteza joc, iar al doilea reprezinta constanta cu ajutorul careia se genereaza traseul
void animation(int gameSpeed, int generateBlocksConstant){
  
  //se afiseaza personajul
  drawJumpingMan();
  //se afiseaza traseul
  for(int i=1;i<16;i++){
    lcd.setCursor(i,1);
    lcd.write(byte(terr[i]));  
  }
  lcd.setCursor(13,0);
  lcd.print(score);
  //se verifica existenta unei coliziuni intre personaj si un obstacol
  checkCollision();
  //obstacolele si spatiile goale sunt generate in blocuri, pentru a lasa timp utilizatorului sa sara pentru a evita obstacolele
  //daca contorul pentru obstacole nu este 0, se deseneaza in continuare un obstacol, dupa care se decrementeaza contorul
  if(obstaclesCounter!=0){
    shiftLeftOnePos(2);
    obstaclesCounter--;  
  }
  else{
    //daca contorul pentru spatii goale nu este, se deseneaza in continuare un spatiu liber, dupa care se decrementeaza contorul
    if(noObstaclesCounter!=0){
      shiftLeftOnePos(3); 
      noObstaclesCounter--; 
    }
    else{
      //daca cei doi contori sunt 0, trebuie luata o decizie in privinta urmatorului caracter generat
      //aceste decizie este luata de functia random: daca este generat un 0, atunci se intentioneaza desenarea unui spatiu gol,
      //iar daca este generat un 1, atunci se intentioneaza desenarea unui obstacol
      //totodata, pentru nu crea siruri prea lungi de obstacole sau spatii libere, sunt create restrictii de generare
      if(random(2)==0){
          //daca trebuie desenat un spatiu liber si ultimul caracter generat este un obstacol, atunci se poate incepe 
          //generarea unui bloc de spatii libere; daca ultimul caracter generat este un spatiu liber, atunci se va incepe
          //generarea unui bloc de obstacole
         if(terr[15]==2){
            shiftLeftOnePos(3);
            noObstaclesCounter=random(3)+generateBlocksConstant;
         }
         else{
            shiftLeftOnePos(2);
            obstaclesCounter=random(3)+generateBlocksConstant; 
         }
      }
      else{
         //analog, se trateaza cazul in care se doreste generarea unui obstacol
         if(terr[15]==3){
            shiftLeftOnePos(2);
            obstaclesCounter=random(3)+generateBlocksConstant; 
         }
         else{
            shiftLeftOnePos(3);
            noObstaclesCounter=random(3)+generateBlocksConstant; 
         }
      }  
    }  
  }
  //se incrementeaza scorul la fiecare iteratie si contorul pentru desenarea personajului
  score++;
  jumpingManCarachterContor++;
  delay(gameSpeed);
    
}

//functie care initializeaza informatiile despre nivele
//in cazul in care se realizeaza comunicarea seriala cu programul java,
//informatiile despre cel mai bun scor si starea de blocare/deblocare pentru fiecare nivel vor fi suprascrise
//aceasta functie face posibila rularea jocului si fara o comunicare seriala
void initLevels(){
  
  levels[0].bestScore=0;
  levels[0].unlocked="true";
  levels[0].unlockScore=0;
  levels[0].generateBlocksConstant=3;
  levels[0].gameSpeed=250;
  
  levels[1].bestScore=0;
  levels[1].unlocked="false";
  levels[1].unlockScore=50;
  levels[1].generateBlocksConstant=4;
  levels[1].gameSpeed=125;

  levels[2].bestScore=0;
  levels[2].unlocked="false";
  levels[2].unlockScore=100;
  levels[2].generateBlocksConstant=5;
  levels[2].gameSpeed=75; 
}

void setup() {
  
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.createChar(2,obstacle);
  lcd.createChar(3,noObstacle);
  lcd.createChar(4,jumpingMan1);
  lcd.createChar(5,jumpingMan2);
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
        if(preStartPlaying){
          initialize();
          preStartPlaying=false;
        }
        else{
          if(!endGame){
            animation(levels[pickLevel-1].gameSpeed,levels[pickLevel-1].generateBlocksConstant);
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
