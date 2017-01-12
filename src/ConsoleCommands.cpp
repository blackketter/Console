#include "Console.h"
#include "Command.h"

// Some built-in commands

class HelloCommand : public Command {
  public:
    const char* getName() { return "hello"; }
    const char* getHelp() { return "Greets you"; }
    void execute(Stream* c, uint8_t paramCount, char** params) { c->print("Hello world!\n"); }
};

HelloCommand theHelloCommand;


class DebugCommand : public Command {
  public:
    const char* getName() { return "debug"; }
    const char* getHelp() { return ("0|1 - disable/enable debug output"); }
    void execute(Stream* c, uint8_t paramCount, char** params) {
      Console* console = ((Console*)c);
      if (paramCount == 1) {
        bool debugState = atoi(params[1]);
        console->debugEnable(debugState);
      } else if (paramCount == 0) {
        console->debugEnable(!console->debugEnabled());
      }
      c->printf("Debug: %s\n", console->debugEnabled() ? "on" : "off");
    }
};

DebugCommand theDebugCommand;

class HelpCommand : public Command {
  public:
    const char* getName() { return "help"; }
    const char* getHelp() { return "Prints out this help information"; }
    void execute(Stream* c, uint8_t paramCount, char** params);
};

void HelpCommand::execute(Stream* c, uint8_t paramCount, char** params) {
  Command* firstCommand = this;
  while (firstCommand->prev()) {
    firstCommand = firstCommand->prev();
  }

  Command* currCommand = firstCommand;
  int commandLen = 0;
  while (currCommand) {
    int currLen = strlen(currCommand->getName());
    if (currLen > commandLen) { commandLen = currLen; }
    currCommand = currCommand->next();
  }

  currCommand = firstCommand;
  while (currCommand) {
    c->print("  ");
    c->print(currCommand->getName());
    for (int i = strlen(currCommand->getName()); i < commandLen + 3; i++) { c->write(' '); }
    c->print(currCommand->getHelp());
    c->write('\n');
    currCommand = currCommand->next();
  }
}

HelpCommand theHelpCommand;

