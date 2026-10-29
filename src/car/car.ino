#include "Eugene.h"

//List of states (follows the state diagram)
enum ROVER_STATE{AWAIT_COMMAND=0,BT_CONTROL,SCAN,AUTONOMOUS};

//steps inside the scan state, each one is timed with millis()
enum SCAN_STEP{SCAN_START=0,SCAN_TURN_RIGHT,SCAN_READ_RIGHT,SCAN_TURN_LEFT,
               SCAN_READ_LEFT,SCAN_REVERSE,SCAN_FACE_BEST,SCAN_DONE};


ROVER_STATE currentState = AWAIT_COMMAND;
SCAN_STEP scanStep = SCAN_START;

//true while smart.mode is active, so scan returns to autonomy
bool autonomyActive = false;

//true while driving forward in bluetooth mode (for obstacle auto-stop)
bool movingForward = false;


//variables to manage concurrency timing
unsigned long commandControl = 50;
unsigned long prevControlTime = 0;
unsigned long current_Time = 0;
unsigned long stepStartTime = 0;
unsigned long faceTime = 0;

//tuning values, adjust them on the surface you present on
const float OBSTACLE_DISTANCE = 25.0;   //cm, autonomy starts a scan
const float SAFETY_DISTANCE = 15.0;     //cm, bluetooth forward auto-stop
const unsigned long TURN_TIME = 350;    //ms of pivot for ~45 degrees
const unsigned long SETTLE_TIME = 150;  //ms to wait for a fresh reading
const unsigned long REVERSE_TIME = 400; //ms backing up when boxed in
const unsigned long PING_INTERVAL = 60; //ms between ultrasonic pings
const int ROVER_SPEED = 200;            //pwm 0 - 255

//scan results
float frontDist = 0;
float rightDist = 0;
float leftDist = 0;

//pin variables
const int In1  = 7;
const int In2  = 8;
const int In3 = 9;
const int In4 = 10;
const int EnA = 6;
const int EnB = 11;

//HC-SR04 pins, echo must be on D2 or D3 (interrupt pins)
const int TrigPin = 4;
const int EchoPin = 2;

//create object of type Command to handle command methods
Command cmd;

//send pins to Movement constructor
Movement rover(In1,In2,In3,In4,EnA,EnB);

//ultrasonic sensor object
Ultrasonic sonar(TrigPin,EchoPin,PING_INTERVAL);


//changes state and restarts the step timer
void changeState(ROVER_STATE newState){
  currentState = newState;
  stepStartTime = millis();
}

//moves to the next step inside the scan
void nextStep(SCAN_STEP step){
  scanStep = step;
  stepStartTime = millis();
}

//begins a new scan from the first step
void startScan(){
  rover.stopMotors();
  movingForward = false;
  scanStep = SCAN_START;
  changeState(SCAN);
}

//prints a scan reading with a label
void printReading(const __FlashStringHelper *label, float dist){
  Serial.print(label);
  Serial.print(dist);
  Serial.println(F(" cm"));
}

//executes movement commands, only used in bluetooth control
void executeMovement(COMMAND_TYPE c){
  switch(c){
    case CMD_FRONT:
      if(sonar.getDistance() < SAFETY_DISTANCE){
        Serial.println(F("OBSTACLE AHEAD, CAN'T MOVE FORWARD"));
        break;
      }
      Serial.println(F("MOVING FORWARD..."));
      rover.forward();
      movingForward = true;
      break;

    case CMD_BACK:
      Serial.println(F("MOVING BACKWARD..."));
      rover.backward();
      movingForward = false;
      break;

    case CMD_PIVOT_RIGHT:
      Serial.println(F("PIVOT RIGHT"));
      rover.pivotRight();
      movingForward = false;
      break;

    case CMD_PIVOT_LEFT:
      Serial.println(F("PIVOT LEFT"));
      rover.pivotLeft();
      movingForward = false;
      break;

    default:
      break;
  }
}

//decides what to do with each command depending on the current state
void handleCommand(COMMAND_TYPE c){
  switch(c){
    case CMD_NONE:
      return;

    case CMD_HELP:
      cmd.listOfCommands();
      break;

    case CMD_UNKNOWN:
      Serial.println(F("Unknown command"));
      cmd.listOfCommands();
      break;

    case CMD_BT_MODE:
      rover.stopMotors();
      movingForward = false;
      autonomyActive = false;
      changeState(BT_CONTROL);
      Serial.println(F("BLUETOOTH CONTROL MODE"));
      break;

    case CMD_SMART_MODE:
      autonomyActive = true;
      changeState(AUTONOMOUS);
      Serial.println(F("AUTONOMY MODE"));
      break;

    case CMD_SCAN:
      autonomyActive = false;
      Serial.println(F("SCANNING..."));
      startScan();
      break;

    case CMD_IDLE:
      rover.stopMotors();
      movingForward = false;
      autonomyActive = false;
      changeState(AWAIT_COMMAND);
      Serial.println(F("AWAITING COMMAND"));
      break;

    case CMD_STOP:
      rover.stopMotors();
      movingForward = false;
      Serial.println(F("STOPPING..."));

      //stop in autonomy or scan cancels it, in bluetooth it stays in bluetooth
      if(currentState != BT_CONTROL){
        autonomyActive = false;
        changeState(AWAIT_COMMAND);
      }
      break;

    //movement commands
    default:
      if(currentState == BT_CONTROL) executeMovement(c);
      else Serial.println(F("Send bt.mode first to drive manually"));
      break;
  }
}

