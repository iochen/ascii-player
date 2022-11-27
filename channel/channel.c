#include "channel.h"
#include "channel_err.h"
#include <pthread.h>
#include <stdlib.h>


Channel *alloc_channel(int cap) {
    if (cap < 1) {
        return NULL;
    }
    void *buf = malloc(cap * sizeof(void *));
    Channel *ch = malloc(sizeof(Channel));
    *ch = (Channel){
        PTHREAD_MUTEX_INITIALIZER, buf, cap, 0, 0, 0, PTHREAD_COND_INITIALIZER,
        PTHREAD_COND_INITIALIZER};
    return ch;
}

void free_channel(Channel *ch) {
    free(ch->buf);
    free(ch);
}

int add_element(Channel *ch, void *ele) {
    pthread_mutex_lock(&ch->lock);
    if (ch->len > ch->cap) {
        return OVERFLOW;
    }
    if (ch->len == ch->cap) {
        pthread_cond_wait(&ch->producer_cond, &ch->lock);
    }
    if (ch->fill_n < 0 || ch->fill_n >= ch->cap) {
        return WRONG_FILL_N;
    }
    ch->buf[ch->fill_n++] = ele;
    ch->fill_n %= ch->cap;
    ch->len++;
    pthread_mutex_unlock(&ch->lock);
    pthread_cond_signal(&ch->consumer_cond);
    return 0;
}

int read_element(Channel *ch, void **p_ele) {
    pthread_mutex_lock(&ch->lock);
    if (ch->len < 0) {
        return UNDERFLOW;
    }
    if (ch->len == 0) {
        pthread_cond_wait(&ch->consumer_cond, &ch->lock);
    }
    if (ch->read_n < 0 || ch->read_n >= ch->cap) {
        return WRONG_READ_N;
    }
    *p_ele = ch->buf[ch->read_n++];
    ch->read_n %= ch->cap;
    ch->len--;
    pthread_mutex_unlock(&ch->lock);
    pthread_cond_signal(&ch->producer_cond);
    return 0;
}