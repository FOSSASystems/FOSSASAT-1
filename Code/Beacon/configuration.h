#ifndef CONFIGURATION_H_INCLUDED
#define CONFIGURATION_H_INCLUDED

#include <LoRaLib.h>

/////////////////////////////////////////////
// IN HOUSE PROTOTYPE DEBUG                //
/////////////////////////////////////////////
extern bool DEBUG;   // whether to print debug infomation at all.
extern bool DEBUG_SERIAL; // whether to print debug info to serial.
extern bool DEBUG_SX1278; // whether to send debug info to SX1278.

//////////////////////////
// Transmission control //
//////////////////////////
extern bool TRANSMISSION_ENABLED; // when disabled will only parse transmission on message.

//////////////////////
// EEPROM Addresses //
//////////////////////
extern int EEPROM_DEPLOYMENT_ADDR;
extern int EEPROM_RESTART_COUNTER_ADDR;

///////////////////////
// Anlog in voltages //
///////////////////////
extern int ANALOG_IN_TOTAL_SOLAR_VOLTAGE_PIN;
extern int ANALOG_IN_BATTERY_CHARGING_VOLTAGE_PIN;
extern int ANALOG_IN_BATTERY_VOLTAGE_PIN;

///////////////////////////////
// System state control pins //
///////////////////////////////
extern int DIGITAL_OUT_MOSFET_1; // set to high after 5s, then never set again.
extern int DIGITAL_OUT_MOSFET_2; // set to high after 5s, then never set again.
extern int DIGITAL_OUT_WATCHDOG_HEARTBEAT; // if the system stops pinging the watching will restart.
extern int DIGITAL_IN_DEPLOYMENT_STATE; // state of the deployment mechanism, HIGH = deployment success, LOW = deployment failed.
extern int DIGITAL_IN_RESET_PERSISTANT_STORAGE;

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
extern float BANDWIDTH; // KHz
extern float LOCATION_BANDWIDTH; // KHz
extern int SPREADING_FACTOR;
extern int CODING_RATE; // can be 12.
extern char SYNC_WORD;
extern int OUTPUT_POWER; // dBm

////////////////////////////
// Transmission signature //
////////////////////////////
extern String TRANSMISSION_SIGNATURE;
extern String TRANSMISSION_PASSCODE;

void Configuration_SetupPins();

#endif
