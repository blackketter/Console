#ifndef _Command_
#define _Command_

#include <Arduino.h>

class Command {
  public:
    virtual void execute(Stream* c, uint8_t paramCount, char** params) = 0;
    const char* getName() { return _name; }
    const char* getHelp() { return _help; }
    void setName(const char* name) { _name = name; }
    void setHelp(const char* help) { _help = help; }
    void printError(Stream* c);

    void addCommand(Command* add);
    void removeCommand();
    Command* next() { return _next; }
    Command* prev() { return _prev; }

  protected:
    const char* _name;
    const char* _help;
    Command* _next = nullptr;
    Command* _prev = nullptr;
};

#endif
