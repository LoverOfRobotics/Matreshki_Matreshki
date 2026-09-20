int vB, vC, vB_fact, vC_fact;
int err, errOld, encB = 0, encC = 0, mm, dist_right, dist_left;//Инициализация пременных
int err_bc, up_bc, err_Old_bc, stopp;
float up, gr;
#include <ServoTimer2.h>//Инициализация библиотек
#include <Wire.h>
#define WIRE Wire
#include <VL53L0X.h>
VL53L0X sensorR;
VL53L0X sensorL;
ServoTimer2 servo;

#define B 10
#define B_back 9//Объявление назначения пинов
#define C 6
#define C_back 5
byte v_global = 50;
float kp = 0.3;//Коэф на линию
float kd = 0.5;

#include <SPI.h>//Библиотека радио-модуля
#include <RF24.h>


RF24 radio(8, 7);  // CE, CSN
const uint64_t pipe = 0xA85F57EF29LL;
byte matreshka_code_OK = 4;   //1, 2, 3, 4

byte nextRadioByte = 10;



void setup() {
  // put your setup code here, to run once:
  pinMode(B,OUTPUT);
  pinMode(B_back,OUTPUT);
  pinMode(C,OUTPUT);//Назначение типа пина
  pinMode(C_back,OUTPUT);
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(2,INPUT);
  pinMode(3,INPUT);
  Serial.begin(115200);
  radio.begin();
  radio.setChannel(0x67);     
  radio.setDataRate(RF24_250KBPS);  
  radio.setPALevel(RF24_PA_LOW);      // Уровень питания усилителя RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH and RF24_PA_MAX ((RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm).
  radio.setAutoAck(false);         
  radio.setPayloadSize(1);         
  radio.openReadingPipe(0, pipe);
  radio.startListening();
  attachInterrupt(digitalPinToInterrupt(3),counterB,RISING);
  attachInterrupt(digitalPinToInterrupt(2),counterC,RISING);
  servo.attach(4);
  servo.write(2300);// фулл вниз
  //servo.write(900);   фулл вверх


  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);

  Wire.begin();
  digitalWrite(A2, HIGH);//Дальномеры для "Потерялася"
  delay(150);
  sensorL.init(true);
  delay(100);
  sensorL.setAddress((uint8_t)01);

  digitalWrite(A3, HIGH);
  delay(150);
  sensorR.init(true);
  delay(100);
  sensorR.setAddress((uint8_t)02);

  sensorL.startContinuous();
  sensorR.startContinuous();


  Serial.println("Приёмник запущен, ждём данные...");


  // while (true){
  //   // delay(10);
  //   // Serial.print(encB);
  //   // Serial.print("    ");
  //   // Serial.print(encC);
  //   // Serial.print("    ");
  //   Serial.print(analogRead(A0));
  //   Serial.print("    ");
  //   Serial.print(analogRead(A1));
  //   Serial.print("    ");
  //   Serial.print(s1());
  //   Serial.print("    ");
  //   Serial.println(s2());
  // }

  //   Serial.print(sensorL.readRangeContinuousMillimeters());
  //   Serial.print("    ");
  //   Serial.println(sensorR.readRangeContinuousMillimeters());
  // }
  //   // if (sensorR.readRangeContinuousMillimeters() > 60 && sensorR.readRangeContinuousMillimeters() < 120) set_speed(100,0);
  //   // else if (sensorL.readRangeContinuousMillimeters() > 60 && sensorL.readRangeContinuousMillimeters() < 220) set_speed(0,100);
  //   // else set_speed(48,60);
  //   // servo.write(700);
  //   // delay(1000);
  //   // servo.write(2000);
  //   // delay(1000);
  //}


  // while (true){
  //   SendOK();
  //   waitForNextRadio();
  //   delay(3000);
  // }

  waitForNextRadio();//Ожидание данных с радио-модуля
  delay(15000);
 // delay(1500);
  lineG(60,250);
  move(230,150);
  while (s2() > 70) {
    set_speed(0,100);
  }
  moveC(100,60);
  right();
  kp = 0.3;
  kd = 1;
  lineG(v_global-3,150);
  lineP(v_global);
  right();
  lineP(v_global);
  lineG(v_global,170);
  move_sync(-v_global,v_global,80);
  left();
  // delay(7000);
  SendOK();
  waitForNextRadio();
  delay(300);
  kp = 0.3;
  kd = 1;
  move(-v_global+6,75);
  lineG(v_global-6,150);
  move(-v_global+6,150);
  kp = 0.3;
  kd = 1;
  lineG(v_global-6,75);
  delay(300);
  lineP(v_global-6);
  SendOK();
  waitForNextRadio();
  right();
  kp = 0.3;
  kd = 1;
  lineG(v_global-1,900);
  lineP(v_global+1);
  kp = 0.2;
  moveB(v_global,170);
  left();
  delay(2000);
  SendOK();
  
  waitForNextRadio();
  move_sync(-v_global,v_global,80);
  delay(100);

  //Начало дуги

  encB = 0;
  encC = 0;
  int errOldg, errg; 
  float upg;
  float k = 2.45;
  while (((encB+encC)/2) < 1100){
    errg = encB - encC*k;
    upg = errg * 2 + (errg - errOldg) * 2;
    set_speed(30*k-upg,33+upg);
    Serial.print(upg);
    Serial.print("   ");
    Serial.print(encB);
    Serial.print("   ");
    Serial.println(encC);
    errOldg = errg;
    delay(10);
  }
  stop(50,50);
  delay(2000);
  encB = 0;
  encC = 0;
  while (s1() > 50){
    errg = encB - encC*k;
    upg = errg * 2 + (errg - errOldg) * 2;
    set_speed(30*k-upg,33+upg);
    Serial.print(upg);
    Serial.print("   ");
    Serial.print(encB);
    Serial.print("   ");
    Serial.println(encC);
    errOldg = errg;
    delay(10);
  }
  encB = 0;
  encC = 0;
  while (((encB+encC)/2) < 350){ 
    errg = encB - encC*k;
    upg = errg * 2 + (errg - errOldg) * 2;
    set_speed(30*k-upg,33+upg);
    Serial.print(upg);
    Serial.print("   ");
    Serial.print(encB);
    Serial.print("   ");
    Serial.println(encC);
    errOldg = errg;
    delay(10);
  }

  //Конец дуги


  stop(v_global,v_global);
  delay(1100);
  left();
  kp = 0.3;
  kd = 0.5;
  lineG(v_global+2,400);
  lineP(v_global+7);
  // lineP(v_global+12);
  // lineP(v_global+12);
  right();
  SendOK();
  waitForNextRadio();
  kp = 0.3;
  kd = 0.5;
  lineP(v_global+5);
  left();
  left();
  kp = 0.3;
  kd = 0.5;
  lineP(v_global+5);
  while (s2()>70){
    set_speed(-v_global,v_global);
  }
  stop(-v_global,v_global);
  left();
  SendOK();
  waitForNextRadio();
  delay(2000);
  kp = 0.2;
  kd = 1;
  lineG(v_global-8,800);
  lineP(v_global-8);
  right();
  right();
  SendOK();
  waitForNextRadio();
  delay(350);
  
  k = 1.77;
  encB = 0;
  encC = 0;
  while (((encB+encC)/2) < 550){
    errg = encB - encC*k;
    upg = errg * 2 + (errg - errOldg) * 2;
    set_speed(33*k-upg,30+upg);
    Serial.print(upg);
    Serial.print("   ");
    Serial.print(encB);
    Serial.print("   ");
    Serial.println(encC);
    errOldg = errg;
    delay(10);
  }
  encB = 0;
  encC = 0;
  while (s1() > 50){
    errg = encB - encC*k;
    upg = errg * 2 + (errg - errOldg) * 2;
    set_speed(33*k-upg,30+upg);
    Serial.print(upg);
    Serial.print("   ");
    Serial.print(encB);
    Serial.print("   ");
    Serial.println(encC);
    errOldg = errg;
    delay(10);
  }
  encB = 0;
  encC = 0;
  while (((encB+encC)/2) < 350){
    errg = encB - encC*k;
    upg = errg * 2 + (errg - errOldg) * 2;
    set_speed(33*k-upg,30+upg);
    Serial.print(upg);
    Serial.print("   ");
    Serial.print(encB);
    Serial.print("   ");
    Serial.println(encC);
    errOldg = errg;
    delay(10);
  }
  stop(50,50);
  delay(1000);
  encB = 0;
  encC = 0;
  while (s2() > 50){
    errg = encB - encC*k;
    upg = errg * 2 + (errg - errOldg) * 2;
    set_speed(33*k-upg,30+upg);
    Serial.print(upg);
    Serial.print("   ");
    Serial.print(encB);
    Serial.print("   ");
    Serial.println(encC);
    errOldg = errg;
    delay(10);
  }
  encB = 0;
  encC = 0;
  while (((encB+encC)/2) < 350){
    errg = encB - encC*k;
    upg = errg * 2 + (errg - errOldg) * 2;
    set_speed(33*k-upg,30+upg);
    Serial.print(upg);
    Serial.print("   ");
    Serial.print(encB);
    Serial.print("   ");
    Serial.println(encC);
    errOldg = errg;
    delay(10);
  }
  stop(50,50);

  SendOK();
  delay(15000);
  v_global = 60;
  left();
  v_global = 50;
  kp = 0.5;
  lineG(v_global,300);
  servo.write(700);
  while (true){//Режим "Потерялася"
    set_speed(60,0);
    servo.write(700);
    long tmr = millis();
    while ((millis() - tmr) < 1300) {delay(10); if (sensorR.readRangeContinuousMillimeters() < 300) break;}
    if (sensorR.readRangeContinuousMillimeters() < 300) break;
    set_speed(0,60);
    servo.write(2300);
    tmr = millis();
    while ((millis() - tmr) < 1000) {delay(10); if (sensorR.readRangeContinuousMillimeters() < 300) break;}
    set_speed(0,0);
    if (sensorR.readRangeContinuousMillimeters() < 300) break;
  }
  servo.write(2300);
  long tmr = millis();
  while ((millis() - tmr) < 25000){//Режим нахождения
    if (sensorR.readRangeContinuousMillimeters() > 50 && sensorR.readRangeContinuousMillimeters() < 200) set_speed(50,-10);
    else if (sensorL.readRangeContinuousMillimeters() > 50 && sensorL.readRangeContinuousMillimeters() < 200) set_speed(-10,50);
    else set_speed(48,60);
  }
  set_speed(0,0);

}

