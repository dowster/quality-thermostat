#pragma once

#include "TMP36.h"
#include "../Temperature.h"
#include "Arduino.h"

TMP36::TMP36(int pin)
{
    this->pin = pin;
}

Temperature * TMP36::getTemperature()
{
    return new Temperature(getCelsius(), Temperature::Unit::CELSIUS);
}

float TMP36::getCelsius()
{
    return analogRead(this->pin) * 0.4883f - 50.0f;
}