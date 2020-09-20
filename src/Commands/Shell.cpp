#include "Shell.h"
#include "NullStream.h"
#include "CommandLine.h"
#include "Commands/ShellCommands.h"

Shell::Shell() {
}

void Shell::idle(Console* c) {
  // bail before command line parsing if running command is reading input
  if (_lastCommand && _lastCommand->isRunning() && _lastCommand->isReading()) {
    return;
  }

  String commandLine;
  String prompt = thePromptCommand.getPrompt();
  if (_readline.readline(c, prompt, commandLine)) {
    // control-c
    if (commandLine == "\x03") {
        c->println("Ctrl-C: Stopping");
        Command::killAll();
    } else {
      bool fail = executeCommandLine(c, commandLine.c_str());
      if (fail) {
//        executeCommandLine(c, "help");
      }
      _readline.add_history(commandLine);
    }
  }
}

bool Shell::isReading() {
  return isRunning(); }

bool Shell::isRunning() {
  return true;
}

void Shell::execute(Console* c, uint8_t paramCount, char** params) {
  // TODO: execute files
}

void Shell::kill() {
}

inline bool isSpace(char c) {
  return c == ' ' || c == '\t' || c == '\r';
}
inline bool isEnd(char c) {
  return c == 0;
}
inline bool isQuote(char c) {
  return c == '"';
}

inline bool isEscape(char c) {
  return c == '\\';
}

inline bool isBetween(char c, bool quoting) {
  if (quoting) {
    return isQuote(c) || isEnd(c);
  } else {
    return isSpace(c) || isEnd(c);
  }
}
inline bool isLineNum(char c) {
  return c >= '0' && c <= '9';
}

bool Shell::executeCommandLine(Console* output, const char* line) {

  int max_params = 10;
  // TODO: remove strlen based line sizes
  String paramStrings[max_params];

  int paramCount = 0;
  int commandLineIndex = 0;

  bool failure = false;

  if ((line == nullptr) || (isEnd(line[0]))) {
    // no command, it succeeds
    return false;
  }

  while (isSpace(line[commandLineIndex])) {
    commandLineIndex++;
  }

  if (isLineNum(line[commandLineIndex])) {
    linenumber_t n = atoi(&line[commandLineIndex]);
    if (n) {
      while (isLineNum(line[commandLineIndex])) {
        commandLineIndex++;
      }
      while (isSpace(line[commandLineIndex])) {
        commandLineIndex++;
      }
      if (n) {
        if (strlen(&line[commandLineIndex])) {
          CommandLine* commandLine = new CommandLine(n, &line[commandLineIndex]);
          if (commandLine) {
            addLine(commandLine);  // we always have a first line, a REMark
          }
        } else {
          CommandLine* del = removeLine(n);
          if (del) { delete del; }
        }
        // returns success
        return false; // added the line to the current program
      }
    }
  }


  // parse out the command and parameters
  while (1) {
    bool quoting = false;
    while (isSpace(line[commandLineIndex])) {
      commandLineIndex++;
    }

    if (isEnd(line[commandLineIndex])) {
      break;
    }

    if (isQuote(line[commandLineIndex])) {
      quoting = true;
      commandLineIndex++;
    }

    while (!isBetween(line[commandLineIndex], quoting)) {
      if (isEscape(line[commandLineIndex])) {
        commandLineIndex++;
        char replacement = 0;
        char h, l;
        switch (line[commandLineIndex]) {
          case 'a':
            replacement = '\a';
            break;
          case 'b':
            replacement = '\b';
            break;
          case 'e':
            replacement = '\e';
            break;
          case 'f':
            replacement = '\f';
            break;
          case 'n':
            replacement = '\n';
            break;
          case 'r':
            replacement = '\r';
            break;
          case 't':
            replacement = '\t';
            break;
          case 'v':
            replacement = '\v';
            break;
          case 'x':
            h = line[commandLineIndex+1];
            if (h >= 'A' && h <= 'F') {
              replacement = (h - 'A')<<4;
            } else if (h >= 'a' && h <= 'f') {
              replacement = (h - 'a')<<4;
            } else if (h >= '0' && h <= '9') {
              replacement = (h - '0')<<4;
            } else {
              replacement = 'x';
              break;
            }
            l = line[commandLineIndex+2];
            if (l >= 'A' && l <= 'F') {
              replacement += (l - 'A');
            } else if (l >= 'a' && l <= 'f') {
              replacement += (l - 'a');
            } else if (l >= '0' && l <= '9') {
              replacement += (l - '0');
            } else {
              replacement = 'x';
              break;
            }
            commandLineIndex+=2;
            break;
          default:
            replacement = line[commandLineIndex];
        }
        paramStrings[paramCount] += replacement;
      } else {
        paramStrings[paramCount] += line[commandLineIndex];
      }
      commandLineIndex++;
    }

    if (isQuote(line[commandLineIndex])) {
      commandLineIndex++;
    }

    paramCount++;
    if (paramCount > max_params) {
      output->println("too many parameters");
      failure = true;
      break;
    }

    if (isEnd(line[commandLineIndex])) {
      break;
    }
  };

  if (paramCount && !failure) {

    Command* c = Command::getByName(paramStrings[0].c_str());
    if (c) {
      const char* params[paramCount+1];
      for (int i = 0; i < paramCount; i++) {
        params[i] = paramStrings[i].c_str();
      }

      c->execute(output, paramCount-1, params);
      _lastCommand = c;
    } else {
      failure = true;
      _lastCommand = nullptr;
      output->printf("Unknown Command: %s\n", paramStrings[0].c_str());
    }
  }
  return failure;
}

void Shell::addLine(CommandLine* newLine) {

  CommandLine* cur = _firstLine;
  CommandLine* last = nullptr;

  if (!cur) {
    _firstLine = newLine;
    return;
  }

  while (cur) {
    if (newLine->getNumber() == cur->getNumber()) {
      CommandLine* removed = cur;
      if (cur == _firstLine) {
        _firstLine = newLine;
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

CommandLine* Shell::getLine(linenumber_t n) {
  if (n == NO_LINENUMBER) {
    return nullptr;
  }
  CommandLine* cur = _firstLine;
  while (cur) {
    if (cur->getNumber() == n) {
      return cur;
    } else {
      cur = cur->getNext();
    }
  }
  return nullptr;
}

CommandLine* Shell::removeLine(linenumber_t n) {
  if (n==0) { return nullptr; }
  CommandLine* cur = _firstLine;
  CommandLine* last = nullptr;
  while (cur) {
    if (cur->getNumber() == n) {
      if (cur == _firstLine) {
        _firstLine = cur->getNext();
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

void Shell::clearLines() {
  while (_firstLine) {
    CommandLine* next = _firstLine->getNext();
    delete _firstLine;
    _firstLine = next;
  }
}
