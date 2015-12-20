#include <stdio.h>
#include <stdlib.h>
#include "museum_password.h"
#include "helpers.h"

const int ARTEFACT_MULTIPLIER = 10;

const int length; //długość
const int depth; //głębokość
const int S; //stała opłata
const int A; //ograniczenie artefaktów

int **artefacts; //tablica Teren[][]
int **estimate; //tablica Szacunek[][]

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

int main(int argc, char **argv) {
	get_arguments(argc, argv);
	alloc_data();
	get_input();
}