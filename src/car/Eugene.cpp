#include "Eugene.h"


//reads command and builds string, saves it on private variable
void Command::setCommand() {
  while (Serial.available()) 
  {
    char c = Serial.read();

    if (c != '\n') inputString += c;

    else stringComplete = true;
    }
}

//parsing of commands, includes typecasting to lowercase
void Command::parseCommand() {
  if (!stringComplete) return;
  inputString.trim();
  inputString.toLowerCase();
  Serial.print("Received: ");
  Serial.println(inputString);

  if(inputString == "front") { 
  Serial.println("MOVING FORWARD...");
   }
  
  else if(inputString == "back") {
    Serial.println("MOVING BACKWARD..."); 
    }
  
  else if(inputString == "stop") {
    Serial.println("MOVING BACKWARD..."); 
    }

  else if(inputString == "pivot.right") {
    Serial.println("PIVOT RIGHT"); 
    }

  else if(inputString == "pivot.left") {
    Serial.println("PIVOT LEFT"); 
    }


  else {
    Serial.println("Unknown command");
    listOfCommands();
  }
  

  // Reset buffer
  inputString = "";
  stringComplete = false;
}

//list of possible commands
void Command::listOfCommands(){
  Serial.println(" _________________________________________");
  Serial.println("|        LIST OF POSSIBLE COMMANDS        |");
  Serial.println("|--------------- Movements ---------------|");
  Serial.println("| front - Moves the rover forward         |\n| back - Moves the rover backward         |");
  Serial.println("| stop - Halts movement                   |\n| pivot.left - Pivots rover to the left   |\n| pivor.right - Pivots to the right       |");
  Serial.println(" ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾");
  //list commands here

}
/*-----------------------------------------------------------------------------*/

//movement class constructor
Movement::Movement(int In1,int In2,int In3,int In4,int EnA,int EnB) {}

//method to initialize pins in one instance
void Movement::begin(){
  pinMode(In1,OUTPUT);
  pinMode(In2,OUTPUT);
  pinMode(In3,OUTPUT);
  pinMode(In4,OUTPUT);
  pinMode(EnA,OUTPUT);
  pinMode(EnB,OUTPUT);

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
  digitalWrite(In1,HIGH);
  digitalWrite(In2,LOW);
  digitalWrite(In3,LOW);
  digitalWrite(In4,HIGH);
  analogWrite(EnA,255);
  analogWrite(EnB,255);
}

//moves in reverse
void Movement::backward() {
  digitalWrite(In1,LOW);
  digitalWrite(In2,HIGH);
  digitalWrite(In3,HIGH);
  digitalWrite(In4,LOW);
  analogWrite(EnA,255);
  analogWrite(EnB,255);
}

//pivots to the right
void Movement::pivotRight() {
  digitalWrite(In1,HIGH);
  digitalWrite(In2,LOW);
  digitalWrite(In3,HIGH);
  digitalWrite(In4,LOW);
  analogWrite(EnA,255);
  analogWrite(EnB,255);
}

//pivots left
void Movement::pivotLeft() {
  digitalWrite(In1,LOW);
  digitalWrite(In2,HIGH);
  digitalWrite(In3,LOW);
  digitalWrite(In4,HIGH);
  analogWrite(EnA,255);
  analogWrite(EnB,255);
}
  
