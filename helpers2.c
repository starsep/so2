//Filip Czaplicki 359081
//Funkcje pomocnicze

#include "helpers1.h"
#include "helpers2.h"
#include <stdio.h>

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