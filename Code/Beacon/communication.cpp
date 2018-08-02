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

	Debugging_Utilities_DebugLog("Transmitting... " + transmissionSignature + inFuncId + inMessage);

	if (TRANSMISSION_ENABLED)
	{
		byte state = LORA.transmit(transmissionSignature + inFuncId + inMessage);

		if (state == ERR_NONE)
		{
			// the packet was successfully transmitted
			Debugging_Utilities_DebugLog(" success!");
		}
		else if (state == ERR_PACKET_TOO_LONG)
		{
			// the supplied packet was longer than 256 bytes
			Debugging_Utilities_DebugLog(" too long!");
		}
		else if (state == ERR_TX_TIMEOUT)
		{
			// timeout occurred while transmitting packet
			Debugging_Utilities_DebugLog(" timeout!");
		}  
	}
}
   
/**
 * @brief satellite's Setup() function is called..
 *
 * This function indicates to the ground station that the satellite has started.
 */
void Communication_TransmitStartedSignal()
{
	Communication_SX1278Transmit("1", "");
}

/**
 * @brief satellite's failure sequency function is called..
 *
 * This function indicates to the ground station that the satellite has stopped.
 */
void Communication_TransmitStoppedSignal()
{
	Communication_SX1278Transmit("2", "");
}

/**
 * @brief satellite's SX1278 LoRa library .begin() ran successfully.
 *
 * This function indicates to the ground station that the satellite has began the LoRa object successfully.
 */
void Communication_TransmitSX1278InitializedSuccess()
{
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
	Communication_SX1278Transmit("4", "");
}

/**
 * @brief satellite's Setup() function is called..
 *
 * This function indicates to the ground station that the satellite has started.
 */
void Communication_TransmitPong()
{
	Communication_SX1278Transmit("6", "");
}

/**
 * @brief Satellite PING transmission received.
 *
 * This function is to test the connection using a minimal packet.
 */
void Communication_RecievedPing()
{
	STATE_PING = true;
}

/**
 * @brief Satellite Stop transmission.
 *
 * This prevents the satellite from transmitting and configures it to only listen to start transmitting messages.
 */
void Communication_RecievedStopTransmitting()
{
	TRANSMISSION_ENABLED = false;
}

/**
 * @brief Satellite Start transmission.
 *
 * This re-enables transmitting and receiving for all routes.
 */
void Communication_RecievedStartTransmitting()
{
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
	int solarCell1 = Pin_Interface_GetSolarCellVoltage(1);
	int solarCell2 = Pin_Interface_GetSolarCellVoltage(2);
	int solarCell3 = Pin_Interface_GetSolarCellVoltage(3);
	int solarCell4 = Pin_Interface_GetSolarCellVoltage(4);
	int solarCell5 = Pin_Interface_GetSolarCellVoltage(5);

	bool deploymentState = Deployment_GetDeploymentState();

	int resetCounter = System_Info_GetResetCounter();

	int batteryChargingCurrent = Pin_Interface_GetBatteryChargingCurrent();

	String sysInfoMessage = String("S1:") + String(solarCell1) + ";" + "S2:" + String(solarCell2) + ";" + "S3:" + String(solarCell3) + ";" + "S4:" + String(solarCell4) + ";" + "S5:" + String(solarCell5) + ";" + "B:" + String(batteryChargingCurrent) + ";RC:" + String(resetCounter) + ";DEPS:" + String(deploymentState) + ";";

	Communication_SX1278Transmit("9", sysInfoMessage);
}

/**
 * @brief Satellite's ground station TUNE transmission.
 *
 * This is send every 1s which the ground station tunes its frequency to
 */
void Communication_TransmitTune()
{
	// switch to wide bandwidth location transmission.
	LORA.setBandwidth(LOCATION_BANDWIDTH);
	Debugging_Utilities_DebugLog("(TUNING) Switched to " + String(LOCATION_BANDWIDTH) + "KHz bandwidth.");

	Communication_SX1278Transmit("10", String("FOSSASAT1"));

	// return to local bandwidth transmissions.
	LORA.setBandwidth(BANDWIDTH);
	Debugging_Utilities_DebugLog("(TUNING) Switched to " + String(BANDWIDTH) + "KHz bandwidth.");
}

