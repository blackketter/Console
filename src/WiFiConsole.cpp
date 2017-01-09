#ifdef ESP8266
#include "WiFiConsole.h"
#include "ESP8266WiFi.h"

// todo: make these class fields
WiFiClient _client;
WiFiServer _telnetServer(23);

void WiFiConsole::begin() {
  Console::begin();
  _telnetServer.begin();
  _telnetServer.setNoDelay(true);
};

void WiFiConsole::loop() {

  if (_client && (_client.status() != ESTABLISHED)){
    _client.stop();
  }

  // reject second client attempt
  if (_telnetServer.hasClient()) {
    if (_client && _client.connected()){
      _telnetServer.available().stop();
    } else {
      // get telnet client to do character mode
      _client = _telnetServer.available();

#define TELNET_IAC 255

#define TELNET_DONT 254
#define TELNET_DO 253
#define TELNET_WONT 252
#define TELNET_WILL 251

#define TELNET_ECHO  1
#define TELNET_NAOCRD 10
#define TELNET_NAOLFD 16
#define TELNET_LINEMODE 34
#define TELNET_SUPPRESS_LOCAL_ECHO 45

//      _client.write(TELNET_IAC);
//      _client.write(TELNET_WILL);
//      _client.write(TELNET_ECHO);
    }
  }
  Console::loop();
};

int WiFiConsole::available() {
  int a;
  if (_client && _client.connected()) {
    a = _client.available();
  } else {
    a = Console::available();
  }
  return a;
};


int WiFiConsole::read() {
  int c;
  if (_client && _client.connected()) {
    c = _client.read();
  } else {
    c = Console::read();
  }
  return c;
};


int WiFiConsole::peek() {
  int p;

  if (_client && _client.connected()) {
   p = _client.peek();
  } else {
    p = Console::peek();
  }
  return p;
};


void WiFiConsole::flush() {
  if (_client && _client.connected()) {
    _client.flush();
  } else {
    Console::flush();
  }
};

//#define ASCII_LF 10
#define ASCII_LF 13
#define ASCII_CR 13


size_t WiFiConsole::write(uint8_t b) {
  size_t w;
  if (b == ASCII_LF) { b = ASCII_CR; }
  if (_client && _client.connected()) {
    w = _client.write(b);
  } else {
    w = Console::write(b);
  }
  return w;
};

size_t WiFiConsole::write(const uint8_t *buf, size_t size) {

  size_t w;

  if (_client && _client.connected()) {
    uint8_t fixedBuf[size];
    int i = 0;
    // remap LF to CR for Telnet
    while (1) {
      uint8_t c = buf[i];
      if (c == ASCII_LF) {
        fixedBuf[i] = ASCII_CR;
      } else {
        fixedBuf[i] = c;
      }
      if (c == 0) {
        break;
      }
      i++;
    }
    w = _client.write(fixedBuf,size);

  } else {
    w = Console::write(buf,size);
  }
  return w;
}

#endif
