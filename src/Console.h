#ifndef _Console_
#define _Console_

#include <Arduino.h>
#include "Command.h"

class Console : public Stream {
  public:
    Console();
    virtual void begin();
    virtual void loop();

    void idle() { loop(); } // can't decide the name

	  void debugf(const char* format, ...);
	  void debugln(const char* s);
	  void debug(const char* s);
    void debugEnable(bool enable) { _debugEnabled = enable; };
    bool debugEnabled();

    static void addCommand(Command* command);
    static void removeCommand(Command* command);

// low level virtual functions
	  int available();
	  int read();
    int peek();
    void flush();
    size_t write(uint8_t b);
    size_t write(const uint8_t *buf, size_t size);

  private:
    void debugPrefix();
    void executeCommandLine();

    bool _debugEnabled = true;

    static const int _maxCommandLineLength = 100;  // todo: fixme
    char commandLine[_maxCommandLineLength];
    uint8_t _commandLineLength = 0;
};

#endif
