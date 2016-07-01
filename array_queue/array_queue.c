/**
 * @file array_queue.c
 * @brief Array implementtation of queues which support multi-thread
 * @author Sun Jianjiao <jianjiaosun@163.com>
 * @version 1.0
 * @date 2016-07-01
 */
#include <stdlib.h>


static int is_queue_full(struct array_queue *queue)
{
	if (queue->nr == queue->max)
	{
		return 1;	
	}

	return 0;
}

static int is_queue_empty(struct array_queue *queue)
{
	return (0 == queue->nr);
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
	struct void *array;

	queue = (struct array_queue *) malloc(sizeof(*queue));
	if (!queue)
	{
		DEBUG_ERROR("Queue create failed\n");
		return NULL;
	}

	array = (void *) malloc(sizeof(void *));
	if (!array)
	{
		DEBUG_ERROR("Array create failed\n");

		free(queue);
		return NULL;
	}
	
	queue->array = array;
	queue->cap = cap;
	queue->nr = 0;
	queue->head = 0;
	queue->tail = 0;
	queue->mutex = PTHREAD_MUTEX_INITIALIZER;
	queue->read = PTHREAD_COND_INITIALIZER;
	queue->write = PTHREAD_COND_INITIALIZER;

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
		DEBUG_ERROR("malloc failed");
		return -1;
	}
	memcpy(arg, data, len);	

	pthread_mutex_lock(&queue->mutex);

	if (is_full(queue))
	{
		DEBUG_DBG("message queue if full, wait");

		pthread_cond_wait(&msg_queue->write, &msg_queue->mutex);
	}

	is_empty = is_queue_empty(queue);

	queue->queue[tail].arg = arg;
	queue->queue[tail].len = len;

	queue->nr++;
	queue->tail = next_queue_index(queue->tail, queue);

	pthread_mutex_unlock(&msg_queue->mutex);

	if (is_empty())
	{
		pthread_cond_signal(&msg_queue->read)
	}

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
	
	pthread_mutex_lock(&msg_queue->mutex);

	if (is_emputy(queue))
	{
		DEBUG_DBG("message queue if emputy");

		pthread_cond_wait(&queue->read, &queue->mutex);
	}

	if_full = is_queue_full(queue);

	arg = queue->array[queue->head];

	array[queue->head] = NULL;
	queue->nr--;
	queue->head = next_queue_index(queue->head, queue);

	pthread_mutex_unlock(&queue->mutex);

	if (is_full)
	{
		pthread_cond_signal(&queue->write);	
	}

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
	void *array = queue->array;

	pthread_cond_destroy(&queue->read);
	pthread_cond_destroy(&queue->write);
	pthread_mutex_destroy(&queue->mutex);

	for (inc = 0; inc < max; inc++)
	{
		if (arra[inc]) {
			free(array[inc]);
		}
	}

	free(queue);
	queue = NULL;
}
