/**
 * @file main.c
 * @brief Test entry for debug message 
 *
 * You can distributed and modify it freely.
 *
 * @author Jianjiao sun <Jianjiaosun@163.com>
 * @version 1.0
 * @date 2015-06-12
 */

#include <stdio.h>
#include <stdarg.h>

#include "debug.h"

int debug_test_ut()
{
	int inc;

	for (inc = 0; inc < LOG_BUTT; inc++)
	{
		debug_set_log_level(inc);
		DEBUG_ERROR("Error: current level: %d", inc);
		DEBUG_INFO("Information");
		DEBUG_WARN("Warning");

		printf("---------------------------------\n");
	}

    return 0;
}

#if 0

int main()
{
    return debug_test_ut();
}

#endif
