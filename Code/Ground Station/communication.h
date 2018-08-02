/**
 * \ingroup Configuration
 * 
 * 
 */

#ifndef COMMUNICATION_H_INCLUDED
#define COMMUNICATION_H_INCLUDED

void Communication_SX1278Transmit(String inFuncId, String inMessage);

//////////////////////////////
// FOSSA_PROTOCOL FUNCTIONS //
//////////////////////////////

void Communication_ReceivedStartedSignal();

void Communication_ReceivedStoppedSignal();

void Communication_ReceivedTransmittedOnline();

void Communication_ReceivedDeploymentSuccess();

void Communication_TransmitPing();

void Communication_TransmitStopTransmitting();

void Communication_TransmitStartTransmitting();

void Communication_ReceivedPowerInfo(String inMessage);

void Communication_ReceivedPong();

void Communication_ReceivedTune(float inFrequencyError);

#endif
