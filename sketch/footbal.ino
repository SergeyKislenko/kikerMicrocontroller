//-------НАСТРОЙКИ---------
String firstTeamName = "Белые"; // название первой команды не больше 7ми букв
String secondTeamName = "Желтые"; // название второй команды не больше 7ми букв
//-------НАСТРОЙКИ---------

//-------ОБВЕС---------
#define fLaser 5          //лазер первой команды
#define sLaser 6          //лазер второй команды
#define fRsens 14         //сигнал первого фототранзистора
#define sRsens 15         //сигнал второго фототранзистора
#define piezo 4           //пищалка
#define button 7          //кнопка отмены гола
#define disp_power 12     // питание дисплея
//-------ОБВЕС---------

//-------ПЕРЕМЕННЫЕ---------
unsigned long reset_timer;                     // таймеры 
int firstScore;                                //счет первой команды
int secondScore;                               //счет второй команды
int countPush;
boolean firstPush = true;
boolean isCanceled = false;                    // был ли отменен гол
boolean isGoalF=false;                          // флаг для апаратного прерывания
boolean isGoalS=false;                          // флаг для апаратного прерывания
boolean isFirstBreak=false;    
boolean isSecondBreak=false;    
String whoLast = "empty";                      // кто отменил последний

//-------ПЕРЕМЕННЫЕ---------


//-------БИБЛИОТЕКИ---------
#include "LCD_1602_RUS.h"
//-------БИБЛИОТЕКИ---------

LCD_1602_RUS lcd(0x27, 16, 2);          // создать дисплей

void setup() {  
  Serial.begin(9600);                   // открыть порт для связи с ПК для отладки
  attachInterrupt(0,checkFirst,RISING);
  attachInterrupt(1,checkSecond,RISING);
  delay(500);  
  pinMode(button, INPUT_PULLUP);        // кнопка с подтяжкой
  
  // пины питания как выходы
  pinMode(disp_power, OUTPUT);
  pinMode(fLaser, OUTPUT);
  pinMode(sLaser, OUTPUT);
  pinMode(piezo, OUTPUT);
  
  // подать питание на дисплей и лазеры
  digitalWrite(disp_power, 1);
  digitalWrite(fLaser, 1);
  digitalWrite(sLaser, 1);

  firstScore = 0;
  secondScore = 0;
  delay(1500);

  // инициализация дисплея
  lcd.init();
  lcd.backlight();
  
}

void checkFirst(){    
  if(isFirstBreak){
    isGoalF=true;
  }else{
    isFirstBreak=true;
  }     
}
void checkSecond(){    
  if(isSecondBreak){
    isGoalS=true;
  }else{
    isSecondBreak=true;
  }     
}

void loop() {
  lcd_print(firstScore, secondScore);
  //Пишем счет первой команды
  if(isGoalF){  
    tone(piezo,1318,150);
    prepare_score("first");        
    isGoalF=false;
  }
  //Пишем счет второй команды
  if(isGoalS){   
    tone(piezo, 700, 300);
    prepare_score("second");            
    isGoalS=false;
  }
  //если кнопка зажата
  if(!digitalRead(button)){
    countPush++;
    if(countPush>50 && firstPush){      
      clean_var();   
      lcd.clear(); 
      firstPush=false; 
    }else if(countPush < 50 && !isCanceled){      
       minus_goal();
       lcd.clear();
       lcd_print(firstScore, secondScore);
    }
  }else{
    countPush=0;
    firstPush=true;
  }
  //если нажали кнопку отмены гола
  
  //Если счет достиг 10, обнуляем и начинаем заново
  if(firstScore==10 || secondScore==10){
    reset_timer = millis();
    lcd.clear();
    lcd.setCursor(5, 0); lcd.print(L"ПОБЕДА!!!");      
    while(1){
    //если забили последний гол и он не считовый
     if(!digitalRead(button)){
        minus_goal();   
        lcd.clear();    
        break;  
     }
     if(millis() - reset_timer > 3000){        
        win_sound();
        clean_var();         
        break;
      }    
   }
  }
    
}

void minus_goal(){
  if(whoLast=="f"){       
       firstScore = firstScore-1;         
       isCanceled=true;
    }
    if(whoLast=="s"){
       secondScore = secondScore-1;          
       isCanceled=true;
    }
}

// выводим счет
void lcd_print(int fS, int sS) {
    lcd.setCursor(2, 0); lcd.print(firstTeamName);  // отобразить название первой команды
    lcd.setCursor(9, 0); lcd.print(secondTeamName); // отобразить название второй команды
    lcd.setCursor(4, 1); lcd.print(sS);             // отобразить счет первой команды
    lcd.setCursor(12, 1); lcd.print(fS);            // отобразить счет второй команды  
}

//очищаем переменные
void clean_var(){
    firstScore = 0;
    secondScore = 0;
    isCanceled = false;              
    whoLast = "empty"; 
    lcd.clear();
}
// обновляем счет
void prepare_score(String type) {
    if(type=="first"){
      firstScore = firstScore+1;  
      lcd_print(firstScore, secondScore);
      whoLast="f";
      isCanceled=false;
    }
    if(type=="second"){
      secondScore = secondScore+1;  
      lcd_print(firstScore, secondScore);
      whoLast="s";
      isCanceled=false;
    }      
    reset_timer = millis();
    while(1){
      if(!digitalRead(button)){
           if(type=="first"){
            firstScore = firstScore-1;  
            isCanceled=true;            
          }
          if(type=="second"){
            secondScore = secondScore-1;  
            isCanceled=true;            
          } 
          lcd.clear();
          break;
      }
      if(millis() - reset_timer > 3000){
        break;
      }
    }    
}
void win_sound() {
  tone(piezo,1046,450) ; 
  delay(450);
  tone(piezo,784,150);
  delay(450);
  tone(piezo,659,300);
  delay(450);
  tone(piezo,880,300);
  delay(300);
  tone(piezo,987,150);
  delay(300);
  tone(piezo,932,150);
  delay(150);
  tone(piezo,880,300);
  delay(300);
  tone(piezo,784,210); 
  delay(210);
  tone(piezo,1318,210);
  delay(210);
  tone(piezo,1568,150);
  delay(150);
  tone(piezo,1750,300);
  delay(300);
  tone(piezo,1396,150);
  delay(150);
  tone(piezo,1568,150);
  delay(300);
  tone(piezo,1318,300);
  delay(300);
  tone(piezo,1046,150);
  delay(150);
  tone(piezo,1174,150);
  delay(150);
  tone(piezo,987,450);
  delay(450);
}
