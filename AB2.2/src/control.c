/*
 * control.c
 *
 *  Created on: 13.05.2018
 *      Author: bs
 */
#include "control.h"
#include "threadPool.h"
#include <pthread.h>
#include "fifo.h"
#include <string.h>
#include <time.h>
#include "taskqueue.h"
#include "getcharTimeout.h"

char* helpText =
		"– Tastatureingabe p oder P: Ueber diese Tastatureingabe wird zyklisch zwischen /n "
				" den Modi “Es werden nur Grossbuchstaben erzeugt“, “Es werden nur Kleinbuchstaben /n"
				" erzeugt“ und “Es werden keine Producer Auftraege vergeben“ umgeschaltet. /n"
				"– Tastatureingabe c oder C: Starte / Stoppe die Vergabe von Consumer Auftraegen. /n"
				"– Tastatureingabe q oder Q: Termination des Systems.";

pthread_t ctrlThreadID;
typedef enum {
	LOWERCASE, UPPERCASE, NO_PRODUCTION
} state_t;
state_t productionState;
bool consumerOn;
threadPool_t* consumerPool;
threadPool_t* producerPool;
extern char* nameConsumer;
extern char* nameProducer;

void produce(char letter) {
	fifo_push(letter);
}

void consume(void) {
	fifo_pop();
}

void * ctrlThread(void * notused) {

	while (1) {
		//Productiontasks
		task_t newTask;
		switch (productionState) {
		case (LOWERCASE):
			newTask.arg = 'a';
			newTask.routineForTask = &produce;
			printf("LOG__ProducerTask created with: %c__\n", newTask.arg);
			putTaskInPool(producerPool, newTask);
			break;

		case (UPPERCASE):
			newTask.arg = 'A';
			newTask.routineForTask = &produce;
			printf("LOG__ProducerTask created with: %c__\n", newTask.arg);
			putTaskInPool(producerPool, newTask);
			break;

		case (NO_PRODUCTION):
			break;

		default:
			printf("Enexpected productionState: %d\n", productionState);
			break;

		}

		//Consumptiontasks
		if (consumerOn) {
			task_t newTask;
			newTask.arg = NULL;
			newTask.routineForTask = &consume;
			printf("LOG__ConsumerTask created.__\n");
			putTaskInPool(consumerPool, newTask);
		}

		char input = getcharTimeout(1);
		switch (input) {
		case ('p'):
			productionState++;
			productionState %= 3;
			printf("Changed Productionstate.\n");
			break;

		case ('c'):
			consumerOn = !consumerOn;
			if(consumerOn)
				printf("Consumer activated.\n");
			else
				printf("Consumer deactivated.\n");
			break;

		case ('q'):
			cancelThreadPool(nameProducer, producerPool);
			cancelThreadPool(nameConsumer, consumerPool);
			printf("LOG__ThreadPools are canceled__\n");
			printf("Program was closed!\n");
			pthread_exit(0);
			break;

		case ('h'):
			printf(helpText);
			break;
		}

	}
	return NULL;
}

void initCtrlThread(threadPool_t* producer, threadPool_t* consumer) {
	producerPool = producer;
	consumerPool = consumer;
	productionState = LOWERCASE;
	consumerOn = true;
	pthread_create(&ctrlThreadID, NULL, ctrlThread, NULL);
}

void joinCtrlThread() {
	pthread_join(ctrlThreadID, NULL);
}
