#include "state_machine_declerations.h"

bool STATE_STARTED = false;
bool STATE_STOPPED = false;
bool STATE_DEPLOYMENT_SUCCESS = false;
bool STATE_PING = false;
bool STATE_TRANSMIT_POWER_INFO = false;
bool STATE_TRANSMIT_RTTY_CALLSIGN = false;
bool STATE_STOP = false; // stops the signal going to the hardware watchdog
bool STATE_TRANSMIT_REPEATER_MESSAGE = false;
bool STATE_TRANSMIT_LSF_CALLSIGN = false; // transmit the lower spreading factor (SF7) callsign.
bool STATE_MANUAL_TRANSMIT_SYTEM_INFORMATION = false;
