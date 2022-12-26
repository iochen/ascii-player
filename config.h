#ifndef CONFIG_H
#define CONFIG_H

#include <pthread.h>

#include "channel/channel.h"
#include "log/log.h"

typedef struct {
    pthread_mutex_t lock;
    int has_data;
    pthread_cond_t drain_cond;
} ChannelStatus;

typedef struct {
    // filename can NOT be NULL or empty
    char *filename;
    // as a bool value
    int help;
    // as a bool value
    int license;
    // NULL for argument not supplied
    char *cache;
    // as a bool value
    int no_audio;
    double fps;
    int width;
    int height;
    char grey_ascii[256 + 1];
    float grey_ascii_step;
    char *logfile;
    LogLevel log_level;
    Channel *video_ch;
    Channel *audio_ch;
    ChannelStatus video_ch_status;
} config;

config parse_config(int argc, char *argv[]);

#endif