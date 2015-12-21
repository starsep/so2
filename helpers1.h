//Funkcje pomocnicze1
#ifndef HELPERS1_H
#define HELPERS1_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include "err.h"

int min(int a, int b);

int max(int a, int b);

//wrapper na malloc
void *err_malloc(size_t);

void make_signal_handlers(void(*signal_handler)(int));

#endif //HELPERS1_H