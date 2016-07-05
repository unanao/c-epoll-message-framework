#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

typedef int (*msg_dispatch_fn_t)(void *arg);


struct thread_info {
	unsigned nr;
	pthread_t *id;
	unsigned status;
	pthread_rwlock_t status_lock;
};


struct thread_pool {
	struct thread_info *threads;
	struct array_queue *queue;
	msg_dispatch_fn_t msg_dispatch;
};

extern void thread_pool_destroy(struct thread_pool *thread_pool);
extern int thread_pool_add(struct thread_pool *thread_pool, int len, void *data);
extern struct thread_pool *thread_pool_create(int worker_nr, int queue_size);

#endif
