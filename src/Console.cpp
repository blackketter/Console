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

  while (i-- && available()) {

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
  flush();
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

void Console::debugf(const char* format, ...) {
  if (!debugEnabled()) return;

  char foo[100];
  size_t l = debugPrefix(foo);
  va_list argptr;
  va_start(argptr, format);
  vsnprintf(foo+l, sizeof(foo)-l, format, argptr);
  va_end(argptr);
  print(foo);
};

void Console::debug(const char* s) {
  if (!debugEnabled()) return;
  char pre[100];
  debugPrefix(pre);
  printf("%s%s",pre,s);
}
void Console::debugln(const char* s) {
  if (!debugEnabled()) return;
  debug(s);
  print("\n");
}

size_t Console::debugPrefix(char* s) {
  int t = ::millis();
  sprintf(s, "%8d.%03d: ",t/1000,t%1000);
  return strlen(s);
}

bool Console::debugEnabled() {
  return _debugEnabled && (_commandLineLength == 0);
}

