#ifndef PERSISTANT_STORAGE_H_INCLUDED
#define PERSISTANT_STORAGE__H_INCLUDED

void Persistant_Storage_Wipe();
int Persistant_Storage_Get(int inAddress);
void Persistant_Storage_Set(int inAddress, int inValue);

#endif
