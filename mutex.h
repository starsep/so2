#ifndef MUTEX_H
#define MUTEX_H

int mutex_create(void);
void mutex_wait(int);
void mutex_release(int);

#endif //MUTEX_H