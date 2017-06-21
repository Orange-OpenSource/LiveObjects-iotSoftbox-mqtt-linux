# LiveObjects Iot Device Sample

This is the LiveObjects IoT Device sample for any Linux running device.

The application:

1. Connects to the [Datavenue Live Objects Plaftorm](https://liveobjects.orange-business.com/doc/html/lo_manual.html), using:
  * an optional secure connection (TLS)
  * the LiveObjects mode: [Json+Device](https://liveobjects.orange-business.com/doc/html/lo_manual.html#MQTT_MODE_DEVICE)
1. Publishes
  * the [current Status/Info](https://liveobjects.orange-business.com/doc/html/lo_manual.html#MQTT_DEV_INFO).
  * the [current Configuration Parameters](https://liveobjects.orange-business.com/doc/html/lo_manual.html#MQTT_DEV_CFG)
  * the [current Resources](https://liveobjects.orange-business.com/doc/html/lo_manual.html#MQTT_DEV_RSC)
1. Subscribes to LiveObjects topics to receive notifications:
  * Configuration Parameters update request
  * Resource update request
  * Command request
1. then it waits for an event from LiveObjects platform to :
  * Update "Configuration Parameters"
  * Update one "Resource" : message or image
  * Process a "Command" : RESET or LED     

See [Datavenue Live Objects - complete guide](https://liveobjects.orange-business.com/doc/html/lo_manual.html)
