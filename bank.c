#include "helpers.h"
#include "museum_password.h"
#include "messages.h"
#include "queue.h"

const int F; //liczba firm
const int S; //stała opłata
const int A; //ograniczenie artefaktów

int *id; //id firmy
int *balance; //stany kont firm
int *workers; //liczby pracowników firm
int *password; //hasła do kont

int BANK_REQUESTS;
int BANK_ANSWERS;

void get_arguments(int argc, char **argv) {
	if (argc != 4) {
		fprintf(stderr, "Bad number of arguments\n");
		fprintf(stderr, "Usage:\n");
		fprintf(stderr, "%s F S A\n", argv[0]);
		exit(1);
	}
	*((int *) &F) = atoi(argv[1]);
	*((int *) &S) = atoi(argv[2]);
	*((int *) &A) = atoi(argv[3]);
}

void get_data(void) {
	id = (int *) err_malloc(sizeof(int) * F);
	balance = (int *) err_malloc(sizeof(int) * F);
	workers = (int *) err_malloc(sizeof(int) * F);
	for (int i = 0; i < F; i++) {
		if (scanf("%d%d%d", &id[i], &balance[i], &workers[i]) < 3) {
			fatal("scanf: not enough data");
		}
	}
}

void make_passwords(void) {
	srand(time(NULL));
	password = (int *) err_malloc(sizeof(int) * F);
	for (int i = 0; i < F; i++) {
		password[i] = rand() % RAND_MAX;
	}
}

void exec_companies(void) {
	for (int i = 0; i < F; i++) {
		pid_t pid = fork();
		if (pid < 0) {
			fatal("fork");
		}
		if (pid != 0) {
			execl("./firma", "./firma", itoa(id[i]), itoa(balance[i]), itoa(workers[i]), itoa(S), itoa(A),
				itoa(password[i]), NULL);
			fatal("execl");
		}
	}
}


void cleanup(void) {
	free(id);
	free(balance);
	helpers_cleanup();
}

int find_firm(const int ID) {
	for (int i = 0; i < F; i++) {
		if (id[i] == ID) {
			return i;
		}
	}
	fatal("find_firm");
	return -1;
}

void work(void) {
	while (true) {
		struct bank_request *msg = (struct bank_request *) err_malloc(sizeof(struct bank_request));
		TRY(msgrcv(BANK_REQUESTS, msg, sizeof(struct bank_request) - sizeof(long), 0, 0));
		//printf("WIADOMOSC TYPE: %d ID: %d PASSWORD: %d\n", (int)msg->mtype, msg->id, msg->password);
		int firm = find_firm(msg->id);
		if (msg->change > 0 && msg->password == MUSEUM_PASSWORD) {
			balance[firm] += msg->change;
		}
		else if (msg->change < 0 && msg->password == password[firm]) {
			if (balance[firm] + msg->change < 0) {
				puts("Nie ma tylu środków");
			}
			else {
				balance[firm] += msg->change;
			}
		}
		if (msg->change <= 0 && msg->password == password[firm]) {
			struct account_balance *rsp = (struct account_balance *) err_malloc(sizeof(struct account_balance));
			rsp->mtype = msg->id;
			rsp->balance = balance[firm];
			TRY(msgsnd(BANK_ANSWERS, rsp, sizeof(struct account_balance) - sizeof(long), 0));
		}
		//printf("Bank: I'm still alive\n");
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

void make_signal_handlers(void) {
	struct sigaction setup_action;
	sigset_t block_mask;

	sigemptyset(&block_mask);
	setup_action.sa_handler = signal_handler;
	setup_action.sa_mask = block_mask;
	setup_action.sa_flags = 0;
	TRY(sigaction(SIGINT, &setup_action, NULL));
	TRY(sigaction(SIGUSR1, &setup_action, NULL));
	TRY(sigaction(SIGUSR2, &setup_action, NULL));

	sigaddset(&block_mask, SIGINT);
	TRY(sigprocmask(SIG_BLOCK, &block_mask, NULL));

	TRY(sigprocmask(SIG_UNBLOCK, &block_mask, NULL));
}

void get_queues(void) {
	BANK_ANSWERS = queue_get(BANK_ANSWERS_KEY);
	BANK_REQUESTS = queue_get(BANK_REQUESTS_KEY);
}

int main(int argc, char **argv) {
	get_arguments(argc, argv);
	get_data();
	get_queues();
	make_passwords();
	make_signal_handlers();
	exec_companies();
	work();
	cleanup();
	return 0;
}
