#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>

#include "msg_lib.h"
#include "network.h"
#include "debug.h"


static char g_name[NET_DEV_NAME_LEN];

static void get_name(int fd)
{
	if (0 != response_msg(fd, 0, strlen(g_name)+ 1, g_name))
	{
		DEBUG_ERROR("Response name failed");
	}
}

static void set_name(int fd, void *msg)
{
	strncpy(g_name, (char *)msg, sizeof(g_name));
	g_name[NET_DEV_NAME_LEN - 1] = '\0';

	if (0 != response_errno(fd, 0))
	{
		DEBUG_ERROR("Response error number failed");
	}
}

static void set_get_name(int fd, void *msg)
{
	strncpy(g_name, (char *)msg, sizeof(g_name));
	g_name[NET_DEV_NAME_LEN - 1] = '\0';

	get_name(fd);	
}

void net_test_process(int fd, int op, void *msg)
{
	switch (op)
	{
		case TEST_GET_NAME:
			get_name(fd);
			break;
		case TEST_SET_NAME:
			set_name(fd, msg);
			break;
		case TEST_SET_GET_NAME:
			set_get_name(fd, msg);
			break;
		default:
			DEBUG_ERROR("Invalid operation");
			break;


	}
}
