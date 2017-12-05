#pragma once

#include "TemperatureSensor.h"

class MockTemperatureSensor : public TemperatureSensor
{
public: 
	// Inherited via TemperatureSensor
	virtual Temperature * getTemperature() override;
	void setTemperature(Temperature * temp);
private:
	Temperature * temp = 0;
	
};
