#ifndef _WiFiConsole_
#define _WiFiConsole_

#ifdef ESP8266
#include "Console.h"

#include "ESP8266WiFi.h"

class WiFiConsole : public Console {
  public:
    void begin();
    void loop();

// overrides
	  int available();
	  int read();
    int peek();
    void flush();
    size_t write(uint8_t b);
    size_t write(const uint8_t *buf, size_t size);

  private:
};

#endif

#endif
