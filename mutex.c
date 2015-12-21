//Filip Czaplicki 359081
#include "mutex.h"
#include "helpers1.h"
#include "err.h"
#include "queue.h"
#include <sys/types.h>
#include <sys/ipc.h>

static int mutex_number = 0;
static pthread_mutex_t *mutex = NULL;
static int reserved = 0;

int mutex_create(void) {
	int result = mutex_number++;
	if (mutex == NULL) {
		reserved = 1;
		mutex = (pthread_mutex_t *) err_malloc(sizeof(pthread_mutex_t) * reserved);
	}
	else {
		reserved *= 2;
		mutex = realloc(mutex, sizeof(pthread_mutex_t) * reserved);
		if (mutex == NULL) {
			fatal("realloc");
		}
	}
	if ((pthread_mutex_init(&mutex[result], NULL) != 0)) {
		fatal("pthread_mutex_init");
	}
	return result;
}

void mutex_wait(int id) {
	if (pthread_mutex_lock(&mutex[id]) != 0) {
		fatal("pthread_mutex_lock");
	}
}

void mutex_release(int id) {
	if (pthread_mutex_unlock(&mutex[id]) != 0) {
		fatal("pthread_mutex_unlock");
	}
}

void mutex_cleanup(void) {
	int cp = mutex_number;
	mutex_number = 0;
	for (int i = 0; i < cp; i++) {
		queue_remove(i);
	}
}