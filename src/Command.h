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

    virtual void begin() {};
    virtual void idle() {};

    Command* next() { return _next; }
    void setNext(Command* c) { _next = c; }

    static Command* first() { return _first; }
    static void addCommand(Command* c);
    static Command* getByName(const char* n);
    static void sortCommands();

  private:
    static void setFirst(Command* f) { _first = f; }
    static Command* _first;
    Command* _next = nullptr;
};

#endif
