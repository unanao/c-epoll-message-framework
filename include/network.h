#ifndef _NETWORK_H_
#define _NETWORK_H_

#define							NET_DEV_NAME_LEN		32
#define 						NET_SOCK_PATH			"/tmp/net_sock_file"

/**
 * @brief Model
 */
enum {
	NET_WIRED,		//Module 
	NET_WIRELESS,
	NET_MIRACAST,
	NET_TEST,	//Example for Message test
};

/*
 * Sub-type of NET_MSG_TEST 
 */
enum {
	TEST_GET_NAME,			//Operation 1
	TEST_SET_NAME,			//Operation 2
	TEST_SET_GET_NAME,		//Operation 3
};

#endif
