extern "C"{
#include "mgos.h"
#include <sys/time.h>
}

#include <ctime>

#include "../lib/thermostat/Thermostat.h"
#include "../lib/thermostat/Relay.h"
#include "../lib/thermostat/sensors/TMP36.h"
#include "../lib/thermostat/Temperature.h"
#include "../lib/thermostat/Schedule.h"

Thermostat * thermostatA;
Relay * heatingRelay;
Relay * coolingRelay;
TemperatureSensor * temp;
Schedule * schedule;

mgos_timer_id timerID;

void setup();
void loop(void *arg);

enum mgos_app_init_result mgos_app_init(void) {
  setup();
  return MGOS_APP_INIT_SUCCESS;
}

void setup()
{
    heatingRelay = new Relay(41);
    coolingRelay = new Relay(40);

    temp = new TMP36(36);

     thermostatA  = new Thermostat(
         temp,
         heatingRelay,
         coolingRelay
         );
    LOG(LL_INFO, ("Creating Schedule Data"));
    ScheduleData sd = {61.0f, 62.0f, 63.0f, 64.0f, 65.0f, 66.0f, 67.0f, 68.0f, 69.0f, 70.0f, 71.0f, 72.0f, 73.0f, 74.0f, 75.0f, 76.0f, 77.0f, 78.0f, 79.0f, 80.0f, 81.0f, 82.0f, 83.0f, 84.0f, 85.0f, 86.0f, 87.0f, 88.0f};

    LOG(LL_INFO, ("Creating Schedule"));
    schedule = new Schedule(sd);

    LOG(LL_INFO, ("Setting Schedule"));
    thermostatA->setSchedule(schedule);

    LOG(LL_INFO, ("Setting Target"));
    Temperature temp = Temperature(75, Temperature::Unit::FARENHEIT);
    thermostatA->setTarget(temp);

    LOG(LL_INFO, ("Setting Operating Mode "));
    thermostatA->setOperatingMode(Thermostat::OperatingModes::Heating);

    LOG(LL_INFO, ("Setting Source"));
    thermostatA->setSource(Thermostat::TargetSource::Scheduled);

    timerID = mgos_set_timer(1000, 1, &loop, NULL);
}

void loop(void *arg)
{
    thermostatA->loop();

    LOG(LL_INFO, ("----------------Update Message----------------"));
    thermostatA->getStatus();
    LOG(LL_INFO, ("----------------------------------------------"));   
    
    LOG(LL_INFO, ("Time: %lf", ((std::time(NULL) - 60 * 60 * 24 * 6) % (60 * 60 * 24)) / (60.0f * 60.0f)));
    

    (void)arg;
}