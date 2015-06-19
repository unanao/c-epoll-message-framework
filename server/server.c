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

#define MAX_BUF_LEN		4096
#define SERVER			"From-server: "
#define MAXEVENTS 		64
#define DBG_ERR         printf

static int make_socket_non_blocking (int sfd)
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

int recv_send_msg(int fd, int recv_len, void *recv_buf)
{
	int t, len;
	struct sockaddr_un local, remote;
	char send_buf[MAX_BUF_LEN];
	int done = 0, n;

//	do {
		n = recv(fd, recv_buf, sizeof(recv_buf), 0);
		if (n <= 0) 
		{
			if (n < 0) perror("recv");
			done = 1;
		}

		if (!done) 
		{
			
			if (send(fd, SERVER, strlen(SERVER), 0) < 0) 
			{
				perror("send");
			}

			if (send(fd, recv_buf, sizeof(recv_buf), 0) < 0) 
			{
				perror("send");
				done = 1;
			}
		}
//	} while (!done);

#if 0
	while (1)
	{
		recv_send_msg(sock_fd, sizeof(buf), buf);	
		count = read(fd, buf, sizeof(buf));
		if (count == -1)
		{
			/* If errno == EAGAIN, that means we have read all
			   data. So go back to the main loop. */
			if (errno != EAGAIN)
			{
				perror ("read");
				done = 1;
			}
			break;
		}
		else if (count == 0)
		{
			/* End of file. The remote has closed the
			   connection. */
			done = 1;
			break;
		}


		printf("%s\n", buf);
	}

	if (done)
	{
		close(fd);
	}

#endif

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
		DBG_ERR(" add to epoll error\n");
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
		DBG_ERR("set none blocking error\n");
		return ret;
	}

	return add2epoll(efd, EPOLLIN | EPOLLET, infd);
}

static int miracast_proc(int fd, int op, void *msg)
{
	int ret = 0;

	switch (op)	
	{
		case MIRACAST_APP_ENTER:
			printf("recv miracast app enter \n");
			break;
		default:
			break;
	}

	send(fd, &ret, sizeof(ret), 0);

	return ret;
}

static int dispatch_msg(int fd)
{
	char buf[MAX_BUF_LEN];
	int ret;
	struct NET_DATA *head;
	int n;
	unsigned err = 1;

	n = recv(fd, buf, sizeof(buf), 0);
	if (n <= 0) 
	{
		send(fd, &err, sizeof(err), 0);

		return -1;
	}

	head = (struct NET_DATA *) buf;

	switch (head->type)
	{
		case NET_WIRED:
			break;
		case NET_WIRELESS:
			break;
		case NET_MIRACAST:
			ret = miracast_proc(fd, head->operation, buf + sizeof(*head));
			break;

	}

	return ret;
}

/**
 * @brief We have data on the fd waiting to be read. Read and
		  display it. We must read whatever data is available
	      completely, as we are running in edge-triggered mode
	      and won't get a notification again for the same data. 
 *
 * @param event
 *
 * @return 
 */
static int proc_request(int efd, struct epoll_event *ep_event)
{
	int ret;
	int done = 0;
	ssize_t count;
	int fd = ep_event->data.fd;
	uint32_t event = ep_event->events;

	/* An error has occured on this fd, or the socket is not
	   ready for reading (why were we notified then?) */
	if ((event& EPOLLERR) || (event & EPOLLHUP) || (!(event & EPOLLIN)))
	{
		printf("error epoll event: %d, err = %d, hup = %d, in = %d\n", 
			   event, EPOLLERR, EPOLLHUP, EPOLLIN);

		epoll_ctl(efd, EPOLL_CTL_ADD, fd, NULL);
		close(fd);
	}

	return dispatch_msg(fd);
}

static void eloop_run(int efd, int listen_fd)
{
	struct epoll_event events[MAXEVENTS];
	struct epoll_event *event;
	int n, i;
	int fd;
	int ret;

	while (1)
	{
		n = epoll_wait (efd, events, MAXEVENTS, -1);
		if (-1 == n)
		{
			DBG_ERR("epoll wait error\n");	
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
				ret = proc_request(efd, event);
			}
		}

		if (0 != ret)
		{
			printf("error in eloop_run\n");
		}
	}
}

int main (int argc, char *argv[])
{
	int listen_sock;
	int efd;
	int sock_fd;
	char buf[MAX_BUF_LEN];
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
