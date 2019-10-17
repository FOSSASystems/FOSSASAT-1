#ifndef STATE_MACHINE_DECLS_INFO_H_INCLUDED
#define STATE_MACHINE_DECLS_INFO_H_INCLUDED

//////////////////////
// State management // (toggled, unset once transmitted)
//////////////////////
extern bool STATE_STARTED;
extern bool STATE_STOPPED;
extern bool STATE_DEPLOYMENT_SUCCESS;
extern bool STATE_PING;
extern bool STATE_TRANSMIT_POWER_INFO;
extern bool STATE_TRANSMIT_TUNE;
extern bool STATE_TRANSMIT_RTTY_CALLSIGN;
extern bool STATE_TRANSMIT_PAYLOAD_MESSAGE;
extern bool STATE_STOP;
extern bool STATE_TRANSMIT_REPEATER_MESSAGE;
extern bool STATE_TRANSMIT_LSF_CALLSIGN; // transmit the lower spreading factor (SF7) callsign.
extern bool STATE_MANUAL_TRANSMIT_SYTEM_INFORMATION;

#endif
