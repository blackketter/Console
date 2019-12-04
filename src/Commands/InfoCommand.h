#ifndef _InfoCommand_
#define _InfoCommand_
#include "Command.h"

Clock theClock;
// todo:
//      c->printf("  Free Heap:   %d\n", ESP.getFreeHeap());

uint32_t FreeMem() { // for Teensy 3.0 (wrong for teensy 4.0)
    uint32_t stackTop;
    uint32_t heapTop;

    // current position of the stack.
    stackTop = (uint32_t) &stackTop;

    // current position of heap.
    void* hTop = malloc(1);
    heapTop = (uint32_t) hTop;
    free(hTop);

    // The difference is (approximately) the free, available ram.
    return stackTop - heapTop;
}

void printInfo(Print* p) {

  const char* board = "Unknown";

#if defined(ARDUINO_BOARD)
  board = ARDUINO_BOARD;
#elif defined(BOARD_NAME)
  board = BOARD_NAME
#elif defined(TEENSY40)
  board = "Teensy 4.0";
#elif defined(TEENSY31)

#if defined(__MK20DX256__)
  board = "Teensy LC";
#else
  board = "Teensy 3.1/3.2";
#endif

#else
#warning Unknown board for InfoCommand
#endif

  p->printf("Board: %s\n", board);
  p->printf("Compiled: " __DATE__ " " __TIME__ "\n");
  p->printf("Free ram: %10d\n", FreeMem());
  p->printf("Uptime: %f\n", Uptime::micros()/1000000.0);
  p->printf("CPU Speed: %d\n", (int)F_CPU);

  char string[100];
  theClock.longTime(string);
  p->printf("Time: %s\n", string);
  theClock.longDate(string);
  p->printf("Date: %s\n", string);


  uint32_t clockMillis = theClock.fracMillis();

  uint32_t rtcMillis = (uint32_t)((theClock.getRTCMicros()%1000000)/1000);
  p->printf("RTC millis:%03d, clock: %03d, diff: %d\n", (uint32_t)rtcMillis, (uint32_t)clockMillis, (int)rtcMillis - (int)clockMillis);
}


class InfoCommand : public Command {
  public:
    const char* getName() { return "info"; }
    const char* getHelp() { return "Print System Info"; }
    void execute(Stream* c, uint8_t paramCount, char** params) {
      printInfo(c);
    }
};

InfoCommand theInfoCommand;
#endif
