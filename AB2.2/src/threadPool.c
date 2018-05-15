#include "threadPool.h"
#include <pthread.h>
#include <stdlib.h>
#include "taskqueue.h"
#include <semaphore.h>

#define QUEUELENGHT		50



void killThreadTask(void * arg) {
	//do nothing
}

void * takeTask(void* threadPool) {

	threadPool_t* pool = (threadPool_t*) threadPool;

	while (pool->running) {
		sem_wait(pool->queueOutput);
		pthread_mutex_lock(pool->mutex);
		if (!pool->running)
			pthread_exit(0);

		task_t task = receiveFromTaskQueue(pool->taskqueue);

		if (task.arg != 0) {
			task.routineForTask(task.arg);
		} else {
			task.routineForTask(NULL);
		}

		pool->numberOfTasks--;
		pthread_mutex_unlock(pool->mutex);
		sem_post(pool->queueInput);
	}

	return NULL;
}

void putTaskInPool(threadPool_t* pool, task_t newTask) {

	sem_wait(pool->queueInput);
	pthread_mutex_lock(pool->mutex);

	sendToTaskQueue(pool->taskqueue, newTask, 0, false);
	pool->numberOfTasks++;

	pthread_mutex_unlock(pool->mutex);
	sem_post(pool->queueOutput);
	printf("LOG__Task put in queue, Task Nr. %d\n", pool->numberOfTasks);
}

threadPool_t createThreadPool(char* name, int size, pthread_mutex_t* mutex, sem_t* input, sem_t* output) {

	sem_init(output, 0, 0);
	sem_init(input, 0, QUEUELENGHT);
	threadPool_t* newThreadPool = (threadPool_t*) (malloc(sizeof(threadPool_t)));
	newThreadPool->NUMBER_OF_THREADS = size;
	newThreadPool->threadArray =
			(pthread_t*) (malloc(sizeof(pthread_t) * size));
	newThreadPool->taskqueue = createTaskQueue(name, QUEUELENGHT);
	newThreadPool->mutex = mutex;

	newThreadPool->queueInput = input;
	newThreadPool->queueOutput = output;
	pthread_mutex_init(newThreadPool->mutex, NULL);

	newThreadPool->running = true;
	newThreadPool->numberOfTasks = 0;

	for (int i = 0; i < size; i++) {
		pthread_create(&newThreadPool->threadArray[i], NULL, takeTask,
				(void*) newThreadPool);
		printf("LOG__Thread Nr. %d created.__\n", i);
	}

	return *newThreadPool;
}

void cancelThreadPool(char* name, threadPool_t * pool) {
	pool->running = false;

	while (pool->numberOfTasks > 0) {
		receiveFromTaskQueue(pool->taskqueue);
		pool->numberOfTasks--;
	}

	task_t task;
	task.routineForTask = &killThreadTask;

	for (int i = 0; i < pool->NUMBER_OF_THREADS; i++) {
		sendToTaskQueue(pool->taskqueue, task, 0, false);
	}

	destroyTaskQueue(name);
}

void joinThreadPool(threadPool_t * pool) {

	int size = pool->NUMBER_OF_THREADS;

	for (int i = 0; i < size; i++) {
		pthread_join(pool->threadArray[i], NULL);
	}

	free(pool->threadArray);
	free(pool);
}
