CC = gcc  
CFLAGS = -O2 -Wall  

SUBDIRS = lib client server
.PHONY:${SUBDIRS}
all:$(SUBDIRS)
${SUBDIRS}:
	${MAKE} -C $@ 




