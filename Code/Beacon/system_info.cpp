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

float System_Info_GetInternalTemperature()
{
  unsigned int wADC;
  double t;

  ADMUX = (_BV(REFS1) | _BV(REFS0) | _BV(MUX3));
  ADCSRA |= _BV(ADEN);

  delay(20);            

  ADCSRA |= _BV(ADSC);  

  while (bit_is_set(ADCSRA,ADSC));

  wADC = ADCW;

  // The offset of 317.31 could be wrong. It is just an indication.
  t = (wADC - 317.31f ) / 1.22f;

  // degrees celcius
  return t;
}
