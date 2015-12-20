#ifndef MESSAGES_H
#define MESSAGES_H

const int BANK_QUEUE_KEY = 1201;
const int COLLECTION_QUEUE_KEY = 1202;

const long TRANSFER_MESSAGE = 1;
const long CHECK_BALANCE_MESSAGE = 2;

struct message {
	long mtype; //typ wiadomości
	void *mtext; //zawartość
};

struct transfer {
	int id; //nr firmy
	int change; //zmiana stanu konta, jeżeli < 0 to przelew z firmy -> muzeum w p. p. z muzeum -> firmy
	int password; //tajne hasło podmiotu, który wysyła
};

struct collection {
	int id; //nr firmy przesyłającej
	int p; //p artefaktów rodzaju p
};

struct account_balance {
	int balance; //stan konta
};


#endif //MESSAGES_H