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

void make_signal_handlers(void(*signal_handler)(int)) {
	struct sigaction setup_action;
	sigset_t block_mask;

	sigemptyset(&block_mask);
	setup_action.sa_handler = signal_handler;
	setup_action.sa_mask = block_mask;
	setup_action.sa_flags = 0;
	TRY(sigaction(SIGINT, &setup_action, NULL));
	TRY(sigaction(SIGUSR1, &setup_action, NULL));
	TRY(sigaction(SIGUSR2, &setup_action, NULL));

	sigaddset(&block_mask, SIGINT);
	TRY(sigprocmask(SIG_BLOCK, &block_mask, NULL));

	TRY(sigprocmask(SIG_UNBLOCK, &block_mask, NULL));
}