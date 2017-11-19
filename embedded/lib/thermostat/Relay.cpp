#include "Relay.h"
#include "mgos_gpio.h"

Relay::Relay(int pin)
{
    this->pin = pin;
    mgos_gpio_set_mode(pin, MGOS_GPIO_MODE_OUTPUT);
}

void Relay::turnOn()
{
    if(!this->activated)
    {
        this->activated = true;
        mgos_gpio_write(this->pin, true);
    }
}

void Relay::turnOff()
{
    if(this->activated)
    {
        this->activated = false;
        mgos_gpio_write(this->pin, false);
    }
}

bool Relay::getActivated()
{
    return this->activated;
}