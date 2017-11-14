#pragma once

#include "Temperature.h"
#include "Relay.h"
#include "TemperatureSensor.h"
#include "Schedule.h"

class Thermostat
{
public:
    enum OperatingModes
    {
        Heating,
        Cooling,
        Off
    };

    enum TargetSource
    {
        Scheduled,
        Manual
    };

    const int WaitPeriod = 15;
    Temperature Threshold = Temperature(1.5f, Temperature::Unit::FARENHEIT);

    Thermostat(
        TemperatureSensor * temperatureSensor, 
        Relay * heatingRelay,
        Relay * coolingRelay);

    void setOperatingMode(OperatingModes mode);

    OperatingModes getOperatingMode();

    void setTarget(Temperature * target);

    Temperature * getTarget();

    Temperature * getTemperature();

    void loop();

    Thermostat() { };

    String getStatus();

private:
   // Thermostat(Thermostat * thermostat) { };

    void runHeat();

    void runOff();

    void runCool();

    OperatingModes operatingMode = Off;
    Temperature * target;
    TemperatureSensor * temperatureSensor;
    Relay * heatingRelay;
    Relay * coolingRelay;
    int debounce = 0;
    TargetSource targetSource = Thermostat::TargetSource::Manual;
};