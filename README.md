# LiveObject IoT Client - IoTSoftBox-MQTT


Please refers to the [Changelog](ChangeLog.md) to check the latest change and improvement.

---

This repository contains LiveObject IoT Client Library (used to connect devices to LiveObject Server from our partners).

Please, have a look to the [user manual](docs/liveobjects_starterkit_linux_v1.2.pdf) to have a presentation of the library and to be more familiar with it.

For more information about datavenue, you can visit [Datavenue Live Objects - complete guide](https://liveobjects.orange-business.com/doc/html/lo_manual.html).

especially the [Device mode](https://liveobjects.orange-business.com/doc/html/lo_manual.html#MQTT_MODE_DEVICE) section.

Here is a table of content for this file:

- [Requirement](#requirement)
	- [Hardware](#hardware)
	- [Software](#software)
	- [LiveObjects API Key](#liveobjects-api-key)
	- [Setup the LiveObjects header file](#setup-the-liveobjects-header-file)
		- [API key](#api-key)
		- [Security](#security)
- [Configure workstation for cross compilation](#configure-workstation-for-cross-compilation)
	- [Linux Ubuntu](#linux-ubuntu)
	- [Linux Debian](#linux-debian)
	- [Windows](#windows)
- [Build](#build)
	- [Submodule update](#submodule-update)
	- [Linux](#linux)
		- [Local build](#local-build)
		- [Cross-Compilation build](#cross-compilation-build)
	- [Windows](#windows)
	- [Debug](#debug)
	- [Syslog](#syslog)
- [Global structure](#global-structure)
- [Detailed structure](#detailed-structure)
	- [Config-template](#config-template)
	- [Example](#example)
	- [Lib](#lib)
		- [jsmn](#jsmn)
		- [mbedTLS](#mbedtls)
		- [paho mqtt](#paho-mqtt)
	- [Mqtt_live_objects](#mqtt_live_objects)
		- [LiveObjects-iotSoftbox-mqtt-core](#liveobjects-iotsoftbox-mqtt-core)
		- [Platforms](#platforms)
	- [Script](#script)
- [Doxygen documentation](#doxygen-documentation)
- [Disable mbedTLS tests and examples](#disable-mbedtls-tests-and-examples)
- [Application Control](#application-control)
	- [Live Objects Portal](#live-objects-portal)
	- [Live Objects Swagger](#live-objects-swagger)

## Requirement

### Hardware

* A Linux platform like a Raspberry Pi board.
* A connection to the Internet.

### Software

* Cmake: to generate the Makefiles
* Cross-compiling toolchain: to build the library to any architechture
* A SSH client (If you're using [Windows](http://www.putty.org/))
* [LiveObjects account](https://liveobjects.orange-business.com)

You can also use SCP to transfer files to your devices ([Windows](http://www.chiark.greenend.org.uk/~sgtatham/putty/download.html)).

### LiveObjects API Key

Visit [IoT Soft Box powered by Datavenue](https://liveobjects.orange-business.com/v2/#/sdk)

1. You need to request the creation of a developer account.
1. Then, with your LiveObjects user identifier, login to the [Live Objects portal](https://liveobjects.orange-business.com/#/login).
1. Go in 'Configuration - API key' tab, and add a new API key.
**Don't forget to copy this API key value** in a local and secure place during this operation.

### Setup the LiveObjects header file

**Warning : each example has independent configuration**

#### API key
In the config directory of every example, you will find 3 files to customize the behavior of the library.
Edit those files to change some values. in particular the **LiveObjects API key** in the main file `nameOfTheExample.c`.

For security purpose, you will need to split the ApiKey in two parts.
The first part is the first sixteen char of the ApiKey and the second one is the last sixteen char of the ApiKey.
 An example is given below:

```c
/* Default LiveObjects device settings : name space and device identifier*/
#define LOC_CLIENT_DEV_NAME_SPACE            "LiveObjectsDomain"
#define LOC_CLIENT_DEV_ID                    "LO_softboxlinux_01"

/** Here, set your LiveObject Apikey. It is mandatory to run the application
 *
 * C_LOC_CLIENT_DEV_API_KEY_P1 must be the first sixteen char of the ApiKey
 * C_LOC_CLIENT_DEV_API_KEY_P1 must be the last sixteen char of the ApiKey
 *
 * If your APIKEY is 0123456789abcdeffedcba9876543210 then
 * it should look like this :
 *
 * #define C_LOC_CLIENT_DEV_API_KEY_P1			0x0123456789abcdef
 * #define C_LOC_CLIENT_DEV_API_KEY_P2			0xfedcba9876543210
 *
 * */

#define C_LOC_CLIENT_DEV_API_KEY_P1			0x0123456789abcdef
#define C_LOC_CLIENT_DEV_API_KEY_P2			0xfedcba9876543210
```

#### Security
From the file `liveobjects_dev_params.h` you can also disable TLS By switching `#define SECURITY_ENABLED 1` to 0.
If the security is disabled your device will communicate in plain text with the platform.

By disabling the security, MbedTLS code's will still be embedded because it is used by the resource appliance.

You can avoid compiling mbedTLS by uncommenting `//#define LOC_FEATURE_MBEDTLS 0` in  `liveobjects_dev_config.h` but resource related feature won't be available.

## Configure workstation for cross compilation

Example given for a Raspberry PI 3 B (`armeabihf` compiler).
For a Raspberry PI 1 B replace `armeabihf` by `armeabi` so for the compiler use `arm-linux-gnueabi-gcc` instead of `arm-linux-gnueabihf-gcc`

### Linux Ubuntu

Tested for Ubuntu 16.04

**Install the cross compiler**

```bash
sudo apt-get update && sudo apt-get upgrade
# Raspberry Pi 1
# sudo apt-get install git git-flow cmake gcc-arm-linux-gnueabi
sudo apt-get install git git-flow cmake gcc-arm-linux-gnueabihf
```

### Linux Debian

Tested for Debian 8.6

**All commands have to be run as root (su command)**

**For the Raspberry Pi 1** replace `armhf` by `armel`.
The cross-compiler remains `arm-linux-gnueabi-gcc` for the Pi 1.

```bash
echo "deb http://emdebian.org/tools/debian/ jessie main" > /etc/apt/sources.list.d/crosstools.list
curl http://emdebian.org/tools/debian/emdebian-toolchain-archive.key | sudo apt-key add -
# Raspberry Pi 1
# sudo dpkg --add-architecture armel
sudo dpkg --add-architecture armhf
sudo apt-get update
# Raspberry Pi 1
# sudo apt-get install git git-flow cmake crossbuild-essential-armel
sudo apt-get install git git-flow cmake crossbuild-essential-armhf
```

### Windows

1. Install the [compiler](http://gnutoolchains.com/raspberry/).
2. Update the cross compiler environment, by downloading the Sysroot. To do so use ```C:\SysGCC\Raspberry\TOOLS\UpdateSysroot.bat``` if ```C:\SysGCC``` is the installation path.
3. Install [CMake](https://cmake.org/download/).
4. Install [Perl](http://strawberryperl.com/).

A detailed installation can be found [there](wiki/detailed-windows-setup.md).

## Build

### Submodule update

You will need to download the third-party libraries.

Two options for that :

* Using --recursive when cloning the main library.

	```bash
	git clone --recursive https://github.com/Orange-OpenSource/LiveObjects-iotSoftbox-mqtt-linux.git
	```

* Using git submodule in the repository

	```bash
	git clone https://github.com/Orange-OpenSource/LiveObjects-iotSoftbox-mqtt-linux.git
	cd LiveObjects-iotSoftbox-mqtt-linux
	git submodule init
	git submodule update
	```

### Linux

#### Local build

```bash
cd LiveObjects-iotSoftbox-mqtt-linux
mkdir build
cd build
cmake ..
make
```

#### Cross-Compilation build

```bash
cd LiveObjects-iotSoftbox-mqtt-linux
mkdir build
cd build
cmake -DCMAKE_C_COMPILER=<Path to the compiler> ..
make
```

### Windows

You can only cross-compile the program on Windows. This is how to do it from cmd.exe:

```bash
cd LiveObjects-iotSoftbox-mqtt-linux
mkdir build
cd build
copy ../script/cmakeWinSetup.bat .
cmakeWinSetup.bat
make
```

If you are using git bash for instance it will be slightly different:
- Use cp instead of copy
- run cmakeWinSetup.bat like this : ```./cmakeWinSetup.bat```

### Debug

To add the debug flag to the compiler, you must run cmake with ```-DCMAKE_BUILD_TYPE=Debug```

You can also change the debug Level (more or less verbose) into each example.
```c
#define DBG_DFT_MAIN_LOG_LEVEL <Debug Level>
```
It goes from 1 (only error) to 6 (everything).

If you want to remove the message dump, change ```DBG_DFT_MSG_DUMP``` to 0x0,
0x1, 0x2, 0x4 and 0x8 are intermediate levels

### Syslog

You can log everything inside the syslog by switching
```c
#define SYSLOG 0
```
 to 1 in "config/liveobjects_dev_params.h". It will print everything into /var/log/syslog instead of the standard output.

## Global structure

```
iotsoftbox-mqtt-linux
├── Config-template
├── examples
├── lib
├── mbedtls_config
├── mqtt_live_objects
├── script
└── wiki

```
- config-template: Setup the library from here
- [examples](examples/README.md): bunch of examples using the library.
- lib: all third-party libraries
- mbedtls_config: extra config for mbedtls
- mqtt_live_objects: specific implementations to platforms and common code with others client (like Arduino or mbed)
- script: a bunch of various script to make things easy
- wiki: Various help / information

## Detailed structure

In alphabetical order.

### Config-template

This repository contains templates of all mandatories configurations files. Those files must be present in each examples.

### Example

Go [there](examples/README.md) to get more informations.

### Lib

There is a list of the third-party libraries used in this library and their utilities:

#### jsmn

[jsmn](https://github.com/zserge/jsmn) (pronounced like 'jasmine') is a minimalistic JSON parser in C. It can be easily integrated into resource-limited or embedded projects.

#### mbedTLS

[mbed TLS](https://github.com/ARMmbed/mbedtls) offers an SSL library with an intuitive API and readable source code, so you can actually understand what the code does. Also the mbed TLS modules are as loosely coupled as possible and written in the portable C language. This allows you to use the parts you need, without having to include the total library.

#### paho mqtt

[paho mqtt](https://github.com/eclipse/paho.mqtt.embedded-c) is part of the Eclipse Paho project, which provides open-source client implementations of MQTT and MQTT-SN messaging protocols aimed at new, existing, and emerging applications for the Internet of Things.

### Mqtt_live_objects

#### LiveObjects-iotSoftbox-mqtt-core

In the `LiveObjects-iotSoftbox-mqtt-core` folder, only platform-free code is present, you should be able to use this code on any device.
`LiveObjects-iotSoftbox-mqtt-core` is a git submodule.

```
mqtt_live_objects
├── LiveObjects-iotSoftbox-mqtt-core
│   ├── iotsoftbox-core
│   ├── liveobjects-client
│   ├── paho-mqttclient-embedded-c
```

* iotsoftbox-core should contains the core code
* liveobjects-client should contains interfaces for users
* paho-mqttclient-embedded-c should contains the MQTTClient.

#### Platforms

```
mqtt_live_objects
├── platforms
│   └── <A platflorm name>
│       ├── iotsoftbox-<A platflorm name>
│       └── liveobjects-sys
```

* iotsoftbox-<A platflorm name\> should contains systems files specific to a platform (.c)
* liveobjects-sys should contains headers files specific to a platform (.h)

### Script

* cmakeWinSetup.bat is used to invoke cmake in Windows with preselected variables like the path to the compiler
* deb_maker.sh allow to automate the creation of a deb archive.
* wiringPiInstaller.sh to automate the installation of WiringPi

## Doxygen documentation

To generate the Doxygen documentation, please, use the following command from the project root folder:

```bash
doxygen mqtt_live_objects/LiveObjects-iotSoftbox-mqtt-core/doxygen/liveobjects_iotsoftbox.doxyfile
```

The doc will be generated into an apidoc folder. Please, open `apidoc/html/index.html` to start browsing the doc.

## Disable mbedTLS tests and examples

During the build, mbedTLS will also compile some tests and examples which will slow down the build considerably.
To disable that, open `lib/mbedtls/CMakeLists.txt` and switch the two flags line 7 and 15 to **OFF**.

## Application Control

### Live Objects Portal

Using your Live Objects user account, go to [Live Objects Portal](https://liveobjects.orange-business.com/#/login).

### Live Objects Swagger

You can use the Swagger to create MQTT/JSON exchanges with your device. For more information, please visit [Live Objects Swagger User Interface](https://liveobjects.orange-business.com/swagger-ui/index.html).
