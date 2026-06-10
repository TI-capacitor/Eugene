#include "Command.h"

void Command::readCommand() {
  while (Serial.available()) 
  {
    char c = Serial.read();

    if (c != '\n') inputString += c;

    else stringComplete = true;
    }
}

void Command::parseCommand() {
  if (!stringComplete) return;
  inputString.trim();
  Serial.print("Received: ");
  Serial.println(inputString);

  if(inputString == "front") { 
  Serial.println("MOVING FORWARD...");
   }
  
  else if(inputString == "back") {
    Serial.println("MOVING BACKWARD..."); 
     }
  
  else if(inputString == "pivot.right") {
    Serial.println("PIVOT RIGHT"); 
    }

  else if(inputString == "pivot.left") {
    Serial.println("PIVOT LEFT"); 
    }


  else  {
    Serial.println("Unknown command");
    listOfCommands();
  }
  

  // Reset buffer
  inputString = "";
  stringComplete = false;
}

void Command::listOfCommands(){
  Serial.println(" _________________________________________");
  Serial.println("|        LIST OF POSSIBLE COMMANDS        |");
  Serial.println("|--------------- Movements ---------------|");
  Serial.println("| front - Moves the rover forward         |\n| back - Moves the rover backward         |");
  Serial.println("| stop - Halts movement                   |\n| pivot.left - Pivots rover to the left   |\n| pivor.right - Pivots to the right       |");
  Serial.println(" ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾");
  //list commands here

}