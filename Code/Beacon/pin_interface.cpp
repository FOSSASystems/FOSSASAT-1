#include <Arduino.h>
#include <LoRaLib.h>
#include "configuration.h"
#include "pin_interface.h"

int Pin_Interface_GetBatteryChargingVoltage()
{
  return analogRead(ANALOG_IN_BATTERY_CHARGING_VOLTAGE_PIN);
}

int Pin_Interface_GetBatteryVoltage()
{
  return analogRead(ANALOG_IN_BATTERY_VOLTAGE_PIN);
}

int Pin_Interface_GetTotalSolarCellVoltage()
{
  return analogRead(ANALOG_IN_TOTAL_SOLAR_VOLTAGE_PIN);
}


///////////////////////
// Watchdog control  //
///////////////////////
bool WATCHDOG_HEARTBEAT_STATE = false;

void Pin_Interface_WatchdogHeartbeat()
{
	if (WATCHDOG_HEARTBEAT_STATE == false)
	{
		digitalWrite(DIGITAL_OUT_WATCHDOG_HEARTBEAT, LOW);
	}
	else
	{
		digitalWrite(DIGITAL_OUT_WATCHDOG_HEARTBEAT, HIGH);
	}

	WATCHDOG_HEARTBEAT_STATE = !WATCHDOG_HEARTBEAT_STATE;
}

////////////////
// EEPROM pin //
////////////////
bool Pin_Interface_ShouldReset()
{
	return (digitalRead(DIGITAL_IN_RESET_PERSISTANT_STORAGE) == HIGH);
}
