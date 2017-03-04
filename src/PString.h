#ifndef _PString_
#define _PString_

#include "WString.h"
#include "Stream.h"

#ifndef SEEK_SET
#define SEEK_SET  (0)  /* set file offset to offset */
#endif
#ifndef SEEK_CUR
#define SEEK_CUR  (1)  /* set file offset to current plus offset */
#endif
#ifndef SEEK_END
#define SEEK_END  (2)  /* set file offset to EOF plus offset */
#endif

// PString is a String that is a Stream that can be read and written to, as well as can seeked.

class PString : public String, public Stream {
  public:
    virtual size_t write(uint8_t character) { concat((char)character); return 1; }
    virtual size_t write(const char *str) { concat(str); return strlen(str); }
// two built-in libraries have slightly different APIs for appending a string of a specified length
#ifdef ESP8266
    virtual size_t write(const uint8_t *buffer, size_t size) { concat((const char*)buffer, size); return size; }
#else
    virtual size_t write(const uint8_t *buffer, size_t size) { append((const char*)buffer, size); return size; }
#endif

    int available() { return length() - _pos; }
    void flush() {};

    int read() {
      if (_pos < length()) {
        int c = charAt(_pos);
        _pos++;
        return c;
      } else {
        return -1;
      }
    }

    int peek() {
      if (_pos < length()) {
        return charAt(_pos);
      } else {
        return -1;
      }
    }

#if NOT_FINISHED
    bool seek(int32_t pos, uint8_t mode) {
      int32_t newPos = _pos;
      switch (mode) {
        case SEEK_SET:
          newPos = pos;
          break;
        case SEEK_CUR:
          newPos += pos;
          break;
        case SEEK_END:
          newPos = pos + size();
          break;
        default:
          // invalid seek mode
          return false;
      }
      if (newPos > (int32_t)size() || newPos < 0) {
        return false;
      } else {
        _pos = newPos;
        return true;
      }
    };

    size_t position() { return _pos; }
    size_t size() { return length(); }
#endif

  private:
     uint32_t _pos = 0;
};

#endif
