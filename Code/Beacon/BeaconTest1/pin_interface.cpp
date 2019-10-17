#include <Arduino.h>
#include <RadioLib.h>
#include <Wire.h>

#include "configuration.h"
#include "pin_interface.h"
#include "power_control.h"
#include "debugging_utilities.h"

/*
 * @brief this function maps the voltages we recieve to the correct post-voltage-divider voltage.
 * 0 = 0V and 1023 = 5V initially, this accounts for the voltage division.
 */
float Pin_Interface_MapVoltageDividedReadings(int inAnalogValue)
{
  return ( (inAnalogValue / 3.3f) / 940.0f ) / 0.2f;
}

/*
 * @brief this function maps the voltages we receive to the current equivilent.
 * NO VOLTAGE DIVISION.
 */
float Pin_Interface_MapCurrentVoltageReadings(int inAnalogValue)
{
  return (inAnalogValue * 5.0f) / 1024.0f;
}




float Pin_Interface_GetBatteryChargingVoltage()
{
  int val = analogRead(ANALOG_IN_BATTERY_CHARGING_VOLTAGE_PIN);
  return Pin_Interface_MapCurrentVoltageReadings(val);
}

float Pin_Interface_GetBatteryVoltage()
{
  Power_Control_SwitchOffMPPT();
  Power_Control_Delay(50);

  int val = analogRead(ANALOG_IN_BATTERY_VOLTAGE_PIN);

  Power_Control_SwitchOnMPPT();
  Power_Control_Delay(50);

  return Pin_Interface_MapVoltageDividedReadings(val);
}

float Pin_Interface_GetSolarCellAVoltage()
{
  int val = analogRead(ANALOG_IN_SOLAR_A_VOLTAGE_PIN);
  return Pin_Interface_MapVoltageDividedReadings(val);
}

float Pin_Interface_GetSolarCellBVoltage()
{
  int val = analogRead(ANALOG_IN_SOLAR_B_VOLTAGE_PIN);
  return Pin_Interface_MapVoltageDividedReadings(val);
}

float Pin_Interface_GetSolarCellCVoltage()
{
  int val = analogRead(ANALOG_IN_SOLAR_C_VOLTAGE_PIN);
  return Pin_Interface_MapVoltageDividedReadings(val);
}

///////////////////////////////
// Battery Control Functions //
///////////////////////////////
float Pin_Interface_GetBatteryTemperature()
{
  Wire.requestFrom(BATTERY_TEMP_SENSOR_I2C_ADDR, 2);

  byte MSB = Wire.read();
  byte LSB = Wire.read();

  int TemperatureSum = 0;
  TemperatureSum = ((MSB << 8) | LSB) >> 4;

  float celsius = TemperatureSum * 0.0625f;
  return celsius;
}


float Pin_Interface_GetBoardTemperature()
{
  Wire.requestFrom(BOARD_TEMP_SENSOR_I2C_ADDR, 2);

  byte MSB = Wire.read();
  byte LSB = Wire.read();

  int TemperatureSum = 0;
  TemperatureSum = ((MSB << 8) | LSB) >> 4;

  float celsius = TemperatureSum * 0.0625f;
  return celsius;
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


///////////////////////////////
// TMP100 Temperature Sensor //
///////////////////////////////
void Pin_Interface_Set_Board_Temperature_Res()
{
  DEBUG_PRINTLN(F("Setting board temperature"));

  Wire.beginTransmission(BOARD_TEMP_SENSOR_I2C_ADDR);
    Wire.write(B00000001); //addresses the configuration register
    Wire.write(1 << 5); //writes the resolution bits
  Wire.endTransmission();

  Wire.beginTransmission(BOARD_TEMP_SENSOR_I2C_ADDR); //resets to reading the temperature
    Wire.write((byte)0x00);
  Wire.endTransmission();


  DEBUG_PRINTLN(F("Done"));
}

void Pin_Interface_Set_Battery_Temperature_Res()
{
  DEBUG_PRINTLN(F("Setting battery temperature"));

  Wire.beginTransmission(BATTERY_TEMP_SENSOR_I2C_ADDR);
    Wire.write(B00000001); //addresses the configuration register
    Wire.write(1 << 5); //writes the resolution bits
  Wire.endTransmission();

  Wire.beginTransmission(BATTERY_TEMP_SENSOR_I2C_ADDR); //resets to reading the temperature
    Wire.write((byte)0x00);
  Wire.endTransmission();

  DEBUG_PRINTLN(F("Done"));
}
