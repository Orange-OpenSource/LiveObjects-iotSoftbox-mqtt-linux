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
 * @file  delay.c
 * @brief Specific definitions for LiveObjects Client library
 */

#include <time.h>
#include <unistd.h>

#include "liveobjects-sys/LiveObjectsClient_Platform.h"
#include "liveobjects-sys/loc_trace.h"

void WAIT_MS(uint32_t dt_ms) {
	struct timespec tim, tim2;
	if (dt_ms > 0) {
		tim.tv_sec = dt_ms / 1000;
		tim.tv_nsec = (dt_ms % 1000) * 1000000;
		nanosleep(&tim, &tim2);
	} else {
		LOTRACE_ERR("ERROR : Sleeping time is negative - Sleep call aborted");
	}
}
