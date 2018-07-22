#include "configuration.h"
#include "state_machine_declerations.h"
#include "pin_interface.h"
#include "deployment.h"
#include "system_info.h"
#include "communication.h"
#include "debugging_utilities.h"


// See header for protocol definition.

void Communication_SX1278Transmit(String inFuncId, String inMessage)  // this is hidden, use SX1278Transmit____ functions.
{
  Serial.println("Transmitting... " + inFuncId + inMessage);
}

// 1                     : Notification : arduino started signal :  N/A      
void Communication_TransmitStartedSignal()
{
  Communication_SX1278Transmit("1", "");
}

// 2                     : Notification : arduino stopped signal : N/A
void Communication_TransmitStoppedSignal()
{
  Communication_SX1278Transmit("2", "");
}

// 3                     : Notification : transmitter initialized success signal : N/A
void Communication_TransmitSX1278InitializedSuccess()
{
  Communication_SX1278Transmit("3", "");
}

// 4                     : Notification : cell and antenna deployment success : N/A
void Communication_TransmitDeploymentSuccess()
{
  Communication_SX1278Transmit("4", "");
}

// 6                     : Data         : Pong : N/A
void Communication_TransmitPong()
{
  Communication_SX1278Transmit("6", "");
}

// 5                     : Command      : Ping : N/A
void Communication_RecievedPing()
{
  STATE_PING = true;
}

// 7                     : Command      : stop transmitting : N/A
void Communication_RecievedStopTransmitting()
{
  TRANSMISSION_ENABLED = false;
}

// 8                     : Command      : start transmitting : N/A
void Communication_RecievedStartTransmitting()
{
  TRANSMISSION_ENABLED = true;
}

// 9                     : Command      : transmit system infomation : N/A
void Communication_TransmitPowerInfo()
{
  String function_id = String(10);

  int solarCell1 = Pin_Interface_GetSolarCellVoltage(1);
  int solarCell2 = Pin_Interface_GetSolarCellVoltage(2);
  int solarCell3 = Pin_Interface_GetSolarCellVoltage(3);
  int solarCell4 = Pin_Interface_GetSolarCellVoltage(4);
  int solarCell5 = Pin_Interface_GetSolarCellVoltage(5);

  bool deploymentState = Deployment_GetDeploymentState();
  Debugging_Utilities_DebugLog(String(deploymentState));

  int resetCounter = System_Info_GetResetCounter();

  // TODO perform validation and verfication on these values.

  int batteryChargingCurrent = Pin_Interface_GetBatteryChargingCurrent();

  // TODO perform validation and verfication on these values.

  // TODO convert transmission protocol to byte formats.

  String sysInfoMessage = String("S1:") + String(solarCell1) + ";" + "S2:" + String(solarCell2) + ";" + "S3:" + String(solarCell3) + ";" + "S4:" + String(solarCell4) + ";" + "S5:" + String(solarCell5) + ";" + "B:" + String(batteryChargingCurrent) + "RC:" + String(resetCounter) + ";DEPS:" + String(deploymentState);

  Communication_SX1278Transmit(function_id, sysInfoMessage);
}
