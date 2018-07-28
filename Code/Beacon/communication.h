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
  // Function ID (char) :  Direction  :  Type 
  // 1                     : To Ground : arduino started signal :  N/A      
  // 2                     : To Ground : arduino stopped signal : N/A
  // 3                     : To Ground : transmitter initialized success signal : N/A
  // 4                     : To Ground : cell and antenna deployment success : N/A
  // 5                     : To Satellite      : Ping : N/A
  // 6                     : To Ground         : Pong : N/A
  // 7                     : To Satellite      : stop transmitting : N/A
  // 8                     : To Satellite      : start transmitting : N/A
  // 9                     : To Ground      : transmit system infomation : N/A
  // 10                    : To Ground         : Send configuration frequency : float
  ///////////////////////////////////////////////
void Communication_SX1278Transmit(String inFuncId, String inMessage);

//////////////////////////////
// FOSSA_PROTOCOL FUNCTIONS //
//////////////////////////////

// 1    
void Communication_TransmitStartedSignal();
// 2
void Communication_TransmitStoppedSignal();
// 3
void Communication_TransmitSX1278InitializedSuccess();
// 4
void Communication_TransmitDeploymentSuccess();
// 6
void Communication_TransmitPong();
// 5
void Communication_RecievedPing();
// 7
void Communication_RecievedStopTransmitting();
// 8 
void Communication_RecievedStartTransmitting();
// 9
void Communication_TransmitPowerInfo();
// 10
void Communication_TransmitTransceiverSettings();
#endif
