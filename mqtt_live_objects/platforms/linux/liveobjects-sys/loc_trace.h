/*
 * Copyright (C) 2016 Orange
 *
 * This software is distributed under the terms and conditions of the 'BSD-3-Clause'
 * license which can be found in the file 'LICENSE.txt' in this package distribution
 * or at 'https://opensource.org/licenses/BSD-3-Clause'.
 */
/**
 * @file   loc_trace.h
 * @brief  Plug LiveObjects traces for Linux system
 */

#ifndef __loc_trace_H_
#define __loc_trace_H_

#ifdef __cplusplus
extern "C" {
#endif

#define LOTRACE_INIT(level)           lo_trace_init(level)

#define LOTRACE_LEVEL(level)          lo_trace_level(level)

#define LOTRACE_ERR_I(...)            lo_trace(1, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOTRACE_ERR(...)              lo_trace(1, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOTRACE_WARN(...)             lo_trace(2, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOTRACE_NOTICE(...)           lo_trace(3, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOTRACE_INF(...)              lo_trace(4, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOTRACE_DBG1(...)             lo_trace(5, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOTRACE_DBG2(...)             lo_trace(6, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOTRACE_DBG_VERBOSE(...)      ((void)0)

#define LOTRACE_PRINTF                lo_trace_printf

void lo_trace_init(int level);

void lo_trace_level(int level);

void lo_trace(int level, const char *file, unsigned int line,
		const char *function, char const *format, ...);

void lo_trace_printf(char const *format, ...);

#ifdef __cplusplus
}
#endif

#endif /* __loc_trace_H_ */
