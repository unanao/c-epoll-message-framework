#ifndef _MSG_LIB_H_
#define _MSG_LIB_H_

struct msg_request_head 
{
	unsigned type;			/* Module ID*/
	unsigned operation;		/* Operation ID*/
};

struct msg_reponse_head
{
	unsigned error_no;	/* Error number*/
	unsigned len;		/* Length of reponsed message*/ 
};

#endif
