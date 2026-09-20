int vB, vC, vB_fact, vC_fact;
int err, errOld, encB = 0, encC = 0, mm, dist_right, dist_left;//Объявление переменных
int err_bc, up_bc, err_Old_bc, stopp;
float up, gr;

#define B 10
#define B_back 9//Объявление назначения пинов
#define C 6
#define C_back 5
byte v_global = 50;
float kp = 0.3;//Коэф на линию
float kd = 0.5;


#include <SPI.h>// Библиотека радио-модуля
#include <RF24.h>

RF24 radio(8, 7);  // CE, CSN
const uint64_t pipe = 0xA85F57EF29LL;
byte matreshka_code_OK = 2;   //1, 2, 3, 4   Собственная цифра матрешки

byte nextRadioByte = 10;

void setup() {
  // put your setup code here, to run once:
  pinMode(B,OUTPUT);
  pinMode(B_back,OUTPUT);
  pinMode(C,OUTPUT);
  pinMode(C_back,OUTPUT);
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  attachInterrupt(digitalPinToInterrupt(3),counterB,RISING);
  attachInterrupt(digitalPinToInterrupt(2),counterC,RISING);
  Serial.begin(115200);
  radio.begin();
  radio.setChannel(0x67);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);      // Уровень питания усилителя RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH and RF24_PA_MAX ((RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm).
  radio.setAutoAck(false);         
  radio.setPayloadSize(1);        
  radio.openReadingPipe(0, pipe); 
  radio.startListening();   
  delay(200);
  // while (true){
  //   delay(10);
  //   Serial.print(analogRead(A0));
  //   Serial.print("    ");
  //   Serial.print(analogRead(A1));
  //   Serial.print("    ");
  //   Serial.print(s1());
  //   Serial.print("    ");
  //   Serial.println(s2());
  //   // Serial.print("    ");
  //   // Serial.print(encB);
  //   // Serial.print("    ");
  //   // Serial.println(encC);
  //   //line(v_global);
  // }


  // while (true){
  //   SendOK();
  //   waitForNextRadio();
  //   delay(3000);
  // }

  // set_speed(100,100);
  // delay(5000);
  // set_speed(-100,-100);
  // delay(5000);
  // stop(-50,-50);


  waitForNextRadio();//Ожидание данных с радио-модуля
  delay(19000);
 // delay(2000);
  set_speed(100,100);
  delay(50);
  //delay(3000);
  kp = 0.1;
  kd = 1;
  lineG(v_global+15,700);
  move(100,150);
  // set_speed(255,190);
  // delay(2500);
  // stop(200,200);
  //move(230,400);
  while (s1() > 70) {
    set_speed(80,0);
  }
  moveB(80,60);
  //move_sync(-60,60,90);
  left();
  kp = 0.1;
  kd = 1;
  lineG(v_global+20,150);
  lineP(v_global+20);
  left();
  lineP(v_global+15);
  lineG(v_global+15,170);
  right();
  move_sync(v_global,-v_global,5);
  SendOK();
  waitForNextRadio();
  delay(500);
  kp = 0.3;
  kd = 0.6;
  move(-v_global,75);
  lineG(v_global,150);
  move(-v_global,150);
  kp = 0.4;
  kd = 1;
  lineG(v_global,75);
  delay(300);
  lineP(v_global);
  kp = 0.5;
  kd = 1;
  SendOK();
   waitForNextRadio();
  left();
  lineG(v_global+8,700);
  lineP(v_global+8);
  moveB(-v_global,5);
  moveC(v_global+10,140);
  right();
  delay(2000);
  SendOK();
  waitForNextRadio();
  move_sync(-v_global-5,v_global+5,80);
  encB = 0;
  encC = 0;
  int errOldg, errg; 
  float upg;
  float k = 2.3;//коэф закругления дуги
  while (((encB+encC)/2) < 1200){
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
  delay(2000);
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



  stop(v_global,v_global);
  delay(700);
  left();
  // kp = 0.1;
  // kd = 1;
  lineG(v_global+10,400);
  lineP(v_global+10);
  right();
  SendOK();
  waitForNextRadio();
  // kp = 0.6;
  // kd = 0.6;
  lineP(v_global+20);
  left();
  left();
  delay(100);
  // kp = 0.6;
  // kd = 0.6;
  lineP(v_global+20);
  move_sync(-v_global,v_global,30);
  left();
  
  SendOK();
  waitForNextRadio();
  // kp = 0.3;
  lineG(v_global-4,800);
  lineP(v_global-4);
  right();
  lineG(v_global,145);
  move_sync(-v_global,v_global,86);
  SendOK();
  waitForNextRadio();
  //delay(500);



  encB = 0;
  encC = 0;
  k = 1.77;
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
  delay(1500);
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



  //конец дуги в одну линию

  SendOK();
  delay(12000);
  right();
  lineG(50,220);
  move(80,620);
}

