#ifndef MOVEMENT_H
#define MOVEMENT_H
#include <Arduino.h>



class Movement{
  private:
    int In1, In2;
    int In3, In4;
    int EnA, EnB;
  public:
    //constructor
    Movement(int,int,int,int,int,int);

    //to be used in pinMode function
    void begin();

    //stops all motors and sets pwm to 0
    void stopMotors();

    //basic movements
    void forward();
    void backward();

    //sharp 90 degree turns (both wheels pivot)
    void pivotRight();
    void pivotLeft();
};

#endif
