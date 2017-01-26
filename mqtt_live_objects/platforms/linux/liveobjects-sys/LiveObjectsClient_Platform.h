/*
 * Copyright (C) 2016 Orange
 *
 * This software is distributed under the terms and conditions of the 'BSD-3-Clause'
 * license which can be found in the file 'LICENSE.txt' in this package distribution
 * or at 'https://opensource.org/licenses/BSD-3-Clause'.
 */

/**
 * @file  LiveObjectsClient_Platform.h
 * @brief Specific definitions for LiveObjects Client library
 */

#ifndef __LiveObjectsClient_Platform_H_
#define __LiveObjectsClient_Platform_H_

#include <inttypes.h>
#include <stdlib.h>

#ifndef PRIu32
#define PRIu32    "u"
#endif

#ifndef PRIi32
#define PRIi32    "d"
#endif

#ifndef PRIu64
#define PRIu64    "lu"
#endif

#ifndef PRIi64
#define PRIi64    "ld"
#endif

#define MEM_ALLOC(len)        ((char*) malloc(len))

#define MEM_FREE(p)            free((void*)(p))

#define WAIT_MS(dt_ms)         delay(dt_ms)

void WAIT_MS(uint32_t dt_ms);

#endif /* __LiveObjectsClient_Platform_H_ */
