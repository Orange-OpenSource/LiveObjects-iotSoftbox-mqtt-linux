/*
 * Copyright (C) 2016 Orange
 *
 * This software is distributed under the terms and conditions of the
 * 'BSD-3-Clause'
 * license which can be found in the file 'LICENSE.txt' in this package
 * distribution
 * or at 'https://opensource.org/licenses/BSD-3-Clause'.
 */

/**
 * @file  minimal.c
 * @brief A minimal user application using basic LiveObjects
 * iotsotbox-mqtt features
 */

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/* Raspberry Pi GPIO */
#include <wiringPi.h>

/* LiveObjects api */
#include "liveobjects_iotsoftbox_api.h"

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

/* Debug */
#define DBG_DFT_MAIN_LOG_LEVEL 3
#define DBG_DFT_LOMC_LOG_LEVEL 3

// set 0x0F to have all message dump (text+hexa)
#define DBG_DFT_MSG_DUMP         0xf

/* Various tag */
#define APPV_VERSION "LINUX MINIMAL SAMPLE V01.1"
#define LOM_BUILD_TAG "BUILD LiveObjects IoT Minimal 1.1"

/* WiringPi */
#define MAXTIMINGS	85
#define DHTPIN		7
#define DATA_SIZE	5
#define DATA_WAIT_TIMEOUT 		255
#define START_SIGNAL_TIMEOUT 	18
#define RESPONSE_SIGNAL_TIMEOUT 40
int dht11_data[DATA_SIZE] = {0};

uint8_t appv_log_level = DBG_DFT_MAIN_LOG_LEVEL;

// ==========================================================
//
// Live Objects IoT Client object (using iotsoftbox-mqtt library)
//
// - Send status information at connection
// - Send collected data each cycle
//

// ----------------------------------------------------------
// STATUS data
//

// Count the number of time the status is sent
int32_t appv_status_counter = 0;

// Message to display as a status in LO
char appv_status_message[150] = "READY";

/// Set of status
LiveObjectsD_Data_t appv_set_status[] = {
		{ LOD_TYPE_STRING_C, "sample_version", APPV_VERSION, 1 },
		{ LOD_TYPE_INT32, "sample_counter", &appv_status_counter, 1 },
		{ LOD_TYPE_STRING_C, "sample_message", appv_status_message, 1 }
};
#define SET_STATUS_NB (sizeof(appv_set_status) / sizeof(LiveObjectsD_Data_t))

int appv_hdl_status = -1;

// ----------------------------------------------------------
// COLLECTED data
//

#define STREAM_PREFIX 0

uint8_t appv_measures_enabled = 1;
// Data
uint32_t appv_measures_counter = 0;
float appv_measures_temp = -50.0;
uint32_t appv_measures_hum = -1;

/// Set of Collected data (published on a data stream)
LiveObjectsD_Data_t appv_set_measures[] = {
		{ LOD_TYPE_UINT32, "counter", &appv_measures_counter, 1 },
		{ LOD_TYPE_FLOAT, "temperature", &appv_measures_temp, 1 },
		{ LOD_TYPE_UINT32, "humidity", &appv_measures_hum, 1 }
};
#define SET_MEASURES_NB (sizeof(appv_set_measures) / sizeof(LiveObjectsD_Data_t))

int appv_hdl_data = -1;

// ----------------------------------------------------------

int read_dht11_data() {
	int ret = 0;
	uint8_t laststate = HIGH;
	uint8_t counter = 0;
	uint8_t j = 0, i;

	memset(dht11_data, 0, sizeof(dht11_data));

	/* pull pin down and wait the start signal */
	pinMode(DHTPIN, OUTPUT);
	digitalWrite(DHTPIN, LOW);
	delay(START_SIGNAL_TIMEOUT);
	/* then pull it up and wait the response */
	digitalWrite(DHTPIN, HIGH);
	delayMicroseconds(RESPONSE_SIGNAL_TIMEOUT);
	/* prepare to read the pin */
	pinMode(DHTPIN, INPUT);

	/* detect change and read data */
	for (i = 0; i < MAXTIMINGS; i++) {
		counter = 0;
		while (digitalRead(DHTPIN) == laststate) {
			counter++;
			delayMicroseconds(1);
			if (counter == DATA_WAIT_TIMEOUT) {
				break;
			}
		}
		laststate = digitalRead(DHTPIN);

		if (counter == DATA_WAIT_TIMEOUT) {
			break;
		}

		/* ignore first 3 transitions */
		if ((i >= 4) && (i % 2 == 0)) {
			/* shove each bit into the storage bytes */
			dht11_data[j / 8] <<= 1;
			if (counter > 16) {
				dht11_data[j / 8] |= 1;
			}
			j++;
		}
	}

	/*
	 * check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
	 * copy it out if data is good
	 */
	 if ((j >= 40) && (dht11_data[4] == ((dht11_data[0] + dht11_data[1] + dht11_data[2] + dht11_data[3]) & 0xFF))) {
		char temp_as_str[DATA_SIZE];
		sprintf(temp_as_str, "%d.%d", dht11_data[2], dht11_data[3]);
		sscanf(temp_as_str, "%f", &appv_measures_temp);
		appv_measures_hum = dht11_data[0];
		printf("Data are Ok, updating values \n");
		ret = 1;
	} else {
		printf("Data aren't good, keeping the previous values \n");
		ret = 0;
	}
	return ret;
}

