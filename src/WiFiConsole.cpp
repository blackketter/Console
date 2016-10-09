#ifdef ESP8266
#include "WiFiConsole.h"

    WiFiClient _client;
    WiFiServer _telnetServer(23);

void WiFiConsole::begin() {
  Console::begin();
  _telnetServer.begin();
  _telnetServer.setNoDelay(true);
};

void WiFiConsole::loop() {
  // reject second client attempt
  if (_telnetServer.hasClient()) {
    if (_client && _client.connected()){
      _telnetServer.available().stop();
    } else {
      _client = _telnetServer.available();
    }
  }
};

int WiFiConsole::available() {
  int a;
  a = Console::available();
  if (_client && _client.connected()) {
    a += _client.available();
  }
  return a;
};


int WiFiConsole::read() {
  int c = Console::read();
  if (c == EOF && _client && _client.connected()) {
    _client.read();
  }
  return c;
};


int WiFiConsole::peek() {
  int p = Console::peek();
  if (_client && _client.connected()) {
   p += _client.peek();
  }
  return p;
};


void WiFiConsole::flush() {
  Console::flush();
  if (_client && _client.connected()) {
   _client.flush();
  }
};


size_t WiFiConsole::write(uint8_t b) {
  size_t w;
  w = Console::write(b);
  if (_client && _client.connected()) {
    w = _client.write(b);
  }

  return w;
};

size_t WiFiConsole::write(const uint8_t *buf, size_t size) {
  size_t w;
  w = Console::write(buf,size);
  if (_client && _client.connected()) {
    w = _client.write(buf,size);
  }
  return w;
}

#endif
