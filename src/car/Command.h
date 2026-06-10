#ifndef COMMAND_H
#define COMMAND_H
#include "Movement.h"


class Command{
  private:
    String inputString = "";
    bool stringComplete = false;

  public:
    //command methods
    void readCommand();
    void parseCommand();
    void listOfCommands();
};

#endif