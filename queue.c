#include "queue.h"

void queue_get(int *id, const int key) {
	*id = msgget(key, 0644);
	if (*id == -1) {
		fatal("queue_get");
	}
}

void queue_create(int *id, const int key) {
	*id = msgget(key, 0644);
	if (*id != -1) {
		queue_remove(*id);
	}
	*id = msgget(key, 0644 | IPC_CREAT | IPC_EXCL);
	if (*id == -1) {
		fatal("queue_create");
	}
}

void queue_remove(const int id) {
	TRY(msgctl(id, IPC_RMID, NULL));
}