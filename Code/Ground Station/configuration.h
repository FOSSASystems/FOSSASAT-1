#ifndef CONFIGURATION_H_INCLUDE
#define CONFIGURATION_H_INCLUDE

#include <LoRaLib.h>

extern bool DEBUG;

extern bool AUTOMATIC_TUNING;

/////////////////////////
// SX1278 antenna pins //
// ------------------- //
// NSS pin:   7       //
// DIO0 pin:  2        //
// DIO1 pin:  3        //
/////////////////////////
extern SX1278 LORA;

///////////////////////////
// LoRa Antenna Settings //
///////////////////////////
extern float CARRIER_FREQUENCY; // MHz
extern float LOCATION_BANDWIDTH; // kHz
extern float BANDWIDTH; // kHz
extern int SPREADING_FACTOR;
extern int CODING_RATE; // can be 12.
extern char SYNC_WORD;
extern int OUTPUT_POWER; // dBm

////////////////////////////
// Transmission signature //
////////////////////////////
extern String TRANSMISSION_SIGNATURE;

#endif
