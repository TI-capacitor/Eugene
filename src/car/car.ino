#include "Movement.h"
#include "Command.h"

enum ROVER_STATE{BT_CONTROL=1,SCAN,AUTONOMOUS};
ROVER_STATE currentState = BT_CONTROL;

unsigned long commandControl = 100;
unsigned long prevControlTime = 0;
unsigned long current_Time = 0;

//pins
const int In1  = 7;
const int In2  = 8;
const int In3 = 9;
const int In4 = 10;
const int EnA = 6;
const int EnB = 11;

//create object of type Command to handle command methods
Command cmd;

//send pins to Movement constructor
Movement rover(In1,In2,In3,In4,EnA,EnB);

void setup() {
  //initialize pins to be used by the rover
  rover.begin();
  Serial.begin(9600);
}
void loop() {
  //save current time
  current_Time = millis();

  //handle command input via serial communication channel
  cmd.readCommand();
  cmd.parseCommand();


  if(current_Time - prevControlTime >= commandControl)
  {
    prevControlTime = current_Time;
    switch(currentState){   

      case SCAN:
        //do ultrasound scan
        break;

      case AUTONOMOUS:
        //autonomy Framework coming soon
        break;

    }
  }

}


