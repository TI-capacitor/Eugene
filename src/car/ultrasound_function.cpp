#include "ultrasound_function.h"

double ultraSound(int trigPin,int echoPin)
{
  double duration, distanceCM = 0;
  
digitalWrite(trigPin,LOW); //pin set to low to avoid issues at start
  delayMicroseconds(2);
  digitalWrite(trigPin,HIGH); //trigger pin sends soundwave
  delayMicroseconds(10); //delay for soundwave to travel back
  digitalWrite(trigPin,LOW);
  duration = pulseIn(echoPin,HIGH); //Pulse in calculates how long a pin detects a signal, in this case we want it to detect the duration of the echo pin in a 1 state
  distanceCM = (duration * 0.0343)/2; //since time is in microseconds, the factor turns to 0.0343. It is divided by 2 since it considers oneway distance
  return distanceCM;
}
