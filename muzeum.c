#include <stdio.h>
#include <stdlib.h>

const int length; //długość
const int depth; //głębokość
const int S; //stała opłata
const int A; //ograniczenie artefaktów

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

int main(int argc, char **argv) {
	get_arguments(argc, argv);
}