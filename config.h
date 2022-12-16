#ifndef CONFIG_H
#define CONFIG_H

#include "channel/channel.h"

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
    char grey_ascii[256];
    float grey_ascii_step;
    Channel *video_ch;
    Channel *audio_ch;
} config;

config parse_config(int argc, char *argv[]);

#endif