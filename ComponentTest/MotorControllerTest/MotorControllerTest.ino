#include <LMotorController.h>

#define MIN_ABS_SPEED 20

//MOTOR CONTROLLER
int ENA = 5;
int IN1 = 4;
int IN2 = 3;
int IN3 = 2;
int IN4 = 1;
int ENB = 0;

double motorSpeedFactorLeft = 0.6;
double motorSpeedFactorRight = 0.6;

LMotorController motorController(ENA, IN1, IN2, ENB, IN3, IN4, motorSpeedFactorLeft, motorSpeedFactorRight);

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  motorController.move(120, MIN_ABS_SPEED);
  delay(500);
  motorController.move(100, MIN_ABS_SPEED);
  delay(500);
  motorController.move(80, MIN_ABS_SPEED);
  delay(500);
  motorController.move(60, MIN_ABS_SPEED);
  delay(500);
  motorController.move(80, MIN_ABS_SPEED);
  delay(500);
  motorController.move(100, MIN_ABS_SPEED);
  delay(500);
  motorController.move(120, MIN_ABS_SPEED);
  delay(500);
}
