#include "Readline.h"

// TODO - add history

bool Readline::readline(Stream* s, String& prompt, String& line) {
  bool success = false;
  if (!_prompted) {
    s->print(prompt);
    _prompted = true;
  }

  if (s->available()) {
    char ch = s->read();
    
    //s->printf("typed: %x\n", ch);

    if (  ch == '\n' 
       || ch == 0
       || ch == '\t'
       || ch > 127 ) {
      // ignore

    } else if (ch == 8 || ch == 127) {
      // backspace
      if (_line.length()) {
        _line.remove(_line.length()-1);
        backspace(s);
      }

    } else if (ch == 0x03) {
      // control-c
      line = "0x03";
      success = true;
      reset();

    } else if (ch == 0x1b) {
      // escape
      _line += ch;
      _escaping = true;      
    } else if (ch == '\r') {
      // newline
      s->write(ch);
      s->write('\n');
      line = _line;

      reset();
      success = true;

    } else {
      if (!_escaping) {
        s->write(ch);
      }
      _line += ch;
    }
    
    if (_escaping) {
      if (_line.endsWith("\x1b\x5b\x41")) { 
        // up arrow
        removeEscape();
        if (_currHistory < _historyLen) {
          clearLine(s);
          _currHistory++;
          _line = _history[_currHistory];
          s->print(_line);
        }

      } else if (_line.endsWith("\x1b\x5b\x42")) {
        // down arrow
        removeEscape();
        if (_currHistory > 0) {
          clearLine(s);
          _currHistory--;
          _line = _history[_currHistory];
          s->print(_line);
        }
      
      } else if (_line.endsWith("\x1b\x5b\x43")) {
        // right arrow
        removeEscape();
        
      } else if (_line.endsWith("\x1b\x5b\x44")) {
        // left arrow
        removeEscape();
      }
    }

  }
  
  s->flush();
  
  return success;
}

void Readline::add_history(String& line) {
  // don't add empty lines to history
  if (line.length() == 0) {
    return;
  }
  
  // don't add duplicate lines to history
  if (line == _history[0]) {
    return;
  }
  
  for (int i = _historyMax-1; i >= 0; i--) {
    _history[i+1] = _history[i];
  }
  _history[0] = line;
  _currHistory = -1;
  _historyLen++;
  if (_historyLen > _historyMax) {
    _historyLen = _historyMax;
  }
}

void Readline::reset() {
  _prompted = false;
  _line = "";
  _escaping = false;
}

void Readline::backspace(Stream* s) {
  s->write(8);
  s->write(' ');
  s->write(8);
}

void Readline::removeEscape() {
  _line.remove(_line.length()-3);
  _escaping = false;
}

void Readline::clearLine(Stream* s) {
  for (int i = 0; i < _line.length(); i++) {
    backspace(s);
  }
}

