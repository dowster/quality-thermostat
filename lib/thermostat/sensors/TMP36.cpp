#define CS_P_ESP32 15
extern "C" {
#include "mgos.h"
}

#include "TMP36.h"
#include "../Temperature.h"
#include "mgos_adc.h"

TMP36::TMP36(int pin)
{
    this->pin = pin;
    mgos_adc_enable(pin);
}

Temperature TMP36::getTemperature()
{
    return Temperature(getCelsius(), Temperature::Unit::CELSIUS);
}

float TMP36::getCelsius()
{
    float converted = mgos_adc_read(this->pin) * (100.0f / 1024.0f) - 50.0f;
    LOG(LL_DEBUG, ("PIN Reading: %d, Converted: %f", mgos_adc_read(this->pin), converted));
    return converted;
}