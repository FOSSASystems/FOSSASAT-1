#include <Arduino.h>
#include <RadioLib.h>
#include <EEPROM.h>

#include "persistant_storage.h"
#include "debugging_utilities.h"
#include "configuration.h"
#include "deployment.h"

////////////////////
// EEPROM Control //
////////////////////
void Persistant_Storage_NonRestoreWipe()
{
  FOSSASAT_DEBUG_PRINTLN(F("Non restore wiping storage."));
  for (int i = 0; i < EEPROM.length(); i++)
  {
    EEPROM.write(i, 0);
  }
  FOSSASAT_DEBUG_PRINTLN(F("Done."));
}

void Persistant_Storage_Wipe()
{
  int deploymentState = Deployment_GetDeploymentState(); // get the deployment state and save it to RAM.

  FOSSASAT_DEBUG_PRINTLN(F("Wiping storage."));
	for (int i = 0; i < EEPROM.length(); i++)
	{
		EEPROM.write(i, 0);
	}
  FOSSASAT_DEBUG_PRINTLN(F("Done."));


  FOSSASAT_DEBUG_PRINTLN(F("Restoring deployment state.")); // restore the deployment state.

  Persistant_Storage_Set(EEPROM_DEPLOYMENT_ADDR, deploymentState);

  FOSSASAT_DEBUG_PRINTLN(F("Done."));
}

int Persistant_Storage_Get(int inAddress)
{
//  int values[6];
//
//  // load each value into an array.
//  for (int i = 0; i < 6; i++)
//  {
//    int thisAddress = inAddress + (i * 100); // address = current address + (index * 100)
//    int thisValue = 0;
//    EEPROM.get(thisAddress, thisValue);
//
//    values[i] = thisValue;
//  }
//
//  // which value is the most common?
//  int uniqueValues[6];
//  int uniqueValuePosition = 0;
//  for (int i = 0; i < 6; i++)
//  {
//    int thisValue = values[i];
//
//    // is this value in the array?
//    bool inArray = false;
//    for (int k = 0; k < 6; k++)
//    {
//      int thisValueB = uniqueValues[k];
//
//      if (thisValue == thisValueB)
//      {
//        inArray = true;
//        break;
//      }
//    }
//
//    if (inArray == false)
//    {
//      uniqueValues[uniqueValuePosition] = thisValue;
//      uniqueValuePosition++;
//    }
//  }
//
//
//  // which one of the unique values is the most common in the first data set?
//  int uniqueIndexRepetitionCount[6];
//
//  // zero the array
//  for (int i = 0; i < 6; i++)
//  {
//    uniqueIndexRepetitionCount[i] = 0;
//  }
//
//  // calculate number of values that match the unique ones.
//  for (int i = 0; i < 6; i++)
//  {
//    int thisValue = values[i];
//
//    for (int k = 0; k < uniqueValuePosition; k++)
//    {
//      int thisUniqueValue = uniqueValues[k];
//
//      if (thisValue == thisUniqueValue)
//      {
//        uniqueIndexRepetitionCount[k] = uniqueIndexRepetitionCount[k] + 1;
//      }
//    }
//  }
//
//  // which index has the most?
//  int currentMaxRepetition = 0;
//  int currentMaxRepetitionIndex = 0;
//  for (int i = 0; i < 6; i++)
//  {
//    int repetitionCount = uniqueIndexRepetitionCount[i];
//
//    if (currentMaxRepetition < repetitionCount)
//    {
//      currentMaxRepetition = repetitionCount;
//      currentMaxRepetitionIndex = i;
//    }
//  }
//
//	return uniqueValues[currentMaxRepetitionIndex];

  int val = 0;
  EEPROM.get(inAddress, val);
  return val;
}

void Persistant_Storage_Set(int inAddress, int inValue)
{
  EEPROM.put(inAddress, inValue);

//  for (int i = 0; i < 6; i++)
//  {
//    int address = inAddress + (i * 100);
//    EEPROM.put(address, inValue);
//  }
}
