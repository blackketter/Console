#ifndef _Shell_
#define _Shell_
#include "Command.h"
#include "Readline.h"
#include "CommandLine.h"
class Console;

class Shell : public Command {
  public:
    Shell();
    virtual ~Shell() = default;
    const char* getName() override { return "shell"; }
    const char* getHelp() override { return "Console shell"; }

    bool isRunning() override;
    bool isReading() override;
    void kill() override;

    void execute(Console* c, uint8_t paramCount, char** params) override;
    void idle(Console* c) override;

    bool executeCommandLine(Console* output, const char* line);
    Command* getLastCommand() { return _lastCommand; }


    CommandLine* firstLine() { return _firstLine; }
    void addLine(CommandLine* newLine);
    CommandLine* getLine(linenumber_t n);
    CommandLine* removeLine(linenumber_t n);
    void clearLines();


  private:
    Command* _lastCommand = nullptr;

    Readline _readline;

    CommandLine* _firstLine = nullptr;


};

#endif
