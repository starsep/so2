#ifndef MESSAGES_H
#define MESSAGES_H

enum Queues {
	QUEUE_KEYS_BEGIN = 6301,
	BANK_ANSWERS_KEY,
	BANK_REQUESTS_KEY,
	BANK_MUSEUM_KEY,
	COLLECTION_QUEUE_KEY,
	MUSEUM_ANSWERS_KEY,
	MUSEUM_REQUESTS_KEY,
	QUEUE_KEYS_END
};

enum BankOperations {
	CHECK_BALANCE = 1,
	TRANSFER,
	WITHDRAW,
	MUSEUM_END
};

enum MuseumOperations {
	SEND_COLLECTION = 1,
	ASK_ESTIMATE
};

enum {
	WITHDRAW_BAD = 1,
	WITHDRAW_OK,
	TRANSFER_OK
};

struct bank_request {
	long mtype;
	int change; //zmiana stanu konta, jeżeli < 0 to przelew z firmy -> muzeum w p. p. z muzeum -> firmy
	int id; //nr firmy
	int password; //tajne hasło podmiotu, który wysyła
};

struct museum_request {
	long mtype;
	int id; //nr firmy przesyłającej
	int l;
	int p;
	int g;
};

struct account_balance {
	long mtype;
	int balance; //stan konta
};


#endif //MESSAGES_H