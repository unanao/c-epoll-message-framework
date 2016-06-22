/**
 * @file debug.c
 * @brief Debug Message
 *
 * Can be distributed and modified freely
 *
 * @author Jianjiao Sun <jianjiaosun@163.com>
 * @version 1.0
 * @date 2015-06-12
 */

#include <stdio.h>
#include <stdarg.h>

#include "debug.h"

int g_debug_log_level = ERROR;

#define MAX_LOG_LEN			512

void debug_print(int level, const char *file, int line, const char *fmt, ...)
{
	char str[MAX_LOG_LEN];
	va_list ap;

	if (level > g_debug_log_level)
	{
		return;
	}

	va_start(ap, fmt);
	vsnprintf(str, sizeof(str), fmt, ap);
	va_end(ap);

	printf("%s:%d, %s\n", file , line, str);
}

void debug_set_log_level(int level)
{
	g_debug_log_level = level;
}
