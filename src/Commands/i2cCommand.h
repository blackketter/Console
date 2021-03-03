#ifndef _i2cCommand_
#include "Console.h"
#include "Wire.h"

////////////////// i2cCommand

class i2cCommand : public Command {
  public:
    const char* getName() { return "i2c"; }
    const char* getHelp() { return ("<sda scl frequency> - i2c bus scan"); }
    void execute(Console* c, uint8_t paramCount, char** params) {
      uint8_t error, address, nDevices;
      int sda = SDA;
      int scl = SCL;
      uint32_t freq = 0;

      if (paramCount > 0) {
        sda = atoi(params[1]);
      }
      if (paramCount > 1) {
        scl = atoi(params[2]);
      }
      if (paramCount > 2) {
        freq = atoi(params[3]);
      }

      c->printf("i2c scan with SDA pin: %d, SCL pin: %d, freq: %d (0 is default)):\n", sda, scl, freq);

#ifdef CORE_TEENSY
      Wire.setSDA(sda);
      Wire.setSCL(scl);
      Wire.begin();
#else
      Wire.begin(sda,scl);
#endif
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
