#include "Command.h"
#include "Console.h"

Command::Command() {
  Console::addCommand(this);
}

void Command::printError(Stream* c) {
  c->printf("Usage: %s %s\n", getName(), getHelp());
}

Variable::Variable(const String &name, const String &value) : String(value) {
  _name = name;
}