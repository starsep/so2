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
int COLLECTION_QUEUE_ID;

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
	BANK_ANSWERS = queue_create(BANK_ANSWERS_KEY);
	BANK_REQUESTS = queue_create(BANK_REQUESTS_KEY);
	COLLECTION_QUEUE_ID = queue_create(COLLECTION_QUEUE_KEY);
}

void cleanup(void) {
	queue_remove(BANK_ANSWERS);
	queue_remove(BANK_REQUESTS);
	queue_remove(COLLECTION_QUEUE_ID);
	mutex_cleanup();
}

void work(void) {
	while (true) {
		sleep(1);
		//printf("Museum: I'm still alive\n");
	}
}

void signal_handler(int sig) {
	switch(sig) {
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

void make_signal_handlers(void) {
	struct sigaction setup_action;
	sigset_t block_mask;

	sigemptyset (&block_mask);
	setup_action.sa_handler = signal_handler;
	setup_action.sa_mask = block_mask;
	setup_action.sa_flags = 0;
	TRY(sigaction (SIGINT, &setup_action, NULL));
	TRY(sigaction (SIGUSR1, &setup_action, NULL));
	TRY(sigaction (SIGUSR2, &setup_action, NULL));

	sigaddset(&block_mask, SIGINT);
	TRY(sigprocmask(SIG_BLOCK, &block_mask, NULL));

	TRY(sigprocmask(SIG_UNBLOCK, &block_mask, NULL));
}

int main(int argc, char **argv) {
	get_arguments(argc, argv);
	make_queues();
	alloc_data();
	get_input();
	make_signal_handlers();
	work();
	cleanup();
}