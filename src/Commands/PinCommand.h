#ifndef _PinCommand_
#define _PinCommand_
#include "Command.h"

class pinCommand : public Command {
  public:
    pinCommand() { for (uint8_t i = 0; i < maxPins; i++) { blinking[i] = 0; } }
    const char* getName() { return "pin"; }
    const char* getHelp() { return "<on|off|blink|read|input|pullup|awrite val|aread> <pin numbers...> - Set or read a pin"; }
    void printState(Stream* c, uint8_t pin) {
      c->printf("  Pin: %d %s\n", pin, digitalRead(pin) ? "HIGH" : "LOW");
    }
    void execute(Stream* c, uint8_t paramCount, char** params) {
      if (paramCount < 2) {
        printError(c);
      } else {
        char* command = params[1];
        for (uint8_t i = 2; i <= paramCount; i++) {
          uint8_t pin = atoi(params[i]);
          if (strcasecmp(command, "on") == 0) {
            blink(pin, 0);
            pinMode(pin, OUTPUT);
            digitalWrite(pin, HIGH);
            printState(c,pin);
          } else if (strcasecmp(command, "off") == 0) {
            blink(pin, 0);
            pinMode(pin, OUTPUT);
            digitalWrite(pin, LOW);
            printState(c,pin);
          } else if (strcasecmp(command, "toggle") == 0) {
            blink(pin, 0);
            pinMode(pin, OUTPUT);
            digitalWrite(pin, !digitalRead(pin));
            printState(c,pin);
          } else if (strcasecmp(command, "read") == 0) {
            printState(c,pin);
          } else if (strcasecmp(command, "input") == 0) {
            blink(pin, 0);
            pinMode(pin, INPUT);
            c->printf("  Pin: %d INPUT\n", pin);
          } else if (strcasecmp(command, "pullup") == 0) {
            pinMode(pin, INPUT_PULLUP);
            c->printf("  Pin: %d INPUT_PULLUP\n", pin);
            blink(pin, 0);
          } else if (strcasecmp(command, "aread") == 0) {
            uint8_t val = analogRead(pin);
            c->printf("  Pin: %d Analog: %d\n", pin, val);
          } else if (strcasecmp(command, "awrite") == 0) {
// TODO: ESP32 needs analogWrite()
#if defined(ESP32)
            c->println("ESP32 doesn't support analogWrite() yet");
#else
            uint8_t val = pin;
            i++;
            pin = atoi(params[i]);
            analogWrite(pin, val);
            c->printf("  Pin: %d  Analog: %d\n", pin, val);
            blink(pin, 0);
#endif
          } else if (strcasecmp(command, "blink") == 0) {
            blink(pin, 1);
            c->printf("  Pin: %d BLINKING\n", pin);
          } else {
            printError(c);
          }
        }
      }
    }

    void idle(Stream* c) override {
      if (Uptime::millis() > lastBlink + blinkInterval) {
        lastBlink = Uptime::millis();
        blinkState = !blinkState;
        for (uint8_t pin = 0; pin < maxPins; pin++) {
          if (isBlinking(pin)) {
            pinMode(pin, OUTPUT);
            digitalWrite(pin, blinkState ? HIGH : LOW);
          }
        }
     }
    }

  private:
    millis_t lastBlink = 0;
    bool blinkState = false;
    static const millis_t blinkInterval = 250;
    static const uint8_t maxPins = NUM_DIGITAL_PINS;
    uint8_t blinking[maxPins];
    void blink(uint8_t pin, uint8_t b) { if (pin < maxPins) blinking[pin] = b; };
    uint8_t isBlinking(uint8_t pin) {  return (pin < maxPins) ? blinking[pin] : 0;}
};

pinCommand thePinCommand;

#endif