void loop() {
  // put your main code here, to run repeatedly:
  //line();
  delay(10);
  Serial.print(encB);
  Serial.print("    ");
  Serial.println(encC);
  // Serial.print(analogRead(A1));
  // Serial.print("    ");
  // Serial.print(analogRead(A0));
  // Serial.print("    ");
  // Serial.print(s1());
  // Serial.print("    ");
  // Serial.println(s2());
}




int s1(){
  int values1 = map(analogRead(A0), 40, 300, 100, 0);
  if (values1 > 100) values1 = 100;
  else if (values1 < 0) values1 = 0;
  return values1;
}//Получение нормализованных значений с датчика 1

int s2(){
  int values2 = map(analogRead(A1), 40, 400, 100, 0);
  if (values2 > 100) values2 = 100;
  else if (values2 < 0) values2 = 0;
  return values2;
}//Получение нормализованных значений с датчика 2

void set_speed(int left, int right){//Установка скорости

  if (left > 255) {left = 255;}
  else if (left < -255) {left = -255;}// ограничение скорости


  if (right > 255) {right = 255;}
  else if (right < -255) {right = -255;}


  if (left >= 0) {analogWrite(B,left); analogWrite(B_back, 0);} // подача ШИМ на моторы
  else if (left < 0) {analogWrite(B_back,abs(left)); analogWrite(B, 0);}
  
  if (right >= 0) {analogWrite(C,right); analogWrite(C_back, 0);}
  else if (right < 0) {analogWrite(C_back,abs(right)); analogWrite(C, 0);}
  
}

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
  while ((s1() + s2()) > 15){
    line(v);
    if ((s1() + s2()) < 15){break;}
  }
  stop(vB,vC);
  move(v,20);
  kp = 0.15;
  lineG(v,30);
  kp = 0.3;
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
    set_speed(-vB+10,vC-10);
    Serial.print(s1());
    Serial.print("    ");
    Serial.println(s2());
    if ((s1() < 70) && (encB > 90)){break;}
    err_Old_bc = err_bc;
  }
  encB = 0;
  encC = 0;
  while (((encB+encC)/2) < 110){
    err_bc = encB - encC;
    up_bc = err_bc * 1 + (err_bc-err_Old_bc) * 5;
    vB = v_global-up_bc;
    vC = v_global+up_bc;
    set_speed(-vB+10,vC-10);
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
    set_speed(vB-10,-vC+10);
    Serial.print(s1());
    Serial.print("    ");
    Serial.println(s2());
    if ((s2() < 70) && (encB > 90)){break;}
    err_Old_bc = err_bc;
  }
  encB = 0;
  encC = 0;
  while (((encB+encC)/2) < 80){
    err_bc = encB - encC;
    up_bc = err_bc * 1 + (err_bc-err_Old_bc) * 5;
    vB = v_global-up_bc;
    vC = v_global+up_bc;
    set_speed(vB-10,-vC+10);
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
  }
  stop(v,v);
}

void moveB(int v, int angle){//Поворот одним колесом (правым)
  encB = 0;
  encC = 0;
  gr = angle * 11.7;
  while (encB < gr){
    set_speed(v,0);
    Serial.print(encB);
    Serial.print("   ");
    Serial.println(encC);
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
  gr = angle * 6.4;
  while (((encB+encC)/2) < gr){
    err_bc = encB - encC;
    up_bc = err_bc * 1 + (err_bc-err_Old_bc) * 5;
    vB = abs(vB_fact)-up_bc;
    vC = abs(vC_fact)+up_bc;
    set_speed(vB*(vB_fact/abs(vB_fact)),vC*(vC_fact/abs(vC_fact)));
    //delay(10);
    Serial.print(encB);
    Serial.print("   ");
    Serial.println(encC);
    err_Old_bc = err_bc;
  }
  stop(vB_fact,vC_fact);
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

void counterB(){//Энкодер правый
  encB++;
}

void counterC(){//Энкодер левый
  encC++;
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
