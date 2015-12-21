//Filip Czaplicki 359081
#include "helpers.h"
#include "messages.h"
#include "queue.h"

//stałe
const int Fid; //id firmy
const int workers; //liczba pracowników
const int S; //stała opłata
const int A; //ograniczenie artefaktów
const int password; //hasło do konta w banku

//id kolejek
int BANK_REQUESTS;
int BANK_ANSWERS;
int MUSEUM_ANSWERS;
int MUSEUM_REQUESTS;
int ARTEFACTS;

int balance; //saldo firmy

bool firm_suspended;
pthread_cond_t *not_suspended_firm;
pthread_mutex_t *mutex;
pthread_attr_t attr;

int *collections;
int collections_size;
int collections_reserved;

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

void make_report(void) {
	int RAPORT_SYNC = queue_get(RAPORT_SYNC_KEY);
	struct empty_message token;
	TRY(msgrcv(RAPORT_SYNC, &token, SIZE(empty_message), 0, 0));
	printf("%d %d\n", Fid, balance);
	for (int i = 0; i < collections_size; i += 2) {
		if (collections[i + 1] != 0) {
			printf("%d %d\n", collections[i], collections[i + 1]);
		}
	}
	printf("\n");
	TRY(msgsnd(RAPORT_SYNC, &token, SIZE(empty_message), 0));
}

void send_end(void) {
	struct museum_request end;
	memset(&end, 0, sizeof(struct museum_request));
	end.mtype = FIRM_END;
	TRY(msgsnd(MUSEUM_REQUESTS, &end, SIZE(museum_request), 0));
}

void cleanup(void) {
	send_end();
	make_report();
	if ((pthread_mutex_destroy(mutex) != 0)) {
		fatal("pthread_mutex_init");
	}
	if (pthread_cond_destroy(not_suspended_firm) != 0) {
		fatal("pthread_cond_init");
	}
	if (pthread_attr_destroy(&attr) != 0) {
		fatal("pthread_attr_destroy");
	}
	free(mutex);
	free(not_suspended_firm);
}


void get_queues(void) {
	BANK_REQUESTS = queue_get(BANK_REQUESTS_KEY);
	BANK_ANSWERS = queue_get(BANK_ANSWERS_KEY);
	MUSEUM_ANSWERS = queue_get(MUSEUM_ANSWERS_KEY);
	MUSEUM_REQUESTS = queue_get(MUSEUM_REQUESTS_KEY);
	ARTEFACTS = queue_get(ARTEFACTS_KEY);
}

int withdraw(int money) {
	struct bank_request rqst;
	memset(&rqst, 0, sizeof(struct bank_request));
	rqst.mtype = (money == 0) ? CHECK_BALANCE : WITHDRAW;
	rqst.id = Fid;
	rqst.change = money;
	rqst.password = password;
	TRY(msgsnd(BANK_REQUESTS, &rqst, SIZE(bank_request), 0));

	struct account_balance rsp;
	TRY(msgrcv(BANK_ANSWERS, &rsp, SIZE(account_balance), Fid, 0));
	return rsp.balance;
}

int get_balance(void) {
	return withdraw(0);
}

void add_artefact(const int p) {
	for (int i = 0; i < collections_size; i += 2) {
		if (collections[i] == p) {
			collections[i + 1]++;
			return;
		}
	}
	collections_size += 2;
	if (collections_size > collections_reserved) {
		collections_reserved *= 2;
		collections = realloc(collections, sizeof(int) * collections_reserved);
		if (collections == NULL) {
			fatal("realloc");
		}
	}
	collections[collections_size - 2] = p;
	collections[collections_size - 1] = 1;
}

void *excavate(void *arg) {
	int index = (long) arg;
	struct artefacts_message msg;
	TRY(msgrcv(ARTEFACTS, &msg, SIZE(artefacts_message), index, 0));

	for (int i = 2; i <= A && msg.value > 1; i++) {
		while (msg.value % i == 0) {
			msg.value /= i;
			add_artefact(i);
		}
	}
	return NULL;
}

