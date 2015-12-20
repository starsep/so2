#include "mutex.h"
#include "helpers1.h"
#include "err.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

static int mutex_number = 1234;

int mutex_create(void) {
	int mutex = semget(mutex_number++, 1, 0644 | IPC_CREAT);
	if (mutex == -1) {
		fatal("semget");
	}
	mutex_release(mutex);
	return mutex;
}

void mutex_wait(int id) {
	struct sembuf *wait_op = (struct sembuf *) err_malloc(sizeof(struct sembuf));
	wait_op->sem_num = 0;
	wait_op->sem_op = -1;
	wait_op->sem_flg = 0;
	if (semop(id, wait_op, 1) == -1) {
		fatal("semop(wait)");
	}
}

void mutex_release(int id) {
	struct sembuf *release_op = (struct sembuf *) err_malloc(sizeof(struct sembuf));
	release_op->sem_num = 0;
	release_op->sem_op = 1;
	release_op->sem_flg = 0;
	if (semop(id, release_op, 1) == -1) {
		fatal("semop(release)");
	}
}