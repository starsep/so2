#include "museum_password.h"
#include "helpers.h"
#include "messages.h"
#include "mutex.h"
#include "queue.h"

const int ARTEFACT_MULTIPLIER = 10;

const int length; //długość
const int depth; //głębokość
const int S; //stała opłata
const int A; //ograniczenie artefaktów

int BANK_REQUESTS;
int BANK_ANSWERS;
int BANK_MUSEUM;
int MUSEUM_REQUESTS;
int MUSEUM_ANSWERS;

int **artefacts; //tablica Teren[][]
int **estimate; //tablica Szacunek[][]

void get_arguments(int argc, char **argv) {
	if (argc != 5) {
		fprintf(stderr, "Bad number of arguments\n");
		fprintf(stderr, "Usage:\n");
		fprintf(stderr, "%s Dlugosc Glebokosc S A\n", argv[0]);
		exit(1);
	}
	*((int *) &length) = atoi(argv[1]);
	*((int *) &depth) = atoi(argv[2]);
	*((int *) &S) = atoi(argv[3]);
	*((int *) &A) = atoi(argv[4]);
}

void alloc_data(void) {
	artefacts = (int **) err_malloc(sizeof(int *) * length);
	for (int i = 0; i < length; i++) {
		artefacts[i] = (int *) err_malloc(sizeof(int) * depth);
	}
	estimate = (int **) err_malloc(sizeof(int *) * length);
	for (int i = 0; i < length; i++) {
		estimate[i] = (int *) err_malloc(sizeof(int) * depth);
	}
}

void get_input(void) {
	for (int i = 0; i < length; i++) {
		for (int j = 0; j < depth; j++) {
			if (scanf("%d", &artefacts[i][j]) < 1) {
				fatal("scanf: not enough data");
			}
			artefacts[i][j] = max(artefacts[i][j], 1);
		}
	}
	for (int i = 0; i < length; i++) {
		for (int j = 0; j < depth; j++) {
			if (scanf("%d", &estimate[i][j]) < 1) {
				fatal("scanf: not enough data");
			}
		}
	}
}

void make_queues(void) {
	for (int i = QUEUE_KEYS_BEGIN + 1; i < QUEUE_KEYS_END; i++) {
		queue_create(i);
	}
	BANK_ANSWERS = queue_get(BANK_ANSWERS_KEY);
	BANK_REQUESTS = queue_get(BANK_REQUESTS_KEY);
	BANK_MUSEUM = queue_get(BANK_MUSEUM_KEY);
	MUSEUM_ANSWERS = queue_get(MUSEUM_ANSWERS_KEY);
	MUSEUM_REQUESTS = queue_get(MUSEUM_REQUESTS_KEY);
}

void cleanup(void) {
	for (int i = QUEUE_KEYS_BEGIN + 1; i < QUEUE_KEYS_END; i++) {
		queue_remove(queue_get(i));
	}
	mutex_cleanup();
	for (int i = 0; i < length; i++) {
		free(artefacts[i]);
	}
	free(artefacts);
	for (int i = 0; i < length; i++) {
		free(estimate[i]);
	}
	free(estimate);
}

void send_collection(const struct museum_request *msg) {
	struct bank_request bank_rqst;
	//memset(bank_rqst, 0, sizeof(struct bank_request));
	bank_rqst.mtype = TRANSFER;
	bank_rqst.id = msg->id;
	bank_rqst.change = ARTEFACT_MULTIPLIER * msg->p;
	bank_rqst.password = MUSEUM_PASSWORD;
	TRY(msgsnd(BANK_REQUESTS, &bank_rqst, sizeof(struct bank_request) - sizeof(long), 0));

	struct account_balance confirmation;
	TRY(msgrcv(BANK_MUSEUM, &confirmation, sizeof(struct account_balance) - sizeof(long), 0, 0));
}

void ask_estimate(const struct museum_request *msg) {
 	struct estimate_message est_msg;
	est_msg.mtype = msg->id;
	for (int i = msg->l - 1; i < msg->p; i++) {
		for (int j = 0; j < msg->g; j++) {
			est_msg.estimate = estimate[i][j];
			TRY(msgsnd(MUSEUM_ANSWERS, &est_msg, sizeof(struct estimate_message) - sizeof(long), 0));
		}
	}

}

void work(void) {
	while (true) {
		struct museum_request msg;
		TRY(msgrcv(MUSEUM_REQUESTS, &msg, sizeof(struct museum_request) - sizeof(long), 0, 0));
		switch (msg.mtype) {
			case SEND_COLLECTION:
				send_collection(&msg);
				break;
			case ASK_ESTIMATE:
				ask_estimate(&msg);
				break;
		}
	}
}

void signal_handler(int sig) {
	switch (sig) {
		case SIGINT:
			cleanup();
			exit(0);
			break;
		case SIGUSR1:
			break;
		case SIGUSR2:
			break;
	}
}

int main(int argc, char **argv) {
	get_arguments(argc, argv);
	make_signal_handlers(signal_handler);
	make_queues();
	alloc_data();
	get_input();
	work();
	cleanup();
}