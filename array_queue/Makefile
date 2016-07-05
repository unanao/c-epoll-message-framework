SRCS = $(wildcard *.c)
CFLAGS += -O3 -fPIC -Wall -g -Wwrite-strings

UT_TEST_OBJ = $(patsubst %c, %o, $(SRCS))
LIB_OBJ = $(filter-out main_test.o, $(patsubst %c, %o, $(SRCS)))

libarray_queue.so : ${LIB_OBJ}
	gcc -shared -o $@ ${LIB_OBJ}

ut_test : $(UT_TEST_OBJ)
	gcc $(CFLAGS) -o $@ $(UT_TEST_OBJ) $(LDFLAGS) -lrt -lpthread

%.o:%.c
	gcc -c $(CFLAGS) $(<) -o $@

clean:
	rm -f *.o *.so ut_test
