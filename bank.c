#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include "helpers.h"
#include "museum_password.h"

const int F; //liczba firm
const int S; //stała opłata
const int A; //ograniczenie artefaktów

int *id; //id firmy
int *balance; //stany kont firm
int *workers; //liczby pracowników firm
int *password; //hasła do kont

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

int main(int argc, char **argv) {
	get_arguments(argc, argv);
	get_data();
	make_passwords();
	exec_companies();
	cleanup();
	return 0;
}
