#ifndef _Commands_
#define _Commands_

// These are utility commands that are included.
// Comment out any you do not want to be included by default
#include "Commands/ConsoleCommands.h"
#include "Commands/FileCommands.h"
#include "Commands/FPSCommand.h"
#include "Commands/InfoCommand.h"


//These use a meaningful amount of string RAM on ESP8266, so disabled by default
#if !defined(ESP8266)

// useful for hardware testing.
#include "Commands/i2cCommand.h"
#include "Commands/SerialCommand.h"
#include "Commands/PinCommand.h"

#endif

#endif
