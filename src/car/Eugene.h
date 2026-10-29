#ifndef EUGENE_H
#define EUGENE_H
#include <Arduino.h>


//list of commands the parser can return to the state machine
enum COMMAND_TYPE{
  CMD_NONE = 0,     //no complete command received yet
  CMD_FRONT,
  CMD_BACK,
  CMD_STOP,
  CMD_PIVOT_LEFT,
  CMD_PIVOT_RIGHT,
  CMD_BT_MODE,      //"bt.mode"    -> bluetooth control
  CMD_SMART_MODE,   //"smart.mode" -> autonomy framework
  CMD_SCAN,         //"scan"       -> one-off HC-SR04 scan
  CMD_IDLE,         //"idle"       -> back to await command
  CMD_HELP,
  CMD_UNKNOWN
};



class Command{
  private:
    String inputString = "";
    bool stringComplete = false;

    //max characters per command, protects the Nano's 2KB of RAM
    static const unsigned int MAX_LENGTH = 32;

  public:
    //reserves buffer memory once to avoid heap fragmentation
    void begin();

    //command methods
    void setCommand();
    COMMAND_TYPE parseCommand();
    void listOfCommands();
};



class Movement{
  private:
    int In1, In2, In3, In4, EnA, EnB;
    int speed;

    //writes direction pins and pwm in one place
    void drive(bool in1, bool in2, bool in3, bool in4);

  public:
    //constructor prototype
    Movement(int,int,int,int,int,int);

    //to be used in pinMode() method
    void begin();

    //pwm duty cycle used by every movement (0 - 255)
    void setSpeed(int newSpeed);

    //stops all motors and sets pwm to 0
    void stopMotors();

    //basic movements
    void forward();
    void backward();

    //right and left pivots
    void pivotRight();
    void pivotLeft();
};



class Ultrasonic{
  private:
    int trigPin, echoPin;
    unsigned long pingInterval;
    unsigned long prevPingTime = 0;
    bool waitingEcho = false;
    float distanceCm;
    bool newReading = false;

    //shared with the interrupt, must be static and volatile
    static int isrEchoPin;
    static volatile unsigned long echoStart;
    static volatile unsigned long echoEnd;
    static volatile bool echoDone;
    static void echoISR();

  public:
    static const int MAX_DISTANCE = 400; //HC-SR04 max range in cm

    //echo pin must be D2 or D3 on the Nano (external interrupt pins)
    Ultrasonic(int trig, int echo, unsigned long interval);

    void begin();

    //call every loop, fires a ping when the interval passes
    void update();

    //latest distance in cm
    float getDistance();

    //true once per new measurement
    bool hasNewReading();
};

#endif