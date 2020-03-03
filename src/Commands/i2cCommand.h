#ifndef _i2cCommand_
#include "Console.h"
#include "Wire.h"

////////////////// i2cCommand

class i2cCommand : public Command {
  public:
    const char* getName() { return "i2c"; }
    const char* getHelp() { return ("i2c scan"); }
    void execute(Console* c, uint8_t paramCount, char** params) {
      uint8_t error, address, nDevices;

      c->println("i2c scan:");
      Wire.begin();
      nDevices = 0;
      for(address = 1; address < 127; address++ )
      {
        // The i2c_scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0) {
          c->printf("  0x%02x - device found\n", address);
          nDevices++;
        }
        else if (error != 2) {
          c->printf("  0x%02x - Error %d\n", address, error);
        }
      }
      c->printf("i2c scan complete: %d devices found\n", nDevices);
    }
};

i2cCommand thei2cCommand;

#endif
