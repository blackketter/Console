#ifndef _Shell_
#define _Shell_
#include "Command.h"

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
    
    void prompt(Stream* c);
    
  private:
    Command* _lastCommand = nullptr;
    static const uint32_t _maxCommandLineLength = 100;  // todo: fixme
    char _commandLine[_maxCommandLineLength];
    uint32_t _commandLineLength = 0;

};

#endif
