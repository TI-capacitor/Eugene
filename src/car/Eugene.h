#ifndef EUGENE_H
#define EUGENE_H
#include <Arduino.h>


class Command{
  private:
    String inputString = "";
    bool stringComplete = false;

  public:
    //command methods
    void setCommand();
    void parseCommand();
    void listOfCommands();
};



class Movement{
  private:
    int In1, In2, In3, In4, EnA, EnB;
   
  public:
    //constructor prototype
    Movement(int,int,int,int,int,int);

    //to be used in pinMode() method
    void begin();

    //stops all motors and sets pwm to 0
    void stopMotors();

    //basic movements
    void forward();
    void backward();

    //right and left pivots
    void pivotRight();
    void pivotLeft();
};

#endif