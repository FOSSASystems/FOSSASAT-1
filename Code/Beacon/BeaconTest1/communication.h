#ifndef COMMUNICATION_H_INCLUDED
#define COMMUNICATION_H_INCLUDED

 ////////////////////
  // FOSSA_PROTOCOL //
  //----------------//
  ////////////////////

  //////////////////
  // Restrictions //
  // ------------ /////////////////////////////////
  // Max send length of a string 256 characters. //
  // 512 bytes in total                  //
  /////////////////////////////////////////////////

  ////////////
  // Format //
  // ------ /////////////////////////////
  // 1 byte = function id (0-256)      //
  // 511 bytes = message of any format //
  ///////////////////////////////////////

  ///////////////////////////////////////////////
  // Function ID (char) :  Message Length :  Category       : Description  :  Type 
  // 1                     : Notification : arduino started signal :  N/A      
  // 2                     : Notification : arduino stopped signal : N/A
  // 3                     : Notification : transmitter initialized success signal : N/A
  // 4                     : Notification : cell and antenna deployment success : N/A
  // 5                     : Command      : Ping : N/A
  // 6                     : Data         : Pong : N/A
  // 7                     : Command      : stop transmitting : N/A
  // 8                     : Command      : start transmitting : N/A
  // 9                     : Command      : transmit system infomation : N/A
  // 10                    : Data         : System infomation : byte array
  ///////////////////////////////////////////////
void Communication_SX1278Transmit(String inFuncId, String inMessage);

//////////////////////////////
// FOSSA_PROTOCOL FUNCTIONS //
//////////////////////////////

// 1                     : Notification : arduino started signal :  N/A      
void Communication_TransmitStartedSignal();
// 2                     : Notification : arduino stopped signal : N/A
void Communication_TransmitStoppedSignal();
// 3                     : Notification : transmitter initialized success signal : N/A
void Communication_TransmitSX1278InitializedSuccess();
// 4                     : Notification : cell and antenna deployment success : N/A
void Communication_TransmitDeploymentSuccess();
// 6                     : Data         : Pong : N/A
void Communication_TransmitPong();
// 5                     : Command      : Ping : N/A
void Communication_RecievedPing();
// 7                     : Command      : stop transmitting : N/A
void Communication_RecievedStopTransmitting();
// 8                     : Command      : start transmitting : N/A
void Communication_RecievedStartTransmitting();
// 9                     : Command      : transmit system infomation : N/A
void Communication_TransmitPowerInfo();

#endif
