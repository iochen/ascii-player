#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <portaudio.h>
#include <libavutil/mem.h>
#include "channel/channel.h"
#include "config.h"

#define DP_MIN(A, B) ((A) < (B) ? (A) : (B))

void *play_video(void *arg) {
    config *conf = (config *)arg;
    uint8_t *data = NULL;
    int err;
    for (int count = 0;; count++) {
        if ((err = read_element(conf->video_ch, (void **)&data)) != 0) {
            printf("Error reading element(code: %d)\n", err);
            exit(2);
        }
        // printf("Read data = %d\n", count);
        clear();
        for (int i = 0; i < conf->height; i++) {
            for (int j = 0; j < conf->width; j++) {
                addch(conf->grey_ascii[(int)(conf->grey_ascii_step *
                                             data[i * conf->width + j])]);
            }
            addch('\n');
        }
        refresh();
        av_free(data);
    }
}

int audio_callback(const void *input, void *output, unsigned long frameCount,
                   const PaStreamCallbackTimeInfo *timeInfo,
                   PaStreamCallbackFlags statusFlags, void *userData) {
                static float *last_buf = NULL;
                config *conf = (config *)userData;
                static unsigned long last_remain = 0;
                float *out = (float *)output;
                if (last_buf) {
                    unsigned long to_be_read = DP_MIN(last_remain, frameCount);
                    for (unsigned long i = 0; i < to_be_read * 2; i++) {
                        *out++ = last_buf[2048 - 2 * last_remain + i];
                    }
                    last_remain -= to_be_read;
                    frameCount -= to_be_read;
                    if (last_remain == 0) {
                        free(last_buf);
                        last_buf = NULL;
                    }
                }
                if (frameCount > 0) {
                        float *buf = NULL;
                        int err = read_element(conf->audio_ch, (void **)&buf);
                        if (err != 0) {
                            printf("Error!!!!!(code: %d)\n", err);
                        }
                        for (unsigned long i = 0; i < frameCount * 2; i++) {
                            *out++ = buf[i];
                        }
                        last_buf = buf;
                        last_remain = 1024 - frameCount;
                }
                return paContinue;

};