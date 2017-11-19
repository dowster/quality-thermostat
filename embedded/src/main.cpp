extern "C"{
#include "mgos.h"
}

#include "../lib/thermostat/Thermostat.h"
#include "../lib/thermostat/Relay.h"
#include "../lib/thermostat/sensors/TMP36.h"
#include "../lib/thermostat/Temperature.h"

Thermostat * thermostatA;
Relay * heatingRelay;
Relay * coolingRelay;
TemperatureSensor * temp;

mgos_timer_id timerID;

void setup();
void loop(void *arg);

enum mgos_app_init_result mgos_app_init(void) {
  setup();
  return MGOS_APP_INIT_SUCCESS;
}

void setup()
{
    heatingRelay = new Relay(5);
    coolingRelay = new Relay(4);

    temp = new TMP36(0);

     thermostatA  = new Thermostat(
         temp,
         heatingRelay,
         coolingRelay
         );

    thermostatA->setTarget(new Temperature(75, Temperature::Unit::FARENHEIT));
    thermostatA->setOperatingMode(Thermostat::OperatingModes::Heating);

    timerID = mgos_set_timer(1000, 1, &loop, NULL);
}

void loop(void *arg)
{
    thermostatA->loop();

    LOG(LL_INFO, ("----------------Update Message----------------"));
    thermostatA->getStatus();
    LOG(LL_INFO, ("----------------------------------------------"));   
    
    (void)arg;
}