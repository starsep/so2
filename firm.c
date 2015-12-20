#include "helpers.h"
#include "messages.h"
#include "queue.h"

const int Fid; //id firmy
const int workers; //liczba pracowników
const int S; //stała opłata
const int A; //ograniczenie artefaktów
const int password; //hasło do konta w banku

int BANK_REQUESTS = -1;
int BANK_ANSWERS = -1;
int COLLECTION_QUEUE_ID = -1;

int balance; //saldo firmy


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



void get_queues(void) {
	queue_get(&COLLECTION_QUEUE_ID, COLLECTION_QUEUE_KEY);
	queue_get(&BANK_REQUESTS, BANK_REQUESTS_KEY);
	queue_get(&BANK_ANSWERS, BANK_ANSWERS_KEY);
}

int get_balance(void) {
	struct bank_request *msg = (struct bank_request *) err_malloc(sizeof(struct bank_request));
	msg->mtype = 1;
	msg->id = Fid;
	msg->password = password;
	TRY(msgsnd(BANK_REQUESTS, msg, sizeof(struct bank_request) - sizeof(long), 0));

	struct account_balance *rsp = (struct account_balance *) err_malloc(sizeof(struct account_balance));
	TRY(msgrcv(BANK_ANSWERS, rsp, sizeof(struct account_balance) - sizeof(long), Fid, 0));
	return rsp->balance;
}

void work() {
	while (1) {
		printf("AKTUALNY STAN: %d\n", get_balance());
		//printf("Firma %d: I'm still alive!\n", Fid);
		sleep(1);
	}
}

int main(int argc, char **argv) {
	get_arguments(argc, argv);
	get_queues();
	work();
	make_raport();
	return 0;
}