#include <Arduino.h>
#include "configuration.h"
#include "persistant_storage.h"
#include "system_info.h"

String System_Info_GetResetCounter()
{
	return String(Persistant_Storage_Get(EEPROM_RESTART_COUNTER_ADDR), DEC);
}

void System_Info_IncrementRestartCounter()
{
	int v = Persistant_Storage_Get(EEPROM_RESTART_COUNTER_ADDR);
	Persistant_Storage_Set(EEPROM_RESTART_COUNTER_ADDR, v + 1);
}

bool System_Info_CheckSystemSignature(String inReceivedSignature)
{
	return (TRANSMISSION_SIGNATURE == inReceivedSignature);
}

String System_Info_GetTransmissionSignature()
{
	return TRANSMISSION_SIGNATURE;
}
