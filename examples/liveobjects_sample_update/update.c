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
 * @file  update.c
 * @brief A simple user application using the resource feature from LiveObjects
 * iotsotbox-mqtt to update a firmware
 */

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

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

#define DBG_DFT_MAIN_LOG_LEVEL 3
#define DBG_DFT_LOMC_LOG_LEVEL 3

// set 0x0F to have all message dump (text+hexa)
#define DBG_DFT_MSG_DUMP         0xf

#define APPV_VERSION "LINUX BASIC SAMPLE V01.1"
#define LOM_BUILD_TAG "BUILD LiveObjects IoT Update 1.1"

uint8_t appv_log_level = DBG_DFT_MAIN_LOG_LEVEL;

// ==========================================================
//
// Live Objects IoT Client object (using iotsoftbox-mqtt library)
//
// - status information at connection
// - resources declaration (firmware, text file, etc.)

// ----------------------------------------------------------
// STATUS data
//

int32_t appv_status_counter = 0;
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
// RESOURCE data
//

char appv_rv_firmware[10] = "01.00";
// 300 KBytes chunks
char appv_rsc_firmware[300 * 1024] = "";
#define RSC_IDX_FIRMWARE 1

/// Set of resources
LiveObjectsD_Resource_t appv_set_resources[] = {
		{ RSC_IDX_FIRMWARE, "firmware",	appv_rv_firmware, sizeof(appv_rv_firmware) - 1 } // resource used to update appv_status_message
};
#define SET_RESOURCES_NB (sizeof(appv_set_resources) / sizeof(LiveObjectsD_Resource_t))

// variables used to process the current resource transfer
uint32_t appv_rsc_size = 0;
uint32_t appv_rsc_offset = 0;

// ==========================================================
// IotSoftbox-mqtt callback functions (in 'C' api)

LiveObjectsD_ResourceRespCode_t main_cb_rsc_ntfy(uint8_t state,
		const LiveObjectsD_Resource_t *rsc_ptr, const char *version_old,
		const char *version_new, uint32_t size) {
	LiveObjectsD_ResourceRespCode_t ret = RSC_RSP_OK; // OK to update the resource

	printf("*** rsc_ntfy: ...\r\n");

	if ((rsc_ptr) && (rsc_ptr->rsc_uref > 0)
			&& (rsc_ptr->rsc_uref <= SET_RESOURCES_NB)) {
		printf("***   user ref     = %d\r\n", rsc_ptr->rsc_uref);
		printf("***   name         = %s\r\n", rsc_ptr->rsc_name);
		printf("***   version_old  = %s\r\n", version_old);
		printf("***   version_new  = %s\r\n", version_new);
		printf("***   size         = %u\r\n", size);
		if (state) {
			if (state == 1) {  // Completed without error
				printf("***   state        = COMPLETED without error\r\n");
				// Update version
				printf(" ===> UPDATE - version %s to %s\r\n",
						rsc_ptr->rsc_version_ptr, version_new);
				strncpy((char *) rsc_ptr->rsc_version_ptr, version_new,
						rsc_ptr->rsc_version_sz);

				if (rsc_ptr->rsc_uref == RSC_IDX_FIRMWARE) {
					FILE *fpDeb = NULL;
					fpDeb = fopen("newFirmware.deb", "a");
					if (fpDeb != NULL) {
						fwrite(appv_rsc_firmware, sizeof(char), size, fpDeb);
						fclose(fpDeb);
						printf("Deb creation done\n");
						system("dpkg -i newFirmware.deb; echo $? > returnCode; rm -rf newFirmware.deb");

						FILE *fpRet = NULL;
						int dpkgRet;
						fpRet = fopen("returnCode", "r");
						fscanf(fpRet, "%d", &dpkgRet);
						fclose(fpRet);

						if (dpkgRet != 0) { // Something went wrong when installing
							printf("ERROR While installing the new firmware\n");
							system("rm -rf returnCode");
							strncpy(appv_rv_firmware, version_old, sizeof(appv_rv_firmware));
							ret = RSC_RSP_ERR_INTERNAL_ERROR;
						} else {
							printf("New firmware installed\n");
							// exit(0);
						}
					} else {
						printf("ERROR While creating the deb\n");
						strncpy(appv_rv_firmware, version_old, sizeof(appv_rv_firmware));
						ret = RSC_RSP_ERR_INTERNAL_ERROR;
					}
				}
			} else {
				printf("***   state        = COMPLETED with error !!!!\r\n");
				// Roll back ?
			}
			appv_rsc_offset = 0;
			appv_rsc_size = 0;

			// Push Status (message has been updated or not)
			LiveObjectsClient_PushStatus(appv_hdl_status);
		} else {
			appv_rsc_offset = 0;
			ret = RSC_RSP_ERR_NOT_AUTHORIZED;
			switch (rsc_ptr->rsc_uref) {
			case RSC_IDX_FIRMWARE:
				if (size < (sizeof(appv_rsc_firmware) - 1)) {
					ret = RSC_RSP_OK;
				}
				break;
			}
			if (ret == RSC_RSP_OK) {
				appv_rsc_size = size;
				printf("***   state        = START - ACCEPTED\r\n");
			} else {
				appv_rsc_size = 0;
				printf("***   state        = START - REFUSED\r\n");
			}
		}
	} else {
		printf("***  UNKNOWN USER REF (x%p %d)  in state=%d\r\n", rsc_ptr,
				rsc_ptr->rsc_uref, state);
		ret = RSC_RSP_ERR_INVALID_RESOURCE;
	}
	return ret;
}

