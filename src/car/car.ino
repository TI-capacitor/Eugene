#include "Movement.h"
#include "Command.h"

const int In1  = 7;
const int In2  = 8;
const int In3 = 9;
const int In4 = 10;
const int EnA = 6;
const int EnB = 11;
Command cmd;

Movement rover(In1,In2,In3,In4,EnA,EnB);

void setup() {
  rover.begin();
  Serial.begin(9600);
}
void loop() {
  cmd.readCommand();
  cmd.parseCommand();

}
  

