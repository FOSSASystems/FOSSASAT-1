#ifndef STATE_MACHINE_DECLS_INFO_H_INCLUDED
#define STATE_MACHINE_DECLS_INFO_H_INCLUDED

//////////////////////
// State management // (toggled, unset once transmitted)
//////////////////////
extern bool STATE_TRANSMIT_PING;
extern bool STATE_TRANSMIT_STOP_TRANSMITTING;
extern bool STATE_TRANSMIT_START_TRANSMITTING;

// Fire once
extern bool HAS_REDUCED_BANDWIDTH;

#endif
