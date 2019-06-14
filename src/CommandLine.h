#ifndef _CommandLine_
#define _CommandLine_

typedef uint16_t linenumber_t;

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