void make_excavation(const int begin) {
	pthread_t thread[workers];
	for (int i = 0; i < workers; i++) {
		pthread_create(&thread[i], &attr, excavate, (void *) (long) (begin + i + 1));
	}
	void *retval;
	for (int i = 0; i < workers; i++) {
		if (pthread_join(thread[i], &retval) != 0) {
			fatal("pthread_join");
		}
	}
}

void send_excavation_request(const int z) {
	struct excavation_request request;
	request.mtype = ASK_EXCAVATION;
	request.id = Fid;
	request.cost = z;
	request.g = 0;
	request.workers = workers;
	TRY(msgsnd(MUSEUM_REQUESTS, &request, SIZE(excavation_request), 0));

	struct excavation_answer answer;
	TRY(msgrcv(MUSEUM_ANSWERS, &answer, SIZE(excavation_answer), Fid, 0));

	withdraw(answer.begin == INVALID ? S : z);
	struct account_balance transfer_status;
	TRY(msgrcv(BANK_ANSWERS, &transfer_status, SIZE(transfer_confirmation), Fid, 0));
	if (answer.begin == INVALID) {
		return;
	}
	make_excavation(answer.begin);
}

int get_estimate(int l, int p, int g) {
	struct museum_request ask_est;
	ask_est.mtype = ASK_ESTIMATE;
	ask_est.id = Fid;
	ask_est.l = l;
	ask_est.p = p;
	ask_est.g = g;
	TRY(msgsnd(MUSEUM_REQUESTS, &ask_est, SIZE(museum_request), 0));

	int sum = 0;
	struct estimate_message est_msg;
	for (int i = l - 1; i < p; i++) {
		for (int j = 0; j < g; j++) {
			TRY(msgrcv(MUSEUM_ANSWERS, &est_msg, SIZE(estimate_message), Fid, 0));
			sum += est_msg.estimate;
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

void send_collection(const int p) {
	struct museum_request request;
	memset(&request, 0, sizeof(struct museum_request));
	request.mtype = SEND_COLLECTION;
	request.id = Fid;
	request.p = p;
	TRY(msgsnd(MUSEUM_REQUESTS, &request, SIZE(museum_request), 0));
}

void sell_collections(void) {
	pthread_mutex_lock(mutex);
	for (int i = 0; i < collections_size; i += 2) {
		while (collections[i + 1] >= collections[i]) {
			collections[i + 1] -= collections[i];
			send_collection(collections[i]);
		}
	}
	pthread_mutex_unlock(mutex);
}

bool ask_on(void) {
	struct museum_request request;
	memset(&request, 0, sizeof(struct museum_request));
	request.mtype = ASK_ON;
	request.id = Fid;
	TRY(msgsnd(MUSEUM_REQUESTS, &request, SIZE(museum_request), 0));

	struct simulation_response response;
	TRY(msgrcv(MUSEUM_ANSWERS, &response, SIZE(simulation_response), Fid, 0));
	return response.status;
}

void work() {
	while (ask_on()) {
		sell_collections();
		balance = get_balance();
		if (balance <= S) {
			break;
		}
		send_excavation_request(S + rand() % (balance - S));
	}
}

void init(void) {
	if (pthread_attr_init(&attr) != 0) {
		fatal("pthread_attr_init");
	}
	if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE) != 0) {
		fatal("pthread_attr_setdetachstate");
	}
	mutex = (pthread_mutex_t *) err_malloc(sizeof(pthread_mutex_t));
	if ((pthread_mutex_init(mutex, NULL) != 0)) {
		fatal("pthread_mutex_init");
	}
	not_suspended_firm = (pthread_cond_t *) err_malloc(sizeof(pthread_cond_t));
	if (pthread_cond_init(not_suspended_firm, NULL) != 0) {
		fatal("pthread_cond_init");
	}
	collections_reserved = 2;
	collections = (int *) err_malloc(sizeof(int) * collections_reserved);
	collections_size = 0;
	collections[0] = 0;
	collections[1] = 0;
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