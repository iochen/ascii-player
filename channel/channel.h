#ifndef CHANNEL_H
#define CHANNEL_H

#include "pthread.h"

// A buffered channel.
typedef struct {
    // mutex lock to keep structure thread-safe
    pthread_mutex_t lock;
    // data buf (void * array)
    void **buf;
    // capacity of buf
    int cap;
    // number of data of buf
    int len;
    // to where the next data should to filled
    // buf[fill_n] = element;
    int fill_n;
    // from where the next data should to read
    // element = buf[read_n];
    int read_n;
    // when buf is full, producer wait for this cond
    pthread_cond_t producer_cond;
    // when buf is empty, consumer wait for this cond
    pthread_cond_t consumer_cond;
} Channel;

/// @brief Allocate Channel on heap.
/// @param cap Channel capaticy (limit: >0).
/// @return The pointer to allocated on heap.
Channel *alloc_channel(int cap);

/// @brief Free Channel.
/// @param ch Pointer to Channel.
void free_channel(Channel *ch);

/// @brief Add an element to Channel.
/// @param ch To which Cahnnel the element should be added.
/// @param ele The element to be added to the Cahnnel.
/// @return =0: Success
///         >0: Mutex lock error
///         <0: ChannelErr error
int add_element(Channel *ch, void *ele);

/// @brief Read element from Channel
/// @param ch From which Channel the element should be read.
/// @param p_ele The pointer to the element pointer.
/// @return =0: Success
///         >0: Mutex lock error
///         <0: ChannelErr error
int read_element(Channel *ch, void **p_ele);

/// @brief Non-blockingly read element from Channel with.
/// @param ch From which Channel the element should be read.
/// @param p_ele The pointer to the element pointer.
/// @return =0: Success and lock is not locked
///         >0: Mutex lock error or lock is busy (= EBUSY)
///         <0: ChannelErr error
int read_element_nb(Channel *ch, void **p_ele);

#endif