#include <stdarg.h>
#include <stdio.h>

#include "Console.h"
#include "Command.h"
#include "CommandLine.h"
#include "NullStream.h"

#include "Commands/Commands.h"

static Command* _commands = nullptr;

Console* Console::_theConsole;

Console::Console() {
  _commandLine[0] = 0;
  _theConsole = this;
  _lines = new CommandLine(0, "REM");
}

void Console::begin() {
  // if a port has not been specified, then default to the main serial port
  if (_port == nullptr) {
    _port = &Serial;
    Serial.begin(115200);
  }
  // this assumes all commands are added by their global static instance constructors
  sortCommands();
}

void Console::idle() {
// loopback (100 chars at a time, max)
  uint8_t i = 100;
  bool toFlush = false;

  while (i-- && available()) {
    toFlush = true;

    uint8_t c = read();

    if (c < 128) {
      // valid character
      if (c == 8 || c == 127) {
        // backspace
        if (_commandLineLength) {
          _commandLineLength--;
          _commandLine[_commandLineLength] = 0;
          write(8); write(' '); write(8);
        }
        // get rid of prompt
        if (_commandLineLength == 0) {
          write(8); write(' '); write(8);
        }
      } else if (c == '\r') {
        if (_commandLineLength == 0) {
          write('>');
        }
        // new line
        write(c);
        write('\n');
        _commandLineLength = 0;
        bool fail = executeCommandLine(_commandLine);
        _commandLine[0] = 0;
        if (fail) {
          executeCommandLine("help");
        }
      } else if (c == '\n') {
        // ignore
      } else if (c == 0) {
        // ignore
      } else if (c == 3) {
        // control-c
        if (_commandLineLength == 0) {
          write('>');
        }
        write('\n');
        _commandLine[0] = 0;
        close();
      }  else {
        // ignore some characters (tab, extra chars past the max line length)
        if (c != '\t' || _commandLineLength < _maxCommandLineLength) {
          // show a prompt
          if (_commandLineLength == 0) {
            write('>');
          }
          _commandLine[_commandLineLength] = c;
          _commandLineLength++;
          _commandLine[_commandLineLength] = 0;
          write(c);
        }
     }
    }
  }

  if (toFlush) {
    flush();
  }

  // idle commands
  Command* idler = _commands;
  while (idler) {
    idler->idle();
    idler = idler->next();
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

inline bool isLineNum(char c) {
  return c >= '0' && c <= '9';
}

bool Console::executeCommandLine(const char* line) {
  return executeCommandLine(this, line);
}

bool Console::executeCommandLine(Stream* output, const char* line) {
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

  NullStream nullStream;
  if (output == nullptr) {
    output = &nullStream;
  }
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
            _lines->addLine(commandLine);  // we always have a first line, a REMark
          }
        } else {
          CommandLine* del = _lines->removeLine(n);
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

  Command* currCommand = _commands;

  bool failure = false;
  if (paramCount) {
    while (currCommand != nullptr) {
      if (strcasecmp(currCommand->getName(), params[0]) == 0) {
        // paramcount is the number of parameters after the name
        currCommand->execute(output, paramCount-1, params);
        break;
      }
      currCommand = currCommand->next();
    }
    if (currCommand == nullptr) {
      failure = true;
      output->printf("Unknown Command: %s\n", params[0]);

    }
  }
  return failure;
}

void Console::addCommand(Command* command) {
  if (_commands) {
    _commands->setPrev(command);
    command->setNext(_commands);
  }
  _commands = command;
}

void Console::sortCommands() {
  Command* toSort = _commands->next();
  _commands->setNext(nullptr);

  toSort->setPrev(nullptr);

  while (toSort) {
    Command* sorted = _commands;
    Command* next = toSort->next();
    do {
      // time to insert?
      if (strcasecmp(sorted->getName(), toSort->getName()) > 0) {
        toSort->setPrev(sorted->prev());
        toSort->setNext(sorted);

        Command* prev = sorted->prev();
        sorted->setPrev(toSort);

        // first in list
        if (sorted == _commands) {
          _commands = toSort;
        } else {
          prev->setNext(toSort);
        }
        break;
      }
      // last in list
      if (sorted->next() == nullptr) {
        sorted->setNext(toSort);
        toSort->setPrev(sorted);
        toSort->setNext(nullptr);
        break;
      } else {
        sorted = sorted->next();
      }
    } while (sorted);

    toSort = next;
  }
};

int Console::available() {
  if (_port) {
    return _port->available();
  } else {
    return 0;
  }
};


int Console::read() {
  if (_port) {
    return _port->read();
  } else {
    return 0;
  }
};


int Console::peek() {
  if (_port) {
    return _port->peek();
  } else {
    return 0;
  }
};


void Console::flush() {
  if (_port) {
    _port->flush();
  }
};


size_t Console::write(uint8_t b) {
  size_t wrote = 1;
  if (_port) {
    wrote = _port->write(b);
  }
  return wrote;
};

size_t Console::write(const uint8_t *buf, size_t size) {
  size_t wrote = size;
  if (_port) {
    wrote = _port->write(buf,size);
  }
  return wrote;
}

#define PREFIX_LEN (15)
void Console::debugPrefix(char* s) {
  uint32_t t = ::millis();
  sprintf(s, "%6d.%03d: ",(int)(t/1000),(int)(t%1000));
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

void Console::appendLog(const char* s) {
  appendLog(s, strlen(s));
}

void Console::appendLog(const char*s, size_t len) {
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
  printLog(this);
}

void Console::printLog(Print* p) {
  char* start;
  size_t len;

  // TODO BUG: ESP32 and ESP8266 fail on the line below when connected via TCP.
  //  teensy and ESP via serial work just fine.  Crazy world.

 if (_debugLogStart < _debugLogEnd) {
    start = _debugLog + _debugLogStart;
    len = _debugLogEnd - _debugLogStart;

    p->write(start, len);

  } else if (_debugLogEnd < _debugLogStart) {

    start = _debugLog + _debugLogStart;
    len = _debugLogSize - _debugLogStart;

    p->write(start, len);

    start = _debugLog;
    len = _debugLogEnd;

#if defined(ESP8266) || defined(ESP32)
  // TODO BUG: ESP32 and ESP8266 fail on the line below when connected via TCP.
  //  teensy and ESP via serial work just fine.  Crazy world.
    size_t i = 0;
    while (i < len) {
      p->write(start[i++]);
    }
#else
    p->write(start, len);
#endif

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
    const char* getHelp() { return "Prints out this help information"; }
    void execute(Stream* c, uint8_t paramCount, char** params);
};

void HelpCommand::execute(Stream* c, uint8_t paramCount, char** params) {
  Command* firstCommand = this;
  while (firstCommand->prev()) {
    firstCommand = firstCommand->prev();
  }
  c->print("Known Commands:\n");

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