//bluetooth control: commands execute instantly, this only watches for obstacles
void runBluetooth(){
  if(movingForward && sonar.getDistance() < SAFETY_DISTANCE){
    rover.stopMotors();
    movingForward = false;
    Serial.println(F("OBSTACLE! STOPPED"));
  }
}

//autonomy framework: drive forward until the sensor sees something, then scan
void runAutonomy(){
  if(sonar.getDistance() < OBSTACLE_DISTANCE){
    Serial.println(F("OBSTACLE DETECTED -> SCAN"));
    startScan();
  }
  else rover.forward();
}

//HC-SR04 scan: look front, right and left, then face the clearest way
void runScan(){
  unsigned long elapsed = current_Time - stepStartTime;

  switch(scanStep){
    case SCAN_START:
      if(elapsed >= SETTLE_TIME){
        frontDist = sonar.getDistance();
        printReading(F("Front: "), frontDist);
        rover.pivotRight();
        nextStep(SCAN_TURN_RIGHT);
      }
      break;

    case SCAN_TURN_RIGHT:
      if(elapsed >= TURN_TIME){
        rover.stopMotors();
        nextStep(SCAN_READ_RIGHT);
      }
      break;

    case SCAN_READ_RIGHT:
      if(elapsed >= SETTLE_TIME){
        rightDist = sonar.getDistance();
        printReading(F("Right: "), rightDist);
        rover.pivotLeft();
        nextStep(SCAN_TURN_LEFT);
      }
      break;

    case SCAN_TURN_LEFT:
      //twice the time, from right side to left side
      if(elapsed >= 2 * TURN_TIME){
        rover.stopMotors();
        nextStep(SCAN_READ_LEFT);
      }
      break;

    case SCAN_READ_LEFT:
      if(elapsed < SETTLE_TIME) break;

      leftDist = sonar.getDistance();
      printReading(F("Left: "), leftDist);

      //manual scan: go back to the center
      if(!autonomyActive){
        rover.pivotRight();
        faceTime = TURN_TIME;
        nextStep(SCAN_FACE_BEST);
      }
      //left is clearest, already facing it
      else if(leftDist >= rightDist && leftDist > OBSTACLE_DISTANCE){
        Serial.println(F("GOING LEFT"));
        nextStep(SCAN_DONE);
      }
      //right is clearest, turn from left side to right side
      else if(rightDist > OBSTACLE_DISTANCE){
        Serial.println(F("GOING RIGHT"));
        rover.pivotRight();
        faceTime = 2 * TURN_TIME;
        nextStep(SCAN_FACE_BEST);
      }
      //boxed in, back up and turn around
      else{
        Serial.println(F("BOXED IN, REVERSING"));
        rover.backward();
        nextStep(SCAN_REVERSE);
      }
      break;

    case SCAN_REVERSE:
      if(elapsed >= REVERSE_TIME){
        rover.pivotRight();
        faceTime = 4 * TURN_TIME;
        nextStep(SCAN_FACE_BEST);
      }
      break;

    case SCAN_FACE_BEST:
      if(elapsed >= faceTime){
        rover.stopMotors();
        nextStep(SCAN_DONE);
      }
      break;

    case SCAN_DONE:
      rover.stopMotors();

      //wait for a fresh reading in the new direction before driving again
      if(elapsed < SETTLE_TIME) break;

      if(autonomyActive) changeState(AUTONOMOUS);
      else{
        Serial.println(F("SCAN COMPLETE, AWAITING COMMAND"));
        changeState(AWAIT_COMMAND);
      }
      break;
  }
}


void setup() {

  //initialize pins to be used by the rover
  rover.begin();
  rover.setSpeed(ROVER_SPEED);
  sonar.begin();
  cmd.begin();
  Serial.begin(9600);

  Serial.println(F("EUGENE READY"));
  cmd.listOfCommands();
}
void loop() {
  //save current time
  current_Time = millis();

  //ultrasonic runs in the background every loop
  sonar.update();

  //handle command input via serial communication channel
  cmd.setCommand();
  handleCommand(cmd.parseCommand());

  //creation of state machine for change of states
  if(current_Time - prevControlTime >= commandControl)
  {
    prevControlTime = current_Time;
    switch(currentState){

      case AWAIT_COMMAND:
        //motors off, waiting for bt.mode, smart.mode or scan
        break;

      case BT_CONTROL:
        runBluetooth();
        break;

      case SCAN:
        runScan();
        break;

      case AUTONOMOUS:
        runAutonomy();
        break;

    }
  }

}