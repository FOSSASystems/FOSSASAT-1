/**
 * @file configuration.cpp
*/

#include <LoRaLib.h>
#include "configuration.h"

/**
* @breif This flag sets whether the ground station should print through the DebugLog via serial.
* Upon prototype finishing, this should be removed along with the debugging functions, this servers as a temporary development flag.
*/
/// Should the ground station output to serial.
bool DEBUG = true;

/**
* @breif This flag sets whether the ground station should tune itself from the Function Id "10" transmissions.
* only used for prototyping, if the tuning system fails to allow hardware debugging.
*/
/// Should use the Transceiver settings message to tune its own antenna.
bool AUTOMATIC_TUNING = true;

/** SX1278 pin layout
* @breif
* NSS PIN = 7
* DIO0 PIN = 2
* DIO1 PIN = 3
*/
SX1278 LORA = new LoRa(7, 2, 3);

/* @breif This is the carrier frequency, however, this variable is tuned by the system. Therefore it is not constant.*/
///Transmission frequency measured in MHz (varies).
float CARRIER_FREQUENCY = 434.0f;

/* @breif This is the default carrier frequency, this value is constant, used for switching back into wide bandwidth listening modes*/
///Transmission frequency measured in MHz (constant).
float DEFAULT_CARRIER_FREQUENCY = 434.0f;

/* @breif This is the value for the high bandwidth mode.*/
///Transmission bandwidth measured in KHz
float BANDWIDTH = 62.5f;

/* @breif This is the value for the low bandwidth (satellite found) mode.*/
///Transmission bandwidth measured in KHz (varies).
float CONNECTED_BANDWIDTH = 7.8f;

///Chip duration
int SPREADING_FACTOR = 12;

///Cod erate
int CODING_RATE = 8; // can be 12.

///Start of transmission frame preamble.
char SYNC_WORD = 0x13;

///Amount of power of radio frequency transmitted
int OUTPUT_POWER = 17; // dBm

///The unique identifyer for this satellite, used as a signature for transmissions.
String TRANSMISSION_SIGNATURE = "FOSSASAT-1";
