#include "Movement.h"

const int leftPin1  = 7;
const int leftPin2  = 8;
const int rightPin3 = 9;
const int rightPin4 = 10;
const int enA = 6;
const int enB = 11;

Movement rover(leftPin1,leftPin2,rightPin3,rightPin4,enA,enB);

void setup() {
  rover.begin();
}
void loop() {
  rover.forward();
  delay(3000);
  rover.stopMotors();
  delay(2000);
  rover.backward();
  delay(3000);
  rover.stopMotors();
  delay(2000);

}

