#define _GNU_SOURCE
#include <pthread.h>
#include "fairlock.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

struct fair_lock *fair_lock_new() 
{
    struct fair_lock *new_lock = (struct fair_lock *)malloc(sizeof(struct fair_lock));
    new_lock->head = new_lock->tail = NULL;
    pthread_mutex_init(&new_lock->mutex, NULL);
    pthread_cond_init(&new_lock->cond, NULL);
    return new_lock;
}

void fair_lock(struct fair_lock *lock)
{
    struct list_node *new_node = (struct list_node*) malloc(sizeof(struct list_node));
    new_node->tid = gettid();
    new_node->next = NULL;
    // 加入tid，等到top == pid
    pthread_mutex_lock(&lock->mutex);
    if (lock->tail == NULL){
        lock->head = lock->tail = new_node;
    } else {
        lock->tail->next = new_node;
        lock->tail = new_node;
    }
    while (lock->head->tid != new_node->tid) {
        pthread_cond_wait(&lock->cond, &lock->mutex);
    }
    pthread_mutex_unlock(&lock->mutex);
}

void fair_unlock(struct fair_lock *lock)
{
    pthread_mutex_lock(&lock->mutex);
    assert(lock->head->tid == gettid());
    struct list_node *next = lock->head->next;
    free(lock->head);
    lock->head = next;
    if (lock->head == NULL) {
        lock->tail = NULL;
    }
    pthread_cond_broadcast(&lock->cond);
    pthread_mutex_unlock(&lock->mutex);
}

struct fair_cond *fair_cond_new(struct fair_lock *lock) {
    struct fair_cond *cond = malloc(sizeof(struct fair_cond));
    cond->lock = lock;
    pthread_cond_init(&cond->cond, NULL);
    pthread_mutex_init(&cond->cond_mutex, NULL);
    cond->value = 0;
    return cond;
}

void fair_cond_wait(struct fair_cond *cond) 
{
    /* unsigned int value = cond->value; */
    fair_unlock(cond->lock);
    pthread_mutex_lock(&cond->cond_mutex);
    /* if (cond->value != value) { */
    pthread_cond_wait(&cond->cond, &cond->cond_mutex);
    /* } else { */
        /* printf("value not equal\n"); */
    /* } */
    pthread_mutex_unlock(&cond->cond_mutex);
    fair_lock(cond->lock);
}

void fair_cond_broadcast(struct fair_cond *cond)
{
    pthread_mutex_lock(&cond->cond_mutex);
    /* cond->value++; */
    pthread_cond_broadcast(&cond->cond);
    pthread_mutex_unlock(&cond->cond_mutex);
}
