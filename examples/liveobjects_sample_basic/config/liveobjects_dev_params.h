/*
 * Copyright (C) 2016 Orange
 *
 * This software is distributed under the terms and conditions of the 'BSD-3-Clause'
 * license which can be found in the file 'LICENSE.txt' in this package distribution
 * or at 'https://opensource.org/licenses/BSD-3-Clause'.
 *
 * This file is a part of LiveObjects iotsoftbox-mqtt library.
 */

/**
 * @file  liveobjects_dev_params.h
 * @brief Default user parameters used to configure this device as a Live Objects Device
 */

#ifndef __liveobjects_dev_params_H_
#define __liveobjects_dev_params_H_

/* Set to 1 to enable TLS feature*/
/* (warning: check that LOC_SERV_PORT is the correct port in this case)*/
#define SECURITY_ENABLED                     1

/* Only used to overwrite the LiveOjects Server settings :*/
/* IP address, TCP port, Connection timeout in milliseconds.*/
//#define LOC_SERV_IP_ADDRESS                  "84.39.42.214"
//#define LOC_SERV_PORT                        8883
//#define LOC_SERV_TIMEOUT                     2000

/* When set to 1, use the MAC address as LiveObject device identifier*/
/* otherwise use LOC_CLIENT_DEV_ID*/
#define LOC_CLIENT_USE_MAC_ADDR              0

/* Default LiveObjects device settings : name space and device identifier*/
#define LOC_CLIENT_DEV_NAME_SPACE            "LiveObjectsDomain"
#if !LOC_CLIENT_USE_MAC_ADDR
#define LOC_CLIENT_DEV_ID                    "LO_softboxlinux_01"
#endif

/* Here, set your LiveObject Apikey. It is mandatory to run the application*/
#define LOC_CLIENT_DEV_API_KEY               "XXXX"

/* 0 -> standard output, 1 -> syslog output /var/log/syslog */
#define SYSLOG 0

#if SECURITY_ENABLED
/* If security is enabled to establish connection to the LiveObjects platform,*/
/* include certificates file*/
#include "liveobjects_dev_security.h"
#endif

#endif /*__liveobjects_dev_params_H_*/
