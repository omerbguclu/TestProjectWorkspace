#include <SoftwareSerial.h>

SoftwareSerial mySerial(7, 8); //RX TX

int data;
int hiz=0;
int hiz2=0;
const int AIN1 = A1; 
const int AIN2 = A0;
const int PWMA = 6;
const int BIN1 = A2;
const int BIN2 = A3;
const int PWMB = 5;
const int KORNA = 12;
const int ONFAR = 11;
const int ARKAFAR = 10;
const int DORTLU = 9;

void setup()
{
  mySerial.begin(9600);
  Serial.begin(9600);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(KORNA, OUTPUT);
  pinMode(ONFAR, OUTPUT);
  pinMode(ARKAFAR, OUTPUT);
  pinMode(DORTLU, OUTPUT);
  //  while(!mySerial.available());
  //  Serial.println("Bağlantı başarılı");
}

void ileri()
{
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, 255);

  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, 250);
}

void sag()
{
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  analogWrite(PWMA, 155);

  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, 155);
  
}

void sol()
{
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, 155);

  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMB, 155);
}

void geri()
{
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  analogWrite(PWMA, 100);

  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMB, 155);
}

void stop()
{
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);

  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
}

void loop()  
{
  if (mySerial.available())
  {
    char data = mySerial.read();
    // Serial.println(data);
    //delay(100);

    if (data == 'F')
    {
      ileri();
      // Serial.println("ileri");
    }
    else if (data == 'B')
    {
      geri();
      // Serial.println("geri");
    }
    else if (data == 'R')
    {
      sag();
      // Serial.println("sağ");
    }
    else if (data == 'L')
    {
      sol();
      // Serial.println("sol");
    }
    else if (data == 'W') {
      digitalWrite(ONFAR, HIGH);
      // Serial.println("front lights on");
    }
    else if (data == 'w') {
      digitalWrite(ONFAR, LOW);
      // Serial.println("front lights off");
    }
    else if (data == 'U') {
      digitalWrite(ARKAFAR, HIGH);
      // Serial.println("front lights on");
    }
    else if (data == 'u') {
      digitalWrite(ARKAFAR, LOW);
      // Serial.println("front lights off");
    }
    else
    {
      stop();
    }
  }
  // else
  // {
  //   Serial.println("not available");
  // }
}