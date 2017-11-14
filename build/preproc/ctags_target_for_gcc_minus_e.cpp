# 1 "/home/dowster/Programs/thermostat/embedded/main/main.ino"
# 1 "/home/dowster/Programs/thermostat/embedded/main/main.ino"


# 4 "/home/dowster/Programs/thermostat/embedded/main/main.ino" 2
# 5 "/home/dowster/Programs/thermostat/embedded/main/main.ino" 2

# 7 "/home/dowster/Programs/thermostat/embedded/main/main.ino" 2
# 8 "/home/dowster/Programs/thermostat/embedded/main/main.ino" 2



Thermostat * thermostatA;
Relay * heatingRelay;
Relay * coolingRelay;

void setup()
{
    heatingRelay = new Relay(5);
    coolingRelay = new Relay(6);

    thermostatA = new Thermostat(
        new TMP36(0),
        heatingRelay,
        coolingRelay
        );

    thermostatA->setTarget(new Temperature(75, Temperature::Unit::FARENHEIT));
    thermostatA->setOperatingMode(Thermostat::OperatingModes::Cooling);

    Serial.begin(115200);

}

void loop()
{
    thermostatA->loop();

    Serial.println("----------------Update Message----------------");
    Serial.println(thermostatA->getStatus());
    Serial.println("----------------------------------------------");

    delay(100);


}
