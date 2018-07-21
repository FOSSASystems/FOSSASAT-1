/*
 Name:		BeaconTest1.ino
 Created:	7/20/2018
 Author:	Richad Bamford (FOSSA Systems)
*/

#include <EEPROM.h>
#include <string.h>

/////////////////////////////////////////////
// IN HOUSE PROTOTYPE DEBUG                //
/////////////////////////////////////////////
bool DEBUG = true;   // whether to print debug infomation at all.
bool DEBUG_SERIAL = true; // whether to print debug info to serial.
bool DEBUG_SX1278 = false; // whether to send debug info to SX1278.

//////////////////////////
// Transmission control //
//////////////////////////
bool TRANSMISSION_ENABLED = true; // when disabled will only parse transmission on message.

//////////////////////
// State management // (one of these is toggled, unset once transmitted)
//////////////////////
bool STATE_STARTED = false;
bool STATE_STOPPED = false;
bool STATE_TRANSMITTER_INITIALIZED = false;
bool STATE_DEPLOYMENT_SUCCESS = false;
bool STATE_PING = false;
bool STATE_TRANSMIT_POWER_INFO = false;

//////////////////////
// EEPROM Addresses //
//////////////////////
int EEPROM_DEPLOYMENT_ADDR = 1;
int EEPROM_RESTART_COUNTER_ADDR = 4;

///////////////
// Watchdog  //
///////////////
bool WATCHDOG_HEARTBEAT_STATE = false;

///////////////////////
// Anlog in voltages //
///////////////////////
int ANALOG_IN_SOLAR_CELL_1_PIN = A0;
int ANALOG_IN_SOLAR_CELL_2_PIN = A1;
int ANALOG_IN_SOLAR_CELL_3_PIN = A2;
int ANALOG_IN_SOLAR_CELL_4_PIN = A3;
int ANALOG_IN_SOLAR_CELL_5_PIN = A4;
int ANALOG_IN_CHARGING_CURRENT_PIN = A5;

///////////////////////////////
// System state control pins //
///////////////////////////////
int DIGITAL_OUT_MOSFET_1 = 4; // set to high after 5s, then never set again.
int DIGITAL_OUT_MOSFET_2 = 5; // set to high after 5s, then never set again.
int DIGITAL_OUT_WATCHDOG_HEARTBEAT = 6; // if the system stops pinging the watching will restart.
int DIGITAL_IN_DEPLOYMENT_STATE = 11; // state of the deployment mechanism, HIGH = deployment success, LOW = deployment failed.
int DIGITAL_IN_RESET_EEPROM = 8;

////////////////////
// EEPROM Control //
////////////////////
void EEPROMWipe()
{
	for (int i = 0; i < EEPROM.length(); i++)
	{
    	EEPROM.write(i, 0);
  	}
  	DebugLog("EEPROM Wiped!");
}

int EEPROMGet(int inAddress)
{
	return EEPROM.read(inAddress);
}

void EEPROMSet(int inAddress, int inValue)
{
	EEPROM.write(inAddress, inValue);
}

//////////////////////////
// DEPLOYMENT FUNCTIONS //
//////////////////////////
void PowerDeploymentMosfets()
{
	int deployState = EEPROMGet(EEPROM_DEPLOYMENT_ADDR);
  
    // check against eeprom for already deployed.
	if (EEPROMGet(EEPROM_DEPLOYMENT_ADDR) == HIGH)
	{
		// the deployment has occured.
		digitalWrite(DIGITAL_OUT_MOSFET_1, LOW);
		digitalWrite(DIGITAL_OUT_MOSFET_2, LOW);

		return;
	}
	else
	{
		// send high signal after 5seconds if not already deployed.
		delay(5000);

		// set the eeprom value so it doesn't turn the mosfets on again.
      	EEPROMSet(EEPROM_DEPLOYMENT_ADDR, HIGH);

		// burn the nichrome wires.
		digitalWrite(DIGITAL_OUT_MOSFET_1, HIGH);
		digitalWrite(DIGITAL_OUT_MOSFET_2, HIGH);
	}

}

bool GetDeploymentState()
{
	// read from the deployment digital pin.
	int val = digitalRead(DIGITAL_IN_DEPLOYMENT_STATE);
	DebugLog("VAL = " + String(val));
	// return true if high and false if low.
	if (val == LOW)
	{
		// deployment pin returning LOW threshold, deployment failed.
		return false;
	}
	else
	{
		// deployment pin returning HIGH threshold, deployment success.
		return true;
	}
}

