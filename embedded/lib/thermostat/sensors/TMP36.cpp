#define CS_P_ESP32 15

#include "TMP36.h"
#include "../Temperature.h"
#include "mgos_adc.h"

TMP36::TMP36(int pin)
{
    this->pin = pin;
    mgos_adc_enable(pin);
}

Temperature * TMP36::getTemperature()
{
    return new Temperature(getCelsius(), Temperature::Unit::CELSIUS);
}

float TMP36::getCelsius()
{
    return mgos_adc_read(this->pin) * 0.4883f - 50.0f;
}