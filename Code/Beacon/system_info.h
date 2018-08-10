#ifndef SYSTEM_INFO_H_INCLUDED
#define SYSTEM_INFO_H_INCLUDED

int System_Info_GetResetCounter();
void System_Info_IncrementRestartCounter();
bool System_Info_CheckSystemSignature(String inSignature);
String System_Info_GetTransmissionSignature();
String System_Info_MapValue(float inValue, float inMin, float inMax, float outMin, float outMax);

#endif
