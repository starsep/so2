//Funkcje pomocnicze

#include "helpers1.h"
#include "helpers2.h"
#include "mutex.h"
#include <stdio.h>

static int HELPERS_MUTEX = -1;

static struct {
	void **pointers;
	int size;
	int reserved;
} pointers_to_free;

static void pointers_to_free_init() {
	if (HELPERS_MUTEX == -1) {
		HELPERS_MUTEX = mutex_create();
	}
	mutex_wait(HELPERS_MUTEX);
	pointers_to_free.size = 0;
	pointers_to_free.reserved = 1;
	pointers_to_free.pointers = (void **) err_malloc(sizeof(void *) * pointers_to_free.reserved);
	mutex_release(HELPERS_MUTEX);
}

static void add_pointer(void *ptr) {
	if (pointers_to_free.pointers == NULL) {
		pointers_to_free_init();
	}
	mutex_wait(HELPERS_MUTEX);
	if (pointers_to_free.size >= pointers_to_free.reserved) {
		pointers_to_free.reserved *= 2;
		pointers_to_free.pointers = (void **) realloc(pointers_to_free.pointers,
			sizeof(void *) * pointers_to_free.reserved);
	}
	pointers_to_free.pointers[pointers_to_free.size++] = ptr;
	mutex_release(HELPERS_MUTEX);
}

char *itoa(int x) {
	bool minus = x < 0;
	if (minus) {
		x = -x;
	}

	int y = x;
	int length = 0;
	do {
		length++;
		y /= 10;
	} while (y > 0);

	length += minus;
	char *result = (char *) err_malloc(sizeof(char) * (length + 1));
	add_pointer(result);
	memset(result, '0', length);

	for (int i = length - 1; x > 0; i--) {
		result[i] += x % 10;
		x /= 10;
	}

	if (minus) {
		result[0] = '-';
	}
	result[length] = '\0';
	return result;
}

void helpers_cleanup(void) {
	if (HELPERS_MUTEX == -1) {
		HELPERS_MUTEX = mutex_create();
	}
	mutex_wait(HELPERS_MUTEX);
	if (pointers_to_free.pointers != NULL) {
		for (int i = 0; i < pointers_to_free.size; i++) {
			free(pointers_to_free.pointers[i]);
		}
		free(pointers_to_free.pointers);
	}
	pointers_to_free.pointers = NULL;
	mutex_release(HELPERS_MUTEX);
}