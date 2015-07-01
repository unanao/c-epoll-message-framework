/**
 * @file msg_lib.c
 * @brief Library for communication between different processes
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

#include "msg_lib.h"

#ifndef _ARPA_INET_H
#include <arpa/inet.h>
#endif


#define DEBUG_ERROR			printf

/**
 * @brief Safe recv when interrupted by signal
 *
 * @param sock 	Socket fd
 * @param buf	Buffer for receiving message
 * @param len	Buffer length
 *
 * @return  	0 	Success
 *				!0 	failed
 */
int recv_safe(int sock, void *buf, size_t len)
{  
	size_t nr = -1;
	int ret = -1;

    do {  
		nr = recv(sock, buf, len, 0);
    } while ((-1 == nr) && (errno == EINTR));

	if (nr < 0)
	{
		DEBUG_ERROR("Receive error: %d", errno);
	}
	else if (0 == nr)
	{
		DEBUG_ERROR("Sever closed the connection");
	}
	else
	{
		ret = 0;
	}

	return ret;
}

/**
 * @brief Safe send when interrupted by signal
 *
 * @param sock 	Socket fd
 * @param buf	Buffer for  message
 * @param len	Buffer length
 *
 * @return  	0 	Success
 *				!0 	failed
 */
int send_safe(int sock, void *buf, size_t len)
{
	ssize_t nr;
	int ret = -1;

	do {	
		nr = send(sock, buf, len, 0); 
    } while ((-1 == nr) && (errno == EINTR));

	if (nr >= 0)
	{
		ret = 0;
	}

	return ret;
}

/**
 * @brief send message
 *
 * @param sock   	Socket fd
 * @param type		Command type ID
 * @param op		Operation for the @type
 * @param len		Length of sending message
 * @param send_buf	Message to be sent	
 * 					
 * @caution	@send_buf should Should Convert byte order if send between different architecture 
 *
 * @return			0 	Success
 					!0  failed
 */
int send_msg(int sock, int type, int op, size_t send_len, const void *send_buf)
{
	char *buf; 
	struct msg_request_head *request_head;
	int len = sizeof(struct msg_request_head) + send_len;
	int ret;

	buf = (char *) malloc(len);
	if (NULL == buf)
	{
		DEBUG_ERROR("Malloc faialed: errno = %d", errno);
		return -ENOMEM;
	}
	request_head = (struct msg_request_head *) buf;

	request_head->type = (unsigned) type;
	request_head->operation = (unsigned) op;
	msg_convert_request_head(request_head);

	if ((0 != send_len) && (NULL != send_buf))
	{
		memcpy(request_head + 1, send_buf, send_len);
	}

	ret = send_safe(sock, buf, len); 
	if (-1 == ret) 
	{
		DEBUG_ERROR("Send failed, errno = %d", errno);
	}
	
	free(buf);

	return ret;
}

/**
 * @brief Receive message 
 *
 * @param sock 		Socket fd
 * @param recv_len	Length fo received buffer
 * @param recv_msg	Buffer for receivign message
 *
 * @caution	@recv_msg should convert byte order if transfer between different architecures
 *
 * @return	0	Success
 *			!0	Fialed
 */
int recv_msg(int sock, size_t recv_len, void *recv_msg)
{
	int nr;
	int ret = -1;
	struct msg_response_head *recv_head;
	size_t len;
	char *buf;

	len = sizeof(*recv_head) + recv_len;

	buf = (char *) malloc(len);
	if (NULL == buf)
	{
		DEBUG_ERROR("Malloc faialed: errno = %d", errno);
		return -ENOMEM;
	}

	ret = recv_safe(sock, buf, len);
	if (!ret)
	{
		recv_head = (struct msg_response_head*) buf;
		msg_convert_response_head(recv_head);

		ret = -recv_head->err_code;   /* Use positive between transfer*/
		if (!ret && recv_msg)
		{
			if (recv_head->len <= recv_len)
			{
				memcpy(recv_msg, recv_head + 1, recv_head->len);
			}
			else
			{
				DEBUG_ERROR("The received buffer is not enough");
				ret = -1;
			}
		}
	}

	free(buf);

	return ret;
}

/**
 * @brief Send and wait message retured 
 *
 * @param sock			Socket fd
 * @param type			Command type ID
 * @param op			Operation of @type
 * @param send_len		Length of sending message
 * @param send_buf		Message to be sent
 * @param recv_len		Buffer ength of receiving message
 * @param recv_msg		Buffer for receiving message
 *
 * @return 				0	Success
 						!0  Failed
 */
int send_msg_recv_msg(int sock, int type, int op, size_t send_len, 
				  const void *send_buf, size_t recv_len, void *recv_buf)
{
	int ret;

	ret = send_msg(sock, type, op, send_len, send_buf);
	if (!ret)
	{
		ret = recv_msg(sock, recv_len, recv_buf);
	}

	return ret;
}

/**
 * @brief Send and wait message retured 
 *
 * @param sock			Socket fd
 * @param type			Command type ID
 * @param op			Operation of @type
 * @param recv_len		Buffer ength of receiving message
 * @param recv_msg		Buffer for receiving message
 *
 * @return 				0	Success
 						!0  Failed
 */
int send_cmd_recv_msg(int sock, int type, int op, size_t recv_len, void *recv_buf)
{
 	return send_msg_recv_msg(sock, type, op, 0, NULL, recv_len, recv_buf);
}

/**
 * @brief Send and wait message retured 
 *		  Same with function of "send_recv_msg", except no message returen , only results
 *
 * @param sock			Socket fd
 * @param type			Command type ID
 * @param op			Operation of @type
 * @param send_len		Length of sending message
 * @param send_buf		Message to be sent
 *
 * @return 				0	Success
 						!0  Failed
 */
int send_msg_recv_ret(int sock, int type, int op, size_t send_len, const void *send_buf)
{
	int ret;
	unsigned err_num;

	ret = send_msg(sock, type, op, send_len, send_buf);
	if (!ret)
	{
		ret = recv_safe(sock, &err_num, sizeof(err_num));
		if (!ret)
		{
			ret = -ntohl(err_num);
		}
	}

	return ret;
}

/**
 * @brief Send error code 
 *
 * @param sock	Socket fd
 * @param err_num	Error Code
 *
 * @return 
 */
int response_errno(int sock, int err_num)
{
	unsigned send_err = htonl(-err_num);

	return send_safe(sock, &send_err, sizeof(send_err));
}


/**
 * @brief Response message
 *
 * @param sock Socket fd
 * @param error_no	Error number
 * @param msg_len	Length of message
 * @param msg		Message to be sent
 *
 * @return 
 */
int response_msg(int sock, int error_no, size_t msg_len, const void *msg)
{
	struct msg_response_head *head;
	int len = sizeof(*head) + msg_len;
	int ret;
	char *buf;

	buf = (char *) malloc(len);
	if (NULL == buf)
	{
		DEBUG_ERROR("Malloc faialed: errno = %d", errno);
		return -ENOMEM;
	}
	head = (struct msg_response_head *) buf;

	head->err_code = (unsigned) -error_no;
	head->len = (unsigned) msg_len;
	msg_convert_response_head(head);

	if ((0 != msg_len) && (NULL != msg))
	{
		memcpy(head + 1, msg, msg_len);
	}

	ret = send_safe(sock, buf, len); 
	if (-1 == ret) 
	{
		DEBUG_ERROR("Send failed, errno = %d", errno);
	}
	
	free(buf);

	return ret;
}
