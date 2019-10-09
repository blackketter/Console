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


    static void addLine(CommandLine* newLine) {

      CommandLine* cur = _first;
      CommandLine* last = nullptr;

      if (!cur) {
        _first = newLine;
        return;
      }

      while (cur) {
        if (newLine->getNumber() == cur->getNumber()) {
          CommandLine* removed = cur;
          if (cur == _first) {
            _first = newLine;
          } else {
            if (last) {
              last->setNext(newLine);
            }
          }
          newLine->setNext(removed->getNext());
          delete removed;
          return;
        } else if (newLine->getNumber() < cur->getNumber()) {
          newLine->setNext(cur);
          last->setNext(newLine);
          return;
        } else {
          last = cur;
          cur = last->getNext();
        }

        if (!cur) {
          last->setNext(newLine);
        }
      }
    }

    static CommandLine* first() { return _first; }

    CommandLine* getNext() { return _next;}

    static CommandLine* getLine(linenumber_t n) {
      if (n == NO_LINENUMBER) {
        return nullptr;
      }
      CommandLine* cur = _first;
      while (cur) {
        if (cur->getNumber() == n) {
          return cur;
        } else {
          cur = cur->getNext();
        }
      }
      return nullptr;
    }

    static CommandLine* removeLine(linenumber_t n) {
      if (n==0) { return nullptr; }
      CommandLine* cur = _first;
      CommandLine* last = nullptr;
      while (cur) {
        if (cur->getNumber() == n) {
          if (cur == _first) {
            _first = cur->getNext();
          } else {
            last->setNext(cur->getNext());
          }
          return cur;
        }
        last = cur;
        cur = cur->getNext();
      }
      return nullptr;
    }

    static void clearAll() {
      while (_first) {
        CommandLine* next = _first->getNext();
        delete _first;
        _first = next;
      }
    }
  private:
    void setNext(CommandLine* next) { _next = next; }
    uint16_t _lineNumber = 0;
    CommandLine* _next = nullptr;
    static CommandLine* _first;
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
