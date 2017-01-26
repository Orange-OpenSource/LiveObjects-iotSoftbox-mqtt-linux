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
 * @file  loc_sys.c
 * @brief System Interface : Mutex , Thread, ..
 */

#include "iotsoftbox-core/loc_sys.h"

#include <pthread.h>
#include <string.h>

#include "liveobjects-client/LiveObjectsClient_Config.h"
#include "liveobjects-client/LiveObjectsClient_Core.h"
#include "liveobjects-sys/loc_trace.h"
#include "liveobjects-sys/socket_defs.h"

static pthread_t _lo_sys_thread_id = 0;
static pthread_t _lo_sys_thread_def;
static uint32_t _lo_sys_thread_max = 0;

static struct {
	pthread_mutex_t mutex;
	pthread_t mutex_id;
} _lo_sys_mutex[LO_SYS_MUTEX_NB];

/*=================================================================================*/
/* Private Functions*/
/*---------------------------------------------------------------------------------*/

static void * _LO_sys_threadExec(void *argument) {
	LOTRACE_DBG1(" _LO_sys_threadExec: go %p...", argument);

	LiveObjectsClient_Run((LiveObjectsD_CallbackState_t) argument);

	LOTRACE_WARN(" _LO_sys_threadExec: EXIT");
}

/*=================================================================================*/
/* Public Functions*/
/*---------------------------------------------------------------------------------*/
/* Initialization*/
void LO_sys_init(void) {
	int i;
	_lo_sys_thread_id = 0;
	_lo_sys_thread_max = 0;

	memset(_lo_sys_mutex, 0, sizeof(_lo_sys_mutex));

	for (i = 0; i < LO_SYS_MUTEX_NB; i++) {
		pthread_mutex_init(&_lo_sys_mutex[i].mutex, NULL);
		/* TODO Think to do something if the initialization goes wrong*/
	}
}
/*=================================================================================*/
/* MUTEX*/
/*---------------------------------------------------------------------------------*/

uint8_t LO_sys_mutex_lock(uint8_t idx) {
	if (idx)
		LOTRACE_DBG1(" => LO_sys_mutex_lock(%u)", idx);
	if (idx < LO_SYS_MUTEX_NB) {
		int ret = pthread_mutex_lock(&_lo_sys_mutex[idx].mutex);
		if (ret != 0) {
			LOTRACE_ERR(" !!!! LO_sys_mutex_lock(%u): ERROR %x", idx, ret);
		}
		return (ret == 0) ? 0 : -1;
	}
	return -2;
}

void LO_sys_mutex_unlock(uint8_t idx) {
	if (idx)
		LOTRACE_DBG1(" <= LO_sys_mutex_unlock(%u)", idx);
	if (idx < LO_SYS_MUTEX_NB)
		pthread_mutex_unlock(&_lo_sys_mutex[idx].mutex);
}

/*=================================================================================*/
/* THREAD*/
/*---------------------------------------------------------------------------------*/

void LO_sys_threadRun(void) {
	pthread_t id = pthread_self();
	if ((_lo_sys_thread_id) && (_lo_sys_thread_id != id)) {
		LOTRACE_WARN(" LO_sys_threadRun: %lu %lu", _lo_sys_thread_id, id);
	} else {
		LOTRACE_WARN("LiveObjectsClient: thread_id= x%lu (x%lu)", id,
				_lo_sys_thread_id);
	}
	_lo_sys_thread_id = id;
}

/*---------------------------------------------------------------------------------*/

uint8_t LO_sys_threadIsLiveObjectsClient(void) {
	pthread_t id = pthread_self();
	return (_lo_sys_thread_id == id) ? 1 : 0;
}

/*---------------------------------------------------------------------------------*/

int LO_sys_threadStart(const void *argument) {
	memset(&_lo_sys_thread_def, 0, sizeof(_lo_sys_thread_def));

	int ret = pthread_create(&_lo_sys_thread_def, NULL, _LO_sys_threadExec,
			(void *) argument);
	if (ret != 0) {
		LOTRACE_ERR("Error while creating LiveObjects Client Thread ..");
		return -1;
	}

	LOTRACE_WARN("LiveObjects Client Thread x%lu is running !!!",
			_lo_sys_thread_id);
	return 0;
}

/*---------------------------------------------------------------------------------*/

void LO_sys_threadCheck(void) {
	/* TODO add some stuff or remove the function*/
}
