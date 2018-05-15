 /*
 * FIFO Header
 */

#ifndef FIFO_H_
#define FIFO_H_

#define PUFFER_SIZE 10

/*
 * Prototypen
 */
void fifo_init(void);
void fifo_push(char);
char fifo_pop(void);
int fifo_getLength(void);


#endif /* FIFO_H_ */
