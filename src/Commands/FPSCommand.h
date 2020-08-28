#ifndef _FPSCommand_
#include "Clock.h"
#include "Command.h"

// TODO: Move this to microseconds and floating point
class FPSCommand : public Command {
  public:
    const char* getName() { return "fps"; }
    const char* getHelp() { return "toggle displaying frame rate in fps"; }
    void execute(Console* c, uint8_t paramCount, char** params) {
      enable = !enable;
      c->printf("FPS display %s\n", enable ? "enabled" : "disabled");
    }

    void newFrame() {
      millis_t now = Uptime::millis();
      millis_t frameDur = now - lastFrame;
      if (frameDur > maxFrame) { maxFrame = frameDur; }
      if (frameDur < minFrame) { minFrame = frameDur; }

      frames++;
      lastFrame = now;
    }

    void idle(Console* c) override {
      millis_t now = Uptime::millis();
      millis_t idleDur = now - lastIdle;
      if (idleDur > maxIdle) { maxIdle = idleDur; }
      lastIdle = now;

      if (now - lastTime > 1000) {
       _lastFPS = ((float)(frames*1000))/(now - lastTime);
        if (enable) {
          c->printf("FPS: %d.%02d (Max frame: %dms, Min frame: %dms, Max idle: %dms)\n", (int)_lastFPS, (int)((_lastFPS-(int)_lastFPS)*100.0f), (int)maxFrame, (int)minFrame, (int)maxIdle);
        }
        lastTime = now;
        frames = 0;
        maxFrame = 0;
        minFrame = 10000;
        maxIdle = 0;
      }

    }

  float lastFPS() { return _lastFPS; }

  private:
    bool enable = false;
    uint32_t frames = 0;
    millis_t lastTime;
    millis_t lastFrame;
    millis_t maxFrame;
    millis_t minFrame;

    float _lastFPS;

    millis_t lastIdle = 0;
    millis_t maxIdle = 0;
};

#endif
