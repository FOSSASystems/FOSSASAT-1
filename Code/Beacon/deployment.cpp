/**
 * @file deployment.cpp
 * @brief This abstracts the deployment mechanism and sequence into callable functions.
 * SX1278.
*/


#include <Arduino.h>
#include "configuration.h"
#include "persistant_storage.h"
#include "deployment.h"

   
/**
 * @brief Power the deployment release mechanism (spring and hinge)
 *
 * Checks against the EEPROM value to see if it has already deployed, if not it waits 5s, then sends a digital high
 * value to the DIGITAL_OUT_MOSFET_1&2 pins and then after another 5s switches them back to low.
 */
void Deployment_PowerDeploymentMosfets()
{
  // check against eeprom for already deployed.
  if (Persistant_Storage_Get(EEPROM_DEPLOYMENT_ADDR) == HIGH)
  {
    // the deployment has occured.
    digitalWrite(DIGITAL_OUT_MOSFET_1, LOW);
    digitalWrite(DIGITAL_OUT_MOSFET_2, LOW);

    return;
  }
  else
  {
    // send high signal after 5seconds if not already deployed.
    delay(5000);

    // set the eeprom value so it doesn't turn the mosfets on again.
    Persistant_Storage_Set(EEPROM_DEPLOYMENT_ADDR, HIGH);

    // burn the nichrome wires.
    digitalWrite(DIGITAL_OUT_MOSFET_1, HIGH);
    digitalWrite(DIGITAL_OUT_MOSFET_2, HIGH);

    delay(5000);

    digitalWrite(DIGITAL_OUT_MOSFET_1, LOW);
    digitalWrite(DIGITAL_OUT_MOSFET_2, LOW);

  }

}

   
/**
 * @brief This function abstracts whether the deployment mechanism has succeeded.
 *
 */
bool Deployment_GetDeploymentState()
{
  // read from the deployment digital pin.
  int val = digitalRead(DIGITAL_IN_DEPLOYMENT_STATE);

  // return true if high and false if low.
  if (val == LOW)
  {
    // deployment pin returning LOW threshold, deployment failed.
    return false;
  }
  else
  {
    // deployment pin returning HIGH threshold, deployment success.
    return true;
  }
}

