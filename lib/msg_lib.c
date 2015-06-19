#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

int send_msg(int sock, int type, int op, size_t len, void *send_msg)
{
	size_t send_len = sizeof(*net_data) + len;
	char *send_buf; 
	struct NET_DATA *net_data = (struct NET_DATA *) send_buf;
	unsigned err;

	send_buf = (char *) malloc(send_len);
	if (NULL == send_buf)
	{
		DEBUG_ERROR("Malloc faialed: errno = %d", errno);
		return -1;
	}

	net_data = (struct NET_DATA *) send_buf;
	net_data->type = (unsigned) type;
	net_data->operation = (unsigned) op;

	ret = send(g_sock, send_buf, sizeof(net_data) + len, 0); 
	if (-1 == ret) 
	{
		DEBUG_ERROR("Send failed");
	}
	
	free(send_buf);

	return ret;
}

int recv_msg(int sock, int type, const void *send_msg, size_t recv_len, void *recv_msg)
{
	int nr;
	int ret = -1;

	nr = recv(g_sock, &err, sizeof(err), 0);
	if ( nr > 0)
	{

	}
	else if (nr < 0)
	{
		DEBUG_ERROR("Receive error: %d", errno);
	}
	else 
	{
		DEBUG_ERROR("Sever closed the connection");
	}

	return ret;
}

int send_recv_msg(int sock, int type, int op, size_t send_len, 
				  const void *send_msg, size_t recv_len, void *recv_msg)
{

}

static int _send_recv_ret(int sock, int type, int op, size_t len, void *send_msg)
{
	int t;


	return 0;
}

static int create_local_socket(const char *sock_file)
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

	size = siezeof(remote.sun_path)
	strncpy(remote.sun_path, sock_file, size);
	if (size > 0)
	{
		remote.sun_path[size - 1] = '\0';
	}

	len = strlen(remote.sun_path) + sizeof(remote.sun_family);
	if (connect(g_sock, (struct sockaddr *)&remote, len) == -1) {
		DEBUG_ERROR("Connect failed, errno = %d", errno);

		close(sock);
		sock = -1;
	}

	return sock;
}

/**
 * @brief Send request to server and wait sever's reply 
 *	      The function used for send action to server, and only concern execute results
 *
 * @param sock_file		Socket file for domain socket
 * @param type			Module ID
 * @param op			Operation ID
 * @param len			Message length for sending 
 * @param send_msg		Message for sending
 *
 * @return				0	Success
 *						!0 	Failed
 */
int send_recv_ret_by_sockfile(const char *sock_file, int type, int op, size_t len, void *send_msg)
{
	int sock;
	int ret = -1;

	sock = create_lock_socket(sock_file);
	if (sock >= 0)
	{
		ret = send_recv_ret_by_sockfd(sock, type, op, len, send_msg);	
	}
	
	return ret;
}
