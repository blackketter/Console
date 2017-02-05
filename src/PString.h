#ifndef _PString_
#define _PString_

#include "WString.h"
#include "Print.h"

class PString : public String, public Print {
  public:
    virtual size_t write(uint8_t character) { concat(character); return 1; }
    virtual size_t write(const char *str) { concat(str); return strlen(str); }

// two built-in libraries have slightly different APIs for appending a string of a specified length
#ifdef ESP8266
    virtual size_t write(const uint8_t *buffer, size_t size) { concat((const char*)buffer, size); return size; }
#else
    virtual size_t write(const uint8_t *buffer, size_t size) { append((const char*)buffer, size); return size; }
#endif
};

#endif
