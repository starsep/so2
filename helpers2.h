//Funkcje pomocnicze2
#ifndef HELPERS2_H
#define HELPERS2_H
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "err.h"

//z inta robi char *
char *itoa(int x);

//czyści całą pamięć zarezerwowaną przez funkcje
void helpers_cleanup(void);

#endif //HELPERS2_H