/*
Name:		BeaconTest1.ino
Created:	7/20/2018
Author:	Richad Bamford (FOSSA Systems)
*/

#include <Arduino.h>
#include <EEPROM.h>
#include <string.h>
#include <LoRaLib.h>
#include <Wire.h>

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
// this * 200ms is the delay between each transmission.
int POWER_INFO_DELAY = 2; // 400ms
int TUNE_TIMER_DELAY = 5; // 1s

int TUNE_TIMER = 0;
int POWER_INFO_TIMER = 0;
    

void setup()
{
  Wire.begin();
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

/*
 * 
 * 
 * @test Signature malforming during transmission?
 * 
 * @todo Create interval adjustments based on power level.
 */
void loop()
{
	Pin_Interface_WatchdogHeartbeat();

	String str;
	byte state = LORA.receive(str);
  
  String signature = str.substring(0, 10);
  String withoutSignature = str.substring(10);

  int indexOfS1 = withoutSignature.indexOf(';');
  String message = withoutSignature.substring(indexOfS1 + 1);

  String function_id = withoutSignature.substring(0, indexOfS1);

  /*
  Serial.println("Signature: " + String(signature));
  Serial.println("Function ID: " + functionId);
  Serial.println("Message: " + message);
   */
               
  if (System_Info_CheckSystemSignature(signature) == false) // invalid signature
  {
    Debugging_Utilities_DebugLog("(SAFETY WARNING) Signature received differs from system signature!");
  }
   
	if (TRANSMISSION_ENABLED) // cirital decision, transmission recieved to turn off transmission is REQUIRED.
	{
    Debugging_Utilities_DebugLog("Invoking the comms subsystem, expecting a print...");
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
   if (STATE_TRANSMIT_TUNE)
   {
      Communication_TransmitTune();
      STATE_TRANSMIT_TUNE = false;
   }

   
    TUNE_TIMER = TUNE_TIMER + 1;
    POWER_INFO_TIMER = POWER_INFO_TIMER + 1;

    if (POWER_INFO_TIMER >= POWER_INFO_DELAY)
    {
      POWER_INFO_TIMER = 0;
      STATE_TRANSMIT_POWER_INFO = true;
    }
    if (TUNE_TIMER >= TUNE_TIMER_DELAY)
    {
      TUNE_TIMER = 0;
      STATE_TRANSMIT_TUNE = true;
    }
    Debugging_Utilities_DebugLog("End of comms system.");
    
	}
	else
	{
    Debugging_Utilities_DebugLog("Transmission disabled... Listening for function id '8'");
    
    if (function_id == "8") // only parse the function id of 8.
    {
      Communication_RecievedStartTransmitting();
    }
    
    Debugging_Utilities_DebugLog("End listening for function id '8'");
	}

  delay(200);
  
  Debugging_Utilities_DebugLog("Checking I2C input for data.");

  Wire.requestFrom(8, 32);    // request 32 bytes from slave device #8

  String payloadTransmissionMessage = String("");
  
  while (Wire.available())
  { // slave may send less than requested
    char c = Wire.read(); // receive a byte as character
    payloadTransmissionMessage += String(c);
  }

  if (payloadTransmissionMessage != "")
  {
    Communication_SX1278TransmitPayloadMessage(payloadTransmissionMessage);
  }
  else
  {
    // Debugging_Utilities_DebugLog("No message to send");
  }

	delay(1000);
}
