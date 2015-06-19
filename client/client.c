#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "network.h"

int g_sock = -1; 

static int send_recv_ret(int type, int op, size_t len, void *send_msg)
{
	char *send_buf; 
	struct NET_DATA *net_data = (struct NET_DATA *) send_buf;
	unsigned err;
	int t;
	size_t send_len = sizeof(*net_data) + len;


	send_buf = (char *) malloc(send_len);
	if (NULL == send_buf)
	{
		return -1;
	}

	net_data = (struct NET_DATA *) send_buf;
	net_data->type = (unsigned) type;
	net_data->operation = (unsigned) op;

	if (send(g_sock, send_buf, sizeof(net_data) + len, 0) == -1) 
	{
		perror("send");
		exit(1);
	}

	if (( t= recv(g_sock, &err, sizeof(err), 0)) <= 0)
	{
		if (t < 0) 
			perror("recv");
		else 
			printf("Server closed connection\n");
	}

	if (err != 0)
	{
		return -1;
	}

	return 0;
}

static int net_miracast_start(char *dev_name)
{
	int ret = 0;
    struct sockaddr_un remote;
	int len;

	if (-1 == g_sock)
	{
		g_sock = socket(AF_UNIX, SOCK_STREAM, 0);
		if (g_sock== -1) 
		{
			perror("socket");
			ret = -1;
		}

		remote.sun_family = AF_UNIX;
		strcpy(remote.sun_path, NET_SOCK_PATH);
		len = strlen(remote.sun_path) + sizeof(remote.sun_family);
		if (connect(g_sock, (struct sockaddr *)&remote, len) == -1) {
			perror("connect");
			exit(1);
		}
	}

	ret = send_recv_ret(NET_MIRACAST, MIRACAST_APP_ENTER, strlen(dev_name) + 1, dev_name);

	if (0 == ret)
	{
		printf("miracast start successfully\n");	
	}
	else
	{
		printf("miracast start failed\n");	
	}

	return ret;
}

static int net_miracast_stop()
{
	close(g_sock);
	g_sock = -1;
	printf("miracast stop successfully\n");	

	return 0;
}

static int proc_request(int op)
{
	int ret = -1;

	printf("op = %d\n", op);
	switch (op)	
	{
		case 1:
			ret = net_miracast_start("jiaoge");
			break;
		case 2:
			ret = net_miracast_stop();
			break;
		default:
			break;
	}

	return ret;
}

int main(int argc, char *argv[])
{
	int op;

	for(; ;)
	{
		printf("\nPlease input the selection\n"
			"1: start app\n"
			"2: stop app\n");

		printf(">");
		scanf("%d", &op);

		if (0 != proc_request(op))
		{
			printf("failed\n");
		}
	}

    return 0;
}
