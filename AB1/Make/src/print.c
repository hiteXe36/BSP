// file print.c

#include "print.h"
#include <stdio.h>

void printIt(char *s) {
   printf("%s\n", s);
   fflush(stdout);
}

