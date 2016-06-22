PROJECT_ROOT = $(abspath ../)
CC = gcc
SRCS = $(wildcard *.c)
OBJS = $(patsubst %c, %o, $(SRCS))
CFLAGS += -O3 -fPIC -Wall -g -I$(PROJECT_ROOT)/debug_lib -L$(PROJECT_ROOT)/debug_lib

libmsg.so : $(OBJS)
		$(CC) -shared -o $@ $(OBJS)

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@ $(CFLAGS)

clean:
	rm *.o libmsg.so
