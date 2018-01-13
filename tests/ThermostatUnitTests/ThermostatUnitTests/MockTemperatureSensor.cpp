#include "stdafx.h"
#include "MockTemperatureSensor.h"

Temperature MockTemperatureSensor::getTemperature()
{
	return Temperature(this->temp);
}

void MockTemperatureSensor::setTemperature(Temperature * temp)
{
	delete this->temp;
	this->temp = temp;
}
