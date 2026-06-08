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

    //
    void begin();
    void stopMotors();
    void forward();
    void backward();
    void pivotRight();
    void pivotLeft();
    void curveRight();
    void curveLeft();
};

#endif