void loop() {
  // put your main code here, to run repeatedly:
  //line(v_global);
  Serial.print(analogRead(A1));
  Serial.print("    ");
  Serial.println(analogRead(A0));
}





int s1(){
  int values1 = map(analogRead(A0), 45, 600, 100, 0);
  if (values1 > 100) values1 = 100;
  else if (values1 < 0) values1 = 0;
  return values1;
}//Получение нормализованных значений с датчика 1

int s2(){
  int values2 = map(analogRead(A1), 45, 600, 100, 0);
  if (values2 > 100) values2 = 100;
  else if (values2 < 0) values2 = 0;
  return values2;
}//Получение нормализованных значений с датчика 2


void line(int v){//Функция движения по линии
  err = s1() - s2();
  up = err * kp + (err-errOld) * kd;
  vB = v+up;
  vC = v-up;
  if (vB > 255) vC = (v-up)/(vB/255);
  else if (vC > 255) vB = (v+up)/(vC/255);
  else if (vB < 0) vB = 1;
  else if (vC < 0) vC = 1;
  set_speed(vB,vC);
  errOld = err;
  delay(10);
}

void lineP(int v){//До перекрестка по линии
  up = 0;
  errOld = 0;
  while ((s1() + s2()) > 20){
    line(v);
    if ((s1() + s2()) < 20){break;}
  }
  stop(vB,vC);
  move(v,20);
  lineG(v,25);
}

