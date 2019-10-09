#ifndef _SerialCommand_
#define _SerialCommand_
#include "Command.h"
#include "SoftwareSerial.h"

class SerialCommand : public Command {
  public:
    const char* getName() override { return "serial"; }
    const char* getHelp() override { return "<rx pin> <tx pin> <baud> - Tunnel serial connection"; }
    void execute(Stream* c, uint8_t paramCount, char** params) override {
      if (_serialPort) {
        c->println("Unexpected open serial port");
        return;
      }
      if (paramCount < 2) {
        printError(c);
        return;
      }
      uint8_t rx = atoi(params[1]);
      uint8_t tx = atoi(params[2]);
      long speed = 115200;
      if (paramCount == 3) {
        speed = atoi(params[3]);
      }
#if defined(ESP32)
      _serialPort = new SoftwareSerial();
      if (_serialPort) {
        _serialPort->begin(speed, rx, tx);
#else
      _serialPort = new SoftwareSerial(rx,tx,false);
      if (_serialPort) {
        _serialPort->begin(speed);

#endif
      } else {
        c->println("Error opening serial port");
      }
    }

    void idle(Stream* c) override {
      if (_serialPort) {
        while (_serialPort->available()) {
          c->write(_serialPort->read());
        }
        while (c->available()) {
          int x = c->read();
          if (x == 3) {
            c->println("Ctrl-c: Closing serial connection");
            kill();
            break;
          }
          _serialPort->write(x);
        }
      }
    }
    void kill() override {
      if (_serialPort) {
        _serialPort->end();
        delete _serialPort;
        _serialPort = nullptr;
      }
    }
  private:
    SoftwareSerial* _serialPort;
};

SerialCommand theSerialCommand;

#endif
