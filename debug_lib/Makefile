SRCS = $(wildcard *.c)
OBJ = $(patsubst %c, %o, $(SRCS))
CFLAGS += -O3 -fPIC -Wall -g -Wwrite-strings

libdebug.so : ${OBJ}
	gcc -shared -o $@ ${OBJ}

%.o:%.c
	gcc -c $(CFLAGS) $(<) -o $@

clean:
	rm *.o libdebug.so 
