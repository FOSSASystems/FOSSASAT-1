#include <LoRaLib.h>
#include "configuration.h"

bool DEBUG = true;

// Should use the Transceiver settings message to tune its own antenna.
bool AUTOMATIC_TUNING = true;

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
float DEFAULT_CARRIER_FREQUENCY = 434.0f; // MHz
float BANDWIDTH = 62.5f; // kHz (this is the bandwidth the ground station starts with)
float CONNECTED_BANDWIDTH = 7.8f; // kHz (this is the bandwidth that the ground station uses after finding a satellite transmission)
int SPREADING_FACTOR = 12;
int CODING_RATE = 8; // can be 12.
char SYNC_WORD = 0x13;
int OUTPUT_POWER = 17; // dBm

String TRANSMISSION_SIGNATURE = "FOSSASAT-1";
