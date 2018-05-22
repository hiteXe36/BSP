/*
 * main.c
 *
 *  Created on: 13.05.2018
 *      Author: bs
 */

#define TASKQUEUE_SIZE 	50

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "fifo.h"
#include "taskqueue.h"
#include "control.h"
#include "threadPool.h"
#include <semaphore.h>

char* nameProducer = "/Producer";
char* nameConsumer = "/Consumer";

pthread_mutex_t producerMutex;
pthread_mutex_t consumerMutex;

sem_t producerInputSem;
sem_t producerOutputSem;
sem_t consumerInputSem;
sem_t consumerOutputSem;

int main()
{
	printf("LOG__Program start.__ \n");
	// create fifo
	fifo_init();

	//create thread pools
	threadPool_t* consumerPool = createThreadPool(nameConsumer, 10);
	threadPool_t* producerPool = createThreadPool(nameProducer, 10);

	// create controllthread
	initCtrlThread(producerPool, consumerPool);

	// joins
	joinThreadPool(producerPool);
	joinThreadPool(consumerPool);
	joinCtrlThread();

	return 0;
}
