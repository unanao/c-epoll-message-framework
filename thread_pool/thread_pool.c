/**
 * @file thread_pool.c
 * @brief  
 * @author Sun Jianjiao <jianjiaosun@163.com>
 * @version 1.0
 * @date 2016-06-06
 */

typedef int (*msg_dispatch_fn_t)(void *arg);

enum {
	STATUS_TERMINATE,
	STATUS_RUN,
};

struct thread_info {
	unsigned nr;
	pthread_t *id;
	unsigned status;
	pthread_rwlock_t rwlock;
};


struct thread_pool {
	struct thread_info *thread;
	struct array_queue *queue;
};

static int create_thread(int nr, thread_info *thread_info)
{
	int inc;
	struct thread_t *thread_id;
	int ret = 0;

	thread_id = (struct pthread_t) malloc(nr * sizeof(pthread_t));
	if (!thread)
	{
		return -1;
	}

	for (inc = 0; inc < nr; inc++)
	{
		ret = pthread_create(&thread_id[inc], NULL, worker_thread_run, &thread_id[inc]);
		if (!ret)
		{
			ret = -1;	

			break;
		}

		if (pthread_join(&thread_id[inc], NULL)) 
		{
			DEBUG_ERROR("");
			ret = -1;
			break;
		}
	}

	if (!ret)
	{
		thread_info->nr = nr;
		thread_info->status = STATUS_RUN;
		thread_info->rwlock = PTHREAD_RWLOCK_INITIALIZER;
	}
	else 
	{
		free(thread_id);
	}
	
	return ret;
}

static void *worker_thread_run(void *arg)
{
	struct msg_queue *msg_queue = thread_pool->queue;
	struct thread_info *thread_info = thead_pool->thread;
	void *arg;
	int ret = -1;

	for (;;)
	{

		/* 
		 * TODO: how to avoid thread between status check and broadcast 
		 * exit when thread is working and miss the waked up testing 
		 */
		pthread_rwlock_rdlock(&thread_info->rwlock);
		if (STATUS_TERMINATE == thread_info->status)
		{
			DEBUG_DBG("Thread %d terminated", pthread_self());
			break;
		}

		pthread_rwlock_unlock(&thread_info->rwlock);

		arg = array_queue_dequeue(thread_pool->queue); 
		if (!arg) {
			DEBUG_ERROR("No data");
			continue;
		}

		/* exit when receive thread is waked up */
		pthread_rwlock_rdlock(&thread_info->rwlock);
		if (STATUS_TERMINATE == thread_info->status)
		{
			DEBUG_DBG("Thread %d terminated", pthread_self());
			break;
		}
		pthread_rwlock_unlock(&thread_info->rwlock);

		if (msg_queue->msg_dispatch(arg))
		{
			DEBUG_ERROR("run failed");
		}
	}
}


static int free_thread(struct thread_info *thread_info)
{
	pthread_rwlock_rwlock(&thread_info->rwlock);
	thread_info->status = STATUS_TERMINATE;		
	pthread_rwlock_unlock();

	pthread_cond_braodcast(&msg_queue->read);

	pthread_mutex_destroy(&thread_info->mutex);

	free(thread_info->id);
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
struct *thread_pool thread_pool_create(int worker_nr, int queue_size)
{
    struct thread_pool *thread_pool;
	sturct array_queue *queue;

    thread_pool = (struct thread_pool *) malloc(sizeof(*pool));
    if (!pool)
    {
        return NULL;
    }
	memset(thread_pool, 0, sizeof(*thread_pool));

	if (create_thread(nr, &thread_pool->thread))
	{
		free(pool);

		return NULL;
	}


	queue = array_queue_create(queue_size);
	if (!queue)
	{
		free(pool);
		free(thread_pool->thread.id)

		return NULL;
	}

	thread_pool->queue = queue;
	
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
	return array_queue_enqueue(thread_pool->queue, len, data);
}

/**
 * @brief Destroy the thread pool
 *
 * @param thread_pool 	Pointer of thread pool
 */
void thread_pool_destroy(struct thread_pool *thread_pool)
{
	array_queue_destory(thread_pool->array_queue);
	free_thread();
	free(thread_pool);

	thread_pool = NULL;
}
