/**
 * @file state_machine_declerations.cpp
 * @brief Where the state machine booleans are held for easy access.
*/


#include "state_machine_declerations.h"


/**
 * @breif Should the ground station transmit PING message.
 */
bool STATE_TRANSMIT_PING = false;
/**
 * @breif Should the ground station transmit STOP TRANSMITTING message.
 */
bool STATE_TRANSMIT_STOP_TRANSMITTING = false;
/**
 * @breif Should the ground station transmit START TRANSMITTING message.
 */
bool STATE_TRANSMIT_START_TRANSMITTING = false;
/**
 * @breif Has the ground station "locked onto" the satellite, received at configuration message?
 * \nfalse = high bandwidth listening mode.
 * \ntrue = low bandwidth listening mode.
 * \nCalled internally.
 */
bool HAS_REDUCED_BANDWIDTH = false;
