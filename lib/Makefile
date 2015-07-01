CC = gcc
SRCS = $(wildcard *.c)
OBJS = $(patsubst %c, %o, $(SRCS))
CFLAGS += -O3 -fPIC -Wall -g

libmsg.so : $(OBJS)
		$(CC) -shared -o $@ $(OBJS) $(CFLAGS)

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm *.o libmsg.so