void lineG(int v, int mm){//На мм по линии
  encB = 0;
  encC = 0;
  up = 0;
  errOld = 0;
  gr = mm * 8.8;
  while (((encB+encC)/2) < gr){
    line(v);
    Serial.print(encB);
    Serial.print("   ");
    Serial.println(encC);
    if (((encB+encC)/2) > gr){break;}
  }
  stop(vB,vC);
}

void left(){//Поворот налево до линии
  encB = 0;
  encC = 0;
  err_bc = 0;
  err_Old_bc = 0;
  up_bc = 0;
  while (true){
    err_bc = encB - encC;
    up_bc = err_bc * 1 + (err_bc-err_Old_bc) * 5;
    vB = v_global-up_bc;
    vC = v_global+up_bc;
    set_speed(-vB,vC);
    Serial.print(s1());
    Serial.print("    ");
    Serial.println(s2());
    if ((s1() < 70) && (encB > 290)){break;}
    err_Old_bc = err_bc;
  }
  encB = 0;
  encC = 0;
  while (((encB+encC)/2) < 110){
    err_bc = encB - encC;
    up_bc = err_bc * 1 + (err_bc-err_Old_bc) * 5;
    vB = v_global-up_bc;
    vC = v_global+up_bc;
    set_speed(-vB,vC);
    Serial.print(encB);
    Serial.print("   ");
    Serial.println(encC);
    err_Old_bc = err_bc;
  }
  stop(-vB,vC);
}

