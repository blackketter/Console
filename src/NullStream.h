#ifndef _NullStream_
#define _NullStream_
#include "Stream.h"

// A /dev/null stream, which throws away any input and just provides zero if you ask

class NullStream : public Stream {
  int available() { return 0; }
  int read() { return 0; }
  int peek() { return 0; }
  void flush() {};
  size_t write(uint8_t b) { return 1; }
  size_t write(const uint8_t *buffer, size_t size) { return size; }
};

#endif
