#include "CommandLine.h"

CommandLine* CommandLine::_first = nullptr;

Variable::Variable(const String &name, const String &value) : String(value) {
  _name = name;
}
