#include "Command.h"
#include "Console.h"

Command::Command() {
  Console::addCommand(this);
}

void Command::printError(Stream* c) {
  c->printf("Usage: %s %s\n", getName(), getHelp());
}