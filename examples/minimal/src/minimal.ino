/*
  Minimal Console Example
 */
#include "Console.h"

Console console;

void setup() {
  console.begin();

  console.debugln("Setup begin");

  console.debugln("Setup done");
}

void loop() {
  console.idle();
}

