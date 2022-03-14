#include <stdio.h>
#include "fairlock.h"
#include <pthread.h>

int counter = 0;
struct fair_lock *lock;

void *worker(void *args)
{
    int id = (int) args;
    for (int i = 0; i < 10000; ++i) {
        fair_lock(lock);
        printf("I am %d\n", id);
        counter++;
        fair_unlock(lock);
    }
    pthread_exit(NULL);
    return NULL;
}

int main()
{
    lock = fair_lock_new();
    pthread_t t[4];
    for (int i = 0; i < 4; ++i) {
        pthread_create(t + i, NULL, worker, (void *)i);
    }

    for (int i = 0; i < 4; ++i) {
        pthread_join(t[i], NULL);
    }
    printf("%d\n", counter);
}

