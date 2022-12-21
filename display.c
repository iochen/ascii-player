#include "display.h"

#include <libavutil/frame.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "channel/channel.h"
#include "config.h"

#define DP_MIN(A, B) ((A) < (B) ? (A) : (B))

void *play_video(void *arg) {
    config *conf = (config *)arg;
    unsigned char *data = NULL;
    int err;

    int dur_u = 1000000 / conf->fps;

    struct timeval start;
    gettimeofday(&start, NULL);

    for (int count = 0;; count++) {
        if ((err = read_element(conf->video_ch, (void **)&data)) != 0) {
            printf("Error reading element(code: %d)\n", err);
            exit(2);
        }
        if (conf->no_audio) {
            struct timeval now;
            gettimeofday(&now, NULL);
            int pause_dur_u = dur_u * count -
                              (now.tv_sec - start.tv_sec) * 1000000 -
                              (now.tv_usec - start.tv_usec);
            if (pause_dur_u > 0) {
                usleep(pause_dur_u);
            }
        }
        clear();
        for (int i = 0; i < conf->height; i++) {
            for (int j = 0; j < conf->width; j++) {
                addch(conf->grey_ascii[(int)(conf->grey_ascii_step *
                                             data[i * conf->width + j])]);
            }
            addch('\n');
        }
        refresh();
        free(data);
    }
}

// It's not safe to use free int callback functions,
// so we use blocking method to play audio,
// instead of using an interrupt method
// int audio_callback(const void *input, void *output, unsigned long frameCount,
//                    const PaStreamCallbackTimeInfo *timeInfo,
//                    PaStreamCallbackFlags statusFlags, void *userData) {

//                 config *conf = (config *)userData;
//                 static float *last_ptr = NULL;
//                 static float *last_buf = NULL;
//                 static unsigned long last_remain = 0;

//                 float *out = (float *)output;

//                 while(frameCount > 0) {
//                     if (last_remain == 0) {
//                         free(last_buf);
//                         APAudioData *apad;
//                         int err = read_element(conf->audio_ch, (void
//                         **)&apad); if (err != 0) {
//                             printf("Error!!!!!(code: %d)\n", err);
//                         }
//                         last_ptr = last_buf = apad->data;
//                         last_remain = apad->nb_samples;
//                         free(apad);
//                     }
//                     unsigned long to_be_read = DP_MIN(last_remain,
//                     frameCount); for (unsigned long i = 0; i < to_be_read *
//                     2; i++) {
//                         *out++ = *last_ptr++;
//                     }
//                     last_remain -= to_be_read;
//                     frameCount -= to_be_read;
//                 }

//                 // if (last_buf) {
//                 //     unsigned long to_be_read = DP_MIN(last_remain,
//                 frameCount);
//                 //     for (unsigned long i = 0; i < to_be_read * 2; i++) {
//                 //         *out++ = last_buf[(nb_samples - last_remain) * 2 +
//                 i];
//                 //     }
//                 //     last_remain -= to_be_read;
//                 //     frameCount -= to_be_read;
//                 //     if (last_remain == 0) {
//                 //         free(last_buf);
//                 //         last_buf = NULL;
//                 //     }
//                 // }
//                 // if (frameCount > 0) {
//                 //         float *buf = NULL;
//                 //         int err = read_element(conf->audio_ch, (void
//                 **)&buf);
//                 //         if (err != 0) {
//                 //             printf("Error!!!!!(code: %d)\n", err);
//                 //         }
//                 //         for (unsigned long i = 0; i < frameCount * 2; i++)
//                 {
//                 //             *out++ = buf[i];
//                 //         }
//                 //         last_buf = buf;
//                 //         last_remain = nb_samples - frameCount;
//                 // }
//                 return paContinue;

// };