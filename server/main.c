/**
 * @file main.c
 * @brief main 
 * @author Jianjiao Sun <jianjiaosun@163.com>
 * @version 1
 * @date 2015-07-01
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>

#include "network.h"
#include "msg_lib.h"
#include "debug.h"
#include "netd.h"

typedef void (*type_handler_fn_t)(int fd, int op, void *msg);

type_handler_fn_t type_handler[] = {
	NULL,
	NULL,
	NULL,
	net_test_process,	
};

static void net_msg_process(int fd, int type, int op, void *msg)
{
	if ((type < NET_TYPE_INVALID) && (type_handler[type]))
	{
		type_handler[type](fd, op, msg);
	}
}

int main (int argc, char *argv[])
{
	int ret;

	ret = msg_init(NET_SOCK_PATH); 
	if (0 == ret)
	{
		msg_run(net_msg_process);
	}

	msg_finit();

	return ret;
}
