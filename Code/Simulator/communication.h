#ifndef COMMUNICATION_H_INCLUDED
#define COMMUNICATION_H_INCLUDED

#include "Simulator.h"

void Communication_Receive_Interrupt();
void Communication_Change_Modem();

void Communication_Set_Modem(uint8_t modem);
int16_t Communication_Set_SpreadingFactor(uint8_t sfMode);
int16_t Communication_Set_Configuration(uint8_t* optData, uint8_t optDataLen);

void Communication_RTTY_Print_Hex(uint8_t val);
void Communication_Send_System_Info(bool useRtty = false);

void Comunication_Parse_Frame(uint8_t* frame, size_t len);
void Communication_Execute_Function(uint8_t functionId, uint8_t* optData = NULL, size_t optDataLen = 0);
int16_t Communication_Send_Response(uint8_t respId, uint8_t* optData = NULL, size_t optDataLen = 0, bool overrideModem = false);
int16_t Communication_Transmit(uint8_t* data, uint8_t len, bool overrideModem = false);
bool Communication_Check_OptDataLen(uint8_t expected, uint8_t actual);

#endif
