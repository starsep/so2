#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "helpers.h"
#include "err.h"

const int F; //liczba firm
const int S; //stała opłata
const int A; //ograniczenie artefaktów

int *id; //id firmy
int *balance; //stany kont firm
int *workers; //liczby pracowników firm

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
	id = (int *) malloc(sizeof(int) * F);
	balance = (int *) malloc(sizeof(int) * F);
	workers = (int *) malloc(sizeof(int) * F);
	for (int i = 0; i < F; i++) {
		if (scanf("%d%d%d", &id[i], &balance[i], &workers[i]) < 3) {
			fatal("scanf: not enough data");
		}
	}
}

void exec_company(const int i) {
	execl("firma", "firma", itoa(id[i]), itoa(balance[i]), itoa(workers[i]), itoa(S), itoa(A), NULL);
	fatal(execl);
}


void exec_companies(void) {
	for (int i = 0; i < F; i++) {
		exec_company(i);
	}
}

void cleanup(void) {
	free(id);
	free(balance);
}

int main(int argc, char **argv) {
	get_arguments(argc, argv);
	get_data();
	exec_companies();
	cleanup();
}