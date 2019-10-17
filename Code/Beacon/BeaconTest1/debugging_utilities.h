#ifndef DEBUGGING_UTILITIES_H_INCLUDED
#define DEBUGGING_UTILITIES_H_INCLUDED

#include <Arduino.h>

#ifdef DEBUG
  #define FOSSASAT_DEBUG_BEGIN(...) { Serial.begin(__VA_ARGS__); }
  #define FOSSASAT_DEBUG_PRINT(...) { Serial.print(__VA_ARGS__); }
  #define FOSSASAT_DEBUG_PRINTLN(...) { Serial.println(__VA_ARGS__); }
#else
  #define FOSSASAT_DEBUG_BEGIN(...) {}
  #define FOSSASAT_DEBUG_PRINT(...) {}
  #define FOSSASAT_DEBUG_PRINTLN(...) {}
#endif

#endif
