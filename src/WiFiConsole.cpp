#if defined(ESP8266) || defined(ESP32)

#include "WiFiConsole.h"
#if defined(ESP8266)
#include "ESP8266WiFi.h"
#else
#include "WiFi.h"
#endif

#define TELNET_IAC 255

#define TELNET_DONT 254
#define TELNET_SB 250
#define TELNET_DO 253
#define TELNET_WONT 252
#define TELNET_WILL 251

#define TELNET_ECHO  1
#define TELNET_NAOCRD 10
#define TELNET_NAOLFD 16
#define TELNET_LINEMODE 34
#define TELNET_SUPPRESS_LOCAL_ECHO 45
#define ASCII_LF 10
#define ASCII_CR 13


// todo: make these class fields
WiFiClient _client;
WiFiServer _telnetServer(23);
bool _telnetServerBegun = false;

void WiFiConsole::begin() {
  Console::begin();
};

void WiFiConsole::idle() {
  // defer beginning telnet server until after we're connected to the WiFi
  if (!_telnetServerBegun && (WiFi.status() == WL_CONNECTED)) {
    _telnetServer.begin(23);
    _telnetServer.setNoDelay(true);
    _telnetServerBegun = true;
  }

// todo: I don't know if this is necessary on ESP32
#if defined(ESP8266)
  if (_client && (_client.status() != ESTABLISHED)){
    _client.stop();
  }
#endif

  // new client kicks old client off
  if (_telnetServer.hasClient()) {
    if (_client && _client.connected()){
      close();
    }
    // new client
    // get telnet client to do character mode
    _client = _telnetServer.available();
    _telnetMode = false; // reset telnet mode to false to start
  }

  checkIAC();
  Console::idle();
};

void WiFiConsole::checkIAC() {
  while (_client && _client.connected() && _client.peek() == TELNET_IAC) {
    // read IAC command and ignore for now
    //uint8_t iac =
      _client.read();
    uint8_t operation = _client.read();
    //uint8_t option = 0;
    uint8_t sub = 0;
    if (operation == TELNET_SB) {
//      debugf("iac: %d %d\n", iac, operation);
      do {
        sub = _client.read();
//        debugf(" sub: %3d '%c'\n", sub, sub >= ' ' && sub < 127 ? sub : 0);
      } while (sub != TELNET_IAC);
      //iac = sub;
      if (_client.peek() != TELNET_IAC) {
        operation = _client.read();
      }
    } else {
      //option =
      _client.read();
    }
//    debugf("iac: %d %d %d\n", iac, operation, option);

    // since we got an iac code, we are now in telnet mode.  Send our demands
    if (!_telnetMode) {
      _telnetMode = true;
      // sequence: IAC DO LINEMODE IAC WILL ECHO
      _client.write(TELNET_IAC);
      _client.write(TELNET_DO);
      _client.write(TELNET_LINEMODE);
      _client.write(TELNET_IAC);
      _client.write(TELNET_WILL);
      _client.write(TELNET_ECHO);
    }
  }
}

int WiFiConsole::available() {
  int a;
  checkIAC();
  if (_client && _client.connected()) {
    a = _client.available();
  } else {
    a = Console::available();
  }
  return a;
};


int WiFiConsole::read() {
  int c;
  checkIAC();
  if (_client && _client.connected()) {
    c = _client.read();
  } else {
    c = Console::read();
  }
  return c;
};


int WiFiConsole::peek() {
  int p;
  checkIAC();
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


size_t WiFiConsole::write(uint8_t b) {
  size_t w;
  if (_client && _client.connected()) {
    if (_telnetMode && b == ASCII_LF) {
      _client.write(ASCII_CR);
    }
    w = _client.write(b);
  } else {
    w = Console::write(b);
  }
  return w;
};

size_t WiFiConsole::write(const uint8_t *buf, size_t size) {

  size_t w;

  if (_client && _client.connected()) {
    if (_telnetMode) {
      uint8_t fixedBuf[size*2];
      size_t i = 0;
      int j = 0;
      // remap LF to CR for Telnet
      while (i < size) {
        uint8_t c = buf[i];
        if (_telnetMode && c == ASCII_LF) {
          fixedBuf[j++] = ASCII_CR;
          fixedBuf[j++] = ASCII_LF;
        } else {
          fixedBuf[j++] = c;
        }
        i++;
      }
      w = _client.write(fixedBuf,j);
    } else {
      w = _client.write(buf,size);
    }
  } else {
    w = Console::write(buf,size);
  }
  return w;
}

void WiFiConsole::close() {
  if (_client && _client.connected()) {
    _client.stop();
  }
}
#endif
