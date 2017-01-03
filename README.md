# xBeeSensorNetwork
One Arduino Uno connected to an xBee configured as end device (jeff),
sending the temperature and humidity of an DHT22 Sensor to the coordinator xBee (daisy).
xBee router (hugo) is just there to complete a mesh network.
Jeff can send the data via hugo to daisy if it's more convenient.

## why?
While trying and failing to get working every implementation of MQTT-SN available,
I decided to do something else (at least something) with my three xBee laying around.
