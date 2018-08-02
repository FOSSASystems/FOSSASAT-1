#ifndef STATE_MACHINE_DECLS_INFO_H_INCLUDED
#define STATE_MACHINE_DECLS_INFO_H_INCLUDED

//////////////////////
// State management // (toggled, unset once transmitted)
//////////////////////
extern bool STATE_STARTED;
extern bool STATE_STOPPED;
extern bool STATE_TRANSMITTER_INITIALIZED;
extern bool STATE_DEPLOYMENT_SUCCESS;
extern bool STATE_PING;
extern bool STATE_TRANSMIT_POWER_INFO;
extern bool STATE_TRANSMIT_TUNE;

#endif
