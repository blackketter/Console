#ifndef _Console_
#define _Console_

#include <Arduino.h>
#include "Command.h"
#include "WString.h"
#include "CommandLine.h"

class Console : public Stream {
  public:
    Console();
    Console(Stream* port);
    virtual void begin();
    virtual void idle();
    virtual void close() { }
    Stream* getPort() { return _port; }
    void setPort(Stream* port) { _port = port; }

    void debugf(const char* format, ...);
    void debugln(const char* s);
    void debug(const char* s);
    void debugEnable(bool enable) { _debugEnabled = enable; };
    bool debugEnabled() { return _debugEnabled; }

    bool printDebug() { return _debugEnabled && (_commandLineLength == 0); }

    static void addCommand(Command* command);
    void sortCommands();

    // returns true for failure, false for success
    bool executeCommandLine(const char* line);

    // execute command line to another stream.  safe to pass nullptr for output if you don't care about the result
    bool executeCommandLine(Stream* output, const char* line);

    CommandLine* getLines() { return _lines; }

// low level virtual functions
    int available();
    int read();
    int peek();
    void flush();
    size_t write(uint8_t b);
    size_t write(const uint8_t *buf, size_t size);

    // debug logging
    void printLog();
    void printLog(Print* p);
    void appendLog(const char* a);
    void appendLog(const char* a, size_t size);
    static Console* get() { return _theConsole; }
  private:

    CommandLine* _lines = nullptr;

    void debugPrefix(char* s);

    Stream* _port = nullptr;

    bool _debugEnabled = true;

    static const int _maxCommandLineLength = 100;  // todo: fixme
    char _commandLine[_maxCommandLineLength];
    uint8_t _commandLineLength = 0;

    static const size_t _debugLogSize = 2000;
    char _debugLog[_debugLogSize]; // log of recent debug output
    size_t _debugLogStart = 0;
    size_t _debugLogEnd = 0;
    static Console* _theConsole;
};

#endif
