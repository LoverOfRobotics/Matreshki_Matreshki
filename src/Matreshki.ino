#include <SPI.h>
#include <RF24.h>

RF24 radio(8, 7);  // CE, CSN
const uint64_t pipe = 0xA85F57EF29LL;
byte matreshka_code_OK = 1;   //1, 2, 3, 4

byte nextRadioByte = 10;
void waitForNextRadio(){
  //Тут может несколько раз (обычно 3 раза) принять старый код - это читает из буфера, но в коде есть защита - ожидание именно nextRadioByte, так что норм
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


void SendOK(){
  radio.stopListening();
  radio.closeReadingPipe(0);
  radio.openWritingPipe(pipe);

  Serial.println("Sending: " + String(matreshka_code_OK));
  for (int i = 0; i < 10; i++){
    if (radio.write(&matreshka_code_OK, 1) == false)
      Serial.println("Ошибка");
    // else
      // Serial.println("Trying " + (String)i + " OK");
    delay(80);
  }
  
  radio.startListening();
  radio.openReadingPipe(0, pipe);
}


void setup() {
  Serial.begin(115200);
  radio.begin();
  radio.setChannel(0x67);            // тот же канал
  radio.setDataRate(RF24_250KBPS);   // та же скорость
  radio.setPALevel(RF24_PA_LOW);    	// Уровень питания усилителя RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH and RF24_PA_MAX ((RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm).
  radio.setAutoAck(false);           // автоответ выключен (как у передатчика)
  radio.setPayloadSize(1);           // ждём 1 байт
  radio.openReadingPipe(0, pipe);    // открываем трубу на приём
  radio.startListening();            // начинаем слушать
  
  pinMode(2, OUTPUT);   digitalWrite(2, 0);
  pinMode(3, OUTPUT);   digitalWrite(3, 0);
  pinMode(4, OUTPUT);   digitalWrite(4, 0);
  pinMode(5, OUTPUT);   digitalWrite(5, 0);
  pinMode(6, OUTPUT);   digitalWrite(6, 0);
  pinMode(9, OUTPUT);   digitalWrite(9, 0);
  pinMode(10, OUTPUT);  digitalWrite(10, 0);

  Serial.println("Приёмник запущен, ждём данные...");
}

void loop() {
  waitForNextRadio();
  Serial.println("Radio byte received!");
  delay(2000);
  SendOK();
  
}