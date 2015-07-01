#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "network.h"
#include "msg_lib.h"
#include "debug.h"

#define NAME				"Hello World!"
#define NAME2				"Hi wonnderfull world!"

static int g_sock = -1;

/**
 * @brief Test function of "send_msg_recv_ret"
 *
 * @return 
 */
static int set_name(void)
{
	int ret = 0;

	ret = send_msg_recv_ret(g_sock, NET_TEST, TEST_SET_NAME, strlen(NAME) + 1, NAME);

	if (0 == ret)
	{
		printf("Set name successfully\n");	
	}
	else
	{
		DEBUG_ERROR("Set name failed failed\n");	
	}

	return ret;
}

/**
 * @brief Tesing function of "send_cmd_recv_msg"
 *
 * @return 0 		success
 		   0thers  	failed
 */
static int get_name(void)
{
	char buf[NET_DEV_NAME_LEN];
	int ret;
		
	ret = send_cmd_recv_msg(g_sock, NET_TEST, TEST_GET_NAME, sizeof(buf), buf);
	if (!ret)
	{
		printf("Name is :%s\n", buf);
	}
	else
	{
		DEBUG_ERROR("get name failed\n");
	}

	return 0;
}

/**
 * @brief Tesing function of "send_msg_recv_msg"
 *
 * @return 0 		success
 		   0thers  	failed
 */
static int set_get_name(void)
{
	char buf[NET_DEV_NAME_LEN];
	int ret;
		
	ret = send_msg_recv_msg(g_sock, NET_TEST, TEST_SET_GET_NAME, strlen(NAME2) + 1, 
							NAME2, sizeof(buf), buf);
	if (!ret)
	{
		printf("Name is :%s\n", buf);
	}
	else
	{
		DEBUG_ERROR("get name failed\n");
	}

	return 0;
}

static int proc_request(int op)
{
	int ret = -1;

	printf("op = %d\n", op);
	switch (op)	
	{
		case 1:
			ret = set_name();
			break;
		case 2:
			ret = get_name();
			break;
		case 3:
			ret = set_get_name();
			break;
		default:
			break;
	}

	return ret;
}

int main(int argc, char *argv[])
{
	int op;
	int sock;
	
	sock = create_local_socket(NET_SOCK_PATH);
	if (-1 == sock)
	{
		DEBUG_ERROR("Create local socket error");
		return -1;
	}
	g_sock = sock;

	for(; ;)
	{
		printf("\nPlease input the selection\n"
			   "1: Set name to server\n"
			   "2: Get name from server\n"
			   "3: Set and get name from server\n");

		printf(">");
		scanf("%d", &op);

		if (0 != proc_request(op))
		{
			DEBUG_ERROR("Process requesting failed");
		}
	}

    return 0;
}
