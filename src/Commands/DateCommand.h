#ifndef _DateCommand_
#define _DateCommand_
#include "Command.h"
#include "Clock.h"

class DateCommand : public Command {
  public:
    const char* getName() { return "date"; }
    const char* getHelp() { return "<year> <month> <day> <hour> <minute> <second> - Print or Set Date"; }
    void execute(Console* c, uint8_t paramCount, char** params) {
      Clock theClock;
      int p = paramCount;
      uint16_t year = 0;
      uint8_t month = 1;
      uint8_t day = 1;
      uint8_t hour = 0;
      uint8_t minute = 0;
      uint8_t second = 0;
      if (p>0) { year = atoi(params[1]); }
      if (p>1) { month = atoi(params[2]); }
      if (p>2) { day = atoi(params[3]); }
      if (p>3) { hour = atoi(params[4]); }
      if (p>4) { minute = atoi(params[5]); }
      if (p>5) { second = atoi(params[6]); }
      if (year && month && day) {
        theClock.setDateTime(year,month,day,hour,minute,second);
      }
      theClock.longDate(*c);
      c->print(" ");
      theClock.longTime(*c);
      c->println();
      if (!theClock.hasBeenSet()) {
        c->println("Clock has not been set");
      }
    }
};

DateCommand theDateCommand;

#endif
