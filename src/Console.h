#ifndef _Console_
#define _Console_

#include <Arduino.h>
#include "Command.h"
#include "WString.h"
#include "Commands/Shell.h"

class Console : public Stream {
  public:
    ~Console();

    // default port of null means use Serial.
    virtual void begin(Stream* port = nullptr, size_t debugLogSize = _defaultDebugLogSize);
    virtual void idle();
    virtual void close() { }
    Stream* getPort() { return _port; }
    Stream* setPort(Stream* port) { Stream* old = _port; _port = port; return old; }

    void debugf(const char* format, ...);
    void debugln(const char* s);
    void debug(const char* s);
    void debugEnable(bool enable) { _debugEnabled = enable; };
    bool debugEnabled() { return _debugEnabled; }

    bool printDebug();

    // returns true for failure, false for success
    bool executeCommandLine(const char* line);

    // redirect output to a stream
    bool executeCommandLine(Stream* s, const char* line);

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

    Shell* getShell() { return &_shell; }

  private:
    void debugPrefix(char* s);

    Stream* _port = nullptr;

    bool _debugEnabled = true;

    static const size_t _defaultDebugLogSize = 2000;
    size_t _debugLogSize;
    char* _debugLog = nullptr; // log of recent debug output
    size_t _debugLogStart = 0;
    size_t _debugLogEnd = 0;
    Shell _shell;
    bool _wasRunning = false;
};

#endif
