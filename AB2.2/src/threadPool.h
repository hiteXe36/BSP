/*
 * threadPool.h
 *
 *  Created on: 14.05.2018
 *      Author: bs
 */

#ifndef SRC_THREADPOOL_H_
#define SRC_THREADPOOL_H_


#include <pthread.h>
#include <stdio.h>
#include "taskqueue.h"
#include <semaphore.h>
#include "fifo.h"

typedef struct threadPool_s {
	char* name;
	int numberOfThreadsMax;
	pthread_t* threadArray;
	mqd_t taskqueue;
	volatile bool running;
	volatile bool active;
	volatile int runningThreads;
	pthread_mutex_t mutex;
} threadPool_t;


void putTaskInPool(threadPool_t* pool, task_t newTask);

threadPool_t* createThreadPool(char* name, int size);

void cancelThreadPool(threadPool_t * pool);

void joinThreadPool(threadPool_t * pool);

#endif /* SRC_THREADPOOL_H_ */
