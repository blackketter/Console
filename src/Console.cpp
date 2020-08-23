#include <stdarg.h>
#include <stdio.h>

#include "Console.h"
#include "Command.h"
#include "CommandLine.h"
#include "Commands/Commands.h"

void Console::begin(Stream* port, size_t debugLogSize) {
  if (port) {
    setPort(port);
  }
  // if a port has not been specified, then default to the main serial port
  if (_port == nullptr) {
    _port = &Serial;
    Serial.begin(115200);
  }

  // this assumes all commands are added by their global static instance constructors
  Command::sortCommands();

  // begin commands
  Command* beginner = Command::first();
  while (beginner) {
    beginner->begin();
    beginner = beginner->next();
  }

  _debugLogSize = debugLogSize;
  if (_debugLog) {
    free(_debugLog);
    _debugLog = nullptr;
  }

  if (_debugLogSize) {
    _debugLog = (char*)malloc(debugLogSize);
    if (!_debugLog) {
      _debugLogSize = 0;
      print("Console: Debug Log Allocation Failed!");
    }
  }
}

Console::~Console() {
  if (_debugLog) {
    free(_debugLog);
    _debugLog = nullptr;
  }
}
void Console::idle() {
  // idle commands
  Command* idler = Command::first();
  while (idler) {
    idler->idle(this);
    idler = idler->next();
  }
};

bool Console::executeCommandLine(const char* line) {
  return _shell.executeCommandLine(this, line);
}

bool Console::executeCommandLine(Stream* s, const char* line) {
  Stream* oldPort = setPort(s);
  bool result = _shell.executeCommandLine(this, line);
  setPort(oldPort);
  return result;
}


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
  if (_debugLogSize == 0) {
    return;
  }
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

bool Console::printDebug() {
  // TODO - pause debug output while editing command line in shell
  return _debugEnabled;
}

void Console::printLog() {
  printLog(this);
}

void Console::printLog(Print* p) {
  if (_debugLogSize == 0) {
    p->print("No log buffer\n");
    return;
  }

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

