#include <Arduino.h>
#include <LoRaLib.h>
#include "configuration.h"
#include "pin_interface.h"

////////////////////////////
// Analog input interface //
////////////////////////////
int Pin_Interface_GetSolarCellVoltage(int inCellIndex)
{
	if (inCellIndex == 1) return analogRead(ANALOG_IN_SOLAR_CELL_1_PIN);
	if (inCellIndex == 2) return analogRead(ANALOG_IN_SOLAR_CELL_2_PIN);
	if (inCellIndex == 3) return analogRead(ANALOG_IN_SOLAR_CELL_3_PIN);
	if (inCellIndex == 4) return analogRead(ANALOG_IN_SOLAR_CELL_4_PIN);
	if (inCellIndex == 5) return analogRead(ANALOG_IN_SOLAR_CELL_5_PIN);
}

int Pin_Interface_GetBatteryChargingCurrent()
{
	return analogRead(ANALOG_IN_CHARGING_CURRENT_PIN);
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
