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
 * @file loc_trace.c
 * @brief Trace/Log Interface.
 */

#include "config/liveobjects_dev_params.h"
#include "liveobjects-sys/loc_trace.h"
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#if SYSLOG
#include <syslog.h>
#endif

#define LOGP_STACK_CHECK
#define LOGP_MSG_TAG 0x5A

#define TACE_LEVELS_MAX 4
#define LOGP_MAX_MSG_SIZE 500
#define LOGP_TAIL_MSG_SIZE 5

static uint32_t _trace_max_msg_size = 0;
static uint32_t _trace_index = 0;
static int _trace_level_current = TACE_LEVELS_MAX;
static char _trace_str[LOGP_MAX_MSG_SIZE + LOGP_TAIL_MSG_SIZE];
static const char _trace_TraceLib[TACE_LEVELS_MAX + 2] = "-EWID";

void lo_trace_init(int level) {
	_trace_level_current = level;

#if SYSLOG
	//TODO CHANGE THE NAME
	openlog("main", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
#endif
}
void lo_trace_level(int level) {
	_trace_level_current = level;
}

void lo_trace(int level, const char *file, unsigned int line,
		const char *function, char const *format, ...) {

	if (level > 0) {
#if SYSLOG
		int priority = 0;
		switch (level) {
		case 1:
			priority = LOG_ERR;
			break;
		case 2:
			priority = LOG_WARNING;
			break;
		case 3:
			priority = LOG_NOTICE;
			break;
		case 4:
			priority = LOG_INFO;
			break;
		case 5:
			priority = LOG_DEBUG;
			break;
		case 6:
			priority = LOG_DEBUG;
			break;
		}
#endif
		va_list ap;
		va_start(ap, format);
		if (level > TACE_LEVELS_MAX)
			level = TACE_LEVELS_MAX;
		if (level <= _trace_level_current) {
#if 0
			struct tm timestamp;
#endif
			char *pt_str = _trace_str;
			char *end_str = _trace_str + LOGP_MAX_MSG_SIZE;

#ifdef LOGP_STACK_CHECK
			char *end_buf = pt_str + sizeof(_trace_str);
			char *pc = end_str;
			while (pc < end_buf)
				*pc++ = LOGP_MSG_TAG;
#endif
			*end_str = LOGP_MSG_TAG;

			pt_str += snprintf(pt_str, end_str - pt_str, "%04u",
					++_trace_index);

#if 0
			if (pt_str < end_str) {
				/*Get timestamp*/
				tb_getTime(&timestamp);

				/*Convert timestamp into date string in YearDay-HH:MM:SS format*/
				pt_str += tb_convertTimeToDateString(&timestamp, pt_str, end_str-pt_str);
			}
#endif

#if 1
			/*Add  milliseconds since the program started */
			if (pt_str < end_str) {
				char buffer[26];
				int millisec;
				struct tm *tm_info;
				struct timeval tv;

				gettimeofday(&tv, NULL);

				millisec = lrint(tv.tv_usec / 1000.0); /* Round to nearest millisec*/
				if (millisec >= 1000) { /* Allow for rounding up to nearest second*/
					millisec -= 1000;
					tv.tv_sec++;
				}

				tm_info = localtime(&tv.tv_sec);

				strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
				pt_str += snprintf(pt_str, end_str - pt_str, ":%s.%03d", buffer,
						millisec);
			}
#endif
#if 0
			/* And add Task Id (tid)*/
			if (pt_str < end_str) {
				pt_str += snprintf(pt_str, end_str-pt_str, ":t%02d", os_tsk_self());
			}
#endif

			/*Add debug level to log*/
			if (pt_str < end_str)
				pt_str += snprintf(pt_str, end_str - pt_str, ":%c:",
						*(_trace_TraceLib + level));

			/*Add file:line: */
			if (pt_str < end_str) {
				const char *name = strrchr(file, '\\');
				if ((name) && (*name == '\\')) {
					name++;
				} else {
					name = strrchr(file, '/');
					if ((name) && (*name == '/'))
						name++;
					else
						name = file;
				}
				pt_str += snprintf(pt_str, end_str - pt_str, "%s:%u:%s:", name,
						line, function);
			}

			if (pt_str < end_str) {
				/* add user data*/
				pt_str += vsnprintf(pt_str, end_str - pt_str, format, ap);
				/* Store the size of the biggest trace*/
				uint32_t msg_size = pt_str - _trace_str;
				if (_trace_max_msg_size < msg_size) {
					_trace_max_msg_size = msg_size;
				}
			}

			if (pt_str >= end_str) {
				/* Truncated message*/
				pt_str = end_str - 1;
#ifdef LOGP_STACK_CHECK
				if (*pt_str) {
					/* LogBug(0);*/
				}
				pc = end_str;
				while (pc < end_buf) {
					if (*pc != LOGP_MSG_TAG) {
						/* LogBug(1);*/
					}
					pc++;
				}
#endif
				*pt_str = 0;
			}

			if (*end_str != LOGP_MSG_TAG) {
#if 0
				fprintf(stderr, "\nLogRecord: Corrupted stack end_ptr=%p 0x%X != 0x%X", end_str, *end_str, LOGP_MSG_TAG);
#endif
				/* CATCH_SOFT_EXCEPTION(EXCEPTION_CODE_LOG_OVERLOAD); // Don't LOG
				 * Macro*/
				/* because it calls  LOG_... function !!*/
				va_end(ap);
				return;
			}

			if (*(pt_str - 1) != '\n') {
				*pt_str++ = '\n';
				*pt_str = 0;
			} else if ((*(pt_str) != 0)) {
				*pt_str = 0;
			}

#if SYSLOG
			syslog(priority, _trace_str);
#else
			printf("%s\n", _trace_str);
#endif

		}
	}
}

void lo_trace_printf(char const *format, ...) {
	va_list ap;
	va_start(ap, format);

	char *pt_str = _trace_str;
	char *end_str = _trace_str + LOGP_MAX_MSG_SIZE;

	*end_str = LOGP_MSG_TAG;

	/* add user data*/
	pt_str += vsnprintf(pt_str, end_str - pt_str, format, ap);
	/* Store the size of the biggest trace*/
	uint32_t msg_size = pt_str - _trace_str;
	if (_trace_max_msg_size < msg_size) {
		_trace_max_msg_size = msg_size;
	}
	if (pt_str >= end_str) {
		/* Truncated message*/
		pt_str = end_str - 1;
		*pt_str = 0;
	}

	if (*end_str != LOGP_MSG_TAG) {
#if 0
		fprintf(stderr, "\nLogRecord: Corrupted stack end_ptr=%p 0x%X != 0x%X", end_str, *end_str, LOGP_MSG_TAG);
#endif
		/* CATCH_SOFT_EXCEPTION(EXCEPTION_CODE_LOG_OVERLOAD); Don't LOG Macro
		 because it calls  LOG_... function !!*/
		va_end(ap);
		return;
	}
#if SYSLOG
	syslog(LOG_NOTICE, _trace_str);
#else
	printf("%s\n", _trace_str);
#endif

}
