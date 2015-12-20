#ifndef QUEUE_H
#define QUEUE_H

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

int queue_get(const int key);
int queue_create(const int key);
void queue_remove(int id);

#endif //QUEUE_H