//////////////////////////////
// SYSTEM CONTROL FUNCTIONS //
//////////////////////////////
void WatchdogHeartbeat()
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

int GetResetCounter()
{
	return EEPROMGet(EEPROM_RESTART_COUNTER_ADDR);
}

void IncrementRestartCounter()
{
	int v = EEPROMGet(EEPROM_RESTART_COUNTER_ADDR);
	EEPROMSet(EEPROM_DEPLOYMENT_ADDR, v + 1);
}

///////////////////////////
// SYSTEM INFO FUNCTIONS //
///////////////////////////
int GetSolarCellVoltage(int inCellIndex)
{
	if (inCellIndex == 1) return analogRead(ANALOG_IN_SOLAR_CELL_1_PIN);
	if (inCellIndex == 2) return analogRead(ANALOG_IN_SOLAR_CELL_2_PIN);
	if (inCellIndex == 3) return analogRead(ANALOG_IN_SOLAR_CELL_3_PIN);
	if (inCellIndex == 4) return analogRead(ANALOG_IN_SOLAR_CELL_4_PIN);
	if (inCellIndex == 5) return analogRead(ANALOG_IN_SOLAR_CELL_5_PIN);
}

int GetBatteryChargingCurrent()
{
	return analogRead(ANALOG_IN_CHARGING_CURRENT_PIN);
}

////////////////////////////
// TRANSMISSION FUNCTIONS //
////////////////////////////
void SX1278Transmit(String inFuncId, String inMessage)  // this is hidden, use SX1278Transmit____ functions.
{
	////////////////////
	// FOSSA_PROTOCOL //
	//----------------//
	////////////////////

	//////////////////
	// Restrictions //
	// ------------ /////////////////////////////////
	// Max send length of a string 256 characters. //
	// 512 bytes in total				           //
	/////////////////////////////////////////////////

	////////////
	// Format //
	// ------ /////////////////////////////
	// 1 byte = function id (0-256)      //
	// 511 bytes = message of any format //
	///////////////////////////////////////

	///////////////////////////////////////////////
	// Function ID (char) :  Message Length :  Category       : Description  :  Type 
	// 1                     : Notification : arduino started signal :  N/A      
	// 2                     : Notification : arduino stopped signal : N/A
	// 3                     : Notification : transmitter initialized success signal : N/A
	// 4                     : Notification : cell and antenna deployment success : N/A
	// 5                     : Command      : Ping : N/A
	// 6                     : Data         : Pong : N/A
	// 7                     : Command      : stop transmitting : N/A
	// 8                     : Command      : start transmitting : N/A
	// 9                     : Command      : transmit system infomation : N/A
	// 10                    : Data         : System infomation : byte array
	///////////////////////////////////////////////

  	Serial.println("Transmitting... " + inFuncId + inMessage);
}

//////////////////////////////
// FOSSA_PROTOCOL FUNCTIONS //
//////////////////////////////

// 1                     : Notification : arduino started signal :  N/A      
void TransmitStartedSignal()
{
	SX1278Transmit("1", "");
}

// 2                     : Notification : arduino stopped signal : N/A
void TransmitStoppedSignal()
{
	SX1278Transmit("2", "");
}

// 3                     : Notification : transmitter initialized success signal : N/A
void TransmitSX1278InitializedSuccess()
{
	SX1278Transmit("3", "");
}

// 4                     : Notification : cell and antenna deployment success : N/A
void TransmitDeploymentSuccess()
{
	SX1278Transmit("4", "");
}

// 6                     : Data         : Pong : N/A
void TransmitPong()
{
	SX1278Transmit("6", "");
}

// 5                     : Command      : Ping : N/A
void RecievedPing()
{
	STATE_PING = true;
}

// 7                     : Command      : stop transmitting : N/A
void RecievedStopTransmitting()
{
	TRANSMISSION_ENABLED = false;
}

// 8                     : Command      : start transmitting : N/A
void RecievedStartTransmitting()
{
	TRANSMISSION_ENABLED = true;
}

