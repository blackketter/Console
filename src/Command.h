#ifndef _Command_
#define _Command_

#include <Arduino.h>

class Command {
  public:
    Command();
    virtual void execute(Stream* c, uint8_t paramCount, char** params) = 0;
    virtual const char* getName() = 0;
    virtual const char* getHelp() = 0;

    virtual void begin() {};
    virtual void idle(Stream* c) {};

    // commands can continue running after execute
    virtual bool isRunning() { return false; };
    virtual void kill() {};

    // commands can consume input
    virtual bool isReading() { return false; };

    // command list management
    Command* next() { return _next; }
    static Command* first() { return _first; }
    static void addCommand(Command* c);
    static Command* getByName(const char* n);
    static void sortCommands();

    // utility methods
    void printError(Stream* c);
    static void killAll();

  private:
    Command* _next = nullptr;

    void setNext(Command* c) { _next = c; }
    static void setFirst(Command* f) { _first = f; }
    static Command* _first;
};

#endif
