#include "helpers.h"
#include "messages.h"
#include "queue.h"

const int Fid; //id firmy
const int workers; //liczba pracowników
const int S; //stała opłata
const int A; //ograniczenie artefaktów
const int password; //hasło do konta w banku

int BANK_REQUESTS;
int BANK_ANSWERS;
int MUSEUM_ANSWERS;
int MUSEUM_REQUESTS;

int balance; //saldo firmy

bool firm_suspended;
pthread_cond_t *not_suspended_firm;
pthread_mutex_t *mutex;

void get_arguments(int argc, char **argv) {
	if (argc != 7) {
		fprintf(stderr, "Bad number of arguments\n");
		fprintf(stderr, "Usage:\n");
		fprintf(stderr, "%s Fid Fsaldo Fk S A Password\n", argv[0]);
		exit(1);
	}
	*((int *) &Fid) = atoi(argv[1]);
	*((int *) &balance) = atoi(argv[2]);
	*((int *) &workers) = atoi(argv[3]);
	*((int *) &S) = atoi(argv[4]);
	*((int *) &A) = atoi(argv[5]);
	*((int *) &password) = atoi(argv[6]);
}

void make_raport(void) {
	printf("%d %d\n", Fid, balance);
	//TODO lista artefaktów
	//sleep(1);
	//exit(Fid);
}

void cleanup(void) {
	make_raport();
	if ((pthread_mutex_destroy(mutex) != 0)) {
		fatal("pthread_mutex_init");
	}
	if (pthread_cond_destroy(not_suspended_firm) != 0) {
		fatal("pthread_cond_init");
	}
	free(mutex);
	free(not_suspended_firm);
}


void get_queues(void) {
	BANK_REQUESTS = queue_get(BANK_REQUESTS_KEY);
	BANK_ANSWERS = queue_get(BANK_ANSWERS_KEY);
	MUSEUM_ANSWERS = queue_get(MUSEUM_ANSWERS_KEY);
	MUSEUM_REQUESTS = queue_get(MUSEUM_REQUESTS_KEY);
}

int withdraw(int money) {
	struct bank_request rqst;
	memset(&rqst, 0, sizeof(struct bank_request));
	rqst.mtype = (money == 0) ? CHECK_BALANCE : WITHDRAW;
	rqst.id = Fid;
	rqst.change = money;
	rqst.password = password;
	TRY(msgsnd(BANK_REQUESTS, &rqst, sizeof(struct bank_request) - sizeof(long), 0));

	struct account_balance rsp;
	TRY(msgrcv(BANK_ANSWERS, &rsp, sizeof(struct account_balance) - sizeof(long), Fid, 0));
	return rsp.balance;
}

int get_balance(void) {
	return withdraw(0);
}

int get_estimate(int l, int p, int g) {
	struct museum_request ask_est;
	ask_est.mtype = ASK_ESTIMATE;
	ask_est.id = Fid;
	ask_est.l = l;
	ask_est.p = p;
	ask_est.g = g;
	TRY(msgsnd(MUSEUM_REQUESTS, &ask_est, sizeof(struct museum_request) - sizeof(long), 0));

	int sum = 0;
	struct estimate_message est_msg;
	for (int i = l - 1; i < p; i++) {
		for (int j = 0; j < g; j++) {
			TRY(msgrcv(MUSEUM_ANSWERS, &est_msg, sizeof(struct estimate_message) - sizeof(long), Fid, 0));
			sum += est_msg.estimate;
			//printf("%d%c", est_msg.estimate, j == g - 1 ? '\n' : ' ');
		}
	}
	return sum;
}

void signal_handler(int sig) {
	switch (sig) {
		case SIGINT:
			cleanup();
			exit(0);
			break;
		case SIGUSR1:
			firm_suspended = true;
			break;
		case SIGUSR2:
			firm_suspended = false;
			break;
	}
}

void work() {
	while (true) {
		//get_balance();
		printf("AKTUALNY STAN: %d\n", get_balance());
		printf("SUM: %d\n", get_estimate(1, 2, 2));
		//printf("Firma %d: I'm still alive!\n", Fid);
		sleep(3);
	}
}

void init(void) {
	mutex = (pthread_mutex_t *) err_malloc(sizeof(pthread_mutex_t));
	if ((pthread_mutex_init(mutex, NULL) != 0)) {
		fatal("pthread_mutex_init");
	}
	not_suspended_firm = (pthread_cond_t *) err_malloc(sizeof(pthread_cond_t));
	if (pthread_cond_init(not_suspended_firm, NULL) != 0) {
		fatal("pthread_cond_init");
	}
	//TRY(pthread_condattr_destroy(cond_attr));
}

int main(int argc, char **argv) {
	get_arguments(argc, argv);
	init();
	make_signal_handlers(signal_handler);
	get_queues();
	work();
	cleanup();
	return 0;
}