// 9                     : Command      : transmit system infomation : N/A
void TransmitPowerInfo()
{
	String function_id = String(10);

	int solarCell1 = GetSolarCellVoltage(1);
	int solarCell2 = GetSolarCellVoltage(2);
	int solarCell3 = GetSolarCellVoltage(3);
	int solarCell4 = GetSolarCellVoltage(4);
  	int solarCell5 = GetSolarCellVoltage(5);
  
  	bool deploymentStatus = GetDeploymentState();
  DebugLog(String(deploymentStatus));
  
  	int resetCounter = GetResetCounter();

  	// TODO perform validation and verfication on these values.

	int batteryChargingCurrent = GetBatteryChargingCurrent();

	// TODO perform validation and verfication on these values.

	// TODO convert transmission protocol to byte formats.

	String sysInfoMessage = String("S1:") + String(solarCell1) + ";" + "S2:" + String(solarCell2) + ";" + "S3:" + String(solarCell3) + ";" + "S4:" + String(solarCell4) + ";" + "S5:" + String(solarCell5) + ";" + "B:" + String(batteryChargingCurrent) + "RC:" + String(resetCounter) + ";DEPS:" + String(deploymentStatus);

	SX1278Transmit(function_id, sysInfoMessage);
}


void DebugLog(String inLine)
{
	if (DEBUG)
	{
		if (DEBUG_SERIAL)
		{
			Serial.println(inLine);
		}
	}
}

void setup()
{
	Serial.begin(9600);

	STATE_STARTED = true;

	// Set up digital pins.
	pinMode(OUTPUT, DIGITAL_OUT_MOSFET_1);  // burns the nichrome wires.
	pinMode(OUTPUT, DIGITAL_OUT_MOSFET_2); // to deploy solar panels and antenna.
	pinMode(OUTPUT, DIGITAL_OUT_WATCHDOG_HEARTBEAT); // on system failure.
	pinMode(INPUT, DIGITAL_IN_DEPLOYMENT_STATE); // deployment signal/flag.
	pinMode(INPUT, DIGITAL_IN_RESET_EEPROM);
  
  	if (digitalRead(DIGITAL_IN_RESET_EEPROM) == HIGH)
    {
  		EEPROMWipe();
    }
  
	PowerDeploymentMosfets();
}

void loop()
{
  	WatchdogHeartbeat();
    
	// Get most recent message.
	String str = String("");
  
	if (TRANSMISSION_ENABLED)
	{
		///////////////
		// RECIEVING //
		///////////////

		// process every message.
		char function_id = str.charAt(0);
		String message = str.substring(1);

		// 5                     : Command      : Ping : N/A
		if (function_id == '5')
		{
			RecievedPing();
		}

		// 7                     : Command      : stop transmitting : N/A
		if (function_id == '7')
		{
			RecievedStopTransmitting();
		}

		// 8                     : Command      : start transmitting : N/A
		if (function_id == '8')
		{
			RecievedStartTransmitting();
		}

		//////////////////
		// TRANSMITTING //
		//////////////////

		if (STATE_STARTED)
		{
			TransmitStartedSignal();
			STATE_STARTED = false;
		}

		// 2                     : Notification : arduino stopped signal : N/A
		if (STATE_STOPPED)
		{
			TransmitStoppedSignal();
			STATE_STOPPED = false;
		}

		// 3                     : Notification : transmitter initialized success signal : N/A
		if (STATE_TRANSMITTER_INITIALIZED)
		{
			TransmitSX1278InitializedSuccess();
			STATE_TRANSMITTER_INITIALIZED = false;
		}

		// 4                     : Notification : cell and antenna deployment success : N/A
		if (STATE_DEPLOYMENT_SUCCESS)
		{
			TransmitDeploymentSuccess();
			STATE_DEPLOYMENT_SUCCESS = false;
		}

		// 6                     : Data         : Pong : N/A
		if (STATE_PING)
		{
			TransmitPong();
			STATE_PING = false;
		}

		// 9                     : Command      : transmit system infomation : N/A
		if (STATE_TRANSMIT_POWER_INFO)
		{
			TransmitPowerInfo();
			STATE_TRANSMIT_POWER_INFO = false;
		}

		STATE_TRANSMIT_POWER_INFO = true;
	}
	else
	{
		// process every message.
		char function_id = str.charAt(0);
		String message = str.substring(1);
      if (function_id == '8')
      {
		RecievedStartTransmitting();
      }
	}

	delay(1000);
}