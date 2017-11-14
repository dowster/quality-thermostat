#pragma once

#include "Relay.h"
#include "Arduino.h"

Relay::Relay(int pin)
{
    this->pin = pin;
    pinMode(pin, OUTPUT);
}

void Relay::turnOn()
{
    if(!this->activated)
    {
        this->activated = true;
        digitalWrite(this->pin, HIGH);
    }
}

void Relay::turnOff()
{
    if(this->activated)
    {
        this->activated = false;
        digitalWrite(this->pin, LOW);
    }
}

bool Relay::getActivated()
{
    return this->activated;
}