# Generic Thermostat Library

This library provides a abstract thermostat system with 7 day Morning, Daytime, Evening, and Night schedule. It supports heating and cooling controls via a relay abstraction layer and also temperature readings via a sensor abstraction layer. 

This was previously written speciffically for the Mongoose-OS platform by Cesanta. I have since isolated the functionality of the Thermostat into a library which should be compatible with any embedded platform. 

## Relay Abstraction Layer
To connect to your relays, create an implementation of the Relay class. Be sure to update the `Relay::on` variable so that the thermostat can check your relay's status. 

## Sensor Abstraction Layer
When creating a sensor be sure to properly format the temperature into a Temperature object. As long as the Temperature object is created with the proper units attached to it the thermostat can handle converting the temperature for display modes and scheduling.