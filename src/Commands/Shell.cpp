#include "Shell.h"
#include "NullStream.h"
#include "CommandLine.h"
#include "Commands/ShellCommands.h"

Shell::Shell() {
  _commandLine[0] = 0;
}

void Shell::idle(Console* c) {
  // bail before command line parsing if running command is reading input
  if (_lastCommand && _lastCommand->isRunning() && _lastCommand->isReading()) {
    return;
  }
  
  String commandLine;
  String prompt = thePromptCommand.getPrompt();
  if (_readline.readline(c, prompt, commandLine)) {
    // control-c
    if (commandLine == "\x03") { 
        c->println("Ctrl-C: Stopping");
        Command::killAll();    
    } else {
      bool fail = executeCommandLine(c, commandLine.c_str());
      if (fail) {
        executeCommandLine(c, "help");
      }
      _readline.add_history(commandLine);
    }
  }
}

bool Shell::isReading() { 
  return isRunning(); }

bool Shell::isRunning() {
  return true;
}

void Shell::execute(Console* c, uint8_t paramCount, char** params) {
}

void Shell::kill() {
}

inline bool isSpace(char c) {
  return c == ' ' || c == '\t' || c == '\r';
}
inline bool isEnd(char c) {
  return c == 0;
}
inline bool isBetween(char c) {
  return isSpace(c) || isEnd(c);
}

inline bool isLineNum(char c) {
  return c >= '0' && c <= '9';
}

bool Shell::executeCommandLine(Console* output, const char* line) {
  const int maxParams = 5;
  char* params[maxParams];
  char currLine[_maxCommandLineLength];
  int paramCount = 0;
  int commandLineIndex = 0;

  if ((line == nullptr) || (isEnd(line[0]))) {
    // no command, it succeeds
    return false;
  }

  char* commandStart = currLine;
  strcpy(currLine, line);

  while (isSpace(commandStart[commandLineIndex])) {
    commandLineIndex++;
  }

  if (isLineNum(commandStart[commandLineIndex])) {
    linenumber_t n = atoi(&commandStart[commandLineIndex]);
    if (n) {
      while (isLineNum(commandStart[commandLineIndex])) {
        commandLineIndex++;
      }
      while (isSpace(commandStart[commandLineIndex])) {
        commandLineIndex++;
      }
      if (n) {
        if (strlen(&commandStart[commandLineIndex])) {
          CommandLine* commandLine = new CommandLine(n, &commandStart[commandLineIndex]);
          if (commandLine) {
            CommandLine::addLine(commandLine);  // we always have a first line, a REMark
          }
        } else {
          CommandLine* del = CommandLine::removeLine(n);
          if (del) { delete del; }
        }
        // returns success
        return false; // added the line to the current program
      }
    }
  }

  // parse out the command and parameters
  while (1) {
//    printf("(first:%d)", commandStart[commandLineIndex]);
    while (isSpace(commandStart[commandLineIndex])) {
      commandLineIndex++;
    }

    if (isEnd(commandStart[commandLineIndex])) {
      break;
    }

    commandStart += commandLineIndex;
    commandLineIndex = 0;
    params[paramCount] = commandStart;

    while (!isBetween(commandStart[commandLineIndex])) {
      commandLineIndex++;
    }

    paramCount++;

    if (paramCount >= maxParams) {
      break;
    }
    if (isEnd(commandStart[commandLineIndex])) {
      break;
    }

    commandStart[commandLineIndex] = 0;
    commandStart += commandLineIndex + 1;
    commandLineIndex = 0;
  };

  bool failure = false;
  if (paramCount) {
    Command* c = Command::getByName(params[0]);
    if (c) {
      c->execute(output, paramCount-1, params);
      _lastCommand = c;
    } else {
      failure = true;
      _lastCommand = nullptr;
      output->printf("Unknown Command: %s\n", params[0]);
    }
  }
  return failure;
}
