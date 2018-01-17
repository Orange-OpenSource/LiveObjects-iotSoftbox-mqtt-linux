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
 * @file  basic.c
 * @brief A simple user application using all available LiveObjects
 * iotsotbox-mqtt features
 */

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#define DBG_DFT_MSG_DUMP         0xF

#define APPV_VERSION "LINUX BASIC SAMPLE V01.1"
#define LOM_BUILD_TAG "BUILD LiveObjects IoT Basic 1.1"

uint8_t appv_log_level = DBG_DFT_MAIN_LOG_LEVEL;

// ==========================================================
//
// Live Objects IoT Client object (using iotsoftbox-mqtt library)
//
// - status information at connection
// - collected data to send
// - supported configuration parameters
// - supported commands
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
// 'COLLECTED DATA'
//
#define STREAM_PREFIX 0

uint8_t appv_measures_enabled = 1;

int32_t appv_measures_temp_grad = -1;
float appv_measures_volt_grad = -0.2;

// contains a counter incremented after each data sent
uint32_t appv_measures_counter = 0;

// contains the temperature level
int32_t appv_measures_temp = 20;

// contains the battery level
float appv_measures_volt = 5.0;

/// Set of Collected data (published on a data stream)
LiveObjectsD_Data_t appv_set_measures[] = {
		{ LOD_TYPE_UINT32, "counter", &appv_measures_counter, 1 },
		{ LOD_TYPE_INT32, "temperature", &appv_measures_temp, 1 },
		{ LOD_TYPE_FLOAT, "battery_level", &appv_measures_volt, 1 }
};
#define SET_MEASURES_NB (sizeof(appv_set_measures) / sizeof(LiveObjectsD_Data_t))

int appv_hdl_data = -1;

// ----------------------------------------------------------
// CONFIGURATION data
//
uint32_t appv_cfg_timeout = 10;

// a structure containing various kind of parameters (char[], int and float)
struct conf_s {
	char name[20];
	int32_t threshold;
	float gain;
} appv_conf = { "TICTAC", -3, 1.05 };

// definition of identifier for each kind of parameters
#define PARM_IDX_NAME 1
#define PARM_IDX_TIMEOUT 2
#define PARM_IDX_THRESHOLD 3
#define PARM_IDX_GAIN 4

/// Set of configuration parameters
LiveObjectsD_Param_t appv_set_param[] = {
		{ PARM_IDX_NAME, { LOD_TYPE_STRING_C, "name", appv_conf.name, 1 } },
		{ PARM_IDX_TIMEOUT, { LOD_TYPE_UINT32, "timeout", (void *) &appv_cfg_timeout, 1 } },
		{ PARM_IDX_THRESHOLD, { LOD_TYPE_INT32, "threshold", &appv_conf.threshold, 1 } },
		{ PARM_IDX_GAIN, { LOD_TYPE_FLOAT, "gain", &appv_conf.gain, 1 } }
};
#define SET_PARAM_NB (sizeof(appv_set_param) / sizeof(LiveObjectsD_Param_t))

// ----------------------------------------------------------
// COMMANDS
// All the data are simulated
// Digital output to change the status of the RED LED
int32_t app_led_user = 0;

/// counter used to postpone the LED command response
static int cmd_cnt = 0;

#define CMD_IDX_RESET 1
#define CMD_IDX_LED 2

/// set of commands
LiveObjectsD_Command_t appv_set_commands[] = {
		{ CMD_IDX_RESET, "RESET", 0 },
		{ CMD_IDX_LED, "LED", 0 }
};
#define SET_COMMANDS_NB (sizeof(appv_set_commands) / sizeof(LiveObjectsD_Command_t))

// ----------------------------------------------------------
// RESOURCE data
//
char appv_rsc_image[5 * 1024] = "";

char appv_rv_message[10] = "01.00";
char appv_rv_image[10] = "01.00";

#define RSC_IDX_MESSAGE 1
#define RSC_IDX_IMAGE 2

/// Set of resources
LiveObjectsD_Resource_t appv_set_resources[] = {
		{ RSC_IDX_MESSAGE, "message", appv_rv_message, sizeof(appv_rv_message) - 1 },  // resource used to update appv_status_message
		{ RSC_IDX_IMAGE, "image", appv_rv_image, sizeof(appv_rv_image) - 1 }
};
#define SET_RESOURCES_NB (sizeof(appv_set_resources) / sizeof(LiveObjectsD_Resource_t))

