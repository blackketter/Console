#ifndef _InfoCommand_
#define _InfoCommand_
#include "Command.h"

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
  Clock theClock;

  const char* board = "Unknown";

#if defined(ARDUINO_BOARD)
  board = ARDUINO_BOARD;

#elif defined(BOARD_NAME)
  board = BOARD_NAME

#elif defined(TEENSY40)  || defined(ARDUINO_TEENSY40)
  board = "Teensy 4.0";

#elif defined(TEENSY41)  || defined(ARDUINO_TEENSY41)
  board = "Teensy 4.1";

#elif defined(TEENSY36)  || defined(ARDUINO_TEENSY36)
  board = "Teensy 3.6";

#elif defined(TEENSY35)  || defined(ARDUINO_TEENSY35)
  board = "Teensy 3.5";

#elif defined(TEENSYLC)  || defined(ARDUINO_TEENSYLC)
//  board = "Teensy LC";

#elif defined(TEENSY31)  || defined(ARDUINO_TEENSY31)
  board = "Teensy 3.1/3.2";

#else
#warning Unknown board for InfoCommand
#endif

  p->printf("Board: %s\n", board);
  p->printf("Compiled: " __DATE__ " " __TIME__ "\n");
  p->printf("Free ram: %10d\n", FreeMem());
  millis_t up = Uptime::millis();
  p->printf("Uptime: %d.%d\n", (int)(up/1000), (int)(up%1000));
  p->printf("CPU Speed: %d\n", (int)F_CPU);

  char string[100];
  theClock.longTime(string);
  p->printf("Time: %s\n", string);
  theClock.longDate(string);
  p->printf("Date: %s\n", string);
}


class InfoCommand : public Command {
  public:
    const char* getName() { return "info"; }
    const char* getHelp() { return "Print System Info"; }
    void execute(Console* c, uint8_t paramCount, char** params) {
      printInfo(c);
    }
};

InfoCommand theInfoCommand;
#endif
