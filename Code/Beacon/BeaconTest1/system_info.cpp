#include <Arduino.h>
#include "configuration.h"
#include "persistant_storage.h"
#include "system_info.h"
#include "debugging_utilities.h"

int System_Info_GetResetCounter()
{
  return SATELLITE_RESTART_COUNTER;
}

void System_Info_IncrementRestartCounter()
{
	int v = Persistant_Storage_Get(EEPROM_RESTART_COUNTER_ADDR);
	Persistant_Storage_Set(EEPROM_RESTART_COUNTER_ADDR, v + 1);
}

void System_Info_LoadRestartCounter()
{
  SATELLITE_RESTART_COUNTER = Persistant_Storage_Get(EEPROM_RESTART_COUNTER_ADDR);
}

const char* System_Info_GetTransmissionPassword()
{
	return TRANSMISSION_PASSWORD;
}

/*
 * @todo ensure this callsign is properly parsed
 */
void System_Info_SetCallsign(char* inCallsign)
{
  if (strlen(inCallsign) > MAX_STRING_LENGTH) return;

  strcpy(CALLSIGN, inCallsign);
}

char* System_Info_GetCallsign()
{
  return CALLSIGN;
}
