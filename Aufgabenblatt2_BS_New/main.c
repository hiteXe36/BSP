#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "fifo.h"
#include <signal.h>

//#define SEMAPHORE 1

//Konstanten
#define ALPHABET_LAENGE 26
#define SLEEP_CONSUMER 2
#define SLEEP_PRODUCER 3

//Mutex für den Zugriff auf den FIFO Puffer
static pthread_mutex_t fifo_mutex = PTHREAD_MUTEX_INITIALIZER;

//Mutex Variablen für Producer und Consumer Threads
static pthread_mutex_t producer1_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t producer2_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t consumer_mutex = PTHREAD_MUTEX_INITIALIZER;

static int producer1_isRunning;
static int producer2_isRunning;
static int consumer1_isRunning;

//threadvariablen
pthread_t producer_1_t;
pthread_t producer_2_t;
pthread_t consumer_t;
pthread_t control_t;

//wenn semaphore an dann:
#ifdef SEMAPHORE
sem_t puffer_input;
sem_t puffer_output;
#else
//conditional variables
//cv fürs produzieren
pthread_cond_t cond_p = PTHREAD_COND_INITIALIZER;
//cv fürs consumen
pthread_cond_t cond_c = PTHREAD_COND_INITIALIZER;
#endif

/*
 * Funktion die alle 3 Sekunden einen char in den Puffer schreibt
 */
void produce(char*buchstaben, pthread_mutex_t* producer_mutex, int *producer_isRunning){
	//Zeiger auf den aktuellen Buchstaben
	int charZeiger = 0;

	while(*producer_isRunning){
		pthread_mutex_lock(producer_mutex);
		pthread_mutex_unlock(producer_mutex);
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);//for die if
#ifdef SEMAPHORE
			sem_wait(&puffer_input); //check ob der puffer frei ist
			//beginn kritischer Abschnitt
			pthread_mutex_lock(&fifo_mutex);
			fifo_push(buchstaben[charZeiger]);
			pthread_mutex_unlock(&fifo_mutex);
			sem_post(&puffer_output);
#else
			//beginn kritischer Abschnitt
			pthread_mutex_lock(&fifo_mutex);
			while(fifo_getLength() == PUFFER_SIZE) pthread_cond_wait(&cond_p, &fifo_mutex);
			fifo_push(buchstaben[charZeiger]);
			pthread_cond_signal(&cond_c);
			pthread_mutex_unlock(&fifo_mutex);
#endif
            pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    		sleep(SLEEP_PRODUCER);
			charZeiger++;
			if(charZeiger == ALPHABET_LAENGE)
			{
				charZeiger = 0;
			}
	}

}

void *producer1_function(void *notUsed){
	char* alphabetKlein = "abcdefghijklmopqrstuvwxyz";
	produce(alphabetKlein, &producer1_mutex, &producer1_isRunning);
	return NULL;
}

void *producer2_function(void *notUsed){
	char* alphabetGross = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	produce(alphabetGross, &producer2_mutex, &producer2_isRunning);
	return NULL;
}

//fertig
void *consumer_function(void *a){
    printf("Consumer gestartet...\n");
	while(1){
		pthread_mutex_lock(&consumer_mutex);
		pthread_mutex_unlock(&consumer_mutex);
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
#ifdef SEMAPHORE
		sem_wait(&puffer_output);//check ob der puffer leer ist
		pthread_mutex_lock(&fifo_mutex);
		fifo_pop();
		pthread_mutex_unlock(&fifo_mutex);
		sem_post(&puffer_input);
#else
		pthread_mutex_lock(&fifo_mutex);
		while(fifo_getLength() == 0) pthread_cond_wait(&cond_c, &fifo_mutex);
		fifo_pop();
		pthread_cond_signal(&cond_p); //vor unlock gesetzt
		pthread_mutex_unlock(&fifo_mutex);
#endif
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        sleep(SLEEP_CONSUMER);
	}
	return NULL;
}

void *control_Function(void *notUsed){

	char eingabe = 0;
	int producer1_blocked = 0;
	int producer2_blocked = 0;
	int consumer_blocked = 0;

	while(1){

        eingabe = getchar();

		switch(eingabe){

		case '1':
			if(producer1_blocked == 0){
				pthread_mutex_lock(&producer1_mutex);
                printf("Producer 1 blockiert...\n");
				producer1_blocked = 1;
			}else{
				pthread_mutex_unlock(&producer1_mutex);
                printf("Producer 1 gestartet...\n");
				producer1_blocked = 0;
			}
			break;

		case '2':
			if(producer2_blocked == 0){
				pthread_mutex_lock(&producer2_mutex);
                printf("Producer 2 blockiert...\n");
				producer2_blocked = 1;
			}else{
				pthread_mutex_unlock(&producer2_mutex);
                printf("Producer 1 gestartet...\n");
				producer2_blocked = 0;
			}
			break;

		case 'c' : case 'C':
			if(consumer_blocked == 0){
				pthread_mutex_lock(&consumer_mutex);
                printf("Consumer blockiert...\n");
				consumer_blocked = 1;
			}else {
				pthread_mutex_unlock(&consumer_mutex);
                printf("Consumer gestartet...\n");
				consumer_blocked = 0;
			}
			break;

            case 'q' : case 'Q':
			if(consumer_blocked){
				pthread_mutex_unlock(&consumer_mutex);
			}

			if(producer1_blocked){
				pthread_mutex_unlock(&producer1_mutex);
			}

			if(producer2_blocked){
				pthread_mutex_unlock(&producer2_mutex);
			}
			pthread_mutex_unlock(&fifo_mutex);

			pthread_cancel(producer_1_t);
            pthread_cancel(producer_2_t);
            pthread_cancel(consumer_t);
			pthread_exit(0);

			break;
		}

	}
}

int main(){
	printf("Das Programm wurde gestartet...\n");

#ifdef SEMAPHORE
	printf("SEMAPHORE VARIANTE!\n");
#else
	printf("CV VARIANTE!\n");
#endif
        fifo_init();

		producer1_isRunning = 1;
		producer2_isRunning = 1;
		consumer1_isRunning = 1;

        #ifdef SEMAPHORE
			sem_init(&puffer_input, 0, PUFFER_SIZE);
			sem_init(&puffer_output, 0, 0);
		#endif

        pthread_create(&control_t, NULL, control_Function, NULL);
        pthread_create(&producer_1_t, NULL, producer1_function, NULL);
        pthread_create(&producer_2_t, NULL, producer2_function, NULL);
        pthread_create(&consumer_t, NULL, consumer_function, NULL);

        pthread_join(control_t, NULL);
        pthread_join(producer_1_t, NULL);
        pthread_join(producer_2_t, NULL);
        pthread_join(consumer_t, NULL);


		#ifdef SEMAPHORE
			sem_destroy(&puffer_input);
			sem_destroy(&puffer_output);
		#else
			pthread_cond_destroy(&cond_c);
			pthread_cond_destroy(&cond_p);
		#endif

		pthread_mutex_destroy(&producer1_mutex);
		pthread_mutex_destroy(&producer2_mutex);
		pthread_mutex_destroy(&consumer_mutex);
		pthread_mutex_destroy(&fifo_mutex);

		printf("\nDas Program wurde beendet!");


		return 0;
	}

