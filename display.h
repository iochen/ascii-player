#ifndef DISPLAY_H
#define DISPLAY_H

#include <portaudio.h>

#include "channel/channel.h"
#include "config.h"
#include "apcache.h"

// typedef struct {
//     int nb_samples;
//     float *data;
// } APAudioData;

// typedef struct {
//     int ts_u;
//     unsigned char *data;
// } APVideoData;

void *play_video(void *arg);

int play_from_cache(config conf);

int audio_callback(const void *input, void *output, unsigned long frameCount,
                   const PaStreamCallbackTimeInfo *timeInfo,
                   PaStreamCallbackFlags statusFlags, void *userData);

#endif