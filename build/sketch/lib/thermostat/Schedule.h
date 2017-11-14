#pragma once

#include "Temperature.h"

class Schedule
{
public:
    Schedule() {};

    Temperature getTarget() {return Temperature(0.0, Temperature::Unit::CELSIUS); };
};