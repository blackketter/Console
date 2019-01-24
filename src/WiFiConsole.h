#ifndef _WiFiConsole_
#define _WiFiConsole_

#if defined(ESP32) || defined(ESP8266)

#include "Console.h"

class WiFiConsole : public Console {
  public:
    void begin();
    void idle();
    void setTelnetMode(bool telnetMode) { _telnetMode = telnetMode; };
    bool getTelnetMode() { return _telnetMode; };

// overrides
    virtual int available();
    virtual int read();
    virtual int peek();
    virtual void flush();
    virtual size_t write(uint8_t b);
    virtual size_t write(const uint8_t *buf, size_t size);
    virtual void close();

  private:
    bool _telnetMode = true;
    void checkIAC();
};

#endif

#endif
