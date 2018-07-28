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

//////////////////////////////////////////////////
// Timers for transceiver settings transmission //
//////////////////////////////////////////////////
// this * 200ms is the delay between each transmission.
int POWER_INFO_DELAY = 2; // 400ms
int FREQ_INFO_DELAY = 5; // 1s
// we create seperate timers so we don't cause overflows.
int FREQ_INFO_TIMER = 0;
int POWER_INFO_TIMER = 0; 

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
  String withoutSignature = str.substring(10);

  int indexOfS1 = withoutSignature.indexOf('S');
  String message = withoutSignature.substring(indexOfS1);

  String function_id = withoutSignature.substring(0, indexOfS1);

  /*
  Serial.println("Signature: " + String(signature));
  Serial.println("Function ID: " + functionId);
  Serial.println("Message: " + message);
   */
               
  if (System_Info_CheckSystemSignature(signature) == false) // invalid signature
  {
    Debugging_Utilities_DebugLog("(SAFETY ERROR) Signature received differs from system signature!");
    return;
  }
   
	if (TRANSMISSION_ENABLED) // cirital decision, transmission recieved to turn off transmission is REQUIRED.
	{
		///////////////
		// RECIEVING //
		///////////////
		if (function_id == "5")
		{
			Communication_RecievedPing();
		}
		if (function_id == "7")
		{
			Communication_RecievedStopTransmitting(); // switch the TRANSMISSION_ENABLED boolean.
		}

		////////////////////////////
		// TRANSMITTING TO GROUND //
		///////////////////////////
		if (STATE_STARTED)
		{
			Communication_TransmitStartedSignal();
			STATE_STARTED = false;
		}
		if (STATE_STOPPED)
		{
			Communication_TransmitStoppedSignal();
			STATE_STOPPED = false;
		}
		if (STATE_TRANSMITTER_INITIALIZED)
		{
			Communication_TransmitSX1278InitializedSuccess();
			STATE_TRANSMITTER_INITIALIZED = false;
		}
		if (STATE_DEPLOYMENT_SUCCESS)
		{
			Communication_TransmitDeploymentSuccess();
			STATE_DEPLOYMENT_SUCCESS = false;
		}
		if (STATE_PING)
		{
			Communication_TransmitPong();
			STATE_PING = false;
		}
		if (STATE_TRANSMIT_POWER_INFO)
		{
			Communication_TransmitPowerInfo();
			STATE_TRANSMIT_POWER_INFO = false;
		}
   if (STATE_TRANSMIT_TRANSCEIVER_SETTINGS_INFO)
   {
      Communication_TransmitTransceiverInfo();
      STATE_TRANSMIT_TRANSCEIVER_SETTINGS_INFO = false;
   }

   
    // this * 200ms is the delay between each transmission.
    ///////////////////////////////
    int POWER_INFO_DELAY = 2; // 400ms
    int FREQ_INFO_DELAY = 5; // 1s
    
    int FREQ_INFO_TIMER = 0;
    int POWER_INFO_TIMER = 0; 
    FREQ_INFO_TIMER = FREQ_INFO_TIMER + 1;
    POWER_INFO_TIMER = POWER_INFO_TIMER + 1;

    if (FREQ_INFO_TIMER >= FREQ_INFO_DELAY)
    {
      FREQ_INFO_TIMER = 0;
      STATE_TRANSMIT_POWER_INFO = true;
    }
    if (POWER_INFO_TIMER >= POWER_INFO_TIMER)
    {
      POWER_INFO_TIMER = 0;
      STATE_TRANSMIT_TRANSCEIVER_SETTINGS_INFO = true;
    }
	}
	else
	{
		if (function_id == "8") // only parse the function id of 8.
		{
			Communication_RecievedStartTransmitting();
		}
	}

	delay(200);
}
