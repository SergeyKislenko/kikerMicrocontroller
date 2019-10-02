//-------НАСТРОЙКИ---------
String firstTeamName = "Белые";   // название первой команды не больше 7ми букв
String secondTeamName = "Желтые"; // название второй команды не больше 7ми букв
int scoreLimit = 10;              // количество голов для победы
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
int counterSongs;
boolean firstPush = true;
boolean isCanceled = false;                    // был ли отменен гол
boolean isGoalF=false;                          // флаг для апаратного прерывания
boolean isGoalS=false;                          // флаг для апаратного прерывания
boolean isFirstBreak=false;
boolean isSecondBreak=false;
String whoLast = "empty";                      // кто отменил последний

//-------ПЕРЕМЕННЫЕ---------

//-------МЕЛОДИИ---------
//общее количество мелодий
int countSongs = 2;

//!!!!!!!МАРИО!!!!!!!!
const byte MARIO_COUNT_NOTES = 17; //Размер массива с песней
int mario_frequences[MARIO_COUNT_NOTES] = {1046,784,659,880,987,932,880,784,1318,1568,1750,1396,1568,1318,1046,1174,987}; //частоты ноты
int mario_durations[MARIO_COUNT_NOTES] = {450,150,300,300,150,150,300,210,210,150,300,150,150,300,150,150,450};//длительность нот
int mario_delTime[MARIO_COUNT_NOTES] = {450,450,450,300,300,150,300,210,210,150,300,150,300,300,150,150,450};//задержка нот

//!!!!!!!Звездные войны!!!!!!!!
const byte STARS_COUNT_NOTES = 18; //Размер массива с песней
int stars_frequences[STARS_COUNT_NOTES] = {392, 392, 392, 311, 466, 392, 311, 466, 392, 587, 587, 587, 622, 466, 369, 311, 466, 392}; //частоты ноты
int stars_durations[STARS_COUNT_NOTES] = {350, 350, 350, 250, 100, 350, 250, 100, 700, 350, 350, 350, 250, 100, 350, 250, 100, 700};//длительность нот
int stars_delTime[STARS_COUNT_NOTES] = {350, 350, 350, 250, 100, 350, 250, 100, 700, 350, 350, 350, 250, 100, 350, 250, 100, 700};//задержка нот

//-------МЕЛОДИИ---------

//-------БИБЛИОТЕКИ---------
#include "LCD_1602_RUS.h"
//-------БИБЛИОТЕКИ---------

LCD_1602_RUS lcd(0x27, 16, 2);          // создать дисплей

void setup() {
  Serial.begin(9600);                   // открыть порт для связи с ПК для отладки
  attachInterrupt(0,checkSecond,RISING);
  attachInterrupt(1,checkFirst,RISING);
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
  Serial.print(firstTeamName);Serial.print("-----");Serial.print(firstScore);Serial.print(secondTeamName);Serial.print("-----");Serial.println(secondScore);
  lcd_print(firstScore, secondScore);
  //Пишем счет первой команды
  if(isGoalF){
    tone(piezo,1318,1000);
    prepare_score("first");
    isGoalF=false;
  }
  //Пишем счет второй команды
  if(isGoalS){
    tone(piezo, 700, 1000);
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
  if(firstScore==scoreLimit || secondScore==scoreLimit){
    reset_timer = millis();
    lcd.clear();
    while(1){
      //если забили последний гол и он не считовый
      if(firstScore==scoreLimit){
        lcd.setCursor(4, 0); lcd.print(L"ПОБЕДИЛИ");
        lcd.setCursor(5, 1); lcd.print(firstScore + "!!!");
      }
      if(secondScore==scoreLimit){
       lcd.setCursor(6, 0); lcd.print(secondScore);
       lcd.setCursor(6, 1); lcd.print(L"ВИН!!!");

      }

     if(!digitalRead(button)){
        minus_goal();
        lcd.clear();
        break;
     }
     if(millis() - reset_timer > 50){
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
    lcd.setCursor(4, 1); lcd.print(fS);             // отобразить счет первой команды
    lcd.setCursor(12, 1); lcd.print(sS);            // отобразить счет второй команды
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
  //воспроизводим мелодию
  switch (counterSongs) {
    case 0:
      sing_sound(mario_frequences, mario_durations, mario_delTime, MARIO_COUNT_NOTES);
      break;
    case 1:
      sing_sound(stars_frequences, stars_durations, stars_delTime, STARS_COUNT_NOTES);
      break;
  }
  counterSongs++;
  if(counterSongs == countSongs){
    counterSongs==0;
  }
}

void sing_sound(int frequences[] ,int durations[] , int delTime[], const byte COUNT_NOTES) {
    for (int i = 0; i <= COUNT_NOTES; i++  ) {
        tone(piezo, frequences[i], durations[i]);
        delay(delTime[i]);
        noTone(piezo);
    }
}
