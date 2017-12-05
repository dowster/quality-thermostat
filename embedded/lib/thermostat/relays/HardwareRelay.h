#include "Relay.h"

class HardwareRelay : public Relay
{
public:    
    HardwareRelay(int pin);

    void turnOn();

    void turnOff();  

private:
    int pin = -1;
};