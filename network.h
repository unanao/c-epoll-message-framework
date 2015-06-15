#ifndef _NETWORK_H_
#define _NETWORK_H_

#define			NET_DEV_NAME_LEN		32
#define 		NET_SOCK_PATH			"/tmp/net_sock_file"

/**
 * @brief Model
 */
enum {
	NET_WIRED,
	NET_WIRELESS,
	NET_MIRACAST,
};

/*
 * Sub-type of miracast 
 */
enum {
	MIRACAST_APP_ENTER,
};

struct NET_DATA 
{
	unsigned type;
	unsigned operation;
	char dev_name[NET_DEV_NAME_LEN];
};

#endif
