#ifndef COMMUNICATION_H_INCLUDED
#define COMMUNICATION_H_INCLUDED

void Communication_SX1278Transmit(String inFuncId, String inMessage);

void Communication_SX1278TransmitPayloadMessage(String inPayloadMessage);

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
void Communication_TransmitTune();
#endif
