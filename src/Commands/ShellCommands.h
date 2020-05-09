#ifndef _ShellCommands_
#define _ShellCommands_
#include "Console.h"
#include "Clock.h"
// Some built-in commands for the shell


// TODO: Move state from commands to shell so that multiple shells can run

class ShellCommand : public Command {
};

////////////////// HelloCommand

class HelloCommand : public ShellCommand {
  public:
    const char* getName() { return "hello"; }
    const char* getHelp() { return "Greets you"; }
    void execute(Console* c, uint8_t paramCount, char** params) { c->print("Hello Console!\n"); }
};

HelloCommand theHelloCommand;

////////////////// List Command
class ListCommand : public ShellCommand {
  public:
    const char* getName() { return "list"; }
    const char* getHelp() { return "Prints out listing of current program"; }
    void execute(Console* c, uint8_t paramCount, char** params) {
      CommandLine* line = c->getShell()->firstLine();
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
class RunCommand : public ShellCommand {
  public:
    const char* getName() { return "run"; }
    const char* getHelp() { return "Runs current program"; }
    void execute(Console* c, uint8_t paramCount, char** params) {
      if (paramCount == 1) {
        gotoLine(c, atoi(params[1]));
      } else {
        _nextLine = c->getShell()->firstLine();
      }
    }
    void kill() override {
      _nextLine = nullptr;
      _currCommand = nullptr;
    }

    bool isRunning() override {
      return (_nextLine != nullptr);
    }

    void idle(Console* c) override {
      if (_currCommand && _currCommand->isRunning()) {
        return;
      }
      if (_nextLine) {
        CommandLine* thisLine = _nextLine;
        _nextLine = _nextLine->getNext();
        bool fail = c->getShell()->executeCommandLine(c, thisLine->c_str());
        if (fail) {
          kill();
        } else {
          _currCommand = c->getShell()->getLastCommand();
        }
      }
    }

    void gotoLine(Console* c, linenumber_t g) {
      CommandLine* line = c->getShell()->getLine(g);
      _nextLine = line;
    }

    linenumber_t nextLineNumber() {
      if (_nextLine) {
        return _nextLine->getNumber();
      } else {
        return NO_LINENUMBER;
      }
    }

  private:
    CommandLine* _nextLine = nullptr;
    Command* _currCommand = nullptr;
};
RunCommand theRunCommand;

////////////////// End Command
class EndCommand : public ShellCommand {
  public:
    const char* getName() { return "end"; }
    const char* getHelp() { return "Ends program"; }
    void execute(Console* c, uint8_t paramCount, char** params) {
        theRunCommand.gotoLine(c, NO_LINENUMBER);
    }
};
EndCommand theEndCommand;

////////////////// Goto Command
class GotoCommand : public Command {
  public:
    const char* getName() { return "goto"; }
    const char* getHelp() { return "<line number> - Jump to line"; }
    void execute(Console* c, uint8_t paramCount, char** params) {
      theRunCommand.execute(c,paramCount,params);
      if (paramCount == 1) {
        theRunCommand.gotoLine(c, atoi(params[1]));
      } else {
        printError(c);
      }
    }
};
GotoCommand theGotoCommand;

////////////////// Gosub Command
class GosubCommand : public ShellCommand {
  public:
    const char* getName() { return "gosub"; }
    const char* getHelp() { return "<line number> - Go to subroutine"; }
    void execute(Console* c, uint8_t paramCount, char** params) {
      if (paramCount == 1) {
        linenumber_t returnLineNumber = theRunCommand.nextLineNumber();
        linenumber_t subLineNumber = atoi(params[1]);

        if (subLineNumber != NO_LINENUMBER) {
          if (_currDepth >= MAX_GOSUB_DEPTH) {
            subLineNumber = NO_LINENUMBER;
          } else {
            _return[_currDepth] = returnLineNumber;
            _currDepth++;
          }
        }
        theRunCommand.gotoLine(c, subLineNumber);
      } else {
        printError(c);
      }
    }

    void returnSub(Console* c) {
      linenumber_t returnLineNumber = NO_LINENUMBER;
      if (_currDepth) {
        _currDepth--;
        returnLineNumber = _return[_currDepth];
      }
      theRunCommand.gotoLine(c, returnLineNumber);
    }
  private:
    static const uint8_t MAX_GOSUB_DEPTH = 5;
    linenumber_t _return[MAX_GOSUB_DEPTH];
    uint8_t _currDepth = 0;
};
GosubCommand theGosubCommand;

////////////////// Return Command
class ReturnCommand : public ShellCommand {
  public:
    const char* getName() { return "return"; }
    const char* getHelp() { return ("Return from subroutine"); }
    void execute(Console* c, uint8_t paramCount, char** params) {
      theGosubCommand.returnSub(c);
    }
};
ReturnCommand theReturnCommand;

////////////////// New Command
class NewCommand : public ShellCommand {
  public:
    const char* getName() { return "new"; }
    const char* getHelp() { return ("Erase the program"); }
    void execute(Console* c, uint8_t paramCount, char** params) {
      // todo - initialize running state (variables, etc.)?
      c->getShell()->clearLines();
    }
};
NewCommand theNewCommand;

////////////////// Print Command
class PrintCommand : public ShellCommand {
  public:
    const char* getName() { return "print"; }
    const char* getHelp() { return ("<...> - Print data"); }
    void execute(Console* c, uint8_t paramCount, char** params) {

      for (uint8_t i = 1; i <= paramCount; i++ ) {
        if (i!=1) {
          c->write(' ');
        }
        c->print(params[i]);
      }
      c->write('\n');
    }
};
PrintCommand thePrintCommand;

////////////////// Log Command

class LogCommand : public ShellCommand {
  public:
    const char* getName() { return "log"; }
    const char* getHelp() { return ("Print out debug log"); }
    void execute(Console* c, uint8_t paramCount, char** params) {
      c->printLog(c);
    }
};

LogCommand theLogCommand;

////////////////// Rem Command

class RemCommand : public ShellCommand {
  public:
    const char* getName() { return "rem"; }
    const char* getHelp() { return ("Just a remark"); }
    void execute(Console* c, uint8_t paramCount, char** params) {
    }
};

RemCommand theRemCommand;

////////////////// WaitCommand

class WaitCommand : public ShellCommand {
  public:
    const char* getName() { return "wait"; }
    const char* getHelp() { return ("<ms> - wait a specified number of milliseconds"); }
    void execute(Console* c, uint8_t paramCount, char** params) {
      if (paramCount != 1) {
        printError(c);
        return;
      }
      int delay = atoi(params[1]);
      if (delay <= 0) {
        printError(c);
        return;
      }
      _until = Uptime::millis() + delay;
    }

    void kill() {
      _until = 0;
    }
    bool isRunning() {
      // check to see if time's up
      if (_until < Uptime::millis()) {
        _until = 0;
      }

      // we're not there yet
      return (_until != 0);
    }

  private:
    millis_t _until = 0;
};

WaitCommand theWaitCommand;

////////////////// DebugCommand

class DebugCommand : public ShellCommand {
  public:
    const char* getName() { return "debug"; }
    const char* getHelp() { return ("<0|1> - disable/enable debug output"); }
    void execute(Console* c, uint8_t paramCount, char** params) {
      if (paramCount == 1) {
        bool debugState = atoi(params[1]);
        c->debugEnable(debugState);
      } else if (paramCount == 0) {
        c->debugEnable(!c->debugEnabled());
      }
      c->printf("Debug: %s\n", c->debugEnabled() ? "on" : "off");
    }
};

DebugCommand theDebugCommand;

////////////////// Prompt Command

class PromptCommand : public ShellCommand {
  public:
    const char* getName() { return "prompt"; }
    const char* getHelp() { return ("<0|1> - disable/enable command prompt"); }
    void execute(Console* c, uint8_t paramCount, char** params) {
      if (paramCount == 1) {
        setEnabled(atoi(params[1]));
      } else if (paramCount == 0) {
        setEnabled(!enabled());
      }
      c->printf("Prompt: %s\n", enabled() ? "on" : "off");
    }
    bool enabled() { return _enabled; }
    void setEnabled( bool e ) { _enabled = e; }
    const char*  getPrompt() { return _enabled ? ">" : ""; }
  private:
    bool _enabled = true;

};

PromptCommand thePromptCommand;

////////////////// HelpCommand

class HelpCommand : public ShellCommand {
  public:
    const char* getName() { return "help"; }
    const char* getHelp() { return "<command> - Prints out this help"; }
    void execute(Console* c, uint8_t paramCount, char** params);
};

void HelpCommand::execute(Console* c, uint8_t paramCount, char** params) {
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
      const char* help = currCommand->getHelp();
      if (help) {
        c->print("  ");
        c->print(currCommand->getName());
        for (int i = strlen(currCommand->getName()); i < commandLen + 3; i++) { c->write(' '); }
        c->println(help);
      }
      currCommand = currCommand->next();
    }
  }
}

HelpCommand theHelpCommand;

class HelpQCommand : public HelpCommand {
  public:
    const char* getName() { return "?"; }
    const char* getHelp() { return nullptr; }
};

HelpQCommand theHelpQCommand;

#endif
