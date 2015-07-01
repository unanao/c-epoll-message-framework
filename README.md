# c-epoll-message-framework

1, Execute command of "make" in the root directory

2, Start server 
  1) export export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$your-lib-directory
    for example:export export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$your-lib-directory/home/unanao/open-source/c-epoll-message-framework/lib/
  2) cd server; ./netd
  
3、Start client
    1) start a new console
    2) export export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$your-lib-directory
      for example:export export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$your-lib-directory/home/unanao/open-source/c-epoll-message-framework/lib/
    3)cd client; ./client
