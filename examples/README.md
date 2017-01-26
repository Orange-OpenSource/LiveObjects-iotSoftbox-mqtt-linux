# LiveObjects Iot Device Examples

This directory contains Live Objects IoT Device Examples using iotsoftbox-mqtt library.

See [Datavenue Live Objects - complete guide](https://liveobjects.orange-business.com/doc/html/lo_manual.html)

For now, there are three examples:

* **[Basic](liveobjects_sample_basic/README.md)**:  example to demonstrate all available LiveObjects IoT Device features, using any Linux platform.

* **[Minimal](liveobjects_sample_minimal/README.md)**: example using the minimal amount of code to get data from a sensor and push them to Live Objects (Wiring the sensor with a Raspberry Pi board).

* **[Update](liveobjects_sample_update/README.md)**: example using the minimal amount of code to update the firmware with a deb package.

## Adding an example

The best way to add an example is to copy another one and change the following things :

* Change the folder name to be unique
* If necessary, change the API_KEY
* In the `CMakeLists.txt` of the example, change the name of the executable to be unique
* Check that the **c** file containing your code is well specified in the `CMakeLists.txt`

By default the executable and the c file are wearing the same name.

* Add the example in the top level `CMakeLists.txt`

## Config

Each examples must have a config folder with the following files :

```
config
├── liveobjects_dev_config.h
├── liveobjects_dev_params.h
└── liveobjects_dev_security.h
```

Thanks to this, each examples have an independent configuration.

## Warning

- **Two examples folders can't have the same name**
- **Two examples executables can't have the same name**
- **Don't forget the API key**
- **If you want to update a firmware with `dpkg` you will need to run the program with super user rights.**
