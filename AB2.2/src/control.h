/*
 * control.h
 *
 *  Created on: 13.05.2018
 *      Author: bs
 */

#ifndef CONTROL_H_
#define CONTROL_H_

#include "taskqueue.h"
#include <pthread.h>
#include "threadPool.h"


void initCtrlThread(threadPool_t* producer, threadPool_t* consumer);
void produce(void*);
void consume(void*);
void joinCtrlThread();

#endif /* CONTROL_H_ */
