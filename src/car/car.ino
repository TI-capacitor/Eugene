#include "ultrasound_function.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int leftPin1  = 7;
const int leftPin2  = 8;
const int rightPin3 = 9;
const int rightPin4 = 10;
const int enA = 6;
const int enB = 11;
const int trigPin   = 2;  // ultrasound trigger
const int echoPin   = 3;  // ultrasound echo

float value = 0;


void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.noAutoscroll();
  lcd.setCursor(5,5);
  lcd.print("^ - ^");
  pinMode(leftPin1,OUTPUT);
  pinMode(leftPin2,OUTPUT);
  pinMode(rightPin3,OUTPUT);
  pinMode(rightPin4,OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);

  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);

  Serial.begin(9600);

  
  analogWrite(enA, 97);
  analogWrite(enB, 97);
}
void loop() {
  value = ultraSound(trigPin, echoPin); //ultrasound function call
  Serial.println(value);
  delay(100);

  if (value < 20.0 && value > 0) {
    digitalWrite(leftPin1,HIGH);
    digitalWrite(leftPin2,LOW);
    digitalWrite(rightPin3,LOW);
    digitalWrite(rightPin4,HIGH);
    delay(400);


    digitalWrite(leftPin1,LOW);
    digitalWrite(leftPin2,HIGH);
    digitalWrite(rightPin3,LOW);
    digitalWrite(rightPin4,LOW);
    delay(799);

    

  } else {
    digitalWrite(leftPin1,LOW);
    digitalWrite(leftPin2,HIGH);  // left backward
    digitalWrite(rightPin3,HIGH);
    digitalWrite(rightPin4,LOW);  // right forward
  }
}

