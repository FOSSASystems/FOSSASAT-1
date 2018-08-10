/**
 * @file communication.cpp
 * @brief This code manages the protocol transmission. It takes raw values
 * from the system, packs them into the radio protocol and sends it through the
 * SX1278.
*/

#include "configuration.h"
#include "state_machine_declerations.h"
#include "pin_interface.h"
#include "deployment.h"
#include "system_info.h"
#include "communication.h"
#include "debugging_utilities.h"

/**
 * @brief The main transmission function.
 * @param inFuncId The number that represents the function being sent.
 * @param inMessage The string that will be transmistted to the satellite.
 * 
 * This should never be called directly in the main code body. There are
 * more specific functions that handle custom functionality.
 * 
 * @todo Debug Logging removal. 
 * @todo Transmission signature containment.
 * @test inFuncId range String(-inf) to String(inf).
 * @test inFuncId String length above 256 
 * @test InFuncId empty string.
 * @test inMessage String empty.
 * @test inMessage length above 256.
 */
void Communication_SX1278Transmit(String inFuncId, String inMessage)  // this is hidden, use SX1278Transmit____ functions.
{
	String transmissionSignature = System_Info_GetTransmissionSignature();
  String transmissionPacket = transmissionSignature + inFuncId + ";" + inMessage;

	Debugging_Utilities_DebugLog("Transmitting... " + transmissionPacket);

	if (TRANSMISSION_ENABLED)
	{
		byte state = LORA.transmit(transmissionPacket);

		if (state == ERR_NONE)
		{
			// the packet was successfully transmitted
			Debugging_Utilities_DebugLog("Packet successfully transmitted to ground station.");
		}
		else if (state == ERR_PACKET_TOO_LONG)
		{
			// the supplied packet was longer than 256 bytes
			Debugging_Utilities_DebugLog("Packet size > 256 bytes!");
		}
		else if (state == ERR_TX_TIMEOUT)
		{
			// timeout occurred while transmitting packet
			Debugging_Utilities_DebugLog("Timed out while transmitting packet.");
		}  
	}
}

 /*
 * @brief an abstraction layer for the payload transmissions.
 */
void Communication_SX1278TransmitPayloadMessage(String inPayloadMessage)
{
  int indexOfSeperator = inPayloadMessage.indexOf(';'); // first ; is the seperator between funcid and message data.
  String message = inPayloadMessage.substring(indexOfSeperator + 1); // from after the first ; to the end
  String functionId = inPayloadMessage.substring(0, indexOfSeperator);  // from the start to the first ; (exluding);

  Communication_SX1278Transmit(functionId, message);
}

/**
 * @brief satellite's Setup() function is called..
 *
 * This function indicates to the ground station that the satellite has started.
 */
void Communication_TransmitStartedSignal()
{
  Debugging_Utilities_DebugLog("(T) Transmit satellite started.");
	Communication_SX1278Transmit("1", "");
}

/**
 * @brief satellite's failure sequency function is called..
 *
 * This function indicates to the ground station that the satellite has stopped.
 */
void Communication_TransmitStoppedSignal()
{
  Debugging_Utilities_DebugLog("(T) Transmit satellite stopped.");
	Communication_SX1278Transmit("2", "");
}

/**
 * @brief satellite's SX1278 LoRa library .begin() ran successfully.
 *
 * This function indicates to the ground station that the satellite has began the LoRa object successfully.
 */
void Communication_TransmitSX1278InitializedSuccess()
{
  Debugging_Utilities_DebugLog("(T) Transmit sx1278 initialized success.");
	Communication_SX1278Transmit("3", "");
}

/**
 * @brief When the Atmega setup() is called.
 *
 * This function indicates to the ground station that the satellite has deployed successfully.
 * @todo !!! How do we handle missing packets? we could miss a deployment success and assume it has failed?
 */
void Communication_TransmitDeploymentSuccess()
{
  Debugging_Utilities_DebugLog("(T) Transmit deployment success.");
	Communication_SX1278Transmit("4", "");
}

/**
 * @brief satellite's Setup() function is called..
 *
 * This function indicates to the ground station that the satellite has started.
 */
void Communication_TransmitPong()
{
  Debugging_Utilities_DebugLog("(T) Transmit pong.");
	Communication_SX1278Transmit("6", "");
}

/**
 * @brief Satellite PING transmission received.
 *
 * This function is to test the connection using a minimal packet.
 */
void Communication_RecievedPing()
{
  Debugging_Utilities_DebugLog("(R) Received ping.");
	STATE_PING = true;
}

/**
 * @brief Satellite Stop transmission.
 *
 * This prevents the satellite from transmitting and configures it to only listen to start transmitting messages.
 */
void Communication_RecievedStopTransmitting()
{
  Debugging_Utilities_DebugLog("(R) Received stop transmitting.");
	TRANSMISSION_ENABLED = false;
}

/**
 * @brief Satellite Start transmission.
 *
 * This re-enables transmitting and receiving for all routes.
 */
void Communication_RecievedStartTransmitting()
{
  Debugging_Utilities_DebugLog("(R) Received start transmitting.");
	TRANSMISSION_ENABLED = true;
}

/**
 * @brief Power infomation send to ground station every 400ms.
 *
 * Collects Solar cell voltages, deployment state, reset count and batter charging current
 * then transmits it to the ground station.
 */
void Communication_TransmitPowerInfo()
{
  Debugging_Utilities_DebugLog("(T) Transmitting power info packet.");
  
  String batteryChargingVoltage = System_Info_MapValue(Pin_Interface_GetBatteryChargingVoltage(), 0.0f, 1023.0f, 0.4f, 4.2f);
  String batteryVoltage = System_Info_MapValue(Pin_Interface_GetBatteryVoltage(), 0.0f, 1023.0f, 0.4f, 4.2f);
  String totalSolarCellVoltage = System_Info_MapValue(Pin_Interface_GetTotalSolarCellVoltage(), 0.0f, 1023.0f, 0.4f, 4.2f);
  
	bool deploymentState = Deployment_GetDeploymentState();
	int resetCounter = System_Info_GetResetCounter();

	String sysInfoMessage = String("");
	sysInfoMessage += String("BC:") + (batteryChargingVoltage) + ";";
  sysInfoMessage += String("B:") + (batteryVoltage) + ";";
  sysInfoMessage += String("TS:") + (totalSolarCellVoltage) + ";";
  sysInfoMessage += String("RC:") + String(resetCounter) + ";";
  sysInfoMessage += String("DS:") + String(deploymentState) + ";";

	Communication_SX1278Transmit("9", sysInfoMessage);
}

/**
 * @brief Satellite's ground station TUNE transmission.
 *
 * This is send every 1s which the ground station tunes its frequency to
 */
void Communication_TransmitTune()
{
  Debugging_Utilities_DebugLog("(T) Transmitting tuning packet.");
  
	// switch to wide bandwidth location transmission.
	LORA.setBandwidth(LOCATION_BANDWIDTH);
  
	Debugging_Utilities_DebugLog("(TUNING) Switched to " + String(LOCATION_BANDWIDTH) + "KHz bandwidth.");

	Communication_SX1278Transmit("10", String("FOSSASAT1"));

	// return to local bandwidth transmissions.
	LORA.setBandwidth(BANDWIDTH);
 
	Debugging_Utilities_DebugLog("(TUNING) Switched to " + String(BANDWIDTH) + "KHz bandwidth.");
}

