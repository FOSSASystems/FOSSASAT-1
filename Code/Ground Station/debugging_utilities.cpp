/**
 * @file debugging_utilities.cpp
 * @brief Serial println abstraction with DEBUG configuration switch.
 * This is so all printlns can be traced.
*/

#include <Arduino.h>
#include <LoRaLib.h>
#include "configuration.h"
#include "debugging_utilities.h"

// branches/functions to be removed.

void Debugging_Utilities_DebugLog(String inLine)
{
	if (DEBUG)
	{
		Serial.println(inLine);
	}
}
