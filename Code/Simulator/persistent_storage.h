#ifndef PERSISTENT_STORAGE_H_INCLUDED
#define PERSISTENT_STORAGE_H_INCLUDED

#include "Simulator.h"

template <class T>
T Persistent_Storage_Read(uint16_t addr);

template <class T>
void Persistent_Storage_Write(uint16_t addr, T val);

void Persistent_Storage_Wipe();

#endif
