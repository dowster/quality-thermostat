extern "C"{
#include "mgos.h"
}

#include <cstddef>

#include "Thermostat.h"
#include "Temperature.h"

Thermostat::Thermostat(
    TemperatureSensor * temperatureSensor,
    Relay * heatingRelay, 
    Relay * coolingRelay)
{
    this->temperatureSensor = temperatureSensor;

    this->heatingRelay = heatingRelay;
    this->coolingRelay = coolingRelay;

    //this->setOperatingMode(Thermostat::OperatingModes::Off);
    //this->setTarget(new Temperature(69, Temperature::Unit::FARENHEIT));    
}

void Thermostat::setOperatingMode(OperatingModes mode)
{
    this->operatingMode = mode;
}

Thermostat::OperatingModes Thermostat::getOperatingMode()
{
    return this->operatingMode;
}

void Thermostat::setTarget(Temperature * target)
{
    //if(this->target != NULL)
        delete this->target;

    this->target = target;
}

Temperature * Thermostat::getTarget()
{
    return this->target;
}

Temperature * Thermostat::getTemperature()
{
    return this->temperatureSensor->getTemperature();
}

void Thermostat::loop()
{
    switch(this->operatingMode)
    {
        case Heating:
            this->runHeat();
            break;
        case Cooling: 
            this->runCool();
            break;
        case Off:
            this->runOff();
            break;
    }
}

void Thermostat::runHeat()
{
        Temperature * currentTemp = this->temperatureSensor->getTemperature();

        if(
            !this->heatingRelay->getActivated() &&
            *currentTemp < *this->getTarget() - this->Threshold 
        )
        {
            if(++this->debounce >= this->WaitPeriod)
            {
                this->heatingRelay->turnOn();
                this->debounce = 0;
            }
        }
        else if(
            this->heatingRelay->getActivated() &&
            *currentTemp > *this->getTarget() + this->Threshold
        )
        {
            if(++this->debounce >= this->WaitPeriod)
            {
                this->heatingRelay->turnOff();
                this->debounce = 0;
            }
        }
        else
        {
            this->debounce = 0;
        }

        delete currentTemp;
    
}

void Thermostat::runCool()
{
        Temperature * currentTemp = this->temperatureSensor->getTemperature();

        if(
            !this->coolingRelay->getActivated() &&
            *currentTemp > *this->getTarget() + this->Threshold
        )
        {
            if(++this->debounce >= this->WaitPeriod)
            {
                this->coolingRelay->turnOn();
                this->debounce = 0;
            }
        }
        else if(
            this->coolingRelay->getActivated() &&
            *currentTemp < *this->getTarget() - this->Threshold
        )
        {
            if(++this->debounce >= this->WaitPeriod)
            {
                this->coolingRelay->turnOff();
                this->debounce = 0;
            }
        }
        else
        {
            this->debounce = 0;
        }

        delete currentTemp;
    
}

void Thermostat::runOff()
{
    if(this->heatingRelay->getActivated())
        this->heatingRelay->turnOff();
    if(this->coolingRelay->getActivated())
        this->coolingRelay->turnOff();
    if(this->debounce != 0)
        this->debounce = 0;
}

void Thermostat::getStatus()
{
    Temperature * currentTemp = this->getTemperature();

    LOG(LL_INFO, ("  Current Temperature: %f F", currentTemp->getTemperature(Temperature::Unit::FARENHEIT)));
    delete currentTemp;

    LOG(LL_INFO, ("  Current Threshold: %f F", this->Threshold.getTemperature(Temperature::Unit::FARENHEIT)));

    LOG(LL_INFO, ("  Current Target: %f F", this->target->getTemperature(Temperature::Unit::FARENHEIT)));

    LOG(LL_INFO, ("  Current Debounce Value: %i", this->debounce));

    LOG(LL_INFO, ("  Heating Relay Status: %s", (this->heatingRelay->getActivated()) ? "On\n" : "Off\n"));
    LOG(LL_INFO, ("  Cooling Relay Status: %s", (this->coolingRelay->getActivated()) ? "On\n" : "Off\n"));
}