/**
 * @file array_queue.c
 * @brief Array implementtation of queues which support multi-thread
 *        Compile with option -- " -lrt -lpthread"
 *
 * @author Sun Jianjiao <jianjiaosun@163.com>
 * @version 1.0
 * @date 2016-07-01
 */
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>

#include "array_queue.h"

#define DEBUG_ERROR 	printf
#define DEBUG_INFO		printf
#define DEBUG_DBG		


static int is_queue_full(struct array_queue *queue)
{
	if (queue->nr == queue->cap)
	{
		return 1;	
	}

	return 0;
}

static int is_queue_empty(struct array_queue *queue)
{
	return (queue->nr <= 0);
}

static int next_queue_index(int idx, struct array_queue *queue)
{
	idx++;
	
	if (idx == queue->cap)
	{
		idx = 0;
	}

	return idx;
}

/**
 * @brief Create array queue 
 *
 * @param cap Capability of the queue
 *
 * @return NULL		Failed
 *		   !NULL 	Success
 */
struct array_queue *array_queue_create(int cap)
{
	struct array_queue *queue;
	void *array;

	queue = (struct array_queue *) malloc(sizeof(*queue));
	if (!queue)
	{
		DEBUG_ERROR("Queue create failed\n");
		return NULL;
	}

	array = (void *) malloc(sizeof(struct job) * cap);
	if (!array)
	{
		DEBUG_ERROR("Array create failed\n");

		free(queue);
		return NULL;
	}
	
	queue->array_job = array;
	queue->cap = cap;
	queue->nr = 0;
	queue->head = 0;
	queue->tail = 0;
	pthread_mutex_init(&queue->mutex, NULL);
	pthread_cond_init(&queue->read, NULL);
	pthread_cond_init(&queue->write, NULL);

	return queue;
}


/**
 * @brief Add message into queue
 *
 * @param thread_pool	Pointer of thread_pool
 * @param len			Length of data
 * @param data			Data
 *
 * @return 	0	success
 *			-1	Failed
 */
int array_queue_enqueue(struct array_queue *queue, int len, void *data)
{
	void *arg;
	int ret = -1;
	int is_empty;

	arg = malloc(len);
	if (!arg)
	{
		DEBUG_ERROR("malloc failed\n");
		return -1;
	}
	memcpy(arg, data, len);	

	pthread_mutex_lock(&queue->mutex);

	DEBUG_DBG("Producer: nr = %d, tail = %d\n", queue->nr, queue->tail);

	if (is_queue_full(queue))
	{
		DEBUG_DBG("Array queue if full, wait\n");

		pthread_cond_wait(&queue->write, &queue->mutex);
	}

	if (is_queue_empty(queue))
	{
		DEBUG_INFO("Array queue not empty, notify consumer\n");
		pthread_cond_signal(&queue->read);
	}

	queue->array_job[queue->tail].arg = arg;
	queue->array_job[queue->tail].len = len;

	queue->nr++;
	queue->tail = next_queue_index(queue->tail, queue);
	DEBUG_DBG("Producer: equeueed, nr = %d, tail = %d\n", queue->nr, queue->tail);

	pthread_mutex_unlock(&queue->mutex);

	return 0;
}

/**
 * @brief Get data from queue
 *
 * @param queue Structure when create the queue
 *
 * @return Dequeued data 
 * @attention Should free the pointer of returned
 */
void *array_queue_dequeue(struct array_queue *queue)
{
	int is_full;
	void *arg;
	struct job *array;
	
	pthread_mutex_lock(&queue->mutex);

	DEBUG_DBG("Consumer: nr = %d, head = %d\n", queue->nr, queue->head);

	if (is_queue_empty(queue))
	{
		DEBUG_DBG("Consumer array queue is empty, wait \n");

		pthread_cond_wait(&queue->read, &queue->mutex);
	}

	if (is_queue_full(queue))
	{
		DEBUG_INFO("Consumer: array queue is not full, notify producer.\n");
		pthread_cond_signal(&queue->write);	
	}

	array =queue->array_job; 
	arg = array[queue->head].arg;

	array[queue->head].arg = NULL;
	array[queue->head].len = 0;
	queue->nr--;
	queue->head = next_queue_index(queue->head, queue);
	DEBUG_DBG("Consumer: after dequeued,  nr = %d, head = %d\n", queue->nr, queue->head);

	pthread_mutex_unlock(&queue->mutex);

	return arg;
}

/**
 * @brief Release all the resource of the array queue
 *
 * @param queue	Pointer to array queue
 *
 */
void array_queue_destroy(struct array_queue *queue)
{
	int inc;
	int max = queue->cap;
	struct job *array = queue->array_job;

	pthread_cond_destroy(&queue->read);
	pthread_cond_destroy(&queue->write);
	pthread_mutex_destroy(&queue->mutex);

	for (inc = 0; inc < max; inc++)
	{
		if (array[inc].arg) {
			free(array[inc].arg);
		}
	}

	free(queue);
	queue = NULL;
}


/**
 * @brief Wakeup all dequeue 
 *
 * @param queue	Array queue
 */
void array_queue_wakeup_all_dequeue(struct array_queue *queue)
{
		
	pthread_mutex_lock(&queue->mutex);
	pthread_cond_broadcast(&queue->read);
	pthread_mutex_unlock(&queue->mutex);
}