void right(){//Поворот направо до линии
  encB = 0;
  encC = 0;
  err_bc = 0;
  err_Old_bc = 0;
  up_bc = 0;
  while (true){
    err_bc = encB - encC;
    up_bc = err_bc * 1 + (err_bc-err_Old_bc) * 5;
    vB = v_global-up_bc;
    vC = v_global+up_bc;
    set_speed(vB,-vC);
    Serial.print(s1());
    Serial.print("    ");
    Serial.println(s2());
    if ((s2() < 70) && (encB > 290)){break;}
    err_Old_bc = err_bc;
  }
  encB = 0;
  encC = 0;
  while (((encB+encC)/2) < 110){
    err_bc = encB - encC;
    up_bc = err_bc * 1 + (err_bc-err_Old_bc) * 5;
    vB = v_global-up_bc;
    vC = v_global+up_bc;
    set_speed(vB,-vC);
    Serial.print(encB);
    Serial.print("   ");
    Serial.println(encC);
    err_Old_bc = err_bc;
  }

  stop(vB,-vC);
}

void move(int v, int mm){//Синхронизированная езда по энкодерам на мм
  encB = 0;
  encC = 0;
  err_bc = 0;
  err_Old_bc = 0;
  up_bc = 0;
  gr = mm * 8.8;
  while (((encB+encC)/2) < gr){
    if (v < 0) err_bc = encC - encB;
    else err_bc = encB - encC;
    up_bc = err_bc * 1 + (err_bc-err_Old_bc) * 1;
    set_speed(v-up_bc,v+up_bc);
    err_Old_bc = err_bc;
    Serial.print(encB);
    Serial.print("   ");
    Serial.println(encC);
    delay(10);
  }
  stop(v,v);
}

void moveB(int v, int angle){//Поворот одним колесом (правым)
  encB = 0;
  encC = 0;
  gr = angle * 12;
  while (encB < gr){
    set_speed(v,0);
    Serial.print(encB);
    Serial.print("   ");
    Serial.println(encC);
    delay(10);
  }
  analogWrite(B,0);
  analogWrite(B_back,255);
  delay(15);
  analogWrite(B_back,0);
}

