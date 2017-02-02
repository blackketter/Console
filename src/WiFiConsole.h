#ifndef _WiFiConsole_
#define _WiFiConsole_

#ifdef ESP8266
#include "Console.h"

class WiFiConsole : public Console {
  public:
    void begin();
    void loop();

// overrides
    virtual int available();
    virtual int read();
    virtual int peek();
    virtual void flush();
    virtual size_t write(uint8_t b);
    virtual size_t write(const uint8_t *buf, size_t size);
    virtual void stop();

  private:
};

#endif

#endif
