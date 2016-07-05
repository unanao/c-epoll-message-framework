SRCS = $(wildcard *.c)
CFLAGS += -O3 -fPIC -Wall -g -Wwrite-strings

UT_TEST_OBJ = $(patsubst %c, %o, $(SRCS))
LIB_OBJ = $(filter-out main_test.o, $(patsubst %c, %o, $(SRCS)))
LDFLAGS += -L../array_queue/

libthread_pool.so : ${LIB_OBJ}
	gcc -shared -o $@ ${LIB_OBJ} ${LDFLAGS} -lrt -lpthread -larray_queue

ut_test : $(UT_TEST_OBJ)
	gcc $(CFLAGS) -o $@ $(UT_TEST_OBJ) $(LDFLAGS) -lrt -lpthread -larray_queue

%.o:%.c
	gcc -c $(CFLAGS) $(<) -o $@

clean:
	rm -f *.o *.so ut_test
