#include <unistd.h>
#include <pthread.h>

struct list_node{
    pid_t tid;
    struct list_node *next;
};

struct fair_lock {
    struct list_node *head;
    struct list_node *tail;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

struct fair_cond{
    unsigned int value;
    struct fair_lock *lock;
    pthread_cond_t cond;
    pthread_mutex_t cond_mutex;
};

// create a new fair lock
struct fair_lock *fair_lock_new();

// lock this fair lock
void fair_lock(struct fair_lock *lock);

// unlock this fair lock
void fair_unlock(struct fair_lock *lock);

// wait on this fair condition variable
void fair_cond_wait(struct fair_cond *cond);

// wake up all threads waiting on that fair condition variable
void fair_cond_broadcast(struct fair_cond *cond);

// create a fair condition variable tied to the given fair lock
struct fair_cond *fair_cond_new(struct fair_lock *lock);
