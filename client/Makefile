PROJECT_ROOT = $(abspath ../)
CC = gcc
SRCS = $(wildcard *.c)
OBJS = $(patsubst %c, %o, $(SRCS))
CFLAGS += -O3 -fPIC -Wall
CFLAGS += -I $(PROJECT_ROOT)/lib -I $(PROJECT_ROOT)/include
LDFLAGS += -L $(PROJECT_ROOT)/lib
LDFLAGS += -lmsg



client : $(OBJS)
		$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

