#ifndef DEBUGGING_UTILITIES_H_INCLUDED
#define DEBUGGING_UTILITIES_H_INCLUDED

#include "Simulator.h"

// uncomment to enable debug output
// RadioLib debug can be enabled in RadioLib/src/TypeDef.h
#define FOSSASAT_DEBUG

#ifdef FOSSASAT_DEBUG
  #define FOSSASAT_DEBUG_BEGIN(...) { Serial.begin(__VA_ARGS__); }
  #define FOSSASAT_DEBUG_PRINT(...) { Serial.print(__VA_ARGS__); }
  #define FOSSASAT_DEBUG_PRINTLN(...) { Serial.println(__VA_ARGS__); }
  #define FOSSASAT_DEBUG_WRITE(...) { Serial.write(__VA_ARGS__); }
  #define FOSSASAT_DEBUG_PRINT_BUFF(BUFF, LEN) { \
    for(size_t i = 0; i < LEN; i++) { \
      Serial.print(F("0x")); \
      Serial.print(BUFF[i], HEX); \
      Serial.print('\t'); \
      Serial.write(BUFF[i]); \
      Serial.println(); \
    } }
  #define FOSSASAT_DEBUG_DELAY(MS) { delay(MS); }
#else
  #define FOSSASAT_DEBUG_BEGIN(...) {}
  #define FOSSASAT_DEBUG_PRINT(...) {}
  #define FOSSASAT_DEBUG_PRINTLN(...) {}
  #define FOSSASAT_DEBUG_WRITE(...) {}
  #define FOSSASAT_DEBUG_PRINT_BUFF(BUFF, LEN) {}
#define FOSSASAT_DEBUG_DELAY(MS) {}
#endif

#endif
