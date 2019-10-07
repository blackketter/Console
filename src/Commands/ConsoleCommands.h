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
      CommandLine* line = CommandLine::first();
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
      // todo - initialize running state (variables, etc.)?
      if (paramCount == 1) {
        gotoLine(atoi(params[1]));
      } else {
        _nextLine = CommandLine::first();
      }
    }
    void kill() override {
      _nextLine = nullptr;
      _currCommand = nullptr;
    }

    bool isRunning() override {
      return (_nextLine != nullptr);
    }

    void idle(Stream* c) override {
      if (_currCommand && _currCommand->isRunning()) {
        return;
      }
      if (_nextLine) {
        CommandLine* thisLine = _nextLine;
        _nextLine = _nextLine->getNext();
        bool fail = Console::get()->executeCommandLine(c, thisLine->c_str());
        if (fail) {
          kill();
        } else {
          _currCommand = Console::get()->getLastCommand();
        }
      }
    }

    void gotoLine(linenumber_t g) {
      CommandLine* line = CommandLine::getLine(g);
      if (line) {
        _nextLine = line;
      }
    }

  private:
    CommandLine* _nextLine = nullptr;
    Command* _currCommand = nullptr;
};
RunCommand theRunCommand;

////////////////// Goto Command
class GotoCommand : public Command {
  public:
    const char* getName() { return "goto"; }
    const char* getHelp() { return "<line number> - Jump to line"; }
    void execute(Stream* c, uint8_t paramCount, char** params) {
      theRunCommand.execute(c,paramCount,params);
      if (paramCount == 1) {
        theRunCommand.gotoLine(atoi(params[1]));
      } else {
        printError(c);
      }
    }
};
GotoCommand theGotoCommand;

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

////////////////// WaitCommand

class WaitCommand : public Command {
  public:
    const char* getName() { return "wait"; }
    const char* getHelp() { return ("<ms> - wait a specified number of milliseconds"); }
    void execute(Stream* c, uint8_t paramCount, char** params) {
      if (paramCount != 1) {
        printError(c);
        return;
      }
      int delay = atoi(params[1]);
      if (delay <= 0) {
        printError(c);
        return;
      }
      _until = millis() + delay;
    }

    void kill() {
      _until = 0;
    }
    bool isRunning() {
      // check to see if time's up
      if (_until < millis()) {
        _until = 0;
      }

      // we're not there yet
      return (_until != 0);
    }

  private:
    // TODO: use clock library
    // millis_t _until = 0;
    uint32_t _until = 0;
};

WaitCommand theWaitCommand;

////////////////// DebugCommand

class DebugCommand : public Command {
  public:
    const char* getName() { return "debug"; }
    const char* getHelp() { return ("<0|1> - disable/enable debug output"); }
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

////////////////// Rem Command

class ExitCommand : public Command {
  public:
    const char* getName() { return "exit"; }
    const char* getHelp() { return ("Close connection, if possible"); }
    void execute(Stream* c, uint8_t paramCount, char** params) {
      c->println("TODO - close connection");
      //close();
    }
};

ExitCommand theExitCommand;

#endif
