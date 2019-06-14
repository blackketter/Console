#ifndef _FPSCommand_
#include "Clock.h"
#include "Command.h"

class FPSCommand : public Command {
  public:
    const char* getName() { return "fps"; }
    const char* getHelp() { return "toggle displaying frame rate in fps"; }
    void execute(Stream* c, uint8_t paramCount, char** params) {
      enable = !enable;
      c->printf("FPS display %s\n", enable ? "enabled" : "disabled");
      cons = c;
    }
    void newFrame() {
      frames++;
      millis_t now = Uptime::millis();
      millis_t frameDur = now - lastFrame;
      if (frameDur > maxFrame) { maxFrame = frameDur; }
      if (frameDur < minFrame) { minFrame = frameDur; }
      if (now - lastTime > 1000) {
       _lastFPS = ((float)(frames*1000))/(now - lastTime);
        if (enable && cons) {
          cons->printf("FPS: %.2f (Max frame: %dms, Min frame: %dms, Max idle: %dms)\n", _lastFPS, (int)maxFrame, (int)minFrame, (int)maxIdle);
        }
        lastTime = now;
        frames = 0;
        maxFrame = 0;
        minFrame = 10000;
        maxIdle = 0;
      }
      lastFrame = now;
    }

    void idled() {
      millis_t now = Uptime::millis();
      millis_t idleDur = now - lastIdle;
      if (idleDur > maxIdle) { maxIdle = idleDur; }
      lastIdle = now;
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
    Stream* cons = nullptr;
};

#endif
