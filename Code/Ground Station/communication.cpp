#include "configuration.h"
#include "state_machine_declerations.h"
#include "communication.h"
#include "debugging_utilities.h"

// See header for protocol definition.

void Communication_SX1278Transmit(String inFuncId, String inMessage)  // this is hidden, use SX1278Transmit____ functions.
{
  String signature = TRANSMISSION_SIGNATURE; // todo wrap this up in a container.
  
  Debugging_Utilities_DebugLog("Transmitting... " + signature + inFuncId + inMessage);

  if (TRANSMISSION_ENABLED)
  {
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

void Communication_ReceivedTransceiverSettings(String inMessage, float inFrequencyError)
{
    Debugging_Utilities_DebugLog("(DATA - TRANS. INFO) TRANSCEIVER TUNING...");
    Debugging_Utilities_DebugLog("(DATA - TRANS. INFO) Received " + inMessage);
    Debugging_Utilities_DebugLog("(DATA - TRANS. INFO) Frequency error of " + inFrequencyError + " (TODO - is this number ever negative?)");

    if (AUTOMATIC_TUNING)
    {
      // calculate what the new frequency is.
      CARRIER_FREQUENCY = CARRIER_FREQUENCY + inFrequencyError;

      // change the LORA modules frequency.
      LORA.setFrequency(CARRIER_FREQUENCY);
      Debugging_Utilities_DebugLog("(DATA - TRANS. INFO) Tuning SX1278 to Freq: " + String(CARRIER_FREQUENCY));

      // reduce the LORA modules bandwidth
      if (HAS_REDUCED_BANDWIDTH == false)
      {
        LORA.setBandwidth(CONNECTED_BANDWIDTH);
        HAS_REDUCED_BANDWIDTH = true;
        Debugging_Utilities_DebugLog("(DATA - TRANS. INFO) Set SX1278 to Bandwidth: " + String(CONNECTED_BANDWIDTH));
      }
    }
}

