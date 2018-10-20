#ifndef _NullStream_
#define _NullStream_
#include "Stream.h"

// A /dev/null Stream, which throws away any input and just provides zero if you ask

class NullStream : public Stream {
  int available() { return 0; }
  int read() { return 0; }
  int peek() { return 0; }
  void flush() {};
  size_t write(uint8_t b) { return 1; }
  size_t write(const uint8_t *buffer, size_t size) { return size; }
};

// a connector class that allows you to use a Print class where a Stream is needed
class PrintStream : public Stream {
  public:
    PrintStream(Print* p) { _print = p; }
    int available() { return 0; }
    int read() { return 0; }
    int peek() { return 0; }
    void flush() {};
    size_t write(uint8_t b) { return _print->write(b); }
    size_t write(const uint8_t *buffer, size_t size) { return _print->write(buffer, size); }
  private:
    Print* _print;
};

#endif
