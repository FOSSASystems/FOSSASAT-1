#include <Arduino.h>
#include <LoRaLib.h>
#include "configuration.h"
#include "debugging_utilities.h"

// branches/functions to be removed.

void Debugging_Utilities_DebugPrintLine(String inString)
{
	if (DEBUG)
	{
		if (DEBUG_SERIAL)
		{
			Serial.println(inString);
		}
	}
}

void Debugging_Utilities_DebugPrint(String inString)
{
  if (DEBUG)
  {
    if (DEBUG_SERIAL)
    {
      Serial.print(inString + String(" "));
    }
  }
}

