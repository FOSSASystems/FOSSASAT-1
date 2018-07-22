#include <Arduino.h>
#include "configuration.h"
#include "persistant_storage.h"
#include "deployment.h"

//////////////////////////
// DEPLOYMENT FUNCTIONS //
//////////////////////////
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
  }

}

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

