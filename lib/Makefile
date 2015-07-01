CC = gcc
SRCS = $(wildcard *.c)
OBJS = $(patsubst %c, %o, $(SRCS))
CFLAGS += -O3 -fPIC -Wall

libmsg.so : $(OBJS)
		$(CC) $(CFLAGS) -shared -o $@ $(OBJS)

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

