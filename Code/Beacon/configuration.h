#ifndef CONFIGURATION_H_INCLUDED
#define CONFIGURATION_H_INCLUDED

#include <LoRaLib.h>

extern bool ENABLE_I2C_BUS;

//////////////////////////////////////////////////
// Timers for transceiver settings transmission //
/////////////////////////////////////////////////
// this * 200ms is the delay between each transmission.
extern int POWER_INFO_DELAY ; // 400ms
extern int TUNE_TIMER_DELAY; // 1s

extern int TUNE_TIMER;
extern int POWER_INFO_TIMER;

/////////////////////////////////////////////
// IN HOUSE PROTOTYPE DEBUG                //
/////////////////////////////////////////////
extern bool DEBUG;   // whether to print debug infomation at all.
extern bool DEBUG_SERIAL; // whether to print debug info to serial.
extern bool DEBUG_SX1278; // whether to send debug info to SX1278.

extern int VOLTAGE_DECIMAL_PLACES;

//////////////////////////
// Transmission control //
//////////////////////////
extern bool TRANSMISSION_ENABLED; // when disabled will only parse transmission on message.

//////////////////////
// EEPROM Addresses //
//////////////////////
extern int EEPROM_DEPLOYMENT_ADDR;
extern int EEPROM_RESTART_COUNTER_ADDR;
extern int EEPROM_TRANSMISSION_STATE_ADDR;

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

/////////////////////////////
// SX1278 Chip Config      //
/////////////////////////////
extern float CARRIER_FREQUENCY; // MHz
extern float BANDWIDTH; // KHz
extern float LOCATION_BANDWIDTH; // KHz
extern int SPREADING_FACTOR;
extern int CODING_RATE; // can be 12.
extern char SYNC_WORD;
extern int OUTPUT_POWER; // dBm

////////////////////////////////////////
// SX1278 FSK Direct Mode RTTY Config //
////////////////////////////////////////
extern int RTTY_BASE; // hz, base tone for 0.
extern int RTTY_SHIFT; // hz, offset for tone of 1.
// baud rate    delay [us]
// 45           22222
// 45.45        22000
// 50           20000
extern int RTTY_BAUD_RATE; // microseconds (222222)

////////////////////////////
// Transmission signature //
////////////////////////////
extern String TRANSMISSION_SIGNATURE;
extern String TRANSMISSION_PASSCODE;

void Configuration_SetupPins();

#endif
