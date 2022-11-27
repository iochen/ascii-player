#ifndef CHANNEL_ERR_H
#define CHANNEL_ERR_H

typedef enum {
    SUCCESS = 0,
    UNKNOWN = -1,
    OVERFLOW = -2,
    UNDERFLOW = -3,
    WRONG_FILL_N = -4,
    WRONG_READ_N = -5,
} ChannelErr;

#endif