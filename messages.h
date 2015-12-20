#ifndef MESSAGES_H
#define MESSAGES_H

const int BANK_ANSWERS_KEY     = 2301;
const int BANK_REQUESTS_KEY    = 2302;
const int COLLECTION_QUEUE_KEY = 2303;

const long TRANSFER_MESSAGE      = 1;
const long CHECK_BALANCE_MESSAGE = 2;

struct bank_request {
	long mtype;
	int change; //zmiana stanu konta, jeżeli < 0 to przelew z firmy -> muzeum w p. p. z muzeum -> firmy
	int id; //nr firmy
	int password; //tajne hasło podmiotu, który wysyła
};

struct collection {
	int id; //nr firmy przesyłającej
	int p; //p artefaktów rodzaju p
};

struct account_balance {
	long mtype;
	int balance; //stan konta
};


#endif //MESSAGES_H