#include "configuration.h"
#include "state_machine_declerations.h"
#include "communication.h"
#include "debugging_utilities.h"

// See header for protocol definition.

void Communication_SX1278Transmit(String inFuncId, String inMessage)  // this is hidden, use SX1278Transmit____ functions.
{
  String signature = TRANSMISSION_SIGNATURE; // todo wrap this up in a container.
  
  Debugging_Utilities_DebugLog("Transmitting... " + signature + inFuncId + inMessage);
  
  byte state = LORA.transmit(signature + inFuncId + inMessage);

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

void Communication_ReceivedStartedSignal()
{
    Debugging_Utilities_DebugLog("(R) Arduino satellite started!");
}

void Communication_ReceivedStoppedSignal()
{
    Debugging_Utilities_DebugLog("(R) Arduino satellite stopped...");
}

void Communication_ReceivedTransmittedOnline()
{
    Debugging_Utilities_DebugLog("(R) Arduino satellite's transceiver online.");
}

void Communication_ReceivedDeploymentSuccess()
{
    Debugging_Utilities_DebugLog("(R) Arduino satellite's deployment sequence successfully completed.");
}

void Communication_ReceivedPong()
{
    Debugging_Utilities_DebugLog("(R) Just recieved a pong!");
}

void Communication_TransmitPing()
{
    Communication_SX1278Transmit("5", "");
}

void Communication_TransmitStopTransmitting()
{
    Communication_SX1278Transmit("7", "");
}

void Communication_TransmitStartTransmitting()
{
    Communication_SX1278Transmit("8", "");
}

void Communication_ReceivedPowerInfo(String inMessage)
{
    Debugging_Utilities_DebugLog("(DATA - POWER INFO) Received " + inMessage );
}

