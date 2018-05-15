#include "getcharTimeout.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int  getcharTimeout(unsigned int timeout) {
    fd_set rfds;
    struct timeval tv;
    unsigned char c;

    /* Watch stdin (fd 0) to see when it has input. */
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);

    tv.tv_sec = (long) timeout;   // set timeout
    tv.tv_usec = 0;
    /* Don't rely on the value of tv after select has been finished */

    switch (select(1, &rfds, NULL, NULL, &tv)) {
	case -1: // Error
           perror("Error in function select in getchTimeout");
           exit(EXIT_FAILURE);
           break;
	case  0: // Timeout, keine Eingabe liegt vor.
	   return EOF;
           break;
	default: // Zeichen liegt vor
           read(0,&c,1); // muss read sein, das select auf auf fd arbeitet
           return (int) c; 
    }
}