// ----------------------------------------------------------

uint32_t loop_cnt = 0;

void appli_sched(void) {
	++loop_cnt;
	appv_measures_counter = loop_cnt;
	if (appv_log_level > 1) {
		printf("thread_appli: %u\r\n", loop_cnt);
	}

	/* Read data from the DHT11 */
	/* if data are ok, copy them ,then push */
	if (read_dht11_data()) {
		if (appv_log_level > 2) {
			printf("thread_appli: %u - %s PUBLISH - humidity=%d %% temp=%f *C\r\n", loop_cnt,
					appv_measures_enabled ? "DATA" : "NO", appv_measures_hum, appv_measures_temp);
		}
		if (appv_measures_enabled) {
			printf("LiveObjectsClient_PushData...\n");
			LiveObjectsClient_PushData(appv_hdl_data);
		}
	}
}

// ----------------------------------------------------------

bool mqtt_start(void *ctx) {

	LiveObjectsClient_SetDbgLevel(appv_log_level);
	LiveObjectsClient_SetDevId(LOC_CLIENT_DEV_ID);
	LiveObjectsClient_SetNameSpace(LOC_CLIENT_DEV_NAME_SPACE);

	unsigned long long apikey_p1 = C_LOC_CLIENT_DEV_API_KEY_P1;
	unsigned long long apikey_p2 = C_LOC_CLIENT_DEV_API_KEY_P2;

	printf("mqtt_start: LiveObjectsClient_Init ...\n");
	if(LiveObjectsClient_Init(NULL, apikey_p1, apikey_p2)) {
		printf("mqtt_start: ERROR returned by LiveObjectsClient_Init\n");
		return false;
	}

	// Attach my local STATUS data to the LiveObjects Client instance
	// --------------------------------------------------------------
	appv_hdl_status = LiveObjectsClient_AttachStatus(appv_set_status, SET_STATUS_NB);
	if (appv_hdl_status) {
		printf(" !!! ERROR (%d) to attach status !\r\n", appv_hdl_status);
	} else {
		printf("mqtt_start: LiveObjectsClient_AttachStatus -> OK\n");
	}

	// Attach one set of collected data to the LiveObjects Client instance
	// --------------------------------------------------------------------
	appv_hdl_data = LiveObjectsClient_AttachData(STREAM_PREFIX, "LO_sample_measures", "mV1", "\"Test\"",
			NULL, appv_set_measures, SET_MEASURES_NB);
	if (appv_hdl_data < 0) {
		printf(" !!! ERROR (%d) to attach a collected data stream !\r\n", appv_hdl_data);
	} else {
		printf("mqtt_start: LiveObjectsClient_AttachData -> OK\n");
	}

	// Connect to the LiveObjects Platform and update the status
	// ---------------------------------------------------------
	printf("mqtt_start: LiveObjectsClient_Connect ...\n");

	if (LiveObjectsClient_Connect()) {
		printf("mqtt_start: ERROR returned by LiveObjectsClient_Connect\n");
		return false;
	}

	if (LiveObjectsClient_PushStatus(appv_hdl_status)) {
		printf("mqtt_start: ERROR returned by LiveObjectsClient_PushStatus\n");
	}

	printf("mqtt_start: OK\n");
	return true;
}

// ----------------------------------------------------------
/// Entry point to the program

int main() {
	int ret = 0;

	printf("Starting LiveObject Client Example\n");
	LiveObjectsClient_InitDbgTrace(appv_log_level);
	LiveObjectsClient_SetDbgMsgDump(DBG_DFT_MSG_DUMP);

	if (wiringPiSetup() == -1) {
		printf("Failed to setup wiringPi, exiting now !");
		ret = -1;
	}

	if (ret == 0 && mqtt_start(NULL)) {
		while (1) {
			appli_sched();
			/* Launch the cycle every 5 sec */
			LiveObjectsClient_Cycle(5000);
		}
	}

	return ret;
}
