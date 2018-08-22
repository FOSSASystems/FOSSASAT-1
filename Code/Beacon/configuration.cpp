/**
 * @file configuration.cpp
 * @brief Configurable settings for prototyping; pin layouts, lora lib settings, debug.
 * SX1278.
*/
 
 
#include <Arduino.h>
#include <LoRaLib.h>
#include "configuration.h"
 
bool ENABLE_I2C_BUS = false;
 
//////////////////////////////////////////////////
// Timers for transceiver settings transmission //
// this * 200ms is the delay between each transmission.
int POWER_INFO_DELAY = 2; // 400ms
int TUNE_TIMER_DELAY = 5; // 1s
 
int TUNE_TIMER = 0;
int POWER_INFO_TIMER = 0;
 
/* Should we print debug infomation? */
bool DEBUG = true;
/* Should we print debug infomation to the Serial package? */
bool DEBUG_SERIAL = true;
/* Should we print debug infomation through the transmittor? (NO) */
bool DEBUG_SX1278 = false;
 
int VOLTAGE_DECIMAL_PLACES = 2;
 
/* EEPROM addreses */
int EEPROM_DEPLOYMENT_ADDR = 1;
/* EEPROM addreses */
int EEPROM_RESTART_COUNTER_ADDR = 4;
 
int EEPROM_TRANSMISSION_STATE_ADDR = 8;
 
///////////////////////
// Anlog in voltages //
///////////////////////
int ANALOG_IN_TOTAL_SOLAR_VOLTAGE_PIN = 3;
int ANALOG_IN_BATTERY_CHARGING_VOLTAGE_PIN = 1;
int ANALOG_IN_BATTERY_VOLTAGE_PIN = 2;
 
///////////////////////////////
// System state control pins //
///////////////////////////////
int DIGITAL_OUT_MOSFET_1 = 9; // set to high after 5s, then never set again.
int DIGITAL_OUT_MOSFET_2 = 8; // set to high after 5s, then never set again.
int DIGITAL_OUT_WATCHDOG_HEARTBEAT = 4; // if the system stops pinging the watching will restart.
int DIGITAL_IN_DEPLOYMENT_STATE = 5; // state of the deployment mechanism, HIGH = deployment success, LOW = deployment failed.
int DIGITAL_IN_RESET_PERSISTANT_STORAGE = 6;
int DIGITAL_OUT_SX1278_DIRECT = 10;
 
/////////////////////////
// SX1278 antenna pins //
// ------------------- //
// NSS pin:   7       //
// DIO0 pin:  2        //
// DIO1 pin:  3        //
/////////////////////////
SX1278 LORA = new LoRa(7, 2, 3);
 
/////////////////////////////
// LoRa chip configuration //
/////////////////////////////
float CARRIER_FREQUENCY = 434.0f; // MHz
float BANDWIDTH = 62.5f; // KHz low bandiwdth mode for communicating with ground stations
float LOCATION_BANDWIDTH = 125.0f; // KHz high bandiwdth mode for locating the ground stations
int SPREADING_FACTOR = 11;
int CODING_RATE = 8; // can be 12.
char SYNC_WORD = 0x13;
int OUTPUT_POWER = 10; // dBm
 
////////////////////////////////
// FSK RTTY Protocol settings //
////////////////////////////////
int RTTY_BASE = 2125; // Hz
int RTTY_SHIFT = 170; // 3 * 61Hz
int RTTY_BAUD_RATE = 22222; // micro-seconds (us)
 
String TRANSMISSION_SIGNATURE = "FOSSASAT-1";
 
 
void Configuration_SetupPins()
{
    // Set up digital pins.
    pinMode(OUTPUT, DIGITAL_OUT_MOSFET_1);  // burns the nichrome wires.
    pinMode(OUTPUT, DIGITAL_OUT_MOSFET_2); // to deploy solar panels and antenna.
    pinMode(OUTPUT, DIGITAL_OUT_WATCHDOG_HEARTBEAT); // on system failure.
    pinMode(OUTPUT, DIGITAL_OUT_SX1278_DIRECT);
    pinMode(INPUT, DIGITAL_IN_DEPLOYMENT_STATE); // deployment signal/flag.
    pinMode(INPUT, DIGITAL_IN_RESET_PERSISTANT_STORAGE);
}
