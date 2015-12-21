//Filip Czaplicki 359081
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
bool *reserved; //czy dana działka zarezerwowana
bool simulation_on = true;

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
	reserved = (bool *) err_malloc(sizeof(bool) * length);
	memset(reserved, false, sizeof(bool) * length);
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
	simulation_on = false;
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
	free(reserved);
}

void send_collection(const struct museum_request *msg) {
	struct bank_request bank_rqst;
	//memset(bank_rqst, 0, sizeof(struct bank_request));
	bank_rqst.mtype = TRANSFER;
	bank_rqst.id = msg->id;
	bank_rqst.change = ARTEFACT_MULTIPLIER * msg->p;
	bank_rqst.password = MUSEUM_PASSWORD;
	TRY(msgsnd(BANK_REQUESTS, &bank_rqst, SIZE(bank_request), 0));

	struct transfer_confirmation confirmation;
	TRY(msgrcv(BANK_MUSEUM, &confirmation, SIZE(transfer_confirmation), MUSEUM_ID, 0));
	if (confirmation.status != TRANSFER_OK) {
		fatal("send_collection: TRANSFER NOT OK");
	}
}

void ask_estimate(const struct museum_request *msg) {
	struct estimate_message est_msg;
	est_msg.mtype = msg->id;
	for (int i = msg->l - 1; i < msg->p; i++) {
		for (int j = 0; j < msg->g; j++) {
			est_msg.estimate = estimate[i][j];
			TRY(msgsnd(MUSEUM_ANSWERS, &est_msg, SIZE(estimate_message), 0));
		}
	}

}

inline int pref_sum(int *pref, int begin, int end) {
	return (begin == 0) ? (pref[end]) : (pref[end] - pref[begin - 1]);
}

int find_begin(const int cost, const int workers) {
	int pref_reserved[length], pref_cost[length];
	pref_cost[0] = estimate[0][0];
	pref_reserved[0] = reserved[0];
	for (int i = 1; i < length; i++) {
		pref_cost[i] = pref_cost[i - 1] + estimate[i][0];
		pref_reserved[i] = pref_reserved[i - 1] + reserved[i];
	}
	for (int begin = 0; begin + workers <= length; begin++) {
		int end = begin + workers - 1;
		int interval_cost = pref_sum(pref_cost, begin, end);
		int interval_reserved = pref_sum(pref_reserved, begin, end);
		if (interval_cost > 0 && interval_reserved == 0 && interval_cost < cost - S) {
			return begin;
		}
	}
	return INVALID;
}

int find_depth(const int cost, const int workers, const int begin) {
	if (begin == INVALID) {
		return INVALID;
	}
	if (depth == 1) {
		return 0;
	}
	const int end = begin + workers - 1;
	int sum = 0;
	for (int i = 0; i < depth; i++) {
		for (int j = begin; j <= end; j++) {
			sum += estimate[j][i];
		}
		if (cost <= sum + S) {
			return i - 1;
		}
	}
	return depth - 1;
}

void ask_excavation(const struct excavation_request *request) {
	printf("GOT REQUEST FROM %d, Fk = %d, z = %d\n", request->id, request->workers, request->cost);
	struct excavation_answer answer;
	answer.mtype = request->id;
	answer.begin = find_begin(request->cost, request->workers);
	answer.depth = find_depth(request->cost, request->workers, answer.begin);
	TRY(msgsnd(MUSEUM_ANSWERS, &answer, SIZE(excavation_answer), 0));

	struct transfer_confirmation transfer_status;
	TRY(msgrcv(BANK_MUSEUM, &transfer_status, SIZE(transfer_confirmation), MUSEUM_ID, 0));
	if (transfer_status.status == WITHDRAW_BAD) {
		return;
	}
	int end = answer.begin + request->workers - 1;
	for (int i = answer.begin; i <= end; i++) {
		reserved[i] = true;
	}

}

void ask_on(const struct museum_request *request) {
	struct simulation_response response;
	response.mtype = request->id;
	response.status = simulation_on;
	TRY(msgsnd(MUSEUM_ANSWERS, &response, SIZE(simulation_response), 0));
}

void work(void) {
	while (true) {
		struct museum_request msg;
		TRY(msgrcv(MUSEUM_REQUESTS, &msg, SIZE(museum_request), 0, 0));
		switch (msg.mtype) {
			case SEND_COLLECTION:
				send_collection(&msg);
				break;
			case ASK_ESTIMATE:
				ask_estimate(&msg);
				break;
			case ASK_EXCAVATION:
				ask_excavation((const struct excavation_request *) &msg);
				break;
			case ASK_ON:
				ask_on(&msg);
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