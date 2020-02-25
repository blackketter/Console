#ifndef _SerialCommand_
#define _SerialCommand_
#include "Command.h"
#include "SoftwareSerial.h"
#include "Clock.h"
class SerialCommand : public Command {
  public:
    const char* getName() override { return "serial"; }
    const char* getHelp() override { return "<rx pin> <tx pin> <baud> - Tunnel serial connection"; }
    bool isReading() override { return isRunning(); }
    bool isRunning() override { return _serialPort != nullptr; }
    void execute(Stream* c, uint8_t paramCount, char** params) override {
      if (paramCount < 2) {
        printError(c);
        return;
      }
      uint8_t rx = atoi(params[1]);
      uint8_t tx = atoi(params[2]);
      long speed = DEFAULT_SPEED;
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
        // send serial data to console
        while (_serialPort->available()) {
          if (c->write(_serialPort->peek()) == 0) {
            break;
          }
          _serialPort->read();
          _timeout = Uptime::millis();
        }

        // send console data to serial port
        while (c->available()) {
          int x = c->peek();
          if (0 && x == 3) {
            c->println("Ctrl-c: Closing serial connection");
            c->read();
            kill();
            break;
          }
          if (_serialPort->write(x) == 0) {
            break;
          }
          c->read();
          _timeout = Uptime::millis();
        }

        // timeout connection
        if (Uptime::millis() > _timeout + TIMEOUT_LIMIT) {
          c->println("Timeout: Closing serial connection");
          kill();
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
  protected:
    void openSerial(Stream* c, uint8_t rx, uint8_t tx, uint32_t speed) {
      if (speed == 0) {
        speed = DEFAULT_SPEED;
      }
      if (_serialPort) {
        c->println("Unexpected open serial port");
        return;
      }
#if defined(ESP32)
      _serialPort = new SoftwareSerial();
      if (_serialPort) {
        _serialPort->begin(speed, SWSERIAL_8N1, rx, tx);
#else
      _serialPort = new SoftwareSerial(rx,tx,false);
      if (_serialPort) {
        _serialPort->begin(speed);
#endif
        _timeout = Uptime::millis();
      } else {
        c->println("Error opening serial port");
      }
    }
  private:
    SoftwareSerial* _serialPort;
    static const uint32_t DEFAULT_SPEED = 115200;
    millis_t _timeout = 0;
    static const millis_t TIMEOUT_LIMIT = 10000;
};

extern SerialCommand theSerialCommand;


#endif
