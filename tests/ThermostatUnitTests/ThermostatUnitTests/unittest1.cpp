#define LL_INFO 0
#define LOG(a,b) (void)

#include "stdafx.h"
#include "CppUnitTest.h"

#include <string.h>
#include <cstring>

#include "Thermostat.h"
#include "MockRelay.h"
#include "MockTemperatureSensor.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ThermostatUnitTests
{		
	TEST_CLASS(UnitTest1)
	{
	public:

		TEST_METHOD(TestCoolingTurnOnFromTarget)
		{
			Relay * hr = new MockRelay();
			Relay * cr = new MockRelay();
			MockTemperatureSensor * tempSensor = new MockTemperatureSensor();

			Thermostat * testThermostat = new Thermostat(tempSensor, hr, cr);

			Assert::IsFalse(hr->getActivated());
			Assert::IsFalse(cr->getActivated());

			testThermostat->changeOperatingMode(Thermostat::OperatingModes::Cooling);
			testThermostat->setSource(Thermostat::TargetSource::Manual);
			testThermostat->setTarget(Temperature(75, Temperature::Unit::FARENHEIT));

			tempSensor->setTemperature(new Temperature(80, Temperature::Unit::FARENHEIT));

			for (int i = 0; i < 14; i++)
				testThermostat->loop();

			Assert::IsFalse(hr->getActivated());
			Assert::IsFalse(cr->getActivated());

			testThermostat->loop();

			Assert::IsFalse(hr->getActivated());
			Assert::IsTrue(cr->getActivated());
		}

		TEST_METHOD(TestCoolingTurnOffFromTarget)
		{
			Relay * hr = new MockRelay();
			Relay * cr = new MockRelay();
			MockTemperatureSensor * tempSensor = new MockTemperatureSensor();

			Thermostat * testThermostat = new Thermostat(tempSensor, hr, cr);

			cr->turnOn();

			Assert::IsFalse(hr->getActivated());
			Assert::IsTrue(cr->getActivated());

			testThermostat->changeOperatingMode(Thermostat::OperatingModes::Cooling);
			testThermostat->setSource(Thermostat::TargetSource::Manual);
			testThermostat->setTarget(Temperature(75, Temperature::Unit::FARENHEIT));

			tempSensor->setTemperature(new Temperature(70, Temperature::Unit::FARENHEIT));

			for (int i = 0; i < 14; i++)
				testThermostat->loop();

			Assert::IsFalse(hr->getActivated());
			Assert::IsTrue(cr->getActivated());

			testThermostat->loop();
			testThermostat->loop();



			Assert::IsFalse(hr->getActivated());
			Assert::IsFalse(cr->getActivated());
		}
		
		TEST_METHOD(TestHeatingTurnOnFromTarget)
		{
			Relay * hr = new MockRelay();
			Relay * cr = new MockRelay();
			MockTemperatureSensor * tempSensor = new MockTemperatureSensor();

			Thermostat * testThermostat = new Thermostat(tempSensor, hr, cr);

			Assert::IsFalse(hr->getActivated());
			Assert::IsFalse(cr->getActivated());

			testThermostat->changeOperatingMode(Thermostat::OperatingModes::Heating);
			testThermostat->setSource(Thermostat::TargetSource::Manual);
			testThermostat->setTarget(Temperature(75, Temperature::Unit::FARENHEIT));

			tempSensor->setTemperature(new Temperature(70, Temperature::Unit::FARENHEIT));

			for (int i = 0; i < 14; i++)
				testThermostat->loop();

			Assert::IsFalse(hr->getActivated());
			Assert::IsFalse(cr->getActivated());

			testThermostat->loop();

			Assert::IsTrue(hr->getActivated());
			Assert::IsFalse(cr->getActivated());
		}

		TEST_METHOD(TestHeatingTurnOffFromTarget)
		{
			Relay * hr = new MockRelay();
			Relay * cr = new MockRelay();
			MockTemperatureSensor * tempSensor = new MockTemperatureSensor();

			Thermostat * testThermostat = new Thermostat(tempSensor, hr, cr);

			hr->turnOn();

			Assert::IsTrue(hr->getActivated());
			Assert::IsFalse(cr->getActivated());

			testThermostat->changeOperatingMode(Thermostat::OperatingModes::Heating);
			testThermostat->setSource(Thermostat::TargetSource::Manual);
			testThermostat->setTarget(Temperature(75, Temperature::Unit::FARENHEIT));

			tempSensor->setTemperature(new Temperature(80, Temperature::Unit::FARENHEIT));

			for (int i = 0; i < 14; i++)
				testThermostat->loop();

			Assert::IsTrue(hr->getActivated());
			Assert::IsFalse(cr->getActivated());

			testThermostat->loop();



			Assert::IsFalse(hr->getActivated());
			Assert::IsFalse(cr->getActivated());
		}

		TEST_METHOD(TemperatureConversionFromFahrenheitToCelsius)
		{
			Temperature temp1 = Temperature(50.0f, Temperature::Unit::FARENHEIT);
			Assert::AreEqual(temp1.getTemperature(Temperature::Unit::CELSIUS), 10.0f);
			Assert::AreEqual(temp1.getTemperature(Temperature::Unit::FARENHEIT), 50.0f);
		}

		TEST_METHOD(TemperatureConversionFromCelsiusToFahrenheit)
		{
			Temperature temp1 = Temperature(10.0f, Temperature::Unit::CELSIUS);
			Assert::AreEqual(temp1.getTemperature(Temperature::Unit::CELSIUS), 10.0f);
			Assert::AreEqual(temp1.getTemperature(Temperature::Unit::FARENHEIT), 50.0f);
		}

		TEST_METHOD(TestScheduleReturnsCorrectValues)
		{
			ScheduleData data = { 61.0f, 62.0f, 63.0f, 64.0f, 65.0f, 66.0f, 67.0f, 68.0f, 69.0f, 70.0f, 71.0f, 72.0f, 73.0f, 74.0f, 75.0f, 76.0f, 77.0f, 78.0f, 79.0f, 80.0f, 81.0f, 82.0f, 83.0f, 84.0f, 85.0f, 86.0f, 87.0f, 88.0f };
			Schedule temp1 = Schedule(data);

			for (int dayOfWeek = 0; dayOfWeek < 7; dayOfWeek++)
			{
				Assert::AreEqual(
					temp1.getTarget(dayOfWeek, 2.0f).getTemperature(Temperature::Unit::FARENHEIT)
					, data[dayOfWeek * 4 + 3]);
				Assert::AreEqual(
					temp1.getTarget(dayOfWeek, 4.0f).getTemperature(Temperature::Unit::FARENHEIT)
					, data[dayOfWeek * 4 + 0]);
				Assert::AreEqual(
					temp1.getTarget(dayOfWeek, 6.0f).getTemperature(Temperature::Unit::FARENHEIT)
					, data[dayOfWeek * 4 + 0]);
				Assert::AreEqual(
					temp1.getTarget(dayOfWeek, 8.0f).getTemperature(Temperature::Unit::FARENHEIT)
					, data[dayOfWeek * 4 + 1]);
				Assert::AreEqual(
					temp1.getTarget(dayOfWeek, 12.0f).getTemperature(Temperature::Unit::FARENHEIT)
					, data[dayOfWeek * 4 + 1]);
				Assert::AreEqual(
					temp1.getTarget(dayOfWeek, 14.0f).getTemperature(Temperature::Unit::FARENHEIT)
					, data[dayOfWeek * 4 + 2]);
				Assert::AreEqual(
					temp1.getTarget(dayOfWeek, 18.0f).getTemperature(Temperature::Unit::FARENHEIT)
					, data[dayOfWeek * 4 + 2]);
				Assert::AreEqual(
					temp1.getTarget(dayOfWeek, 20.0f).getTemperature(Temperature::Unit::FARENHEIT)
					, data[dayOfWeek * 4 + 3]);
				Assert::AreEqual(
					temp1.getTarget(dayOfWeek, 22.0f).getTemperature(Temperature::Unit::FARENHEIT)
					, data[dayOfWeek * 4 + 3]);
			}
		}


		TEST_METHOD(TestTransitionSystemFromCoolingToOff)
		{
			Relay * hr = new MockRelay();
			Relay * cr = new MockRelay();
			MockTemperatureSensor * tempSensor = new MockTemperatureSensor();

			Thermostat * testThermostat = new Thermostat(tempSensor, hr, cr);

			Assert::IsFalse(hr->getActivated());
			Assert::IsFalse(cr->getActivated());

			testThermostat->changeOperatingMode(Thermostat::OperatingModes::Cooling);
			testThermostat->setSource(Thermostat::TargetSource::Manual);
			testThermostat->setTarget(Temperature(75, Temperature::Unit::FARENHEIT));

			tempSensor->setTemperature(new Temperature(80, Temperature::Unit::FARENHEIT));

			for (int i = 0; i < 15; i++)
				testThermostat->loop();

			Assert::IsFalse(hr->getActivated());
			Assert::IsTrue(cr->getActivated());

			testThermostat->changeOperatingMode(Thermostat::OperatingModes::Off);

			testThermostat->loop();

			Assert::IsFalse(hr->getActivated());
			Assert::IsFalse(cr->getActivated());
		}

		TEST_METHOD(TestTransitionSystemFromHeatingToOff)
		{
			Relay * hr = new MockRelay();
			Relay * cr = new MockRelay();
			MockTemperatureSensor * tempSensor = new MockTemperatureSensor();

			Thermostat * testThermostat = new Thermostat(tempSensor, hr, cr);

			Assert::IsFalse(hr->getActivated());
			Assert::IsFalse(cr->getActivated());

			testThermostat->changeOperatingMode(Thermostat::OperatingModes::Heating);
			testThermostat->setSource(Thermostat::TargetSource::Manual);
			testThermostat->setTarget(Temperature(75, Temperature::Unit::FARENHEIT));

			tempSensor->setTemperature(new Temperature(70, Temperature::Unit::FARENHEIT));

			for (int i = 0; i < 15; i++)
				testThermostat->loop();

			Assert::IsTrue(hr->getActivated());
			Assert::IsFalse(cr->getActivated());

			testThermostat->changeOperatingMode(Thermostat::OperatingModes::Off);

			testThermostat->loop();

			Assert::IsFalse(hr->getActivated());
			Assert::IsFalse(cr->getActivated());
		}

		TEST_METHOD(TestSystemOff)
		{
			Relay * hr = new MockRelay();
			Relay * cr = new MockRelay();
			MockTemperatureSensor * tempSensor = new MockTemperatureSensor();

			Thermostat * testThermostat = new Thermostat(tempSensor, hr, cr);

			Assert::IsFalse(hr->getActivated());
			Assert::IsFalse(cr->getActivated());

			testThermostat->changeOperatingMode(Thermostat::OperatingModes::Off);
			testThermostat->setSource(Thermostat::TargetSource::Manual);
			testThermostat->setTarget(Temperature(75, Temperature::Unit::FARENHEIT));

			tempSensor->setTemperature(new Temperature(70, Temperature::Unit::FARENHEIT));

			for (int i = 0; i < 150; i++)
				testThermostat->loop();

			Assert::IsFalse(hr->getActivated());
			Assert::IsFalse(cr->getActivated());
		}

	};
}