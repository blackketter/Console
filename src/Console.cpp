#include <stdarg.h>
#include <stdio.h>

#include "Console.h"
#include "Command.h"

static Command* _commands = nullptr;

Console::Console() {
  commandLine[0] = 0;
}

void Console::begin() {
  Serial.begin(115200);
}

void Console::loop() {
// loopback (100 chars at a time, max)
  uint8_t i = 100;
  uint8_t skip = 0;
  bool toFlush = false;

  while (i-- && available()) {
    toFlush = true;

    uint8_t c = read();

// ignore telnet sequences for now
    if (c==255 && skip==0) {
      skip = 2;
    }

    if (c < 128 && (!skip)) {
      // valid character
      if (c == 8) {
        // backspace
        if (_commandLineLength) {
          _commandLineLength--;
          commandLine[_commandLineLength] = 0;
          write(8); write(' '); write(8);
        }
        // get rid of prompt
        if (_commandLineLength == 0) {
          write(8); write(' '); write(8);
        }
      } else if (c == '\r') {
        // new line
        write(c);
        write('\n');
        executeCommandLine();
        _commandLineLength = 0;
        commandLine[0] = 0;
      } else if (c == '\n') {
        // ignore
      } else if (c == 0) {
        // ignore
      }  else {
        // ignore some characters (tab, extra chars past the max line length)
        if (c != '\t' || _commandLineLength < _maxCommandLineLength) {
          // show a prompt
          if (_commandLineLength == 0) {
            write('>');
          }
          commandLine[_commandLineLength] = c;
          _commandLineLength++;
          commandLine[_commandLineLength] = 0;
          write(c);
        }
     }
    }

    if (skip && c !=255) {
      skip--;
    }
  }

  if (toFlush) {
    flush();
  }
};

inline bool isSpace(char c) {
  return c == ' ' || c == '\t' || c == '\r';
}
inline bool isEnd(char c) {
  return c == 0;
}
inline bool isBetween(char c) {
  return isSpace(c) || isEnd(c);
}

void Console::executeCommandLine() {
  const int maxParams = 5;
  char* params[maxParams];
  int paramCount = 0;
  int commandLineIndex = 0;
  char* commandStart = commandLine;

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

  Command* currCommand = _commands;

  if (paramCount) {
    while (currCommand != nullptr) {
      if (strcmp(currCommand->getName(), params[0]) == 0) {
        // paramcount is the number of parameters after the name
        currCommand->execute(this, paramCount-1, params);
        break;
      }
      currCommand = currCommand->next();
    }
    if (currCommand == nullptr) {
      printf("Unknown Command: %s\n", params[0]);
      strcpy(commandLine, "help");
      executeCommandLine();
    }
  }
}

void Console::addCommand(Command* command) {
  if (_commands) {
    _commands->addCommand(command);
  } else {
    _commands = command;
  }

};

void Console::removeCommand(Command* command) {
  if (command == _commands) {
    _commands = command->next();
  } else {
    command->removeCommand();
  }
};

int Console::available() {
  return Serial.available();
};


int Console::read() {
  return Serial.read();
};


int Console::peek() {
  return Serial.peek();
};


void Console::flush() {
  Serial.flush();
};


size_t Console::write(uint8_t b) {
  return Serial.write(b);
};

size_t Console::write(const uint8_t *buf, size_t size) {
  return Serial.write(buf,size);
}

#define PREFIX_LEN (15)
void Console::debugPrefix(char* s) {
  int t = ::millis();
  sprintf(s, "%8d.%03d: ",t/1000,t%1000);
}

void Console::debugf(const char* format, ...) {
  char prefix[PREFIX_LEN];
  debugPrefix(prefix);
  if (printDebug()) {
    print(prefix);
  }
  appendLog(prefix);

// this is lifted from ESP8266 Print.cpp, as we don't have vdprintf() on that platform
  va_list arg;
  va_start(arg, format);
  char temp[64];
  char* buffer = temp;
  size_t len = vsnprintf(temp, sizeof(temp), format, arg);
  va_end(arg);
  if (len > sizeof(temp) - 1) {
      buffer = new char[len + 1];
      if (!buffer) {
          return;
      }
      va_start(arg, format);
      vsnprintf(buffer, len + 1, format, arg);
      va_end(arg);
  }
  appendLog(buffer);
  if (printDebug()) {
    len = write((const uint8_t*) buffer, len);
  }
  if (buffer != temp) {
      delete[] buffer;
  }

/*  va_list argptr;
  va_start(argptr, format);
  vdprintf((int)this,  format, argptr);
  va_end(argptr);
*/
};

void Console::debug(const char* s) {
  char prefix[PREFIX_LEN];
  debugPrefix(prefix);
  appendLog(prefix);
  appendLog(s);

  if (!printDebug()) return;
  print(prefix);
  print(s);
}
void Console::debugln(const char* s) {
  debug(s);
  appendLog("\n");
  if (printDebug()) { write('\n'); }
}

void Console::appendLog(const char *s) {
  appendLog(s, strlen(s));
}

void Console::appendLog(const char *s, size_t len) {
  const char* curChar = s;
  size_t remaining = len;
  while (remaining != 0) {
    _debugLog[_debugLogEnd]  = *curChar;

    _debugLogEnd++;

    if (_debugLogEnd >= _debugLogSize) {
      _debugLogEnd = 0;
    }

    if (_debugLogStart == _debugLogEnd) {
      _debugLogStart++;
    }

    if (_debugLogStart >= _debugLogSize) {
      _debugLogStart = 0;
    }

    curChar++;
    remaining--;
  }
}
void Console::printLog() {
  printLog(*this);
}
void Console::printLog(Print& p) {
  if (_debugLogStart > _debugLogEnd) {
    p.write((uint8_t*)_debugLog + _debugLogStart, _debugLogSize - _debugLogStart);
    p.write((uint8_t*)_debugLog, _debugLogEnd);
  } else if (_debugLogStart < _debugLogEnd) {
    p.write((uint8_t*)_debugLog + _debugLogStart, _debugLogEnd - _debugLogStart);
  } else {
    return;
  }
};

// Some built-in commands
////////////////// HelloCommand

class HelloCommand : public Command {
  public:
    const char* getName() { return "hello"; }
    const char* getHelp() { return "Greets you"; }
    void execute(Stream* c, uint8_t paramCount, char** params) { c->print("Hello world!\n"); }
};

HelloCommand theHelloCommand;

////////////////// Log Command

class LogCommand : public Command {
  public:
    const char* getName() { return "log"; }
    const char* getHelp() { return ("Print out debug log"); }
    void execute(Stream* c, uint8_t paramCount, char** params) {
      Console* console = ((Console*)c);
      console->printLog();
    }
};

LogCommand theLogCommand;

////////////////// DebugCommand

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

////////////////// HelpCommand

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
