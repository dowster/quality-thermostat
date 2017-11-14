
#include "Thermostat.h"
#include "Temperature.h"
#include "Arduino.h"

Thermostat::Thermostat(
    TemperatureSensor * temperatureSensor,
    Relay * heatingRelay, 
    Relay * coolingRelay)
{
    this->temperatureSensor = temperatureSensor;

    this->heatingRelay = heatingRelay;
    this->coolingRelay = coolingRelay;

    this->setOperatingMode(Thermostat::OperatingModes::Off);
    this->setTarget(new Temperature(69, Temperature::Unit::FARENHEIT));    
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
    if(this->target != NULL)
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

String Thermostat::getStatus()
{
    String statusString = String("Thermostat Status:\n");
    statusString += "  Current Temperature: ";
    Temperature * currentTemp = this->getTemperature();
    statusString += currentTemp->getTemperature(Temperature::Unit::FARENHEIT);
    statusString += "F\n";

    // statusString += "  Target - Threshold: " + String((*this->getTarget() - this->Threshold).getTemperature(Temperature::Unit::FARENHEIT)) + "\n";

    // statusString += "  Current Temp < (Target - Threshold): " + String((*currentTemp < *this->getTarget() - this->Threshold) ? "Yes\n" : "No\n");

    delete currentTemp;

    statusString += "  Current Threshold: " + String(this->Threshold.getTemperature(Temperature::Unit::FARENHEIT)) + "F\n";

    statusString += "  Current Target: " + String(this->target->getTemperature(Temperature::Unit::FARENHEIT)) + "F\n";

    statusString += "  Current Debounce Value: " + String(this->debounce) + "\n";

    statusString += "  Heating Relay Status: " + String((this->heatingRelay->getActivated()) ? "On\n" : "Off\n");
    statusString += "  Cooling Relay Status: " + String((this->coolingRelay->getActivated()) ? "On\n" : "Off\n");

    return statusString;
}