/**
 * Called (by the LiveObjects thread) to request the user
 * to read data from current resource transfer.
 */
int main_cb_rsc_data(const LiveObjectsD_Resource_t *rsc_ptr, uint32_t offset) {
	int ret;

	if (appv_log_level > 1)
		printf("*** rsc_data: rsc[%d]='%s' offset=%u - data ready ...\r\n",
				rsc_ptr->rsc_uref, rsc_ptr->rsc_name, offset);

	if (rsc_ptr->rsc_uref == RSC_IDX_FIRMWARE) {
		if (offset > (sizeof(appv_rsc_firmware) - 1)) {
			printf(
					"*** rsc_data: rsc[%d]='%s' offset=%u > %u - OUT OF ARRAY\r\n",
					rsc_ptr->rsc_uref, rsc_ptr->rsc_name, offset,
					sizeof(appv_rsc_firmware) - 1);
			return -1;
		}
		int data_len = sizeof(appv_rsc_firmware) - offset - 1;
		ret = LiveObjectsClient_RscGetChunck(rsc_ptr,
				&appv_rsc_firmware[offset], data_len);
		if (ret > 0) {
			if ((offset + ret) > (sizeof(appv_rsc_firmware) - 1)) {
				printf("*** rsc_data: rsc[%d]='%s' offset=%u - read=%d => %u > "
						"%u - OUT OF ARRAY\r\n", rsc_ptr->rsc_uref,
						rsc_ptr->rsc_name, offset, ret, offset + ret,
						sizeof(appv_rsc_firmware) - 1);
				return -1;
			}
			appv_rsc_offset += ret;
			if (appv_log_level > 0)
				printf(
						"*** rsc_data: rsc[%d]='%s' offset=%u - read=%d/%d - %u/%u\r\n",
						rsc_ptr->rsc_uref, rsc_ptr->rsc_name, offset, ret,
						data_len, appv_rsc_offset, appv_rsc_size);
		} else {
			printf(
					"*** rsc_data: rsc[%d]='%s' offset=%u - read error (%d) - %u/%u\r\n",
					rsc_ptr->rsc_uref, rsc_ptr->rsc_name, offset, ret,
					appv_rsc_offset, appv_rsc_size);
		}
	} else {
		ret = -1;
	}

	return ret;
}

// ----------------------------------------------------------

bool mqtt_start(void *ctx) {
	int ret;

	LiveObjectsClient_SetDbgLevel(appv_log_level);
	LiveObjectsClient_SetDevId(LOC_CLIENT_DEV_ID);
	LiveObjectsClient_SetNameSpace(LOC_CLIENT_DEV_NAME_SPACE);

	unsigned long long apikey_p1 = C_LOC_CLIENT_DEV_API_KEY_P1;
	unsigned long long apikey_p2 = C_LOC_CLIENT_DEV_API_KEY_P2;

	printf("mqtt_start: LiveObjectsClient_Init ...\n");
	ret = LiveObjectsClient_Init(NULL, apikey_p1, apikey_p2);
	if (ret) {
		printf("mqtt_start: ERROR returned by LiveObjectsClient_Init\n");
		return false;
	}

	// Attach my local RESOURCES to the LiveObjects Client instance
	// ------------------------------------------------------------
	ret = LiveObjectsClient_AttachResources(appv_set_resources,
			SET_RESOURCES_NB, main_cb_rsc_ntfy, main_cb_rsc_data);
	if (ret) {
		printf(" !!! ERROR (%d) to attach RESOURCES !\r\n", ret);
	} else {
		printf("mqtt_start: LiveObjectsClient_AttachResources -> OK\n");
	}

	// Attach my local STATUS data to the LiveObjects Client instance
	// --------------------------------------------------------------
	appv_hdl_status = LiveObjectsClient_AttachStatus(appv_set_status,
			SET_STATUS_NB);
	if (appv_hdl_status)
		printf(" !!! ERROR (%d) to attach status !\r\n", appv_hdl_status);
	else
		printf("mqtt_start: LiveObjectsClient_AttachStatus -> OK\n");

	// Enable the receipt of resource update requests
	ret = LiveObjectsClient_ControlResources(true);
	if (ret < 0) {
		printf(
				" !!! ERROR (%d) to enable the receipt of resource update request "
						"!\r\n", ret);
	}

	// Connect to the LiveObjects Platform
	// -----------------------------------
	printf("mqtt_start: LiveObjectsClient_Connect ...\n");
	ret = LiveObjectsClient_Connect();
	if (ret) {
		printf("mqtt_start: ERROR returned by LiveObjectsClient_Connect\n");
		return false;
	}

	ret = LiveObjectsClient_PushStatus(appv_hdl_status);
	if (ret) {
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
	LiveObjectsClient_InitDbgTrace(DBG_DFT_MAIN_LOG_LEVEL);
	LiveObjectsClient_SetDbgMsgDump(DBG_DFT_MSG_DUMP);

	if (mqtt_start(NULL)) {
		while (1) {
			LiveObjectsClient_Cycle(5000);
		}
	}

	return ret;
}
