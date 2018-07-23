#include <LoRaLib.h>
#include "configuration.h"

bool DEBUG = true;

/////////////////////////
// SX1278 antenna pins //
// ------------------- //
// NSS pin:   7       //
// DIO0 pin:  2        //
// DIO1 pin:  3        //
/////////////////////////
SX1278 LORA = new LoRa(7, 2, 3);

///////////////////////////
// LoRa Antenna Settings //
///////////////////////////
float CARRIER_FREQUENCY = 434.0f; // MHz
float BANDWIDTH = 7.8f; // kHz
int SPREADING_FACTOR = 12;
int CODING_RATE = 8; // can be 12.
char SYNC_WORD = 0x13;
int OUTPUT_POWER = 17; // dBm

String TRANSMISSION_SIGNATURE = "FOSSASAT-1";
