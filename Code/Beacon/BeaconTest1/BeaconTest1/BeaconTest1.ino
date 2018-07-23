/*
Name:		BeaconTest1.ino
Created:	7/20/2018
Author:	Richad Bamford (FOSSA Systems)
*/

#include <Arduino.h>
#include <EEPROM.h>
#include <string.h>
#include <LoRaLib.h>

#include "configuration.h"
#include "state_machine_declerations.h"
#include "debugging_utilities.h"
#include "persistant_storage.h"
#include "pin_interface.h"
#include "system_info.h"
#include "communication.h"
#include "deployment.h"

void setup()
{
	Serial.begin(9600);

  if (Pin_Interface_ShouldReset())
  {
    Persistant_Storage_Wipe();
  }

  System_Info_IncrementRestartCounter();

	STATE_STARTED = true;

	// Initialize the SX1278 interface with default settings.
	// See the PDF reports on previous PocketQube attempts for more info.
	Debugging_Utilities_DebugLog("SX1278 interface :\nCARRIER_FREQUENCY "
		+ String(CARRIER_FREQUENCY) + " MHz\nBANDWIDTH: "
		+ String(BANDWIDTH) + " kHz\nSPREADING_FACTOR: "
		+ String(SPREADING_FACTOR) + "\nCODING_RATE: "
		+ String(CODING_RATE) + "\nSYNC_WORD: "
		+ String(SYNC_WORD) + "\nOUTPUT_POWER: "
		+ String(OUTPUT_POWER));

	byte err_check = LORA.begin(CARRIER_FREQUENCY, BANDWIDTH, SPREADING_FACTOR, CODING_RATE, SYNC_WORD, OUTPUT_POWER);

	if (err_check == ERR_NONE)
	{
		Debugging_Utilities_DebugLog("(S) SX1278 Online!");
		STATE_TRANSMITTER_INITIALIZED = true;
	}
	else
	{
		// TODO If we reach this, we cannot communicate with the satellite, therefore we need the system to be restarted.
		Debugging_Utilities_DebugLog("(E) SX1278 Error code = 0x" + String(err_check, HEX));
		while (true);
	}

	Deployment_PowerDeploymentMosfets();
}

void loop()
{
	Pin_Interface_WatchdogHeartbeat();

	String str;
	byte state = LORA.receive(str);
 
  String signature = str.substring(0, 10);
  if (System_Info_CheckSystemSignature(signature) == false) // invalid signature
  {
    Debugging_Utilities_DebugLog("(SAFETY ERROR) Signature received differs from system signature!");
    return;
  }
  
  String function_id = str.substring(10, 11);
  String message = str.substring(11);
   
	if (TRANSMISSION_ENABLED) // cirital decision, transmission recieved to turn off transmission is REQUIRED.
	{
		///////////////
		// RECIEVING //
		///////////////
		// 5                     : Command      : Ping : N/A
		if (function_id == '5')
		{
			Communication_RecievedPing();
		}

		// 7                     : Command      : stop transmitting : N/A
		if (function_id == '7')
		{
			Communication_RecievedStopTransmitting();
		}

		// 8                     : Command      : start transmitting : N/A
		if (function_id == '8')
		{
			Communication_RecievedStartTransmitting();
		}

		//////////////////
		// TRANSMITTING //
		//////////////////

		if (STATE_STARTED)
		{
			Communication_TransmitStartedSignal();
			STATE_STARTED = false;
		}

		// 2                     : Notification : arduino stopped signal : N/A
		if (STATE_STOPPED)
		{
			Communication_TransmitStoppedSignal();
			STATE_STOPPED = false;
		}

		// 3                     : Notification : transmitter initialized success signal : N/A
		if (STATE_TRANSMITTER_INITIALIZED)
		{
			Communication_TransmitSX1278InitializedSuccess();
			STATE_TRANSMITTER_INITIALIZED = false;
		}

		// 4                     : Notification : cell and antenna deployment success : N/A
		if (STATE_DEPLOYMENT_SUCCESS)
		{
			Communication_TransmitDeploymentSuccess();
			STATE_DEPLOYMENT_SUCCESS = false;
		}

		// 6                     : Data         : Pong : N/A
		if (STATE_PING)
		{
			Communication_TransmitPong();
			STATE_PING = false;
		}

		// 9                     : Command      : transmit system infomation : N/A
		if (STATE_TRANSMIT_POWER_INFO)
		{
			Communication_TransmitPowerInfo();
			STATE_TRANSMIT_POWER_INFO = false;
		}

		STATE_TRANSMIT_POWER_INFO = true;
	}
	else
	{
		if (function_id == '8')
		{
			Communication_RecievedStartTransmitting();
		}
	}

	delay(200);
}
