#include "threadPool.h"
#include <pthread.h>
#include <stdlib.h>

#define QUEUELENGHT		50
#define LOG(where)		printf("LOG__%s: \tThreads: %d/%d\t Active: %d\t Function: %s\n", pool->name, pool->runningThreads, pool->numberOfThreadsMax, pool->active, where)


void killThreadTask(void * arg) {
	//do nothing
}

void * takeTask(void* threadPool) {

	threadPool_t* pool = (threadPool_t*) threadPool;

	pthread_mutex_lock(&(pool->mutex));
	pool->runningThreads++;
	LOG("takeTask - init");
	pthread_mutex_unlock(&(pool->mutex));

	while (!(pool->active));

	while (pool->running) {

		task_t task = receiveFromTaskQueue(pool->taskqueue);
		LOG("takeTask - took Task");

		if (task.arg != 0) {
			task.routineForTask(task.arg);
		} else {
			task.routineForTask(NULL);
		}
	}

	pthread_mutex_lock(&(pool->mutex));
	pool->runningThreads--;
	if (pool->runningThreads == 0)
		pool->active = false;
	LOG("takeTask - destroy");
	pthread_mutex_unlock(&(pool->mutex));
	pthread_exit(0);
	return NULL;
}

void putTaskInPool(threadPool_t* pool, task_t newTask) {

	printFehler(sendToTaskQueue(pool->taskqueue, newTask, 0, false), "Taskqueue is full!");
	LOG("putTaskInPool");
}

threadPool_t* createThreadPool(char* name, int size) {

	threadPool_t* pool = (threadPool_t*) (malloc(sizeof(threadPool_t)));
	pool->name = name;
	pool->numberOfThreadsMax = size;
	pool->threadArray = (pthread_t*) (malloc(sizeof(pthread_t) * size));
	pool->taskqueue = createTaskQueue(name, QUEUELENGHT);
	pthread_mutex_init(&(pool->mutex), NULL);
	pool->running = true;
	pool->runningThreads = 0;

	for (int i = 0; i < size; i++) {
		pthread_create(&pool->threadArray[i], NULL, takeTask, (void*) pool);
	}

	while(pool->runningThreads != pool->numberOfThreadsMax);

	pool->active = true;
	LOG("createThreadPool");
	return pool;
}

void cancelThreadPool(threadPool_t * pool) {

	pool->running = false;

	task_t task;
	task.routineForTask = &killThreadTask;

	fifo_cleanUp();
	cleanTaskQueue(pool->taskqueue);

	for (int i = 0; i < pool->numberOfThreadsMax; i++) {
		sendToTaskQueue(pool->taskqueue, task, 0, false);
	}
	LOG("cancelThreadPool");
	while (pool->active);
	LOG("cancelThreadPool");
	destroyTaskQueue(pool->name);
	printFehler(pthread_mutex_destroy(&(pool->mutex)), "Zerstören des eines Mutex fehlgeschlagen!");
	free(pool->threadArray);
	free(pool);
}

void joinThreadPool(threadPool_t * pool) {

	int size = pool->numberOfThreadsMax;

	for (int i = 0; i < size; i++) {
		pthread_join(pool->threadArray[i], NULL);
	}
}
