//Filip Czaplicki 359081
#include "helpers.h"
#include "museum_password.h"
#include "messages.h"
#include "queue.h"

const int F; //liczba firm
const int S; //stała opłata
const int A; //ograniczenie artefaktów

int *id; //id firm
int *balance; //stany kont firm
int *workers; //liczby pracowników firm
int *password; //hasła do kont

int firm; //nr aktualnej firmy

int BANK_REQUESTS;
int BANK_ANSWERS;
int BANK_MUSEUM;

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
	free(workers);
	free(password);
}

int find_firm(const int ID) {
	for (int i = 0; i < F; i++) {
		if (id[i] == ID) {
			return i;
		}
	}
	return INVALID;
}

void check_balance(const struct bank_request *msg) {
	if (msg->password == password[firm]) {
		struct account_balance rsp;
		memset(&rsp, 0, sizeof(struct account_balance));
		rsp.mtype = msg->id;
		rsp.balance = balance[firm];
		TRY(msgsnd(BANK_ANSWERS, &rsp, SIZE(account_balance), 0));
	}
}

void transfer(const struct bank_request *msg) {
	struct account_balance rsp;
	rsp.mtype = msg->id;
	rsp.balance = TRANSFER_OK;
	if (msg->change > 0 && msg->password == MUSEUM_PASSWORD) {
		balance[firm] += msg->change;
	}
	TRY(msgsnd(BANK_MUSEUM, &rsp, SIZE(account_balance), 0));
}

void withdraw(const struct bank_request *msg) {
	struct account_balance rsp;
	rsp.mtype = msg->id;
	rsp.balance = WITHDRAW_BAD;
	if (msg->change < 0 && msg->password == password[firm] && balance[firm] + msg->change >= 0) {
		rsp.balance = WITHDRAW_OK;
		balance[firm] += msg->change;
	}
	TRY(msgsnd(BANK_ANSWERS, &rsp, SIZE(account_balance), 0));
	rsp.mtype = MUSEUM_ID;
	TRY(msgsnd(BANK_MUSEUM, &rsp, SIZE(account_balance), 0));
}

void work(void) {
	while (true) {
		struct bank_request msg;
		TRY(msgrcv(BANK_REQUESTS, &msg, SIZE(bank_request), 0, 0));
		firm = find_firm(msg.id);
		switch (msg.mtype) {
			case CHECK_BALANCE:
				check_balance(&msg);
				break;
			case TRANSFER:
				transfer(&msg);
				break;
			case WITHDRAW:
				withdraw(&msg);
				check_balance(&msg);
				break;
			case MUSEUM_END:
				cleanup();
				exit(0);
				break;
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

void get_queues(void) {
	BANK_ANSWERS = queue_get(BANK_ANSWERS_KEY);
	BANK_REQUESTS = queue_get(BANK_REQUESTS_KEY);
	BANK_MUSEUM = queue_get(BANK_MUSEUM_KEY);
}

int main(int argc, char **argv) {
	get_arguments(argc, argv);
	make_signal_handlers(signal_handler);
	get_data();
	get_queues();
	make_passwords();
	exec_companies();
	work();
	cleanup();
	return 0;
}
