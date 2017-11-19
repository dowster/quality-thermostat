#pragma once

class Relay
{
public:    
    Relay(int pin);

    void turnOn();

    void turnOff();

    bool getActivated();

    Relay() { };
    
   // Relay(const Relay& relay) { };

private:
    bool activated = false;
    int pin = -1;
};