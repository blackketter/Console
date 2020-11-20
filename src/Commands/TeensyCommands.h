#include "Command.h"
#include "core_pins.h"
#define CPU_RESTART_ADDR (uint32_t *)0xE000ED0C
#define CPU_RESTART_VAL 0x5FA0004
#define CPU_RESTART (*CPU_RESTART_ADDR = CPU_RESTART_VAL);

// System reboot
void reboot() {
#if defined(__IMXRT1062__)
  SCB_AIRCR = 0x05FA0004;
#else
      while (1) {
        delay(100);
        CPU_RESTART
      };
#endif
}

// Firmware upload
void reload() {
#if defined(__IMXRT1062__) || defined(__MK66FX1M0__) || defined(__MKL26Z64__)
      asm("bkpt #251"); // run bootloader
#endif
}

class RebootCommand : public Command {
  public:
    const char* getName() { return "reboot"; }
    const char* getHelp() { return "Reboots system"; }
    void execute(Console* c, uint8_t paramCount, char** params) {
      c->println("Rebooting...");
      delay(10);
      reboot();
    }
};

class ReloadCommand : public Command {
  public:
    const char* getName() { return "reload"; }
    const char* getHelp() { return "Jump to Teensy bootloader"; }
    void execute(Console* c, uint8_t paramCount, char** params) {
      c->println("Jumping to bootloader...");
      delay(10);
      reload();
    }
};

// add these to your sketch as needed
RebootCommand theRebootCommand;
ReloadCommand theReloadCommand;
