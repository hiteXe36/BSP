/*
 * threadPool.h
 *
 *  Created on: 14.05.2018
 *      Author: bs
 */

#ifndef SRC_THREADPOOL_H_
#define SRC_THREADPOOL_H_

#include <pthread.h>
#include "taskqueue.h"
#include <semaphore.h>

typedef struct threadPool_s {
	int NUMBER_OF_THREADS;
	pthread_t* threadArray;
	mqd_t taskqueue;
	int numberOfTasks;
	bool running;
	pthread_mutex_t* mutex;
	sem_t* queueOutput;
	sem_t* queueInput;
} threadPool_t;


void putTaskInPool(threadPool_t* pool, task_t newTask);

threadPool_t createThreadPool(char* name, int size, pthread_mutex_t* mutex, sem_t* input, sem_t* output);

void cancelThreadPool(char* name, threadPool_t * pool);

void joinThreadPool(threadPool_t * pool);

#endif /* SRC_THREADPOOL_H_ */
