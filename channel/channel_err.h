#ifndef CHANNEL_ERR_H
#define CHANNEL_ERR_H

typedef enum {
    CH_ERR_SUCCESS = 0,
    CH_ERR_NULLCH = -1,
    CH_ERR_NULLBUF = -2,
    CH_ERR_OVERFLOW = -3,
    CH_ERR_UNDERFLOW = -4,
    CH_ERR_WRONG_FILL_N = -5,
    CH_ERR_WRONG_READ_N = -6,
} ChannelErr;

#endif