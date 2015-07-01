#ifndef _MSG_LIB_H_
#define _MSG_LIB_H_

/*
 * As we use Unix Socket, Therefore there is no need convering byte order
 */
#define LOCAL_SOCKET_IPC

struct msg_request_head 
{
	unsigned type;			/* Module ID*/
	unsigned operation;		/* Operation ID*/
};

/**
 * @brief Byte order convert
 *		  hton and ntoh is same on same architecture
 *		  Therefore we only use the same convert function on both client and server
 *
 * @param request_head Head of request message
 */
static inline void msg_convert_request_head(struct msg_request_head *request_head)
{
#ifndef LOCAL_SOCKET_IPC
	request_head->type = htonl(request_head->type); 
	request_head->Operation = htonl(request_head->Operation);
#endif
}

struct msg_reponse_head
{
	unsigned err_code;	/* Error number*/
	unsigned len;		/* Length of reponsed message*/ 
};

static inline void msg_convert_response_head(struct msg_reponse_head *response_head)
{
#ifndef LOCAL_SOCKET_IPC
	response_head->error_no = htonl(response_head->error_no);
	response_head->len = htonl(response_head->len);
#endif
}

/* Process communication */
extern int send_msg(int sock, int type, int op, size_t len, const void *send_msg);
extern int recv_msg(int sock, size_t recv_len, void *recv_msg);

extern int send_msg_recv_msg(int sock, int type, int op, size_t send_len, 
				  		     const void *send_msg, size_t recv_len, void *recv_msg);
extern int send_msg_recv_ret(int sock, int type, int op, size_t len, const void *send_msg);
extern int send_cmd_recv_msg(int sock, int type, int op, size_t len, void *recv_msg);

extern int send_errno(int sock, int err_num);

/*Unix socket*/
extern int create_local_socket(const char *sock_file);

#endif
