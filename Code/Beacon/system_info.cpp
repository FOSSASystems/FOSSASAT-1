#include <Arduino.h>
#include "configuration.h"
#include "persistant_storage.h"
#include "system_info.h"

int System_Info_GetResetCounter()
{
	return Persistant_Storage_Get(EEPROM_RESTART_COUNTER_ADDR);
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

/* @breif platform dependent float values, very unusual behaviour...
 *  
 *  @test 1023 integer input
 *  @test 0 integer input
 *  @test negative value input
 *  @test inf value input
 */
String System_Info_MapValue(float inValue, float inMin, float inMax, float outMin, float outMax)
{
  float value = ( ((inValue - inMin) * (outMax - outMin)) / (inMax - inMin) ) + outMin;

  return String(value, DEC);
}

