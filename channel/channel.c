#include "channel.h"

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "channel_err.h"

/// @brief Allocate Channel on heap.
/// @param cap Channel capaticy (limit: >0).
/// @return The pointer to allocated on heap.
Channel *alloc_channel(int cap) {
    // validate cap
    if (cap < 1) {
        return NULL;
    }
    // allocate memory for buf
    void *buf = malloc(cap * sizeof(void *));
    if (!buf) {
        return NULL;
    }
    // allocate memory for Channel
    Channel *ch = malloc(sizeof(Channel));
    if (!ch) {
        free(buf);
        return NULL;
    }
    // init channel
    *ch = (Channel){
        PTHREAD_MUTEX_INITIALIZER, buf,  cap, 0, 0, 0, PTHREAD_COND_INITIALIZER,
        PTHREAD_COND_INITIALIZER,  {NULL, NULL}, {NULL, NULL}};
    return ch;
}

/// @brief Free Channel.
/// @param ch Pointer to Channel.
void free_channel(Channel *ch) {
    if (!ch) {
        return;
    }
    // free buf
    free(ch->buf);
    // free ch
    free(ch);
}

/// @brief Add an element to Channel.
/// @param ch To which Cahnnel the element should be added.
/// @param ele The element to be added to the Cahnnel.
/// @return =0: Success
///         >0: Mutex lock error
///         <0: ChannelErr error
int add_element(Channel *ch, void *ele) {
    // check ch
    if (!ch) {
        return CH_ERR_NULLCH;
    }
    // lock mutex and check err
    int err = pthread_mutex_lock(&ch->lock);
    if (err != 0) {
        return err;
    }
    // error: len > cap
    if (ch->len > ch->cap) {
        return CH_ERR_OVERFLOW;
    }
    // buf full
    if (ch->len == ch->cap) {
        pthread_cond_wait(&ch->producer_cond, &ch->lock);
    }
    // invalid fill_n
    if (ch->fill_n < 0 || ch->fill_n >= ch->cap) {
        return CH_ERR_WRONG_FILL_N;
    }
    // buf is NULL
    if (!ch->buf) {
        return CH_ERR_NULLBUF;
    }
    // add element && fill_n++
    ch->buf[ch->fill_n++] = ele;
    // limit fill_n to [0, cap)
    ch->fill_n %= ch->cap;
    // len++
    ch->len++;
    if (ch->add_callback.callback) {
        ch->add_callback.callback(ch->add_callback.arg);
    }
    // unlock mutex lock
    pthread_mutex_unlock(&ch->lock);
    // signal consumer_cond to unblock may-existing blocking consumer thread
    pthread_cond_signal(&ch->consumer_cond);
    return 0;
}

/// @brief Read element from Channel.
/// @param ch From which Channel the element should be read.
/// @param p_ele The pointer to the element pointer.
/// @return =0: Success
///         >0: Mutex lock error
///         <0: ChannelErr error
int read_element(Channel *ch, void **p_ele) {
    // check ch
    if (!ch) {
        return CH_ERR_NULLCH;
    }
    // lock mutex and check err
    int err = pthread_mutex_lock(&ch->lock);
    if (err != 0) {
        return err;
    }
    // invalid len
    if (ch->len < 0) {
        return CH_ERR_UNDERFLOW;
    }
    // buf empty
    if (ch->len == 0) {
        if (ch->drain_callback.callback) {
            ch->drain_callback.callback(ch->drain_callback.arg);
        }
        pthread_cond_wait(&ch->consumer_cond, &ch->lock);
    }
    // invalid read_n
    if (ch->read_n < 0 || ch->read_n >= ch->cap) {
        return CH_ERR_WRONG_READ_N;
    }
    // buf is NULL
    if (!ch->buf) {
        return CH_ERR_NULLBUF;
    }
    // add element and read_n++
    *p_ele = ch->buf[ch->read_n++];
    // limit read_n to [0, cap)
    ch->read_n %= ch->cap;
    // len--
    ch->len--;
    // unlock mutex lock
    pthread_mutex_unlock(&ch->lock);
    // signal producer_cond to unblock may-existing blocking producer thread
    pthread_cond_signal(&ch->producer_cond);
    return 0;
}

/// @brief Non-blockingly read element from Channel with.
/// @param ch From which Channel the element should be read.
/// @param p_ele The pointer to the element pointer.
/// @return =0: Success and lock is not locked
///         >0: Mutex lock error or lock is busy (= EBUSY)
///         <0: ChannelErr error
int read_element_nb(Channel *ch, void **p_ele) {
    // check ch
    if (!ch) {
        return CH_ERR_NULLCH;
    }
    // try to lock mutex and check err
    int err = pthread_mutex_trylock(&ch->lock);
    if (err != 0) {
        // lock is busy
        if (err == EBUSY) {
            *p_ele = NULL;
        }
        return err;
    }
    // invalid len
    if (ch->len < 0) {
        return CH_ERR_UNDERFLOW;
    }
    // buf empty
    if (ch->len == 0) {
        pthread_cond_wait(&ch->consumer_cond, &ch->lock);
    }
    // invalid read_n
    if (ch->read_n < 0 || ch->read_n >= ch->cap) {
        return CH_ERR_WRONG_READ_N;
    }
    // buf is NULL
    if (!ch->buf) {
        return CH_ERR_NULLBUF;
    }
    // add element and read_n++
    *p_ele = ch->buf[ch->read_n++];
    // limit read_n to [0, cap)
    ch->read_n %= ch->cap;
    // len--
    ch->len--;
    // unlock mutex lock
    pthread_mutex_unlock(&ch->lock);
    // signal producer_cond to unblock may-existing blocking producer thread
    pthread_cond_signal(&ch->producer_cond);
    return 0;
}