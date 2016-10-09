#include "Console.h"
#include <stdarg.h>
#include <stdio.h>

void Console::begin() {
  Serial.begin(115200);
}

void Console::loop() {
  // loopback (100 chars at a time, max)
  int i = 100;
  while (i-- && available()) {
    write(read());
  }
};

int Console::available() {
  return Serial.available();
};


int Console::read() {
  return Serial.read();
};


int Console::peek() {
  return Serial.peek();
};


void Console::flush() {
  Serial.flush();
};


size_t Console::write(uint8_t b) {
  return Serial.write(b);
};

size_t Console::write(const uint8_t *buf, size_t size) {
  return Serial.write(buf,size);
}

// todo: can we override this properly?
void Console::debugf(const char* format, ...) {
  if (!_debugEnabled) return;

  char foo[100];
  debugPrefix();
  va_list argptr;
  va_start(argptr, format);
  vsnprintf(foo, sizeof(foo), format, argptr);
  va_end(argptr);
  print(foo);
};

void Console::debugln(const char* s) {
  if (!_debugEnabled) return;
  debugPrefix();
  println(s);
}

void Console::debugPrefix() {
  print(::millis());
  print(": ");
}

