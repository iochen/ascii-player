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
#define CACHE_AUDIO_BUF_SIZE 512

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

int play_from_cache(config conf) {
    APCache *apc = NULL;
    int err;
    if ((err = apcache_open(conf.filename, &apc)) != 0) {
        printf("Error when open file. (code: %d)\n", err);
        return -1;
    }
    conf.fps = apc->fps;
    conf.no_audio = !apc->sample_rate;
    conf.width = apc->width;
    conf.height = apc->height;

    // PortAudio Stream Params
    PaStreamParameters pa_stm_param;
    // PortAudio Stream
    PaStream *stream;
    // If need audio and not cache
    if (!conf.no_audio) {
        // Initialize PortAudio
        err = Pa_Initialize();
        if (err != paNoError) {
            printf("PortAudio init error(code: %d).\n", err);
            return -20;
        }
        // Get output device
        pa_stm_param.device = Pa_GetDefaultOutputDevice();
        if (pa_stm_param.device == paNoDevice) {
            printf("Can NOT find audio device.\n");
            return -20;
        }
        // Initialize other fields in pa_stm_param
        pa_stm_param.sampleFormat = paFloat32;
        pa_stm_param.channelCount = 2;
        pa_stm_param.suggestedLatency =
            Pa_GetDeviceInfo(pa_stm_param.device)->defaultLowOutputLatency;
        pa_stm_param.hostApiSpecificStreamInfo = NULL;
        // Open audio stream
        err = Pa_OpenStream(&stream, NULL, &pa_stm_param, apc->sample_rate,
                            CACHE_AUDIO_BUF_SIZE, paClipOff, NULL, NULL);
        if (err != paNoError) {
            printf("Error when opening audio stream.(code %d)\n", err);
            return -20;
        }
    }

    // Allocate video channel
    conf.video_ch = alloc_channel(10);

    // Video thread
    pthread_t th_v;

    int image_count = 0, audio_count = 0;
    APFrame *apf = NULL;
    // While not the end of file.
    while ((err = apcache_read_frame(apc, &apf)) == 0) {
        if (apf->type == APAV_VIDEO) {
            add_element(conf.video_ch, apf->data);
            apf->data = NULL;
            if (++image_count == 1) {
                pthread_create(&th_v, NULL, play_video, &conf);
            }
        } else if (apf->type == APAV_AUDIO) {
                if (++audio_count == 1) {
                    Pa_StartStream(stream);
                }
                    // Write data into stream
                    Pa_WriteStream(stream, apf->data,
                                   apf->bsize / (2 * sizeof(float)));
            }
    }
    if (err != 0 && err != APCACHE_ERR_EOF) {
        printf("Error when reading frame: %d\n", err);
        return -1;
    }

    // Wait for video cahnnel empty
    // TODO: use pthread_cond instead of sleep
    sleep(1);
        endwin();

    // Free video channel
    free_channel(conf.video_ch);
    av_frame_free(&apf);
    // Close PortAudio stream
    Pa_CloseStream(stream);
    apcache_close(apc);
    apcache_free(&apc);
    return 0;
}