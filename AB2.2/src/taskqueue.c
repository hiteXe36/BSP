/*
 ********************************************************************
 * @file    taskqueue.c
 * @author  Franz Korf 
 *          HAW-Hamburg
 *          Berliner Tor  7
 *          D-20099 Hamburg
 * @version V0.5
 * @date    Apr 2018
 * @brief   This C Module implements a task queue for a thread pool
 *          BS TI Aufgabe 2
 *******************************************************************
 */

#include "taskqueue.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>

/// Größe eines Queue Elements
#define MSG_SIZE            (sizeof(struct task))
#define PROC_FS_MSG_MAX     "/proc/sys/fs/mqueue/msg_max"
#define PROC_FS_MSGSIZE_MAX "/proc/sys/fs/mqueue/msgsize_max"

/**
 * @brief Diese Funktion liest einen int Wert aus einer Datei. 
 * @param file  Name der Datei, die gelesen werden soll.
 * @param val   Zeiger zur Rückgabe des gelesenen int Werts.
 * @return      -1 im Fehlerfall. errno wird gesetzt. Sonst 0.
 */

static int readIntFromFile(char *file, int *val) {
   FILE *fd = fopen(file, "r");
   bool err = false;
   if (fd == NULL) {
      perror("Cannot open file in readIntFromFile");
      return -1;
   }
   if (EOF == fscanf(fd,"%d",val)){
      perror("Cannot read file in readIntFromFile");
      err = true;
   }
   if (EOF == fclose(fd)) {
      perror("Cannot close file of readIntFromFile");
      err = true;
   }
   // printf("%s = %d\n", file, *val);
   return (err) ? -1 : 0;
}

int sendToTaskQueue(const mqd_t mqdes, const struct task task, 
                    const unsigned int prio, const bool blocking) {
   const struct timespec timeout = {0,0};
   int erg;
   if (blocking) {
      erg = mq_send(mqdes, (char *) &task, MSG_SIZE, prio);
   } else {
      erg = mq_timedsend(mqdes, (char *) &task, MSG_SIZE, prio, &timeout);
   }
   if ((erg == -1) && (!blocking) && (errno == ETIMEDOUT)) {
      return 1;
   }
   if (erg == -1) {
      perror("Error in sendToTaskQueue");
   }
   return erg;
}

mqd_t createTaskQueue(const char *name, const unsigned int size){
   struct mq_attr attr;

   // Set up the attribute structure
   attr.mq_maxmsg = size;
   attr.mq_msgsize = MSG_SIZE;
   attr.mq_flags = 0;

   // check parameter of message queue
   int x = 0;
   readIntFromFile(PROC_FS_MSG_MAX, &x);
   if (x < size) {
      printf("Set maximal msg queue size to %d or higher (s. %s) \n", size, PROC_FS_MSG_MAX);
      exit(EXIT_FAILURE);
   }

   readIntFromFile(PROC_FS_MSGSIZE_MAX, &x);
   if (x < MSG_SIZE) {
      printf("Set maximal size of msg queue element to %d or higher (s. %s) \n", (int) MSG_SIZE, PROC_FS_MSGSIZE_MAX);
      exit(EXIT_FAILURE);
   }

   // Set access mode to read & write for owner & group; to read for others
   mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;

   mqd_t mqdes = mq_open (name, O_RDWR | O_CREAT, mode, &attr);

   if (mqdes == -1) {
      perror("Error in createTaskQueue");
   }
   return mqdes;
}

struct task receiveFromTaskQueue(const mqd_t mqdes) {
   unsigned int prio;
   struct task task = {NULL, NULL};

   if (-1 == mq_receive(mqdes, (char *) &task, MSG_SIZE, &prio)) {
      perror("Error in receiveFromTaskQueue");
   }
   return task;
}

void closeTaskQueue(const mqd_t mqdes) {
   if (-1 == mq_close(mqdes)) {
      perror("Error in closeTaskQueue");
   }
}

void destroyTaskQueue(const char * name){
   if ((-1 == mq_unlink(name)) && (errno != ENOENT)) {
      perror("Error in destroyTaskQueue");
   }
}

// EOF

