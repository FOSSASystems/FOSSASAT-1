/**
 * @file deployment.cpp
 * @brief This abstracts the deployment mechanism and sequence into callable functions.
 * SX1278.
*/


#include <Arduino.h>
#include "configuration.h"
#include "persistant_storage.h"
#include "deployment.h"
#include "debugging_utilities.h"
#include "state_machine_declerations.h"
#include "safety_security.h"
#include "power_control.h"

/**
 * @brief Power the deployment release mechanism (spring and hinge)
 *
 * Checks against the EEPROM value to see if it has already deployed, if not it waits 5s, then sends a digital high
 * value to the DIGITAL_OUT_MOSFET_1&2 pins and then after another 5s switches them back to low.
 */
void Deployment_RunSequence(bool inStartup)
{
  FOSSASAT_DEBUG_PRINTLN(F("Attempting to start deployment sequence"));

  int preSoftwareSignal = Persistant_Storage_Get(EEPROM_DEPLOYMENT_ADDR);
  int postSoftwareSignal = 0;

  if (!inStartup)
  {
    // trigger from a command
      FOSSASAT_DEBUG_PRINTLN(F("(DEPLOYMENT SEQ. MANUAL START.)"));

      // send high signal after 5seconds if not already deployed.
      delay(1000);

      Persistant_Storage_Set(EEPROM_DEPLOYMENT_ADDR, 1);

      // burn the nichrome wires.
      digitalWrite(DIGITAL_OUT_MOSFET_1, HIGH);
      digitalWrite(DIGITAL_OUT_MOSFET_2, HIGH);

      Power_Control_Delay(1000);

      FOSSASAT_DEBUG_PRINTLN(F("(DEPLOYMENT SEQ. SUCCESS.)"));

      digitalWrite(DIGITAL_OUT_MOSFET_1, LOW);
      digitalWrite(DIGITAL_OUT_MOSFET_2, LOW);

      STATE_DEPLOYMENT_SUCCESS = true;
  }
  else
  {
    if (preSoftwareSignal == 0)
    {
        FOSSASAT_DEBUG_PRINTLN(F("(DEPLOYMENT SEQ. START.)"));

        // send high signal after 5seconds if not already deployed.
        Power_Control_Delay(1000);

        Persistant_Storage_Set(EEPROM_DEPLOYMENT_ADDR, 1);

        // burn the nichrome wires.
        digitalWrite(DIGITAL_OUT_MOSFET_1, HIGH);
        digitalWrite(DIGITAL_OUT_MOSFET_2, HIGH);

        Power_Control_Delay(1000);

        FOSSASAT_DEBUG_PRINTLN(F("(DEPLOYMENT SEQ. SUCCESS.)"));

        digitalWrite(DIGITAL_OUT_MOSFET_1, LOW);
        digitalWrite(DIGITAL_OUT_MOSFET_2, LOW);

        STATE_DEPLOYMENT_SUCCESS = true;
    }
    else
    {
        FOSSASAT_DEBUG_PRINTLN(F("(DEPLOY SEQ. IGNORED)"));

        Persistant_Storage_Set(EEPROM_DEPLOYMENT_ADDR, 1);

        digitalWrite(DIGITAL_OUT_MOSFET_1, LOW);
        digitalWrite(DIGITAL_OUT_MOSFET_2, LOW);
    }
  }


  postSoftwareSignal = Persistant_Storage_Get(EEPROM_DEPLOYMENT_ADDR);

  // checks this process for hardware and software faults
  Safety_Security_CheckDeploymentFaults(preSoftwareSignal, postSoftwareSignal);
}


/**
 * @brief This function abstracts whether the deployment mechanism has succeeded.
 *
 */
int Deployment_GetDeploymentState()
{
	// read from the deployment digital pin.
	return Persistant_Storage_Get(EEPROM_DEPLOYMENT_ADDR);
}