// variables used to process the current resource transfer
uint32_t appv_rsc_size = 0;
uint32_t appv_rsc_offset = 0;

// ==========================================================
// IotSoftbox-mqtt callback functions (in 'C' api)

LiveObjectsD_ResourceRespCode_t main_cb_rsc_ntfy(uint8_t state, const LiveObjectsD_Resource_t *rsc_ptr,
		const char *version_old, const char *version_new, uint32_t size) {
	LiveObjectsD_ResourceRespCode_t ret = RSC_RSP_OK;  // OK to update the resource

	printf("*** rsc_ntfy: ...\r\n");

	if ((rsc_ptr) && (rsc_ptr->rsc_uref > 0) && (rsc_ptr->rsc_uref <= SET_RESOURCES_NB)) {
		printf("***   user ref     = %d\r\n", rsc_ptr->rsc_uref);
		printf("***   name         = %s\r\n", rsc_ptr->rsc_name);
		printf("***   version_old  = %s\r\n", version_old);
		printf("***   version_new  = %s\r\n", version_new);
		printf("***   size         = %"PRIu32"\r\n", size);
		if (state) {
			if (state == 1) {  // Completed without error
				printf("***   state        = COMPLETED without error\r\n");
				// Update version
				printf(" ===> UPDATE - version %s to %s\r\n", rsc_ptr->rsc_version_ptr, version_new);
				strncpy((char *) rsc_ptr->rsc_version_ptr, version_new, rsc_ptr->rsc_version_sz);

				if (rsc_ptr->rsc_uref == RSC_IDX_IMAGE) {
					printf("\r\n\r\n");
					printf("%s", appv_rsc_image);
					printf("\r\n\r\n");
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
			case RSC_IDX_MESSAGE:
				if (size < (sizeof(appv_status_message) - 1)) {
					ret = RSC_RSP_OK;
				}
				break;
			case RSC_IDX_IMAGE:
				if (size < (sizeof(appv_rsc_image) - 1)) {
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
		printf("***  UNKNOWN USER REF (x%p %d)  in state=%d\r\n", rsc_ptr, rsc_ptr->rsc_uref, state);
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
		printf("*** rsc_data: rsc[%d]='%s' offset=%"PRIu32" - data ready ...\r\n", rsc_ptr->rsc_uref, rsc_ptr->rsc_name,
				offset);

	if (rsc_ptr->rsc_uref == RSC_IDX_MESSAGE) {
		char buf[40];
		if (offset > (sizeof(appv_status_message) - 1)) {
			printf("*** rsc_data: rsc[%d]='%s' offset=%"PRIu32" > %zu - OUT OF ARRAY\r\n", rsc_ptr->rsc_uref, rsc_ptr->rsc_name,
					offset, sizeof(appv_status_message) - 1);
			return -1;
		}
		ret = LiveObjectsClient_RscGetChunck(rsc_ptr, buf, sizeof(buf) - 1);
		if (ret > 0) {
			if ((offset + ret) > (sizeof(appv_status_message) - 1)) {
				printf("*** rsc_data: rsc[%"PRIu32"]='%s' offset=%"PRIu32" - read=%d => %"PRIu32" > "
						"%zu - OUT OF ARRAY\r\n", rsc_ptr->rsc_uref, rsc_ptr->rsc_name, offset, ret, offset + ret,
						sizeof(appv_status_message) - 1);
				return -1;
			}
			appv_rsc_offset += ret;
			memcpy(&appv_status_message[offset], buf, ret);
			appv_status_message[offset + ret] = 0;
			printf("*** rsc_data: rsc[%d]='%s' offset=%"PRIu32" - read=%d/%zu '%s'\r\n", rsc_ptr->rsc_uref, rsc_ptr->rsc_name,
					offset, ret, sizeof(buf) - 1, appv_status_message);
		}
	} else if (rsc_ptr->rsc_uref == RSC_IDX_IMAGE) {
		if (offset > (sizeof(appv_rsc_image) - 1)) {
			printf("*** rsc_data: rsc[%d]='%s' offset=%"PRIu32" > %zu - OUT OF ARRAY\r\n", rsc_ptr->rsc_uref, rsc_ptr->rsc_name,
					offset, sizeof(appv_rsc_image) - 1);
			return -1;
		}
		int data_len = sizeof(appv_rsc_image) - offset - 1;
		ret = LiveObjectsClient_RscGetChunck(rsc_ptr, &appv_rsc_image[offset], data_len);
		if (ret > 0) {
			if ((offset + ret) > (sizeof(appv_rsc_image) - 1)) {
				printf("*** rsc_data: rsc[%d]='%s' offset=%"PRIu32" - read=%d => %"PRIu32" > "
						"%zu - OUT OF ARRAY\r\n", rsc_ptr->rsc_uref, rsc_ptr->rsc_name, offset, ret, offset + ret,
						sizeof(appv_rsc_image) - 1);
				return -1;
			}
			appv_rsc_offset += ret;
			if (appv_log_level > 0)
				printf("*** rsc_data: rsc[%d]='%s' offset=%"PRIu32" - read=%d/%d - %"PRIu32"/%"PRIu32"\r\n", rsc_ptr->rsc_uref,
						rsc_ptr->rsc_name, offset, ret, data_len, appv_rsc_offset, appv_rsc_size);
		} else {
			printf("*** rsc_data: rsc[%d]='%s' offset=%"PRIu32" - read error (%d) - %"PRIu32"/%"PRIu32"\r\n", rsc_ptr->rsc_uref,
					rsc_ptr->rsc_name, offset, ret, appv_rsc_offset, appv_rsc_size);
		}
	} else {
		ret = -1;
	}

	return ret;
}

// ----------------------------------------------------------
// COMMAND Callback Functions

static int
main_cmd_doSystemReset(LiveObjectsD_CommandRequestBlock_t *pCmdReqBlk);
static int main_cmd_doLED(LiveObjectsD_CommandRequestBlock_t *pCmdReqBlk);

/// Called (by the LiveObjects thread) to perform an 'attached/registered'
/// command
int main_cb_command(LiveObjectsD_CommandRequestBlock_t *pCmdReqBlk) {
	int ret;
	const LiveObjectsD_Command_t *cmd_ptr;

	if ((pCmdReqBlk == NULL) || (pCmdReqBlk->hd.cmd_ptr == NULL) || (pCmdReqBlk->hd.cmd_cid == 0)) {
		printf("**** COMMAND : ERROR, Invalid parameter\r\n");
		return -1;
	}

	cmd_ptr = pCmdReqBlk->hd.cmd_ptr;
	printf("**** COMMAND %d %s - cid=%d\r\n", cmd_ptr->cmd_uref, cmd_ptr->cmd_name, pCmdReqBlk->hd.cmd_cid);

	int i;
	printf("**** ARGS %d : \r\n", pCmdReqBlk->hd.cmd_args_nb);
	for (i = 0; i < pCmdReqBlk->hd.cmd_args_nb; i++) {
		printf("**** ARG [%d] (%d) :  %s %s\r\n", i, pCmdReqBlk->args_array[i].arg_type,
				pCmdReqBlk->args_array[i].arg_name, pCmdReqBlk->args_array[i].arg_value);
	}

	switch (cmd_ptr->cmd_uref) {
		case CMD_IDX_RESET:  // RESET
			printf("main_callbackCommand: command[%d] %s\r\n", cmd_ptr->cmd_uref, cmd_ptr->cmd_name);
			ret = main_cmd_doSystemReset(pCmdReqBlk);
			break;

		case CMD_IDX_LED:  // LED
			printf("main_callbackCommand: command[%d] %s\r\n", cmd_ptr->cmd_uref, cmd_ptr->cmd_name);
			ret = main_cmd_doLED(pCmdReqBlk);
			break;
		default:
			printf("main_callbackCommand: ERROR, unknown command %d\r\n", cmd_ptr->cmd_uref);
			ret = -4;
	}
	return ret;
}

// ----------------------------------------------------------
/// Board reset
static void main_SystemReset(void) {
	printf("SYSTEM REBOOT\n");
}

// ----------------------------------------------------------
/// do a RESET command
static int main_cmd_doSystemReset(LiveObjectsD_CommandRequestBlock_t *pCmdReqBlk) {
	if (LiveObjectsClient_Stop()) {
		printf("doSystemReset: not running => wait 500 ms and reset ...\r\n");
		delay(200);

		main_SystemReset();
	}
	return 1;  // response = OK
}

// ----------------------------------------------------------
/// do a LED command
static int main_cmd_doLED(LiveObjectsD_CommandRequestBlock_t *pCmdReqBlk) {
	int ret;
	// switch on the Red LED
	app_led_user = 0;

	if (pCmdReqBlk->hd.cmd_args_nb == 0) {
		printf("main_cmd_doLED: No ARG\r\n");
		app_led_user = !app_led_user;
		ret = 1;  // Response OK
		cmd_cnt = 0;
	} else {
		int i;
		for (i = 0; i < pCmdReqBlk->hd.cmd_args_nb; i++) {
			if (strncasecmp("ticks", pCmdReqBlk->args_array[i].arg_name, 5)
					&& pCmdReqBlk->args_array[i].arg_type == 0) {
				int cnt = atoi(pCmdReqBlk->args_array[i].arg_value);
				if ((cnt >= 0) || (cnt <= 3)) {
					cmd_cnt = cnt;
				} else {
					cmd_cnt = 0;
				}
				printf("main_cmd_doLED: cmd_cnt = %di (%d)\r\n", cmd_cnt, cnt);
			}
		}
	}

	if (cmd_cnt == 0) {
		app_led_user = !app_led_user;
		ret = 1;  // Response OK
	} else {
		LiveObjectsD_Command_t *cmd_ptr = (LiveObjectsD_Command_t *) (pCmdReqBlk->hd.cmd_ptr);
		app_led_user = 0;
		printf("main_cmd_doLED: ccid=%d (%d)\r\n", pCmdReqBlk->hd.cmd_cid, cmd_ptr->cmd_cid);
		cmd_ptr->cmd_cid = pCmdReqBlk->hd.cmd_cid;
		ret = 0;  // pending
	}
	return ret;  // response = OK
}

// ----------------------------------------------------------
// CONFIGURATION PARAMETERS Callback function
// Check value in range, and copy string parameters


/// Called (by the LiveObjects thread) to update configuration parameters.
int main_cb_param_udp(const LiveObjectsD_Param_t *param_ptr, const void *value, int len) {

if (param_ptr == NULL) {
		printf("UPDATE  ERROR - invalid parameter x%p\r\n", param_ptr);
		return -1;
	}
	printf("UPDATE user_ref=%d %s ....\r\n", param_ptr->parm_uref, param_ptr->parm_data.data_name);
	switch (param_ptr->parm_uref) {
		case PARM_IDX_NAME:
			{
				printf("update name = %.*s\r\n", len, (const char *) value);
				if ((len > 0) && (len < (sizeof(appv_conf.name) - 1))) {
					// Only c-string parameter must be updated by the user application (to
					// check the string length)
					strncpy(appv_conf.name, (const char *) value, len);
					appv_conf.name[len] = 0;
					return 0;  // OK.
				}
			}
			break;
		case PARM_IDX_TIMEOUT:
			{
				uint32_t timeout = *((const uint32_t *) value);
				printf("update timeout = %" PRIu32 "\r\n", timeout);
				if ((timeout > 0) && (timeout <= 120) && (timeout != appv_cfg_timeout)) {
					return 0;  // primitive parameter is updated by library
				}
			}
			break;
		case PARM_IDX_THRESHOLD:
			{
				int32_t threshold = *((const int32_t *) value);
				printf("update threshold = %" PRIi32 "\r\n", threshold);
				if ((threshold >= -10) && (threshold <= 10) && (threshold != appv_conf.threshold)) {
					return 0;  // primitive parameter is updated by library
				}
			}
			break;
		case PARM_IDX_GAIN:
			{
				float gain = *((const float *) value);
				printf("update gain = %f\r\n", gain);
				if ((gain > 0.0) && (gain < 10.0) && (gain != appv_conf.gain)) {
					return 0;  // primitive parameter is updated by library
				}
			}
			break;
	}
	printf("ERROR to update param[%d] %s !!!\r\n", param_ptr->parm_uref, param_ptr->parm_data.data_name);
	return -1;
}

// ----------------------------------------------------------

uint32_t loop_cnt = 0;

void appli_sched(void) {
	++loop_cnt;
	if (appv_log_level > 1)
		printf("thread_appli: %"PRIu32"\r\n", loop_cnt);

	// Simulate measures : Voltage and Temperature ...
	if (appv_measures_volt <= 0.0)
		appv_measures_volt_grad = 0.2;
	else if (appv_measures_volt >= 5.0)
		appv_measures_volt_grad = -0.3;

	if (appv_measures_temp <= -3)
		appv_measures_temp_grad = 1;
	else if (appv_measures_temp >= 20)
		appv_measures_temp_grad = -1;

	appv_measures_volt += appv_measures_volt_grad;
	appv_measures_temp += appv_measures_temp_grad;

	if (appv_log_level > 2)
		printf("thread_appli: %"PRIu32" - %s PUBLISH - volt=%2.2f temp=%d\r\n", loop_cnt,
				appv_measures_enabled ? "DATA" : "NO", appv_measures_volt, appv_measures_temp);

	if (appv_measures_enabled) {
		printf("LiveObjectsClient_PushData...\n");
		LiveObjectsClient_PushData(appv_hdl_data);
		appv_measures_counter++;
	}
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
	ret = LiveObjectsClient_AttachResources(appv_set_resources, SET_RESOURCES_NB, main_cb_rsc_ntfy, main_cb_rsc_data);
	if (ret) {
		printf(" !!! ERROR (%d) to attach RESOURCES !\r\n", ret);
	} else {
		printf("mqtt_start: LiveObjectsClient_AttachResources -> OK\n");
	}

	// Attach my local Configuration Parameters to the LiveObjects Client instance
	// ----------------------------------------------------------------------------
	ret = LiveObjectsClient_AttachCfgParams(appv_set_param, SET_PARAM_NB, main_cb_param_udp);
	if (ret) {
		printf(" !!! ERROR (%d) to attach Config Parameters !\r\n", ret);
	} else {
		printf("mqtt_start: LiveObjectsClient_AttachCfgParams -> OK\n");
	}

	// Attach my local STATUS data to the LiveObjects Client instance
	// --------------------------------------------------------------
	appv_hdl_status = LiveObjectsClient_AttachStatus(appv_set_status, SET_STATUS_NB);
	if (appv_hdl_status)
		printf(" !!! ERROR (%d) to attach status !\r\n", appv_hdl_status);
	else
		printf("mqtt_start: LiveObjectsClient_AttachStatus -> OK\n");

	// Attach one set of collected data to the LiveObjects Client instance
	// --------------------------------------------------------------------
	appv_hdl_data = LiveObjectsClient_AttachData(
	STREAM_PREFIX, "LO_sample_measures", "mV1", "\"Test\"", NULL, appv_set_measures, SET_MEASURES_NB);
	if (appv_hdl_data < 0) {
		printf(" !!! ERROR (%d) to attach a collected data stream !\r\n", appv_hdl_data);
	} else {
		printf("mqtt_start: LiveObjectsClient_AttachData -> OK\n");
	}

	// Attach a set of commands to the LiveObjects Client instance
	// -----------------------------------------------------------
	ret = LiveObjectsClient_AttachCommands(appv_set_commands, SET_COMMANDS_NB, main_cb_command);
	if (ret < 0) {
		printf(" !!! ERROR (%d) to attach a set of commands !\r\n", ret);
	} else {
		printf("mqtt_start: LiveObjectsClient_AttachCommands -> OK\n");
	}

	// Enable the receipt of commands
	ret = LiveObjectsClient_ControlCommands(true);
	if (ret < 0) {
		printf(" !!! ERROR (%d) to enable the receipt of commands !\r\n", ret);
	}

	// Enable the receipt of resource update requests
	ret = LiveObjectsClient_ControlResources(true);
	if (ret < 0) {
		printf(" !!! ERROR (%d) to enable the receipt of resource update request "
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
			appli_sched();
			LiveObjectsClient_Cycle(5000);
		}
	}

	return ret;
}
