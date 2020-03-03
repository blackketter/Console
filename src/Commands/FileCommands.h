#ifndef _FileCommands_
#define _FileCommands_
#include <SD.h>
#include <SPI.h>
#if defined(HAS_SD) || defined(BUILTIN_SDCARD)

void printDirectory(Stream* c, File dir, int numTabs, bool recurse) {
  while(true) {
    File entry =  dir.openNextFile();
    if (! entry) {
     break;
    }
    for (uint8_t i=0; i<numTabs; i++) {
     c->print(' ');
    }
    c->print(entry.name());
    if (entry.isDirectory()) {
     c->println("/");
     if (recurse) {
       printDirectory(c, entry, numTabs+1, recurse);
     }
    } else {
     // files have sizes, directories do not
     c->print("  ");
     c->println(entry.size(), DEC);
    }
    entry.close();
  }
}

class LSCommand : public Command {
  public:
    const char* getName() { return "ls"; }
    const char* getHelp() { return "<-r> <path> - Directory listing"; }
    void execute(Console* c, uint8_t paramCount, char** params) {
      uint8_t pathindex = 1;
      bool recurse = false;
      if (paramCount && (params[1][0] == '-')) {
        if (strchr(params[1], 'r')) {
          recurse = true;
        }
        pathindex++;
      }
      const char* path = "/";
      if (paramCount >= pathindex) {
        path = params[pathindex];
      }
      File dir = SD.open(path);
      printDirectory(c, dir, 0, recurse);
    }
};

LSCommand theLSCommand;
#endif
#endif
