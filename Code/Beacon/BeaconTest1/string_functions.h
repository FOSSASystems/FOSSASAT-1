#ifndef STRING_F_H_INCLUDED
#define STRING_F_H_INCLUDED

#include <Arduino.h>
#include "debugging_utilities.h"
#include "configuration.h"

void String_Functions_FloatToString(char* pStringContainer, float pFloatValue);
void String_Functions_IntegerToString(char* pStringContainer, int pIntegerValue);
void String_Functions_GetPassword(char* pStr, char* pPassword);
bool String_Functions_GetFunctionId(char* pStr, char* pFunctionId);
bool String_Functions_GetMessage(char* pStr, char* pMessage);
void String_Functions_Zero(char* pContainer);

#endif
