#include "fifo.h"
#include <stdio.h>

/*
 * Globale Variablen
 */
static char puffer[PUFFER_SIZE];
static int pufferPushZeiger;
static int pufferPopZeiger;
static int pufferLength;

/*
 * Funktion zum initialisieren des Fifo Puffers
 */
void fifo_init(){
	int i;
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
	puffer[pufferPushZeiger] = c;
	printf("Buchstabe in den Puffer geschrieben: %c\n" , c);
	pufferPushZeiger++;
	if(pufferPushZeiger == PUFFER_SIZE)
	{
		pufferPushZeiger = 0;
	}
	pufferLength++;
    printf("Aktuelle Anzahl Elemente: %d\n\n", pufferLength);
}

/*
 * Pop Funktion zum dekrementieren des FIFO Puffers.
 */
char fifo_pop(){
	char popChar = puffer[pufferPopZeiger];
    printf("Buchstabe aus dem Puffer geschrieben: %c\n" , popChar);
	puffer[pufferPopZeiger] = '_';
	pufferPopZeiger++;
	if(pufferPopZeiger == PUFFER_SIZE)
	{
		pufferPopZeiger = 0;
	}
	pufferLength--;
    printf("Aktuelle Anzahl Elemente: %d\n\n", pufferLength);
	return popChar;
}

/**
 * Getter für Puffergröße
 */
int fifo_getLength(){
	return pufferLength;
}


