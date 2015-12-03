#include <stdio.h>
#include <stdlib.h>

const int Fid; //id firmy
const int workers; //liczba pracowników
const int S; //stała opłata
const int A; //ograniczenie artefaktów

int balance; //saldo firmy

void get_arguments(int argc, char **argv) {
	if (argc != 6) {
		fprintf(stderr, "Bad number of arguments\n");
		fprintf(stderr, "Usage:\n");
		fprintf(stderr, "%s Fid Fsaldo Fk S A\n", argv[0]);
		exit(1);
	}
	*((int *) &Fid) = atoi(argv[1]);
	*((int *) &balance) = atoi(argv[2]);
	*((int *) &workers) = atoi(argv[3]);
	*((int *) &S) = atoi(argv[4]);
	*((int *) &A) = atoi(argv[5]);
}

void make_raport(void) {
	printf("%d %d\n", Fid, balance);
	//TODO lista artefaktów
	sleep(1);
	exit(Fid);
}

int main(int argc, char **argv) {
	get_arguments(argc, argv);
	make_raport();
}