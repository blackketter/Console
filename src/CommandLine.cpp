#include "CommandLine.h"

Variable::Variable(const String &name, const String &value) : String(value) {
  _name = name;
}
