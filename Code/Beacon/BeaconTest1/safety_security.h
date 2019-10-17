#ifndef SAFETY_SECURITY_H_INCLUDED
#define SAFETY_SECURITY_H_INCLUDED


void Safety_Security_SX1278StartupCheck(int pStartupCode);
void Safety_Security_SX1278TransmissionCodeCheck(int pTransmissionCode);
void Safety_Security_SX1278ReceptionCodeCheck(int pReceptionCode);
void Safety_Security_SX1278SettingCodeCheck(int pSettingsCode);

bool Safety_Security_CheckString(char* inString);
void Safety_Security_CheckDeploymentFaults(int pPreSoftwareSignal, int pPostSoftwareSignal);

#endif
