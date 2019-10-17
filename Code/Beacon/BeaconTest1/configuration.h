#ifndef CONFIGURATION_H_INCLUDED
#define CONFIGURATION_H_INCLUDED

#include <RadioLib.h>

//#define ENABLE_DEPLOYMENT_SEQUENCE
#define ENABLE_INTERVAL_CONTROL

#define TRANSMISSION_REPETITION_COUNT 25
#define RECEPTION_REPETITION_COUNT 25 // some reason the receive mode repeating itself breaks.

#define ENABLE_RTTY true


#define BOARD_TEMP_SENSOR_I2C_ADDR B1001011
#define BATTERY_TEMP_SENSOR_I2C_ADDR B1001011

//////////////////////////////////////////////////
// Timers for transceiver settings transmission //
//////////////////////////////////////////////////
#define RTTY_CALLSIGN_TRANSMIT_DELAY 10

/* Should we print debug infomation? */
#define DEBUG
#define ENABLE_INTERVAL_CONTROL

#define VOLTAGE_DECIMAL_PLACES 2

/* EEPROM addreses */
#define EEPROM_DEPLOYMENT_ADDR 0 // 0, 1
#define EEPROM_RESTART_COUNTER_ADDR 2 // 2, 3
#define EEPROM_TRANSMISSION_STATE_ADDR 4 // 4,5
#define EEPROM_LOW_POWER_MODE_ADDR 6
#define EEPROM_LOW_POWER_MODE_ENABLED_ADDR 8
#define EEPROM_MPPT_TEMPERATURE_SWITCH_ENABLED_ADDR 10
#define EEPROM_MPPT_KEEP_ALIVE_MODE_ENABLED_ADDR 12
#define EEPROM_FIRST_RUN_ADDR 14

//////////////////////////
// Battery control pins //
//////////////////////////
#define ANALOG_OUT_BATTERY_CHARGING_PIN 0

/////////////////////////
// MPPT reading switch //
/////////////////////////
#define ANALOG_OUT_MPPT_READING_PIN 0

///////////////////////
// Anlog in voltages //
///////////////////////
#define ANALOG_IN_SOLAR_A_VOLTAGE_PIN 3
#define ANALOG_IN_SOLAR_B_VOLTAGE_PIN 3
#define ANALOG_IN_SOLAR_C_VOLTAGE_PIN 3
#define ANALOG_IN_BATTERY_CHARGING_VOLTAGE_PIN 1
#define ANALOG_IN_BATTERY_VOLTAGE_PIN 2

///////////////////////////////
// System state control pins //
///////////////////////////////
#define DIGITAL_OUT_MOSFET_1 9 // set to high after 5s, then never set again.
#define DIGITAL_OUT_MOSFET_2 8 // set to high after 5s, then never set again.
#define DIGITAL_OUT_WATCHDOG_HEARTBEAT 4 // if the system stops pinging the watching will restart.

/////////////////////////
// SX1278 antenna pins //
// ------------------- //
// NSS pin:   7       //
// DIO0 pin:  2        //
// DIO1 pin:  3        //
/////////////////////////
#define ANTENNA_NSS 7
#define ANTENNA_DIO0  2
#define ANTENNA_DIO1 3
#define ANTENNA_BUSY 9
#define TXC_REF_VOLTAGE 1.6f

/////////////////////////////////////
// Radio (Lora chip) configuration //
/////////////////////////////////////

////////////////////
// INTERRUPT PIN //
///////////////////
#define DIGITAL_PIN_FOR_INTERRUPT 2

///////////////////
// DELAY TIMINGS //
///////////////////
#define TRANSMIT_DELAY 10000 // 10 seconds
#define BATTERY_CHECK_DELAY 300000 // 5 minutes
#define MODEM_SWITCHING_TIME 1000 // 1 second
#define BATTERY_CHARGE_DELAY 60000 // 1 minute

// All configurations.
#define SYNC_WORD_A 0x0F
#define SYNC_WORD_B 0x0F

#define LOWER_SPREADING_FACTOR 7

// Non ISM band LORA mode
#define CARRIER_FREQUENCY 434.0f // MHz
#define BANDWIDTH 125.0f //62.5f // KHz low bandiwdth mode for communicating with ground stations
#define SPREADING_FACTOR 11
#define ALTERNATIVE_SPREADING_FACTOR 10
#define CODING_RATE 8 // can be 12.
#define OUTPUT_POWER 6 // dBm
#define CURRENT_LIMIT 60 // 60mA

// ISM band LORA mode.
#define ISM_CARRIER_FREQUENCY 433.0f
#define ISM_BANDWIDTH 125.0f
#define ISM_SPREADING_FACTOR 11
#define ISM_ALTERNATIVE_SPREADING_FACTOR 10
#define ISM_CODING_RATE 8 // can be 12.
#define ISM_OUTPUT_POWER 10 // dBm
#define ISM_CURRENT_LIMIT 60 // 60mA

// Non ISM band FSK mode.
#define FSK_CARRIER_FREQUENCY 434.0f
#define FSK_FREQUENCY_DERIVATION 10.0
#define FSK_RX_BANDWIDTH 250.0
#define FSK_BIT_RATE 100.0
#define FSK_DATA_SHAPING 0.5
#define FSK_OUTPUT_POWER 10 // dBm
#define FSK_CURRENT_LIMIT 60 // 60mA


////////////////////////////////
// FSK RTTY Protocol settings //
////////////////////////////////
#define RTTY_FREQUENCY_SHIFT 183 // hz
#define RTTY_BAUDRATE 45 // 45 baud
#define RTTY_DATABITS 8 // 8 bit ascii code.
// 1 stop bit

extern volatile bool INTERRUPT_ENABLED;
extern volatile bool RX_FLAG;

extern volatile byte CURRENT_MODEM;

extern unsigned long LAST_TRANSMIT_STAMP;
extern unsigned long LAST_BATTERY_CHECK_STAMP;
extern unsigned long LAST_BATTERY_CHARGE_STAMP;

extern int RTTY_CALLSIGN_TRANSMIT_TIMER;

//////////////////////////
// Transmission control //
//////////////////////////
extern bool TRANSMISSION_ENABLED; // when disabled will only parse transmission on message.

/////////////////////////
// SX1278 antenna pins //
// ------------------- //
// NSS pin:   7       //
// DIO0 pin:  2        //
// DIO1 pin:  3        //
/////////////////////////
extern SX1262 LORA;
extern RTTYClient RTTY;

#define STOPPED_COUNTER_MAX 10 // how many items the function can be ran before we decide that it isn't working.
extern int STOPPED_COUNTER; // represents how many times the Route() function has been iterated since the stopped signal has been recieved.

#define MAX_STRING_LENGTH 32

/////////////////////////////////////
// Transmission signature/password //
////////////////////////////////////
extern const char* TRANSMISSION_PASSWORD;
extern char CALLSIGN[MAX_STRING_LENGTH];

extern int SATELLITE_RESTART_COUNTER;

extern bool SATELLITE_ALTERNATIVE_SF_MODE; // default is OFF (Sf11)

void Configuration_SetupPins();

#endif
