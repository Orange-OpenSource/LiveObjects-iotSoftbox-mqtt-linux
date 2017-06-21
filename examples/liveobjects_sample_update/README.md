# iotsoftbox liveobject sample update

Here is the table of content:

- [Aim of the example](#aim-of-the-example)
- [Enable the example](#enable-the-example)
- [How does it work ?](#how-does-it-work)
- [Warnings](#warnings)

## Aim of the example

This example represents the minimal amount of code necessary to update the firmware from a platform.
It use `dpkg` to install a deb package downloaded as a resource (firmware update).
To use dpkg **you MUST run this example with sudo**

The compilation process is no different from the other one because everything is automated by cmake.

## Enable the example

Go inside the top level cmake and uncomment the following line :

```cmake
#add_subdirectory(liveobjects_sample_update)
```

## How does it work?

- Allocate a memory space big enough to hold the firmware
```c
char appv_rsc_firmware[300 * 1024] = "";
```
This buffer doesn't need to fit the complete size of the resource to download. The operation can be repeated as many times as needed.

- After the download of the new firmware, create a new file and copy the buffer inside it.
```c
FILE *fpDeb = NULL;
fpDeb = fopen("newFirmware.deb", "a");
if (fpDeb != NULL) {
	fwrite(appv_rsc_firmware, sizeof(char), size, fpDeb);
	fclose(fpDeb);
}
```

- Install the package, check that went right and delete the package.
```c
system("dpkg -i newFirmware.deb; echo $? > returnCode; rm -rf newFirmware.deb");
```

- Read the return code, if it is note `0`, something went wrong. You should restore the firmware version to the old one to notify LO that the update failed.
```c
strncpy(appv_rv_firmware, version_old, sizeof(appv_rv_firmware));
```
It will be sent automatically to LO.

## Warnings

**This example only downloads a resource and installs it. It does not relaunch the program.**

**Without `sudo` the program will not be able to install the new firmware**
