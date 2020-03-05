#ifndef _Readline_
#define _Readline_

#include <Arduino.h>

class Readline {
  public:
    bool readline(Stream* s, String& prompt, String& line);
    void add_history(String& line);
  
  private:
    String _line;
    bool _prompted = false;
    bool _escaping = false;
    void reset(); 

    static const int _historyMax = 9;
    int _currHistory = 0;
    int _historyLen = 0;
    String _history[_historyMax+1];
    
    unsigned int _cursor = 0;
    
    void backspace(Stream* s);
    void clearLine(Stream* s);
    void removeEscape();
    void beep(Stream* s);
};
#endif
