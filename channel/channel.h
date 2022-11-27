#ifndef CHANNEL_H
#define CHANNEL_H

#include "pthread.h"

// A buffered channel.
typedef struct {
    pthread_mutex_t lock;
    void **buf;
    int cap;
    int len;
    int fill_n;
    int read_n;
    pthread_cond_t producer_cond;
    pthread_cond_t consumer_cond;
} Channel;

Channel *alloc_channel(int cap);

void free_channel(Channel *ch);

int add_element(Channel *ch, void *ele);

int read_element(Channel *ch, void **p_ele);

#endif