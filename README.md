# Epoll message framework 

The framework for communication between processes.

## code framework
### socket lib  
Use API from libmsg.so to send and receive message.  
Directory: lib

### Array implementation of queue which support multi-threads
Direcotry: array_queue
The array queue support multi-threads

### debug_lib
Debug framework, can be used for any project singlely, which is easy to use and  print line and function number

### server 
Daemon framework, add different action into function of "net_msg_process"

### client
client framework

## Run Example 

### Build  
#### in the root directory

| Command        | Function                |
| -------------- | ----------------------- |
| make           | build the whole project |
| make lib       | build libmsg.so         |
| make client    | build client	           |
| make server    | build server            |
| make debug_lib | build debug lib 		   |

#### in the sub-directory
change into the target directory, then excute

`
make
`

### start Server

	cd script   
	./run_server.sh


### Start client

	cd script   
	./run_client.sh
