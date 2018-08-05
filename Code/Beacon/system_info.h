#ifndef SYSTEM_INFO_H_INCLUDED
#define SYSTEM_INFO_H_INCLUDED

int System_Info_GetResetCounter();
void System_Info_IncrementRestartCounter();
bool System_Info_CheckSystemSignature(String inSignature);
String System_Info_GetTransmissionSignature();
float System_Info_GetInternalTemperature();

#endif
