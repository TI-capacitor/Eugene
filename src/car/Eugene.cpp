#include "Eugene.h"


//reserves memory for the command string once
void Command::begin() {
  inputString.reserve(MAX_LENGTH);
}

//reads command and builds string, saves it on private variable
//non-blocking: only reads what is already in the serial buffer
void Command::setCommand() {
  while (Serial.available() && !stringComplete)
  {
    char c = Serial.read();

    //accept '\n' or '\r' as end of command (bluetooth apps send either)
    if (c == '\n' || c == '\r') {
      if (inputString.length() > 0) stringComplete = true;
    }

    else if (inputString.length() < MAX_LENGTH) inputString += c;
  }
}

//parsing of commands, includes typecasting to lowercase
//returns the command so the state machine decides what to do with it
COMMAND_TYPE Command::parseCommand() {
  if (!stringComplete) return CMD_NONE;
  inputString.trim();
  inputString.toLowerCase();
  Serial.print(F("Received: "));
  Serial.println(inputString);

  COMMAND_TYPE result = CMD_UNKNOWN;

  if(inputString == "front") result = CMD_FRONT;

  else if(inputString == "back") result = CMD_BACK;

  else if(inputString == "stop") result = CMD_STOP;

  else if(inputString == "pivot.right") result = CMD_PIVOT_RIGHT;

  else if(inputString == "pivot.left") result = CMD_PIVOT_LEFT;

  else if(inputString == "bt.mode") result = CMD_BT_MODE;

  else if(inputString == "smart.mode") result = CMD_SMART_MODE;

  else if(inputString == "scan") result = CMD_SCAN;

  else if(inputString == "idle") result = CMD_IDLE;

  else if(inputString == "help") result = CMD_HELP;

  // Reset buffer
  inputString = "";
  stringComplete = false;

  return result;
}

//list of possible commands, F() keeps the text in flash instead of RAM
void Command::listOfCommands(){
  Serial.println(F(" _________________________________________"));
  Serial.println(F("|        LIST OF POSSIBLE COMMANDS        |"));
  Serial.println(F("|----------------- Modes -----------------|"));
  Serial.println(F("| bt.mode - Bluetooth manual control      |"));
  Serial.println(F("| smart.mode - Autonomous navigation      |"));
  Serial.println(F("| scan - Ultrasonic sweep (left/right)    |"));
  Serial.println(F("| idle - Stop and await a command         |"));
  Serial.println(F("|--------------- Movements ---------------|"));
  Serial.println(F("| front - Moves the rover forward         |"));
  Serial.println(F("| back - Moves the rover backward         |"));
  Serial.println(F("| stop - Halts movement                   |"));
  Serial.println(F("| pivot.left - Pivots rover to the left   |"));
  Serial.println(F("| pivot.right - Pivots to the right       |"));
  Serial.println(F("| help - Shows this list                  |"));
  Serial.println(F(" ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾"));
}
/*-----------------------------------------------------------------------------*/

//movement class constructor, saves the pins on the private variables
Movement::Movement(int in1,int in2,int in3,int in4,int enA,int enB)
  : In1(in1), In2(in2), In3(in3), In4(in4), EnA(enA), EnB(enB), speed(200) {}

//method to initialize pins in one instance
void Movement::begin(){
  pinMode(In1,OUTPUT);
  pinMode(In2,OUTPUT);
  pinMode(In3,OUTPUT);
  pinMode(In4,OUTPUT);
  pinMode(EnA,OUTPUT);
  pinMode(EnB,OUTPUT);

  //start with the motors off
  stopMotors();
}

//sets pwm duty cycle, constrained to the valid analogWrite range
void Movement::setSpeed(int newSpeed){
  speed = constrain(newSpeed, 0, 255);
}

//writes direction to both motors and applies the pwm
void Movement::drive(bool in1, bool in2, bool in3, bool in4){
  digitalWrite(In1,in1);
  digitalWrite(In2,in2);
  digitalWrite(In3,in3);
  digitalWrite(In4,in4);
  analogWrite(EnA,speed);
  analogWrite(EnB,speed);
}

//stops all motor movement
void Movement::stopMotors() {
  digitalWrite(In1,LOW);
  digitalWrite(In2,LOW);
  digitalWrite(In3,LOW);
  digitalWrite(In4,LOW);
  analogWrite(EnA,0);
  analogWrite(EnB,0);
}

//Moves forward
void Movement::forward() {
  drive(HIGH,LOW,LOW,HIGH);
}

//moves in reverse
void Movement::backward() {
  drive(LOW,HIGH,HIGH,LOW);
}

//pivots to the right
void Movement::pivotRight() {
  drive(HIGH,LOW,HIGH,LOW);
}

//pivots left
void Movement::pivotLeft() {
  drive(LOW,HIGH,LOW,HIGH);
}
/*-----------------------------------------------------------------------------*/

//static variables shared with the interrupt
int Ultrasonic::isrEchoPin = 2;
volatile unsigned long Ultrasonic::echoStart = 0;
volatile unsigned long Ultrasonic::echoEnd = 0;
volatile bool Ultrasonic::echoDone = false;

//ultrasonic class constructor
Ultrasonic::Ultrasonic(int trig, int echo, unsigned long interval)
  : trigPin(trig), echoPin(echo), pingInterval(interval), distanceCm(MAX_DISTANCE) {}

//initialize pins and attach the echo interrupt
void Ultrasonic::begin(){
  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);
  digitalWrite(trigPin,LOW);

  isrEchoPin = echoPin;
  attachInterrupt(digitalPinToInterrupt(echoPin), echoISR, CHANGE);
}

//runs on every edge of the echo pin, times the pulse without pulseIn()
void Ultrasonic::echoISR(){
  if (digitalRead(isrEchoPin) == HIGH) echoStart = micros();
  else {
    echoEnd = micros();
    echoDone = true;
  }
}

//non-blocking measurement: fire ping, then check for the echo on later loops
void Ultrasonic::update(){
  unsigned long now = millis();

  if (waitingEcho)
  {
    if (echoDone)
    {
      //copy the volatile values safely
      noInterrupts();
      unsigned long duration = echoEnd - echoStart;
      interrupts();

      //sound travels 0.0343 cm/us, round trip -> divide by 58
      distanceCm = duration / 58.0;
      if (duration == 0 || distanceCm > MAX_DISTANCE) distanceCm = MAX_DISTANCE;

      newReading = true;
      waitingEcho = false;
    }

    //no echo (nothing in range or bad wiring), count as clear path
    else if (now - prevPingTime >= 40)
    {
      distanceCm = MAX_DISTANCE;
      newReading = true;
      waitingEcho = false;
    }
  }

  //time for a new ping
  if (!waitingEcho && now - prevPingTime >= pingInterval)
  {
    echoDone = false;

    //10us trigger pulse, the only wait in the code (microseconds)
    digitalWrite(trigPin,LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin,HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin,LOW);

    prevPingTime = now;
    waitingEcho = true;
  }
}

//returns latest distance in cm
float Ultrasonic::getDistance(){
  return distanceCm;
}

//true only once per new reading
bool Ultrasonic::hasNewReading(){
  if (newReading) {
    newReading = false;
    return true;
  }
  return false;
}