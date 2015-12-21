//Filip Czaplicki 359081
#include "queue.h"

static const int PERMISSIONS = 0644;

int queue_get(const int key) {
	int id = msgget(key, PERMISSIONS);
	if (id == INVALID) {
		fatal("queue_get");
	}
	return id;
}

void queue_create(const int key) {
	int id = msgget(key, PERMISSIONS);
	if (id != INVALID) {
		queue_remove(id);
	}
	id = msgget(key, PERMISSIONS | IPC_CREAT | IPC_EXCL);
	if (id == INVALID) {
		fatal("queue_create");
	}
}

void queue_remove(const int id) {
	TRY(msgctl(id, IPC_RMID, NULL));
}