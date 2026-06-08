#include "Movement.h"
#include "Command.h"

const int leftPin1  = 7;
const int leftPin2  = 8;
const int rightPin3 = 9;
const int rightPin4 = 10;
const int enA = 6;
const int enB = 11;
Command cmd;

Movement rover(leftPin1,leftPin2,rightPin3,rightPin4,enA,enB);

void setup() {
  rover.begin();
  Serial.begin(9600);
}
void loop() {
  cmd.readCommand();
  cmd.parseCommand();

}
  

