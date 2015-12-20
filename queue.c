#include "queue.h"

int queue_get(const int key) {
	int id = msgget(key, 0644);
	if (id == -1) {
		fatal("queue_get");
	}
	return id;
}

int queue_create(const int key) {
	int id = msgget(key, 0644);
	if (id != -1) {
		queue_remove(id);
	}
	id = msgget(key, 0644 | IPC_CREAT | IPC_EXCL);
	if (id == -1) {
		fatal("queue_create");
	}
	return id;
}

void queue_remove(const int id) {
	TRY(msgctl(id, IPC_RMID, NULL));
}