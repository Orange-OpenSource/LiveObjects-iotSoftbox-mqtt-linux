/*
 * Copyright (C) 2016 Orange
 *
 * This software is distributed under the terms and conditions of the 'BSD-3-Clause'
 * license which can be found in the file 'LICENSE.txt' in this package distribution
 * or at 'https://opensource.org/licenses/BSD-3-Clause'.
 */

/**
 * @file socket_defs.h
 * @brief Socket  definitions (handle, ..).
 *
 */

#ifndef SOCKET_DEFS_H_
#define SOCKET_DEFS_H_

#if defined(__cplusplus)
extern "C" {
#endif

#define SOCKETHANDLE_NULL   ((socketHandle_t)-1)

typedef int socketHandle_t;

#if defined(__cplusplus)
}
#endif

#endif /* SOCKET_DEFS_H_ */
