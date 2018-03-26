#ifndef _Command_
#define _Command_

#include <Arduino.h>

class Command {
  public:
    Command();
    virtual void execute(Stream* c, uint8_t paramCount, char** params) = 0;
    virtual const char* getName() = 0;
    virtual const char* getHelp() = 0;
    void printError(Stream* c);

    Command* next() { return _next; }
    Command* prev() { return _prev; }
    void setNext(Command* c) { _next = c; }
    void setPrev(Command* c) { _prev = c; }
  protected:
    Command* _next = nullptr;
    Command* _prev = nullptr;
};

typedef uint16_t linenumber_t;

class CommandLine : public String {
  public:
    CommandLine(linenumber_t n, const char* cstr) {
// this wouldn't link, so doing it by hand below
//    String::init();
      buffer = NULL;
      capacity = 0;
      len = 0;

      _lineNumber = n;
      if (cstr) {
        copy(cstr, strlen(cstr));
      }
    }

    linenumber_t getNumber() { return _lineNumber; }
    void setNumber(linenumber_t number) { _lineNumber = number; }

    CommandLine* getNext() { return _next;}
    void setNext(CommandLine* next) { _next = next; }

    void addLine(CommandLine* newLine) {
      if (newLine->getNumber() == 0) { return; }
      CommandLine* cur = this;
      CommandLine* last = cur;
      while (cur) {
        if (newLine->getNumber() == cur->getNumber()) {
          CommandLine* removed = cur;
          last->setNext(newLine);
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
    CommandLine* removeLine(linenumber_t n) {
      if (n==0) { return nullptr; }
      CommandLine* cur = this;
      CommandLine* last = cur;
      while (cur) {
        if (cur->getNumber() == n) {
          last->setNext(cur->getNext());
          return cur;
        }
        last = cur;
        cur = cur->getNext();
      }
      return nullptr;
    }
  private:
    uint16_t _lineNumber = 0;
    CommandLine* _next = nullptr;
};

#endif
