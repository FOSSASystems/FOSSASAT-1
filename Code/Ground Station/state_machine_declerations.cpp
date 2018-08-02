/**
 * @file state_machine_declerations.cpp
 * @brief Where the state machine booleans are held for easy access.
*/


#include "state_machine_declerations.h"


/**
 * @brief Should the ground station transmit PING message.
 */
bool STATE_TRANSMIT_PING = false;
/**
 * @brief Should the ground station transmit STOP TRANSMITTING message.
 */
bool STATE_TRANSMIT_STOP_TRANSMITTING = false;
/**
 * @brief Should the ground station transmit START TRANSMITTING message.
 */
bool STATE_TRANSMIT_START_TRANSMITTING = false;
/**
 * @brief Has the ground station "locked onto" the satellite, received at configuration message?
 * false = high bandwidth listening mode.
 * true = low bandwidth listening mode.
 * Called internally.
 */
bool HAS_REDUCED_BANDWIDTH = false;
