/**
 * @file unix_sock_lib.c
 * @brief Unix socket library for communication between different processes
 * @author Jianjiao Sun <jianjiaosun@163.com>
 * @version 1.0
 * @date 2015-06-23
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "msg_lib.h"
#include "msg_debug.h"

/**
 * @brief Create local socket and connect to server
 *
 * @param sock_file		Domain unix file name
 *
 * @return 	Socket handler
 */
int create_local_socket(const char *sock_file)
{
	int sock;
	int size;
    struct sockaddr_un remote;
	int len;

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sock== -1) 
	{
		DEBUG_ERROR("Create socket failed, errno = %d", errno);
		return -1;
	}

	remote.sun_family = AF_UNIX;

	size = sizeof(remote.sun_path);
	strncpy(remote.sun_path, sock_file, size);
	if (size > 0)
	{
		remote.sun_path[size - 1] = '\0';
	}

	len = strlen(remote.sun_path) + sizeof(remote.sun_family);
	if (connect(sock, (struct sockaddr *)&remote, len) == -1) {
		DEBUG_ERROR("Connect failed, errno = %d", errno);

		close(sock);
		sock = -1;
	}

	return sock;
}

