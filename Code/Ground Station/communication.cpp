/**
 * @file communication.cpp
 * @brief This code manages the protocol transmission. It takes raw values
 * from the system, packs them into the radio protocol and sends it through the
 * SX1278.
*/

#include "configuration.h"
#include "state_machine_declerations.h"
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

/**
 * @brief Called when received Function ID "1".
 *
 * This function indicates to the ground station that the satellite has started.
 *
 * Received: satellite's Setup() function is called..
 */
void Communication_ReceivedStartedSignal()
{
    Debugging_Utilities_DebugLog("(R) Arduino satellite started!");
}

/**
 * @brief Called when received Function ID "2".
 *
 * This function indicates to the ground station that the satellite has stoppped.
 *
 * Received: satellite's failure is detected.
 *
 * @todo Implement satellite stop signal transmission.
 */
void Communication_ReceivedStoppedSignal()
{
    Debugging_Utilities_DebugLog("(R) Arduino satellite stopped...");
}

/**
 * @brief Called when received Function ID "2".
 *
 * This function indicates to the ground station that the satellite has initialised it's transceiver.
 *
 * Received: satellite's failure is detected.
 * 
 * @todo Add output interface.
 * @todo Implement satellite stop signal transmission.
 * 
 */
void Communication_ReceivedTransmittedOnline()
{
    Debugging_Utilities_DebugLog("(R) Arduino satellite's transceiver online.");
}

/**
 * @brief Called when received Function ID "4".
 *
 * This function indicates to the ground station that the deployment sequence was successfull.
 *
 * Received: Satellite's Setup() function.
 *
 * @todo Implement satellite deployment success signal.
 */
void Communication_ReceivedDeploymentSuccess()
{
    Debugging_Utilities_DebugLog("(R) Arduino satellite's deployment sequence successfully completed.");
}

/**
 * @brief Called when received Function ID "6".
 *
 * This function indicates to the ground station that the satellite received the Ping transmission.
 *
 * Received: Satellite receives Ping function "5" transmission.
 */
void Communication_ReceivedPong()
{
    Debugging_Utilities_DebugLog("(R) Just recieved a pong!");
}

/**
 * @brief Function ID "5".
 *
 * This function transmits to the satellite a small packet, the satellite then responds with a Pong packet. This is useful for small packet testing the
 * connection.
 *
 * Transmitted: On user input.
 *
 * @todo Implement user interface for ground station.
 */
void Communication_TransmitPing()
{
    Communication_SX1278Transmit("5", "");
}

/**
 * @brief Function ID "7".
 * 
 * This function signals to the satellite to stop all transmissions.
 *
 * Transmitted: On user input.
 *
 * @todo Reference the security code that requires this functionality.
 */
void Communication_TransmitStopTransmitting()
{
    Communication_SX1278Transmit("7", "");
}

/**
 * @brief Function ID "8".
 *
 * This function signals to the satellite to start transmitting again. When the satellite is given a "Stop Transmitting" signal
 * it will only listen for this signal and nothing else.
 *
 * Transmitted: On user input.
 *
 * @todo Reference the security code that requires this functionality.
 */
void Communication_TransmitStartTransmitting()
{
    Communication_SX1278Transmit("8", "");
}

/**
 * @brief Function ID "9".
 *
 * This function is called when the ground station receives a power infomation transmission. The power info is broadcasted every 400s.
 *
 * Received: every 400ms from the satellite's Loop().
 */
void Communication_ReceivedPowerInfo(String inMessage)
{
    Debugging_Utilities_DebugLog("(DATA - POWER INFO) Received " + inMessage );
}

/**
 * @brief Function ID "10".
 *
 * This function is called when the ground station receives a tranceiver setting transmission. This transmission tunes the ground
 * station to the satellite.
 *
 * This function's associated transmission is sent from the satellite in a wide bandwidth (62.5KHz) mode. Upon the ground station
 * receiving this wide bandwidth transmission, it uses the frequency error (how much the default 434.0 MHz frequency has been distorted)
 * to re-set the frequency the ground stations SX1278 listen to.
 *
 * After the ground station has tuned itself, it switches to a small bandwidth (7.8KHz) mode. If the ground station cannot communicate
 * with the satellite, or packets are failed to be sent, it switches back to the "searching" wide bandwidth mode.
 * 
 * If the ground station LORA.receive() function returns a timeout, it resets to width bandwidth "searching/listening" mode for re-tuning.
 *
 * Received: Every 1s from the satellite's Loop().
 *
 */
void Communication_ReceivedTune(float inFrequencyError)
{
    Debugging_Utilities_DebugLog("(DATA - TRANS. INFO) TRANSCEIVER TUNING PACKET...");
    Debugging_Utilities_DebugLog("(DATA - TRANS. INFO) Frequency error of " + String(inFrequencyError) + " (TODO - is this number ever negative?)");

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

