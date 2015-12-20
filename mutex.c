#include "mutex.h"
#include "helpers1.h"
#include "err.h"
#include "queue.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define MUTEX_BEGIN 1234

static int mutex_number = MUTEX_BEGIN;

struct empty_message {
	long mtype;
};

int mutex_create(void) {
	int mutex = queue_create(mutex_number++);
	mutex_release(mutex);
	return mutex;
}

void mutex_wait(int id) {
	static struct empty_message msg;
	TRY(msgrcv(id, &msg, sizeof(struct empty_message) - sizeof(long), 0, 0));
}

void mutex_release(int id) {
	static struct empty_message msg;
	msg.mtype = 1;
	TRY(msgsnd(id, &msg, sizeof(struct empty_message) - sizeof(long), 0));
}

void mutex_cleanup(void) {
	int cp =  mutex_number;
	mutex_number = MUTEX_BEGIN;
	for (int i = MUTEX_BEGIN; i < cp; i++) {
		queue_remove(i);
	}
}