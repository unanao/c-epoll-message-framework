#ifndef _ARRAY_H_
#define _ARRAY_H_

struct job {
	unsigned len;
	void *arg;
};


struct array_queue {
	struct job *array_job ;
	unsigned cap;			//Max number for array queue
	unsigned nr;			//data number of queue 
	unsigned head;
	unsigned tail;
	pthread_mutex_t mutex;
	pthread_cond_t read;
	pthread_cond_t write;
};

extern struct array_queue *array_queue_create(int cap);
extern int array_queue_enqueue(struct array_queue *queue, int len, void *data);
extern void *array_queue_dequeue(struct array_queue *queue);
extern void array_queue_destroy(struct array_queue *queue);

#endif
