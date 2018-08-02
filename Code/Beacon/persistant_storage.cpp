#include <Arduino.h>
#include <LoRaLib.h>
#include <EEPROM.h>
#include "persistant_storage.h"

////////////////////
// EEPROM Control //
////////////////////
void Persistant_Storage_Wipe()
{
	for (int i = 0; i < EEPROM.length(); i++)
	{
		EEPROM.write(i, 0);
	}
}

int Persistant_Storage_Get(int inAddress)
{
	return EEPROM.read(inAddress);
}

void Persistant_Storage_Set(int inAddress, int inValue)
{
	EEPROM.write(inAddress, inValue);
}
