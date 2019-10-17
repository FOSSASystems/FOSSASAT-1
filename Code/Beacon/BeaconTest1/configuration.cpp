/**
 * @file configuration.cpp
 * @brief Configurable settings for prototyping; pin layouts, lora lib settings, debug.
 * SX1278.
*/

#include <Arduino.h>
#include <RadioLib.h>

#include "configuration.h"
#include "power_control.h"
#include "debugging_utilities.h"

// RX Interrupt settings and current modem variables.
volatile bool INTERRUPT_ENABLED = true;
volatile bool RX_FLAG = false;

unsigned long LAST_TRANSMIT_STAMP = 0;
unsigned long LAST_BATTERY_CHECK_STAMP = 0;
unsigned long LAST_BATTERY_CHARGE_STAMP = 0;

volatile byte CURRENT_MODEM = 0;

bool TRANSMISSION_ENABLED = true;

int SATELLITE_RESTART_COUNTER = 0;

//////////////////////////////////////////////////
// Timers for transceiver settings transmission //
// this * 200ms is the delay between each transmission.
int RTTY_CALLSIGN_TRANSMIT_TIMER = 0;

SX1262 LORA = new Module(ANTENNA_NSS, ANTENNA_DIO0, ANTENNA_DIO1, ANTENNA_BUSY);//new Module(7, 2, 3);

// RTTY Client
RTTYClient RTTY(&LORA);

int STOPPED_COUNTER = 0;

const char* TRANSMISSION_PASSWORD = "FOSSASAT-1"; // password, fixed
char CALLSIGN[MAX_STRING_LENGTH];

bool SATELLITE_ALTERNATIVE_SF_MODE = false;


void Configuration_SetupPins()
{
    FOSSASAT_DEBUG_PRINTLN(F("Setting up pins..."));

    // Set up digital pins.
    pinMode(DIGITAL_OUT_MOSFET_1, OUTPUT);  // burns the nichrome wires.
    pinMode(DIGITAL_OUT_MOSFET_2, OUTPUT); // to deploy solar panels and antenna.
    pinMode(DIGITAL_OUT_WATCHDOG_HEARTBEAT, OUTPUT); // on system failure.

    FOSSASAT_DEBUG_PRINTLN(F("Done."));

    FOSSASAT_DEBUG_PRINTLN(F("Bringing mosfet 1 and 2 low..."));

    digitalWrite(DIGITAL_OUT_MOSFET_1, LOW);
    digitalWrite(DIGITAL_OUT_MOSFET_2, LOW);

    FOSSASAT_DEBUG_PRINTLN(F("Done."));
}
