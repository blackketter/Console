#include "Command.h"
#include "Console.h"

Command::Command() {
  Console::addCommand(this);
}

void Command::addCommand(Command* add) {
  if (add) {
    add->_next = _next;
    add->_prev = this;
    if (_next) {
      _next->_prev = add;
    }
    _next = add;
  }
}

void Command::removeCommand() {
  if (_next) {
    _next->_prev = _prev;
  }
  if (_prev) {
    _prev->_next = _next;
  }
}

void Command::printError(Stream* c) {
  c->printf("Usage: %s %s\n", getName(), getHelp());
}