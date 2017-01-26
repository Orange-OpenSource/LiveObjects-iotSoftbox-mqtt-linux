/*
 * Copyright (C) 2016 Orange
 *
 * This software is distributed under the terms and conditions of the 'BSD-3-Clause'
 * license which can be found in the file 'LICENSE.txt' in this package distribution
 * or at 'https://opensource.org/licenses/BSD-3-Clause'.
 */

/**
 * @file   mqtt_network_interface.h
 * @brief  Define structure used/required by the MQTTClient soft package
 */

#ifndef __MQTT_NETWORK_INTERFACE_H_
#define __MQTT_NETWORK_INTERFACE_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include "liveobjects-sys/MQTTLinux.h"
#include "liveobjects-sys/socket_defs.h"

typedef struct Network Network;

#if defined(__cplusplus)
}
#endif

#endif /* __MQTT_NETWORK_INTERFACE_H_ */
