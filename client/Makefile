PROJECT_ROOT = $(abspath ../)
CC = gcc
SRCS = $(wildcard *.c)
OBJS = $(patsubst %c, %o, $(SRCS))
CFLAGS += -O3 -fPIC -Wall -g
CFLAGS += -I$(PROJECT_ROOT)/lib -I$(PROJECT_ROOT)/include -I$(PROJECT_ROOT)/debug_lib
LDFLAGS += -L$(PROJECT_ROOT)/lib -L$(PROJECT_ROOT)/debug_lib

LDFLAGS += -lmsg -ldebug



client : $(OBJS)
		$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm *.o client
