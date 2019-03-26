extern "C"{
    #include "mgos.h"
    #include <sys/time.h>
    #include "mgos_gpio.h"
    #include "mgos_mqtt.h"
    #include "mgos_aws_shadow.h"
}


#include <ctime>

#include "../lib/thermostat/Thermostat.h"
#include "../lib/thermostat/relays/HardwareRelay.h"
#include "../lib/thermostat/sensors/TMP36.h"
#include "../lib/thermostat/Temperature.h"
#include "../lib/thermostat/Schedule.h"

#define TEMP_UP_BUTTON 33
#define TEMP_DOWN_BUTTON 32
#define CYCLE_MODE_BUTTON 27

#define BUTTON_DEBOUNCE 1000


Thermostat * thermostatA;
HardwareRelay * heatingRelay;
HardwareRelay * coolingRelay;
TemperatureSensor * temp;
Schedule * schedule;

mgos_timer_id timerID;

void setup();
void loop(void *arg);

void increaseTemperatureButton(int pin, void* arg);
void decreaseTemperatureButton(int pin, void* arg);
void cycleOperatingMode(int pin, void* arg);
void sendMQTTUpdate(Thermostat * thermostat);
void MQTTConnectHandler(struct mg_connection *nc, int ev, void *ev_data MG_UD_ARG(void *user_data));

void awsShadowHandler (
    void *arg, enum mgos_aws_shadow_event ev, uint64_t version,
    const struct mg_str reported, const struct mg_str desired,
    const struct mg_str reported_md, const struct mg_str desired_md);

enum mgos_app_init_result mgos_app_init(void) {
    setup();
    return MGOS_APP_INIT_SUCCESS;
}

void setup()
{
    heatingRelay = new HardwareRelay(14);
    coolingRelay = new HardwareRelay(12);

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
    Temperature temp = Temperature(60, Temperature::Unit::FARENHEIT);
    thermostatA->setTarget(temp);

    LOG(LL_INFO, ("Setting Operating Mode "));
    thermostatA->setOperatingMode(Thermostat::OperatingModes::Heating);

    LOG(LL_INFO, ("Setting Source"));
    thermostatA->setSource(Thermostat::TargetSource::Manual);

    timerID = mgos_set_timer(1000, 1, &loop, NULL);
    mgos_gpio_set_mode(TEMP_UP_BUTTON, MGOS_GPIO_MODE_INPUT);
    mgos_gpio_set_mode(TEMP_DOWN_BUTTON, MGOS_GPIO_MODE_INPUT);
    mgos_gpio_set_mode(CYCLE_MODE_BUTTON, MGOS_GPIO_MODE_INPUT);

    mgos_gpio_set_button_handler(TEMP_UP_BUTTON, MGOS_GPIO_PULL_UP, MGOS_GPIO_INT_EDGE_NEG, BUTTON_DEBOUNCE, &increaseTemperatureButton, NULL);
    mgos_gpio_set_button_handler(TEMP_DOWN_BUTTON, MGOS_GPIO_PULL_UP, MGOS_GPIO_INT_EDGE_NEG, BUTTON_DEBOUNCE, &decreaseTemperatureButton, NULL);
    mgos_gpio_set_button_handler(CYCLE_MODE_BUTTON, MGOS_GPIO_PULL_UP, MGOS_GPIO_INT_EDGE_NEG, BUTTON_DEBOUNCE, &cycleOperatingMode, NULL);

    mgos_aws_shadow_set_state_handler(awsShadowHandler, thermostatA);
}

void loop(void *arg)
{
    thermostatA->loop();

    LOG(LL_INFO, ("----------------Update Message----------------"));
    thermostatA->getStatus();
    LOG(LL_INFO, ("----------------------------------------------"));   

    sendMQTTUpdate(thermostatA);

    (void)arg;
}

void increaseTemperatureButton(int pin, void* arg)
{
    LOG(LL_INFO, ("Increase Temperature Button Pressed"));
    
    Temperature newTemp = thermostatA->getTarget() + Temperature(1.0f, Temperature::Unit::FARENHEIT);
    thermostatA->setTarget(newTemp);

    (void) pin;
}

void decreaseTemperatureButton(int pin, void* arg)
{
    LOG(LL_INFO, ("Decrease Temperature Button Pressed"));
    
    Temperature newTemp = thermostatA->getTarget() - Temperature(1.0f, Temperature::Unit::FARENHEIT);
    thermostatA->setTarget(newTemp);

    (void) pin;
}

void cycleOperatingMode(int pin, void* arg)
{
    LOG(LL_INFO, ("Cycle Operation Mode Pressed"));

    switch(thermostatA->getOperatingMode()) 
    {
        case Thermostat::OperatingModes::Cooling:
            thermostatA->setOperatingMode(Thermostat::OperatingModes::Heating);
            break;
        case Thermostat::OperatingModes::Heating:
            thermostatA->setOperatingMode(Thermostat::OperatingModes::Off);
            break;
        case Thermostat::OperatingModes::Off:
            thermostatA->setOperatingMode(Thermostat::OperatingModes::Cooling);
            break;
        default:
            thermostatA->setOperatingMode(Thermostat::OperatingModes::Cooling);
    }

    (void) pin;
}

void sendMQTTUpdate(Thermostat * thermostat) {
    mgos_mqtt_pub("test", "Sending MQTT Update", 19, 1, false);
    mgos_aws_shadow_updatef(
        0, 
        "{reported:{mode: %d, temp: %f, target: %f, heating: %s, cooling: %s, waitTime: %d, debounce: %d}}", 
        thermostat->getOperatingMode(), thermostat->getTemperature().getTemperature(), thermostat->getTarget().getTemperature(),
        thermostat->getHeatingRelay()->getActivated() ? "true" : "false",thermostat->getCoolingRelay()->getActivated() ? "true" : "false", thermostat->WaitPeriod, thermostat->getDebounce());
}

void MQTTConnectHandler(struct mg_connection *nc, int ev, void *ev_data MG_UD_ARG(void *user_data))
{
    LOG(LL_INFO, ("MQTT Con Handler (%d)", ev));
	
    (void) nc;
    (void) ev_data;
    (void) user_data;
}

void awsShadowHandler (
    void *arg, enum mgos_aws_shadow_event ev, uint64_t version,
    const struct mg_str reported, const struct mg_str desired,
    const struct mg_str reported_md, const struct mg_str desired_md) {
    
    LOG(LL_INFO, ("Processing shadow event: %d", ev));

    if(reported.len > 0) {
        int mode = -1;
        float target = -1;

        LOG(LL_INFO, ("Reported=%.*s", reported.len, reported.p));
        
        LOG(LL_INFO, ("Got to 1"));
        int parsed = json_scanf(reported.p, reported.len, 
                                "{mode: %d, target: %f}",
                                &mode,
                                &target);
                                
            LOG(LL_INFO, ("Parsed (%d) parameters: [mode=%d, target=%f]", parsed, mode, target));
        if(parsed == 2) {

            Thermostat * thermostat = (Thermostat*)arg;
            thermostat->getStatus();
            thermostat->setTarget(Temperature(target, Temperature::Unit::FARENHEIT));

            thermostat->setOperatingMode((Thermostat::OperatingModes)mode);
        }

    }
}