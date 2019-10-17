#ifndef COMMUNICATION_H_INCLUDED
#define COMMUNICATION_H_INCLUDED

/* Repeater message */
extern char COMMUNICATION_REPEATER_MESSAGE[MAX_STRING_LENGTH];

void Communication_SwitchFSK();
void Communication_SwitchLORA(bool inLSFMode = false);
void Communication_SwitchRTTY();
void Communication_SwitchFSK_ISM();
void Communication_SwitchLORA_ISM();

void Communication_SetTXC();

void Communication_SX1278RouteReceive(char* inFunctionId, char* inPassword, char* inMessage, bool isPingTransmission);
void Communication_SX1278RouteTransmit();
void Communication_SX1278TransmitPacket(char* inTransmissionPacket);
void Communication_SX1278Transmit(char* inFuncId, char* inMessage, bool inLSFMode = false);

void Communication_DisableTransmitting();
void Communication_EnableTransmitting();
bool Communication_GetTransmittingState();

//////////////////////////////
// FOSSA_PROTOCOL FUNCTIONS //
//////////////////////////////

// 1    
void Communication_TransmitStartedSignal();

// 2
void Communication_TransmitStoppedSignal();

// 3
// void Communication_TransmitSX1278InitializedSuccess();

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

// 10 (previously tuning packet)
void Communication_RecievedResetEEPROM();

// 11
void Communication_RecievedDeployment();

// 12 transmit payload message
void Communication_TransmitPayloadMessage();

// 13 recieved programmed callsign.
void Communication_RecievedCallsignChange(char* inMessage);

// 14 recieved watchdog stop.
void Communication_RecievedWatchdogStop();

// 15 received repeater message
void Communication_RecievedRepeaterMessage(char* inMessage);

// 16 transmit repeater message.
void Communication_TransmitRepeaterMessage();

// 17 received LSF command.
void Communication_RecievedSendLSFCallsign();

// 18 transmit callsign with lower spreading factor.
void Communication_TransmitLSFCallsign();

// 19 switch to alternative spreading factor
void Communication_RecievedEnableAlternativeSF();

// 20 switch to regular spreading factor.
void Communication_RecievedDisabledAlternativeSF();

// 21 enable MPPT charging circuit.
void Communication_ReceivedEnableMPPTTemperatureSwitching();

// 22 disable mppt charging circuit.
void Communication_ReceivedDisableMPPTTemperatureSwitching();

// 23 enable low power mode system.
void Communication_ReceivedEnableLowPowerMode();

// 24 disable low power mode system.
void Communication_ReceivedDisableLowPowerMode();

// 25
void Communication_ReceivedManualSystemInformationPacketSend();

// 26
void Communication_ReceivedMPPTKeepAliveEnable();

// 27
void Communication_ReceivedMPPTKeepAliveDisable();

#endif
