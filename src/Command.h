#ifndef _Command_
#define _Command_

#include <Arduino.h>

class Command {
  public:
    Command();
    virtual void execute(Stream* c, uint8_t paramCount, char** params) = 0;
    virtual const char* getName() = 0;
    virtual const char* getHelp() = 0;
    void printError(Stream* c);

    void addCommand(Command* add);
    void removeCommand();
    Command* next() { return _next; }
    Command* prev() { return _prev; }

  protected:
    Command* _next = nullptr;
    Command* _prev = nullptr;
};

#endif
