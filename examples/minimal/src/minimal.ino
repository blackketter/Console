/*
  Minimal Console Example
 */
#include "Console.h"

Console console;

// this is optional if you want to use the FPSCommand
#include "Commands/FPSCommand.h"
FPSCommand theFPSCommand;


void setup() {
  console.begin();

  console.debugln("Setup begin");

  console.debugln("Setup done");
}

void loop() {
  console.idle();

  // each time you draw a new frame you can tell the FPSCommand to note it for statistics
  theFPSCommand.newFrame();
}

