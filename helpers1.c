#include "helpers1.h"

int min(int a, int b) {
	return a < b ? a : b;
}

int max(int a, int b) {
	return a > b ? a : b;
}

void *err_malloc(size_t size) {
	void *result = malloc(size);
	if (result == NULL) {
		fatal("malloc");
	}
	return result;
}