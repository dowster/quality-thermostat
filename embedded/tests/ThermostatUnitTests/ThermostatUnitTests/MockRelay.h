#pragma once

#include "Relay.h"

class MockRelay : public Relay
{
public:
	void turnOn() {
		this->activated = true;
	};

	void turnOff() {
		this->activated = false;
	};
};