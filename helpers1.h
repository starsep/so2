//Funkcje pomocnicze1
#ifndef HELPERS1_H
#define HELPERS1_H
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "err.h"

int min(int a, int b);

int max(int a, int b);

//wrapper na malloc
void *err_malloc(size_t);

#endif //HELPERS1_H