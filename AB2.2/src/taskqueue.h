/********************************************************************
 * @file    taskqueue.h
 * @author  Franz Korf 
 *          HAW-Hamburg
 *          Berliner Tor  7
 *          D-20099 Hamburg
 * @version V0.5
 * @date    Apr 2018
 * @brief   Dieses C Modul implementiert die Task Queue für einen 
 *          Thread Pool in Rahmen der Praktikumsaufgabe 2 der 
 *          Betriebssystemvorlesung in der TI. 
 *          Die Queue ist thread-safe und als FIFO mit Prioritäten
 *          implementiert. Sie basiert auf Message Queues 
 *          (s. man mq_overview) unter LINUX.
 *          Im /proc Filesystem werden die maximale Größe einer 
 *          Message Queues und eines Queue Element eingestellt
 *          (z.B. /proc/sys/fs/mqueue/msg_max).
 *                         
 ******************************************************************
 */

#ifndef _TASKQUEUE_H
#define _TASKQUEUE_H
#include <mqueue.h>
#include <stdbool.h>

/*
 * @brief Datenstruktur zur Beschreibung eines Auftrags / einer Task
 */
typedef struct task {
   /// @brief Ein Zeiger auf die Funktion, die die Task implementiert.
   void (*routineForTask)(void *arg); 
   /// @brief Der aktuelle Parameter für den Aufruf von routineForTask.
   void *arg;	
} task_t;

/**
 * @brief Diese Funktion erzeugt eine Task Queue
 * @param name Der systemweit bekannte Name der Task Queue. 
 *             Er muss ein Bezeichner, der mit einem "/" beginnt, sein.
 * @param size Maximale Anzahl der Elemente, die die Queue speichern kann.
 * @return     Der Diskriptor auf die erzeugte Queue. Im Fehlerfall wird 
 *             -1 zurück gegen und errno enthält eine genauere Beschreibung
 *             des Fehlers gemäss der Funktion mq_create.
 */
extern mqd_t createTaskQueue(const char * name, const unsigned int size);

/**
 * @brief Diese Funktion schließt eine Task Queue. Falls noch Elemente
 *        in der Queue sind, bleiben diese erhalten.
 * @param mqdes Der Diskritor der Task Queue.
 */
extern void closeTaskQueue(const mqd_t mqdes);

/**
 * @brief Diese Funktion zerstört eine Task Queue und gibt die Ressourcen
 *        dem Betriebssystem zurück. Falls die Task Queue noch geöffnet ist, 
 *        wird sie nach dem letzten closeTaskQueue zerstört.
 * @param name Der systemweit bekannte Name der Task Queue. 
 */
extern void destroyTaskQueue(const char * name);

/**
 * @brief Diese Funktion legt eine Task in einer Task Queue ab.
 * @param mqdes     Der Diskritor der Task Queue.
 * @param t         Die Task, die in die Queue eingefügt wird.
 * @param prio      Die Priorität von t. Es werden die Prioritäten von 
 *                  Message Queues unter LINUX verwendet.
 * @param blocking  Dieser Parameter legt fest, ob der Funktionsaufruf im 
 *                  Fall einer vollen Task Queue blockiert. 
 * @return          Der return ist 0, wenn die Task eingefügt werdeYn konnte.
 *                  Ist die Queue voll und blocking == true, dann wird die 
 *                  Task nicht eingefügt und eine 1 zurück gegeben.
 *                  Im Fehlerfall wird -1 zurück gegen und errno enthält 
 *                  eine Beschreibung des Fehlers gemäss der Funktion mq_send.
 */
extern int sendToTaskQueue(const mqd_t mqdes, const struct task t, const unsigned int prio, bool blocking);

/**
 * @brief Diese Funktion liest eine Task aus einer Task Queue.
 *        Ist die Task Queue leer, blockiert die Funktion bis ein 
 *        Element eingefügt wurde und somit ausgelesen werden kann. 
 * @param mqdes Der Diskritor der Task Queue.
 * @return      Die Task, die ausgelesen wurde.
 */
extern struct task receiveFromTaskQueue(const mqd_t mqdes);

#endif
//EOF

