#include "fifo.h"
#include <stdio.h>
#include <pthread.h>

//Globale Variablen
static char puffer[PUFFER_SIZE];
static int pufferPushZeiger;
static int pufferPopZeiger;
static int length;

pthread_mutex_t mutex;

//Funktion zum initialisieren des FIFO-Puffers
void fifo_init(void)
{
  printFehler(sem_init(&leerePlaetze, 0, PUFFER_SIZE),
   "Fehler beim initialisieren von Semaphore leerePlaetze"); //leere plaetze mit 10 initialisieren
  printFehler(sem_init(&beschriebenePlaetze, 0, 0),
   "Fehler beim initialisieren von Semaphore beschriebenePlaetze");
  int i;
  for(i = 0; i < PUFFER_SIZE; i++)
  {
    puffer[i] = '_';
  }
  pufferPushZeiger = 0;
  pufferPopZeiger = 0;
  length = 0;
}

//einfuegen in der puffer
void fifo_push(char neuerChar){
    
  sem_wait(&leerePlaetze); //-- Atomare Funktion: Ist das Semaphore größer als 0, reduziert sem_wait das semaphore um 1. Ansonsten wartet es.
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
 
  pthread_mutex_lock(&mutex);

  puffer[pufferPushZeiger] = neuerChar;
  pufferPushZeiger++;
  if(pufferPushZeiger == PUFFER_SIZE){
    pufferPushZeiger = 0;
  }
  length++;
  printf("Buchstabe in Puffer geschrieben: %c\n", neuerChar);
  printf("Aktuelle Anzahl Elemente = %d\n\n", length);

  pthread_mutex_unlock(&mutex);
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  sem_post(&beschriebenePlaetze); //++ Atomare Funktion erhöht das Semaphore um 1.
}

char fifo_pop(void){
    
  sem_wait(&beschriebenePlaetze); //--
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
  
  pthread_mutex_lock(&mutex);

  char popChar = puffer[pufferPopZeiger];
  pufferPopZeiger++;
  if(pufferPopZeiger == PUFFER_SIZE){
    pufferPopZeiger = 0;
  }
  length--;
  
  printf("Buchstabe aus Puffer gelesen: %c\n" , popChar);
  printf("Aktuelle Anzahl Elemente = %d\n\n", length);
  
  pthread_mutex_unlock(&mutex);

  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  sem_post(&leerePlaetze); //++
  return popChar;
}

int fifo_getLength(void){
	return length;
}

void fifo_rauemeAuf(void)
{
  printFehler(sem_destroy(&leerePlaetze), "Fehler beim zerstoeren von Semaphore leerePlaetze");
  printFehler(sem_destroy(&beschriebenePlaetze), "Fehler beim zerstoeren von Semaphore beschriebenePlaetze");
  printFehler(pthread_mutex_destroy(&mutex), "Fehler beim zerstoeren des Mutexes");
}
