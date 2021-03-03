#include "Readline.h"

static const char* ANSI_BEGIN = "\x1b[";
static const char* ANSI_CURSOR_UP =    "\x1b[A";
static const char* ANSI_CURSOR_DOWN =  "\x1b[B";
static const char* ANSI_CURSOR_RIGHT = "\x1b[C";
static const char* ANSI_CURSOR_LEFT =  "\x1b[D";
//static const char* ANSI_ERASE_EOL = "\x1b[K";
static const char UP = 'A';
static const char DOWN = 'B';
static const char RIGHT = 'C';
static const char LEFT = 'D';
static const char ESCAPE = 0x1b;

void moveCursor(Stream* s, char dir, unsigned int count = 1) {
  if (count == 0) return;
  
  s->print(ANSI_BEGIN);
  if (count > 1) {
    s->print(count);
  }
  s->print(dir);
}

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
        _cursor--;
        _line.remove(_cursor,1);
        
        moveCursor(s,LEFT);
        String after = _line.substring(_cursor);
        if (after.length()) {
          s->print(after);
        }
        s->print(' ');
        moveCursor(s,LEFT,after.length()+1);
      } else {
        beep(s);
      }

    } else if (ch == 0x03) {
      // control-c
      line = "\x03";
      success = true;
      reset();

    } else if (ch == ESCAPE) {
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
      if (_escaping) {
        _line += ch;
      } else {

          String before, after;
          if (_cursor) {
            before = _line.substring(0, _cursor);
          }
          if (_cursor < _line.length()) {
            after = _line.substring(_cursor);
          }
          _line = before + ch + after;
          
          s->print(ch);
          s->print(after);
          
          moveCursor(s,LEFT,after.length());
          _cursor++;
      }
    }
    
    if (_escaping) {
      if (_line.endsWith(ANSI_CURSOR_UP)) { 
        // up arrow
        removeEscape();
        if (_currHistory < _historyLen) {
          _history[_currHistory] = _line;
          clearLine(s);
          _currHistory++;
          _line = _history[_currHistory];
          s->print(_line);
          _cursor = _line.length();
        } else {
          beep(s);
        }

      } else if (_line.endsWith(ANSI_CURSOR_DOWN)) {
        // down arrow
        removeEscape();
        if (_currHistory > 0) {
          _history[_currHistory] = _line;
          clearLine(s);
          _currHistory--;
          _line = _history[_currHistory];
          s->print(_line);
          _cursor = _line.length();
        } else {
          beep(s);
        }

      
      } else if (_line.endsWith(ANSI_CURSOR_RIGHT)) {
        // right arrow
        removeEscape();
        if (_cursor < _line.length()) {
          moveCursor(s,RIGHT);
          _cursor++;
        } else {
          beep(s);
        }

      } else if (_line.endsWith(ANSI_CURSOR_LEFT)) {
        // left arrow
        removeEscape();
        if (_cursor > 0) {
          moveCursor(s,LEFT);
          _cursor--;
        } else {
          beep(s);
        }

      }
    }

   s->flush();
 }
  
  
  return success;
}

void Readline::add_history(String& line) {
  // don't add empty lines to history
  if (line.length() == 0) {
    return;
  }
  
  // don't add duplicate lines to history
  if (line == _history[1]) {
    _currHistory = 0;
    return;
  }
  
  for (int i = _historyMax-1; i >= 1; i--) {
    _history[i+1] = _history[i];
  }
  _history[1] = line;
  _currHistory = 0;
  _historyLen++;
  if (_historyLen > _historyMax) {
    _historyLen = _historyMax;
  }
}

void Readline::reset() {
  _prompted = false;
  _line = "";
  _escaping = false;
  _cursor = 0;
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
  for (unsigned int i = 0; i < _line.length(); i++) {
    backspace(s);
  }
  _cursor = 0;
}

void Readline::beep(Stream* s) {
 s->write('\x07');
}

