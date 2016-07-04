#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#include "array_queue.h"


#define UT_TEST


#ifdef UT_TEST

#define DEBUG_ERROR     printf
#define DEBUG_INFO              printf
#define DEBUG_DBG               

#define READ_MAX	5
#define QUEUE_SIZE	3
static struct array_queue *queue;

static pthread_t consumer_id[READ_MAX];
static pthread_t producer_id[READ_MAX];

static void *consumer_thread(void *arg) 
{
	int inc; 
	unsigned *data;

	DEBUG_INFO("consumer thread: %ld\n", pthread_self());

	for (inc = 0; inc < READ_MAX; inc++)
	{
		data = array_queue_dequeue(queue);

		DEBUG_INFO("consumer, inc %d: data %d\n", inc, *data);

		free(data);
	}
}

static void *producer_thread(void *arg) 
{
	int inc; 


	for (inc = 0; inc < READ_MAX; inc++)
	{
		array_queue_enqueue(queue, sizeof(inc), &inc);

		DEBUG_INFO("producer, inc %d: data %d\n", inc, inc);
	}

	DEBUG_INFO("producer thread: %ld\n", pthread_self());
}

static void test_consumer(int nr)
{
	int inc;
	pthread_t id;

	for (inc = 0; inc < nr; inc++)
	{
		pthread_create(&consumer_id[inc], NULL, consumer_thread, NULL);

		DEBUG_DBG("Create consumer thread %d\n", inc);
	}
}

static void test_producer(int nr)
{
	int inc;
	pthread_t id;

	for (inc = 0; inc < nr; inc++)
	{
		pthread_create(&producer_id[inc], NULL, producer_thread, NULL);
		DEBUG_DBG("Create producer thread %d\n", inc);
	}
}

static void test_producer_consumer()
{
	test_producer(1);	

	test_consumer(1);

	pthread_join(consumer_id[0], NULL);
	pthread_join(producer_id[0], NULL);
}

int main()
{
	queue = array_queue_create(QUEUE_SIZE);	
	test_producer_consumer();

	DEBUG_DBG("exit\n");
	array_queue_destroy(queue);	
}
#endif
