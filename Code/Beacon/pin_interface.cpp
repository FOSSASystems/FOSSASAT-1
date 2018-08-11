#include <Arduino.h>
#include <LoRaLib.h>
#include "configuration.h"
#include "pin_interface.h"

/*
 * @brief this function maps the voltages we recieve to the correct post-voltage-divider voltage.
 * 0 = 0V and 1023 = 5V initially, this accounts for the voltage division.
 */
float Pin_Interface_MapVoltageDividedReadings(int inAnalogValue)
{
  return (inAnalogValue / 4.092f) / 10.0f;
}

/*
 * @brief this function maps the voltages we receive to the current equivilent.
 * NO VOLTAGE DIVISION.
 */
float Pin_Interface_MapCurrentVoltageReadings(int inAnalogValue)
{
  return (inAnalogValue * 5.0f) / 1024.0f;
}




String Pin_Interface_GetBatteryChargingVoltage()
{
  int val = analogRead(ANALOG_IN_BATTERY_CHARGING_VOLTAGE_PIN);
  float fval = Pin_Interface_MapCurrentVoltageReadings(val);
  return String(fval, 2);
}

String Pin_Interface_GetBatteryVoltage()
{
  int val = analogRead(ANALOG_IN_BATTERY_VOLTAGE_PIN);
  float fval = Pin_Interface_MapVoltageDividedReadings(val);
  return String(fval, 2);
}

String Pin_Interface_GetTotalSolarCellVoltage()
{
  int val = analogRead(ANALOG_IN_TOTAL_SOLAR_VOLTAGE_PIN);
  float fval = Pin_Interface_MapVoltageDividedReadings(val);
  return String(fval, 2);
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
