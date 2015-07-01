CC = gcc  
CFLAGS = -O2 -Wall  

SUBDIRS = lib client server
.PHONY:${SUBDIRS}

${SUBDIRS}:
	${MAKE} -C $@ 




