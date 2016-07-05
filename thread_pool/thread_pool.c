/**
 * @file thread_pool.c
 * @brief  
 * @author Sun Jianjiao <jianjiaosun@163.com>
 * @version 1.0
 * @date 2016-06-06
 */

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "../array_queue/array_queue.h"
#include "../debug_lib/debug.h"
#include "thread_pool.h"

enum THREAD_POOL_STATUS {
	STATUS_TERMINATE,
	STATUS_RUN,
};

static void *worker_thread_run(void *thread_arg)
{
	struct thread_pool *thread_pool = (struct thread_pool *) thread_arg;
	struct thread_info *thread_info = thread_pool->threads;
	void *arg;

	for (;;)
	{

		/* 
		 * exit for status is set to "TERMINATE" when worker thread is run
		 * 
		 */
		pthread_rwlock_rdlock(&thread_info->status_lock);
		if (STATUS_TERMINATE == thread_info->status)
		{

			pthread_rwlock_unlock(&thread_info->status_lock);

			DEBUG_DBG("Thread %d terminated", pthread_self());
			break;
		}


		arg = array_queue_dequeue(thread_pool->queue); 

		/* exit when thread is waiting when status is set to "TERMINATE" */
		if (STATUS_TERMINATE == thread_info->status)
		{
			pthread_rwlock_unlock(&thread_info->status_lock);

			DEBUG_DBG("Thread %d terminated", pthread_self());
			break;
		}

		pthread_rwlock_unlock(&thread_info->status_lock);

		if (arg) 
		{
			if (thread_pool->msg_dispatch(arg))
			{
				DEBUG_ERROR("run failed");
			}
		}
		else
		{
			DEBUG_ERROR("No data");
			continue;
		}
	}


	return NULL;
}

static void _free_threads(struct thread_info *threads, struct array_queue *queue)
{
	int inc;
	pthread_t *thread_id = threads->id;

	array_queue_wakeup_all_dequeue(queue);

	pthread_rwlock_destroy(&threads->status_lock);

	for (inc = 0; inc < threads->nr; inc++)
	{
		if (pthread_join(thread_id[inc], NULL)) 
		{
			DEBUG_ERROR("pthread join failed\n");
		}
	}

	free(thread_id);
}

static struct thread_info *create_threads(int nr, struct array_queue *queue)
{
	int inc;
	pthread_t *thread_id;
	size_t id_len = nr * sizeof(pthread_t);
	int ret = 0;
	struct thread_info *threads;

	threads = (struct thread_info *) malloc(sizeof(*threads));
	if (!threads)
	{
		return NULL;
	}

	thread_id = (pthread_t *) malloc(id_len);
	if (!thread_id)
	{
		free(threads);
		return NULL;
	}

	memset(thread_id, 0, id_len);

	threads->status = STATUS_RUN;
	pthread_rwlock_init(&threads->status_lock, NULL);

	for (inc = 0; inc < nr; inc++)
	{
		ret = pthread_create(&thread_id[inc], NULL, worker_thread_run, threads);
		if (!ret)
		{
			_free_threads(threads, queue);
			return NULL;
		}

		threads->nr = inc;
	}

	
	return threads;
}



static void free_thread(struct thread_pool *thread_pool)
{
	_free_threads(thread_pool->threads, thread_pool->queue);
}

/**
 * @brief Create thread and message queue
 *
 * @param thread_nr		Number of worker thread
 * @param queue_size	Size of queue
 *
 * @return 	!NULL	Success
 *			NULL	Failed
 */
struct thread_pool *thread_pool_create(int worker_nr, int queue_size)
{
    struct thread_pool *thread_pool;
	struct array_queue *queue;
	struct thread_info *threads;

    thread_pool = (struct thread_pool *) malloc(sizeof(*thread_pool));
    if (!thread_pool)
    {
        return NULL;
    }
	memset(thread_pool, 0, sizeof(*thread_pool));

	queue = array_queue_create(queue_size);
	if (!queue)
	{
		free(thread_pool);

		return NULL;
	}
	thread_pool->queue = queue;

	threads = create_threads(worker_nr, queue);
	if (!threads)
	{
		free(thread_pool);

		return NULL;
	}

	return thread_pool;
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
int thread_pool_add(struct thread_pool *thread_pool, int len, void *data)
{
	int ret = -1;
	struct thread_info *thread_info = thread_pool->threads;

	pthread_rwlock_rdlock(&thread_info->status_lock);

	if (STATUS_RUN == thread_pool->threads->status) {
		ret = array_queue_enqueue(thread_pool->queue, len, data);
	}

	pthread_rwlock_unlock(&thread_info->status_lock);

	return ret;
}

/**
 * @brief Destroy the thread pool
 *
 * @param thread_pool 	Pointer of thread pool
 */
void thread_pool_destroy(struct thread_pool *thread_pool)
{
	struct thread_info *threads = thread_pool->threads;

	pthread_rwlock_wrlock(&threads->status_lock);
	threads->status = STATUS_TERMINATE;		
	pthread_rwlock_unlock(&threads->status_lock);

	free_thread(thread_pool);

	array_queue_destroy(thread_pool->queue);

	free(thread_pool);
	thread_pool = NULL;
}
