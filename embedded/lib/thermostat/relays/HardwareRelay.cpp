#include "HardwareRelay.h"
#include "mgos_gpio.h"

HardwareRelay::HardwareRelay(int pin)
{
    this->pin = pin;
    mgos_gpio_set_mode(pin, MGOS_GPIO_MODE_OUTPUT);
}

void HardwareRelay::turnOn()
{
    if(!this->activated)
    {
        this->activated = true;
        mgos_gpio_write(this->pin, true);
    }
}

void HardwareRelay::turnOff()
{
    if(this->activated)
    {
        this->activated = false;
        mgos_gpio_write(this->pin, false);
    }
}