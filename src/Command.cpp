#include "Command.h"
#include "Console.h"

Command* Command::_first = nullptr;

Command::Command() {
  addCommand(this);
}

void Command::addCommand(Command* c) {
  c->setNext(first());
  setFirst(c);
}

void Command::printError(Stream* c) {
  c->printf("Usage: %s %s\n", getName(), getHelp());
}

Variable::Variable(const String &name, const String &value) : String(value) {
  _name = name;
}

Command* Command::getByName(const char* n) {
  Command* currCommand = first();
    while (currCommand != nullptr) {
    if (strcasecmp(currCommand->getName(), n) == 0) {
      break;
    }
    currCommand = currCommand->next();
  }
  return currCommand;
}

void Command::sortCommands() {

  // leave the first one on the list, sort starting after that
  Command* toSort = first()->next();
  first()->setNext(nullptr);

  while (toSort) {
    Command* sorted = first();
    Command* prev = nullptr;
    Command* next = toSort->next();
    do {
      // time to insert?
      if (strcasecmp(sorted->getName(), toSort->getName()) > 0) {
        toSort->setNext(sorted);

        // first in list
        if (sorted == first()) {
          setFirst(toSort);
        } else {
          prev->setNext(toSort);
        }
        break;
      }
      // last in list
      if (sorted->next() == nullptr) {
        sorted->setNext(toSort);
        toSort->setNext(nullptr);
        break;
      } else {
        prev = sorted;
        sorted = sorted->next();
      }
    } while (sorted);

    toSort = next;
  }
};

void Command::killAll() {
  Command* c = _first;
  while (c) {
    c->kill();
    c = c->next();
  }
}
