#ifndef MESSAGES_H
#define MESSAGES_H

const int TRANSFER_QUEUE_ID = 1001;

struct transfer {
	int id; //nr firmy
	int change; //zmiana stanu konta, jeżeli < 0 to przelew z firmy -> muzeum w p. p. z muzeum -> firmy
	int password; //tajne hasło podmiotu, który wysyła
};

#endif //MESSAGES_H