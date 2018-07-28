#include "configuration.h"
#include "state_machine_declerations.h"
#include "pin_interface.h"
#include "deployment.h"
#include "system_info.h"
#include "communication.h"
#include "debugging_utilities.h"

////////////////////////////////////////
// See header for protocol definition //
////////////////////////////////////////

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

// 1   
void Communication_TransmitStartedSignal()
{
  Communication_SX1278Transmit("1", "");
}

// 2
void Communication_TransmitStoppedSignal()
{
  Communication_SX1278Transmit("2", "");
}

// 3
void Communication_TransmitSX1278InitializedSuccess()
{
  Communication_SX1278Transmit("3", "");
}

// 4
void Communication_TransmitDeploymentSuccess()
{
  Communication_SX1278Transmit("4", "");
}

// 6
void Communication_TransmitPong()
{
  Communication_SX1278Transmit("6", "");
}

// 5
void Communication_RecievedPing()
{
  STATE_PING = true;
}

// 7
void Communication_RecievedStopTransmitting()
{
  TRANSMISSION_ENABLED = false;
}

// 8
void Communication_RecievedStartTransmitting()
{
  TRANSMISSION_ENABLED = true;
}

// 9
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

  String sysInfoMessage = String("S1:") + String(solarCell1) + ";" + "S2:" + String(solarCell2) + ";" + "S3:" + String(solarCell3) + ";" + "S4:" + String(solarCell4) + ";" + "S5:" + String(solarCell5) + ";" + "B:" + String(batteryChargingCurrent) + "RC:" + String(resetCounter) + ";DEPS:" + String(deploymentState);

  Communication_SX1278Transmit("9", sysInfoMessage);
}

// 10
void Communication_TransmitTransceiverSettings()
{
  String transceiverSettingsMessage = String("CF:" + CARRIER_FREQUENCY + ";BW:" + BANDWIDTH + ";SF:" + SPREADING_FACTOR + ";CR:" + CODING_RATE + ";SW:" + SYNC_WORD + ";OP:" + OUTPUT_POWER + ";");

  Communication_SX1278Transmit("10", transceiverSettingsMessage);
}

