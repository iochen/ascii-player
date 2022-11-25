#ifndef AV_H
#define AV_H

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/error.h>
#include <pthread.h>
#include <stdatomic.h>

#include "config.h"

void print_averror(int code);

extern int find_codec_context(config *conf, AVFormatContext **p_fmt_ctxt,
                              AVCodecContext **p_a_cdc,
                              AVCodecContext **p_v_cdc, int *p_a_idx,
                              int *p_v_idx);

#endif