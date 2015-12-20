#ifndef MUTEX_H
#define MUTEX_H

int mutex_create(void);
void mutex_wait(int);
void mutex_release(int);
void mutex_cleanup(void);

#endif //MUTEX_H