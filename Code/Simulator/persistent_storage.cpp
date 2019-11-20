#include "persistent_storage.h"

// EEPROM only included once to supress unused variable warning
#include <EEPROM.h>

// EEPROM reading template
template <class T>
T Persistent_Storage_Read(uint16_t addr) {
  T val;
  EEPROM.get(addr, val);
  return(val);
}

// EEPROM writing template
template <class T>
void Persistent_Storage_Write(uint16_t addr, T val) {
  EEPROM.put(addr, val);
}

void Persistent_Storage_Wipe() {
  // wipe EEPROM
  FOSSASAT_DEBUG_PRINTLN(F("Wiping"));
  for (uint16_t i = 0; i < EEPROM.length(); i++) {
    Persistent_Storage_Write<uint8_t>(i, EEPROM_RESET_VALUE);
  }

  // set default variable values

  // set deployment counter to 0
  Persistent_Storage_Write<uint8_t>(EEPROM_DEPLOYMENT_COUNTER_ADDR, 0);

  // reset first run flag
  Persistent_Storage_Write<uint8_t>(EEPROM_FIRST_RUN_ADDR, EEPROM_FIRST_RUN);

  // set reset counter to 0
  Persistent_Storage_Write<uint16_t>(EEPROM_RESTART_COUNTER_ADDR, 0);

  // set default callsign
  System_Info_Set_Callsign((char*)"FOSSASAT-1");
}

// explicitly instantiate templates
template uint8_t Persistent_Storage_Read<uint8_t>(uint16_t);
template uint16_t Persistent_Storage_Read<uint16_t>(uint16_t);
template void Persistent_Storage_Write<uint8_t>(uint16_t, uint8_t);
template void Persistent_Storage_Write<uint16_t>(uint16_t, uint16_t);
