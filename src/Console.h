#ifndef _Console_
#define _Console_

#include <Arduino.h>
#include "Command.h"

class Console : public Stream {
  public:
    Console();
    virtual void begin();
    virtual void loop();
    virtual void close() { }

    void idle() { loop(); } // can't decide the name

	  void debugf(const char* format, ...);
	  void debugln(const char* s);
	  void debug(const char* s);
    void debugEnable(bool enable) { _debugEnabled = enable; };
    bool debugEnabled() { return _debugEnabled; }

    bool printDebug() { return _debugEnabled && (_commandLineLength == 0); }

    static void addCommand(Command* command);
    static void removeCommand(Command* command);

// low level virtual functions
	  int available();
	  int read();
    int peek();
    void flush();
    size_t write(uint8_t b);
    size_t write(const uint8_t *buf, size_t size);

    // debug logging
    void printLog();
    void printLog(Print& p);
    void appendLog(const char* a);
    void appendLog(const char* a, size_t size);

  private:
    void debugPrefix(char* s);
    void executeCommandLine();

    bool _debugEnabled = true;

    static const int _maxCommandLineLength = 100;  // todo: fixme
    char commandLine[_maxCommandLineLength];
    uint8_t _commandLineLength = 0;

    static const size_t _debugLogSize = 1000;
    char _debugLog[_debugLogSize]; // log of recent debug output
    size_t _debugLogStart = 0;
    size_t _debugLogEnd = 0;
};

#endif
