#pragma once

#include "../TemperatureSensor.h"
#include "../Temperature.h"

class TMP36 : public TemperatureSensor
{
public:
    TMP36(int pin);

    virtual Temperature getTemperature();

private:
    float getCelsius();

    int pin = -1;
};