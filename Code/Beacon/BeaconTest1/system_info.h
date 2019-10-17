#ifndef SYSTEM_INFO_H_INCLUDED
#define SYSTEM_INFO_H_INCLUDED

int System_Info_GetResetCounter();
void System_Info_IncrementRestartCounter();
void System_Info_LoadRestartCounter();

const char* System_Info_GetTransmissionPassword();

void System_Info_SetCallsign(char* inCallsign);
char* System_Info_GetCallsign();

#endif
