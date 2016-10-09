#ifndef _Console_
#define _Console_

#include <Arduino.h>

class Console : public Stream {
  public:
    virtual void begin();
    virtual void loop();
    void idle() { loop(); } // can't decide the name

	  void debugf(const char* format, ...);
	  void debugln(const char* s);
    void debug(bool enable) { _debugEnabled = enable; };

// low level
	  int available();
	  int read();
    int peek();
    void flush();
    size_t write(uint8_t b);
    size_t write(const uint8_t *buf, size_t size);

  private:
    void debugPrefix();
    bool _debugEnabled = true;

};

#endif
