//Filip Czaplicki 359081
#ifndef MESSAGES_H
#define MESSAGES_H

enum Queues {
	QUEUE_KEYS_BEGIN = 6121,
	BANK_ANSWERS_KEY,
	BANK_REQUESTS_KEY,
	BANK_MUSEUM_KEY,
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
	ASK_ESTIMATE = 1,
	ASK_EXCAVATION,
	ASK_ON,
	SEND_COLLECTION,
	FIRM_END
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

struct excavation_request {
	long mtype;
	int id; //nr firmy przesyłającej
	int workers; //liczba pracowników
	int cost; //opłata proponowana
	int g; //nic nie znaczy, wielkość z museum_request ma się zgadzać
};

struct excavation_answer {
	long mtype;
	int begin; //pierwsza działka
	int depth; //głębokośc
};

struct estimate_message {
	long mtype;
	int estimate;
};

struct account_balance {
	long mtype;
	int balance; //stan konta
};

struct transfer_confirmation {
	long mtype;
	int status;
};

struct simulation_response {
	long mtype;
	bool status;
};


#endif //MESSAGES_H