void moveC(int v, int angle){//Поворот одним колесом (левым)
  encB = 0;
  encC = 0;
  gr = angle * 12;
  while (encC < gr){
    set_speed(0,v);
    Serial.print(encB);
    Serial.print("   ");
    Serial.println(encC);
    delay(10);
  }
  analogWrite(C,0);
  analogWrite(C_back,255);
  delay(15);
  analogWrite(C_back,0);
}

void move_sync(int vB_fact, int vC_fact, int angle){//Поворот обоими колесами с синхронизацией
  encB = 0;
  encC = 0;
  err_bc = 0;
  err_Old_bc = 0;
  up_bc = 0;
  gr = angle * 6;
  while (((encB+encC)/2) < gr){
    err_bc = encB - encC;
    up_bc = err_bc * 1 + (err_bc-err_Old_bc) * 5;
    vB = abs(vB_fact)-up_bc;
    vC = abs(vC_fact)+up_bc;
    set_speed(vB*(vB_fact/abs(vB_fact)),vC*(vC_fact/abs(vC_fact)));
    delay(10);
    Serial.print(encB);
    Serial.print("   ");
    Serial.println(encC);
    err_Old_bc = err_bc;
  }
  stop(vB_fact,vC_fact);
}

void set_speed(int vB, int vC){//Установка скорости
  if (vB > 255){vB = 255;}
  if (vC > 255){vC = 255;}
  if (vB > 0){
    analogWrite(B_back,0);
    analogWrite(B,vB);
  }else{
    analogWrite(B,0);
    analogWrite(B_back,abs(vB));
  }

  if (vC > 0){
    analogWrite(C_back,0);
    analogWrite(C,vC);
  }else{
    analogWrite(C,0);
    analogWrite(C_back,abs(vC));
  }
}

void stop(int vB, int vC){//Остановка двигателей
  if ((vB > 0) && (vC > 0)){
    stopp = 1;
  }else if ((vB > 0) && (vC < 0)){
    stopp = 2;
  }else if ((vB < 0) && (vC > 0)){
    stopp = -1;
  }else if ((vB < 0) && (vC < 0)){
    stopp = -2;
  }
  analogWrite(B,0);
  analogWrite(B_back,0);
  analogWrite(C,0);
  analogWrite(C_back,0);
  if (stopp == 1){
    analogWrite(C_back,255);
    analogWrite(B_back,255);
  }else if (stopp == 2){
    analogWrite(B_back,255);
    analogWrite(C,255);
  }else if (stopp == -1){
    analogWrite(B,255);
    analogWrite(C_back,255);
  }else if (stopp == -2){
    analogWrite(B,255);
    analogWrite(C,255);
  }
  delay(10);
  analogWrite(B,0);
  analogWrite(B_back,0);
  analogWrite(C,0);
  analogWrite(C_back,0);
}

void waitForNextRadio(){//Функция получения данных с радио-модуля
  byte receivedData = 0;

  while (receivedData != nextRadioByte){
    if (radio.available()) {           // если есть принятые данные
      radio.read(&receivedData, 1);    // читаем 1 байт
      Serial.println("Received: " + String(receivedData));
    }
    // небольшая задержка, чтобы не грузить процессор
    delay(50);
  }

  nextRadioByte++;

  return;
}

void SendOK(){//Функция отправка данных на радио-модуль
  radio.stopListening();
  radio.closeReadingPipe(0);
  radio.openWritingPipe(pipe);

  Serial.println("Sending: " + String(matreshka_code_OK));
  for (int i = 0; i < 10; i++){
    if (radio.write(&matreshka_code_OK, 1) == false)
      Serial.println("Ошибка");
    delay(50);
  }
  
  radio.startListening();
  radio.openReadingPipe(0, pipe);
}


void counterB(){//Энкодер правый
  encB++;
}

void counterC(){//Энкодер левый
  encC++;
}