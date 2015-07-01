/**
 * @file main.c
 * @brief main 
 * @author Jianjiao Sun <sunjianjiao@hisense.com>
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

#define MAX_BUF_LEN		4096
#define MAXEVENTS 		64

static int make_socket_non_blocking(int sfd)
{
	int flags, s;

	flags = fcntl (sfd, F_GETFL, 0);
	if (flags == -1)
	{
		perror ("fcntl");
		return -1;
	}

	flags |= O_NONBLOCK;
	s = fcntl (sfd, F_SETFL, flags);
	if (s == -1)
	{
		perror ("fcntl");
		return -1;
	}

	return 0;
}

int create_socket(char *sock_path)
{
    struct sockaddr_un local;
	int fd = -1;
	int ret;
	int len;

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
		return -1;
    }

    unlink(sock_path);

	len = sizeof(local.sun_path);
    local.sun_family = AF_UNIX;
    strncpy(local.sun_path, sock_path, len);
	local.sun_path[len - 1] = '\0';

	ret = bind(fd, (struct sockaddr *)&local, sizeof(local)); 
    if (ret >= 0) {
		ret = listen(fd, SOMAXCONN);
    }

	if (0 != ret)
	{
		close(fd);
		fd = -1;
	}

	return fd;
}

int add2epoll(int efd, int flags, int fd)
{
	struct epoll_event event;
	int ret;

	event.data.fd = fd;
	event.events = flags;
	
	ret = epoll_ctl(efd, EPOLL_CTL_ADD, fd, &event);
	if (0 != ret)
	{
		DEBUG_ERROR(" add to epoll error\n");
	}

	return ret;
}


/**
 * @brief We have a notification on the listening socket, which
	      means one or more incoming connections. 
 *
 * @return 
 */
int new_connect(int listen_fd, int efd)
{
	int infd;
	int ret = 0;

	infd = accept(listen_fd, NULL, 0);
	if (infd == -1)
	{
		printf("infd is -1\n");
		if ((errno == EAGAIN) ||
			(errno == EWOULDBLOCK))
		{
			/* We have processed all incoming
			   connections. */
		}
		else
		{
			perror("accept");
		}

		return -1;
	}

	/* Make the incoming socket non-blocking and add it to the
	   list of fds to monitor. */
	ret = make_socket_non_blocking(infd);
	if (ret == -1)
	{
		DEBUG_ERROR("set none blocking error\n");
		return ret;
	}

	return add2epoll(efd, EPOLLIN | EPOLLET, infd);
}

static int dispatch_msg(int efd, struct epoll_event *ep_event)
{
	char buf[MAX_BUF_LEN];
	int ret = -1;
	struct msg_request_head *head;
	int n;
	unsigned err = 1;
	int fd = ep_event->data.fd;
	uint32_t event = ep_event->events;

	/* An error has occured on this fd, or the socket is not
	   ready for reading (why were we notified then?) */
	if ((event& EPOLLERR) || (event & EPOLLHUP) || (!(event & EPOLLIN)))
	{
		DEBUG_ERROR("error epoll event: %d, err = %d, hup = %d, in = %d\n", 
			   event, EPOLLERR, EPOLLHUP, EPOLLIN);

		epoll_ctl(efd, EPOLL_CTL_ADD, fd, NULL);
		close(fd);

		return -1;
	}

	n = recv(fd, buf, sizeof(buf), 0);
	if (n <= 0) 
	{
		send(fd, &err, sizeof(err), 0);

		return -1;
	}

	head = (struct msg_request_head *) buf;

	DEBUG_INFO("Dispatch message: type = %d, operation = %d", head->type, head->operation);

	switch (head->type)
	{
		case NET_WIRED:
			break;
		case NET_WIRELESS:
			break;
		case NET_MIRACAST:
			break;
		case NET_TEST:
			ret = net_test_process(fd, head->operation, buf + sizeof(*head));
			break;
		default:
			DEBUG_ERROR("Invalid Module ID");
			break;

	}

	return ret;
}

static void eloop_run(int efd, int listen_fd)
{
	struct epoll_event events[MAXEVENTS];
	struct epoll_event *event;
	int n, i;
	int fd;
	int ret = -1;

	while (1)
	{
		n = epoll_wait(efd, events, MAXEVENTS, -1);
		if (-1 == n)
		{
			DEBUG_ERROR("epoll wait error\n");	
			break;
		}

		for (i = 0; i < n; i++)
		{
			event = &events[i];
			fd = event->data.fd;

			if (listen_fd == fd)
			{
				ret = new_connect(listen_fd, efd);
				if (0 != ret)
				{
					printf("accept error\n");
				}
			}
			else
			{
				ret = dispatch_msg(efd, event);
				if (0 != ret)
				{
					printf("Dispatch message failed\n");
				}
			}

		}

	}
}

int main (int argc, char *argv[])
{
	int listen_sock;
	int efd;
	int ret;

	listen_sock = create_socket(NET_SOCK_PATH); 
	if (-1 == listen_sock) {
		return -1;
	}

	efd = epoll_create1(0);
	if (efd == -1)
	{
		perror ("epoll_create");
		close(listen_sock);

		return -1;
	}

	ret = add2epoll(efd, EPOLLIN, listen_sock);
	if (0 == ret)
	{
		eloop_run(efd, listen_sock);
	}

	close(listen_sock);
	close(efd); 

	return ret;
}
