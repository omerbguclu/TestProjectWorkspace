#include <Servo.h>  // add servo library

#define DEBUG

//Global Variables
int sensorPin = 0;      //pin number to use the ADC
int sensorValue = 0;    //initialization of sensor variable, equivalent to EMA Y
float EMA_a = 0.6;      //initialization of EMA alpha
int EMA_S = 0;          //initialization of EMA S
int val=0;    // variable to read the value from the analog pin
Servo myservo;  // create servo object to control a servo

void setup(){
    #ifdef DEBUG
    Serial.begin(115200);           //setup of Serial module, 115200 bits/second
    #endif
    EMA_S = analogRead(sensorPin);  //set EMA S for t=1
    myservo.attach(9);  // attaches the servo on pin 9 to the servo object
}

void loop(){
    sensorValue = analogRead(sensorPin);                //read the sensor value using ADC
    EMA_S = (EMA_a*sensorValue) + ((1-EMA_a)*EMA_S);    //run the EMA
    val = map(EMA_S, 0, 1023, 0, 180);
    #ifdef DEBUG
    Serial.println(EMA_S);                              //print digital value to serial
    Serial.println(val);
    #endif
    myservo.write(val);                  // sets the servo position according to the scaled value
    delay(2 );                                          //50ms delay
}
