#include <semaphore.h>
//makro zur fehleranalyse
#define printFehler(fehlerCode, funktion) if(fehlerCode) perror(funktion)
//maximale puffer groesse
#define PUFFER_SIZE 10

sem_t beschriebenePlaetze; // verhindert underflow
sem_t leerePlaetze; //kontrolliert puffer zugang //overflow

//prototypen
void fifo_init(void);
void fifo_push(char);
char fifo_pop(void);
int fifo_getLength();
void fifo_rauemeAuf(void);
