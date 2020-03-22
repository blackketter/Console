#ifndef _CommandLine_
#define _CommandLine_
#include "Arduino.h"

typedef uint16_t linenumber_t;
const linenumber_t NO_LINENUMBER = 65535;

class CommandLine : public String {
  public:
    CommandLine(linenumber_t n, const char* cstr) : String() {

      _lineNumber = n;
      if (cstr) {
        copy(cstr, strlen(cstr));
      }
    }

    linenumber_t getNumber() { return _lineNumber; }
    void setNumber(linenumber_t number) { _lineNumber = number; }
    void setNext(CommandLine* next) { _next = next; }
    CommandLine* getNext() { return _next;}

  private:
    linenumber_t _lineNumber = 0;
    CommandLine* _next = nullptr;
};

class Variable : public String {
  public:
    Variable(const String &name, const String &value);
    String & name() { return _name; }
    Variable* getNext() { return _next; }
    void setNext(Variable* next) { _next = next; }
  private:
    String _name;
    Variable* _next;
};

#endif
