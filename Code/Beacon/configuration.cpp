/**
 * @file configuration.cpp
 * @brief Configurable settings for prototyping; pin layouts, lora lib settings, debug.
 * SX1278.
*/


#include <Arduino.h>
#include <LoRaLib.h>
#include "configuration.h"

/* Should we print debug infomation? */
bool DEBUG = true;
/* Should we print debug infomation to the Serial package? */
bool DEBUG_SERIAL = true;
/* Should we print debug infomation through the transmittor? (NO) */
bool DEBUG_SX1278 = false;


/* (internal) Disables transmission and only listens for a start transmitting message. */
bool TRANSMISSION_ENABLED = true;


/* EEPROM addreses */
int EEPROM_DEPLOYMENT_ADDR = 1;
/* EEPROM addreses */
int EEPROM_RESTART_COUNTER_ADDR = 4;

///////////////////////
// Anlog in voltages //
///////////////////////
int ANALOG_IN_SOLAR_CELL_1_PIN = 0;
int ANALOG_IN_SOLAR_CELL_2_PIN = 1;
int ANALOG_IN_SOLAR_CELL_3_PIN = 2;
int ANALOG_IN_SOLAR_CELL_4_PIN = 3;
int ANALOG_IN_SOLAR_CELL_5_PIN = 4;
int ANALOG_IN_CHARGING_CURRENT_PIN = 5;

///////////////////////////////
// System state control pins //
///////////////////////////////
int DIGITAL_OUT_MOSFET_1 = 4; // set to high after 5s, then never set again.
int DIGITAL_OUT_MOSFET_2 = 5; // set to high after 5s, then never set again.
int DIGITAL_OUT_WATCHDOG_HEARTBEAT = 6; // if the system stops pinging the watching will restart.
int DIGITAL_IN_DEPLOYMENT_STATE = 11; // state of the deployment mechanism, HIGH = deployment success, LOW = deployment failed.
int DIGITAL_IN_RESET_PERSISTANT_STORAGE = 8;

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
float BANDWIDTH = 7.8f; // KHz
float LOCATION_BANDWIDTH = 62.5f; // KHz
int SPREADING_FACTOR = 12;
int CODING_RATE = 8; // can be 12.
char SYNC_WORD = 0x13;
int OUTPUT_POWER = 17; // dBm

String TRANSMISSION_SIGNATURE = "FOSSASAT-1";

void Configuration_SetupPins()
{
	// Set up digital pins.
	pinMode(OUTPUT, DIGITAL_OUT_MOSFET_1);  // burns the nichrome wires.
	pinMode(OUTPUT, DIGITAL_OUT_MOSFET_2); // to deploy solar panels and antenna.
	pinMode(OUTPUT, DIGITAL_OUT_WATCHDOG_HEARTBEAT); // on system failure.
	pinMode(INPUT, DIGITAL_IN_DEPLOYMENT_STATE); // deployment signal/flag.
	pinMode(INPUT, DIGITAL_IN_RESET_PERSISTANT_STORAGE);
}

