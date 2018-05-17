#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "fifo.h"
#include <signal.h>
#include <errno.h>



/*
 * Globale Variablen
 */
static char puffer[PUFFER_SIZE];
static int pufferPushZeiger;
static int pufferPopZeiger;
static int pufferLength;

//Mutex für den Zugriff auf den FIFO Puffer
static pthread_mutex_t fifo_mutex = PTHREAD_MUTEX_INITIALIZER;

static sem_t puffer_input; //Groesse 10; schuetzt overflow
static sem_t puffer_output; //Schuetzt underflow

/*
 * Funktion zum initialisieren des Fifo Puffers
 */
void fifo_init(){
	int i;
    
    //initialisieren der Semaphore
    sem_init(&puffer_input, 0, PUFFER_SIZE);
    sem_init(&puffer_output, 0, 0);
    
	for(i = 0; i < PUFFER_SIZE; i++)
	{
		puffer[i] = '_';
	}
	pufferPushZeiger = 0;
	pufferPopZeiger = 0;
	pufferLength = 0;
}

/*
 * Funktionen zum Pushen auf den Fifo Puffer
 */
void fifo_push(char c){
    //Achtung printf basiert auf der write() Funktion und ist ein möglicher Cancelationpoint
    printf("Buchstabe in den Puffer geschrieben: %c\n" , c);
    //Check ob der Puffer frei ist + Achtung sem_wait ist Cancelationpoint
    sem_wait(&puffer_input); // Atomare Funktion: Ist das Semaphore größer als 0, reduziert sem_wait es um 1. Ansonsten wird blockiert. 
    //diesen Abschnitt nicht cancelbar machen
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    //Beginn kritischer Abschnitt
    pthread_mutex_lock(&fifo_mutex);
	puffer[pufferPushZeiger] = c;
	pufferPushZeiger++;
	if(pufferPushZeiger == PUFFER_SIZE)
	{
		pufferPushZeiger = 0;
	}
	pufferLength++;
	pthread_mutex_unlock(&fifo_mutex);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL); //nach die else if
    //Info: sem_post ist kein Cancelationpoint
    sem_post(&puffer_output); //inkrementieren des Semaphore um dem Consumer das konsumieren zu ermöglichen
    printf("Aktuelle Anzahl Elemente: %d\n\n", pufferLength);
}

/*
 * Pop Funktion zum dekrementieren des FIFO Puffers.
 */
char fifo_pop(){
    //Diesen Abschnitt nicht Cancelbar machen. 
    sem_wait(&puffer_output);//check ob der puffer leer ist
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    //Beginn kritischer Abschnitt
    pthread_mutex_lock(&fifo_mutex);
	char popChar = puffer[pufferPopZeiger];
	puffer[pufferPopZeiger] = '_';
	pufferPopZeiger++;
	if(pufferPopZeiger == PUFFER_SIZE)
	{
		pufferPopZeiger = 0;
	}
	pufferLength--;
    //Ende kritischer Abschnitt
    pthread_mutex_unlock(&fifo_mutex);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    sem_post(&puffer_input); 
    printf("Buchstabe aus dem Puffer gelesen: %c\n" , popChar);
    printf("Aktuelle Anzahl Elemente: %d\n\n", pufferLength);
	return popChar;
}

/**
 * Getter für Puffergröße
 */
int fifo_getLength(){
	return pufferLength;
}

/*
 * Aufräumfunktion
 */
void fifo_cleanUp(void){
    printFehler(sem_destroy(&puffer_input), "Fehler beim zerstoeren des Input-Semaphores.");
    printFehler(sem_destroy(&puffer_output), "Fehler beim zerstoeren des Output-Semaphores.");
    printFehler(pthread_mutex_destroy(&fifo_mutex), "Fehler beim zerstoeren des Fifo-Mutex.");
}


 
