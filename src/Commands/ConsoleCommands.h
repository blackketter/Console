#ifndef _ConsoleCommands_
#define _ConsoleCommands_

// Some built-in commands for the console

////////////////// HelloCommand

class HelloCommand : public Command {
  public:
    const char* getName() { return "hello"; }
    const char* getHelp() { return "Greets you"; }
    void execute(Stream* c, uint8_t paramCount, char** params) { c->print("Hello world!\n"); }
};

HelloCommand theHelloCommand;

////////////////// List Command
class ListCommand : public Command {
  public:
    const char* getName() { return "list"; }
    const char* getHelp() { return "Prints out listing of current program"; }
    void execute(Stream* c, uint8_t paramCount, char** params) {
      CommandLine* line = Console::get()->getLines();
      while (line) {
        if (line->getNumber()) {
          c->printf("%5d %s\n", line->getNumber(), line->c_str());
        }
        line = line->getNext();
      }
    }
};
ListCommand theListCommand;

////////////////// Run Command
class RunCommand : public Command {
  public:
    const char* getName() { return "run"; }
    const char* getHelp() { return "Runs current program"; }
    void execute(Stream* c, uint8_t paramCount, char** params) {
      CommandLine* line = Console::get()->getLines();
      while (line) {
        bool fail = Console::get()->executeCommandLine(c, line->c_str());
        if (fail) {
          break;
        }
        line = line->getNext();
      }
    }
};
RunCommand theRunCommand;

////////////////// Log Command

class LogCommand : public Command {
  public:
    const char* getName() { return "log"; }
    const char* getHelp() { return ("Print out debug log"); }
    void execute(Stream* c, uint8_t paramCount, char** params) {
      Console::get()->printLog(c);
    }
};

LogCommand theLogCommand;

////////////////// Rem Command

class RemCommand : public Command {
  public:
    const char* getName() { return "rem"; }
    const char* getHelp() { return ("Just a remark"); }
    void execute(Stream* c, uint8_t paramCount, char** params) {
    }
};

RemCommand theRemCommand;


////////////////// DebugCommand

class DebugCommand : public Command {
  public:
    const char* getName() { return "debug"; }
    const char* getHelp() { return ("0|1 - disable/enable debug output"); }
    void execute(Stream* c, uint8_t paramCount, char** params) {
      if (paramCount == 1) {
        bool debugState = atoi(params[1]);
        Console::get()->debugEnable(debugState);
      } else if (paramCount == 0) {
        Console::get()->debugEnable(!Console::get()->debugEnabled());
      }
      c->printf("Debug: %s\n", Console::get()->debugEnabled() ? "on" : "off");
    }
};

DebugCommand theDebugCommand;

////////////////// HelpCommand

class HelpCommand : public Command {
  public:
    const char* getName() { return "help"; }
    const char* getHelp() { return " <command> - Prints out this help"; }
    void execute(Stream* c, uint8_t paramCount, char** params);
};

void HelpCommand::execute(Stream* c, uint8_t paramCount, char** params) {
  Command* h = nullptr;
  if (paramCount) {
    h = Command::getByName(params[1]);
    if (h) {
      h->printError(c);
    }
  }
  if (h == nullptr) {
    c->print("Known Commands:\n");

    Command* currCommand = first();
    int commandLen = 0;
    while (currCommand) {
      int currLen = strlen(currCommand->getName());
      if (currLen > commandLen) { commandLen = currLen; }
      currCommand = currCommand->next();
    }

    currCommand = first();
    while (currCommand) {
      c->print("  ");
      c->print(currCommand->getName());
      for (int i = strlen(currCommand->getName()); i < commandLen + 3; i++) { c->write(' '); }
      c->println(currCommand->getHelp());
      currCommand = currCommand->next();
    }
  }
}

HelpCommand theHelpCommand;

#endif
