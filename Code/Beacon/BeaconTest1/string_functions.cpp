#include <Arduino.h>
#include "debugging_utilities.h"
#include "configuration.h"

void String_Functions_FloatToString(char* pStringContainer, float pFloatValue)
{
  char floatValueBuffer[10];

  dtostrf(pFloatValue, 4, 2, pStringContainer);
}

void String_Functions_IntegerToString(char* pStringContainer, int pIntegerValue)
{
  char integerValueBuffer[4];
  sprintf(integerValueBuffer, "%i", pIntegerValue);

  memcpy(pStringContainer, integerValueBuffer, sizeof(integerValueBuffer));
}

void String_Functions_GetPassword(char* pStr, char* pPassword)
{
  for (int i = 0; i < strlen(TRANSMISSION_PASSWORD); i++)
  {
    pPassword[i] = pStr[i];
  }

  pPassword[strlen(TRANSMISSION_PASSWORD)] = '\0';
}

bool String_Functions_GetFunctionId(char* pStr, char* pFunctionId)
{
  int fidi = 0;
  bool correct = false;
  for (int i = strlen(TRANSMISSION_PASSWORD); i < strlen(TRANSMISSION_PASSWORD) + 6; i++)
  {
    if (pStr[i] == '\0')
    {
      correct = false;
      break;
    }
    if (pStr[i] == ';')
    {
      correct = true;
      FOSSASAT_DEBUG_PRINTLN(F("FOUND ; CHAR"));
      break;
    }

    pFunctionId[fidi] = pStr[i];
    fidi++;
  }

  if (correct)
  {
    pFunctionId[fidi] = '\0';
  }

  return correct;
}

bool String_Functions_GetMessage(char* pStr, char* pMessage)
{
  // find index of ; character.
  int dataDelim = 0;
  for (int i = 0; i < strlen(pStr); i++)
  {
    char c = pStr[i];
    if (c == ';')
    {
      dataDelim = i;
      break;
    }
  }

  int midi = 0;
  bool correct = false;
  for (int i = dataDelim + 1; i < MAX_STRING_LENGTH; i++)
  {
    if (pStr[i] == '\0')
    {
      correct = true;
      break;
    }
    if (pStr[i] == '\n')
    {
      correct = true;
      break;
    }
    if (pStr[i] == '\r')
    {
      correct = true;
      break;
    }

    pMessage[midi] = pStr[i];
    midi++;
  }

  if (correct)
  {
    pMessage[midi] = '\0';
  }

  return correct;
}

void String_Functions_Zero(char* pContainer)
{
  for (int i = 0; i < sizeof(pContainer); i++)
  {
    pContainer[i] = 0;
  }
}
