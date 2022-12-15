#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libavutil/error.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <ncurses.h>
#include <portaudio.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "av.h"
#include "channel/channel.h"
#include "config.h"
#include "display.h"

void print_help();

// TODO: Add log module
// TODO: modulize some functions

int main(int argc, char *argv[]) {
    // Insucfficient program arguments.
    if (argc < 2) {
        print_help();
        return -1;
    }

    initscr();

    // Parse program arguments into config.
    config conf = parse_config(argc, argv);
    if (conf.help) {
        print_help();
        return 0;
    }

    // conf.height = conf.width = 100;


    // Create Audio and Video Fromat Context.
    AVFormatContext *fmt_ctxt = NULL;
    // Create Audio and Video CoDec Context.
    AVCodecContext *a_cdc = NULL, *v_cdc = NULL;
    // Store stream index.
    int a_idx = -1, v_idx = -1;

    // Find audio and video codec centext and stream index.
    int err =
        find_codec_context(&conf, &fmt_ctxt, &a_cdc, &v_cdc, &a_idx, &v_idx);
    if (err != 0) {
        return err;
    }

    // Allocate AVPacket and AVFrame.
    AVPacket *pckt = av_packet_alloc();
    if (!pckt) {
        printf("Unable to allocate AVPacket\n");
        return -2;
    }
    AVFrame *frame = av_frame_alloc();
    if (!frame) {
        printf("Unable to allocate AVFrame\n");
        return -2;
    }
    AVFrame *frame_grey = av_frame_alloc();
    if (!frame_grey) {
        printf("Unable to allocate AVFrame for greyscale frame\n");
        return -2;
    }
    frame_grey->width = conf.width;
    frame_grey->height = conf.height;

    struct SwsContext *sws_ctxt = sws_getContext(
        v_cdc->width, v_cdc->height, v_cdc->pix_fmt, frame_grey->width,
        frame_grey->height, AV_PIX_FMT_GRAY8, SWS_FAST_BILINEAR, 0, 0, 0);

    conf.video_ch = alloc_channel(10);
    conf.audio_ch = alloc_channel(3);
    pthread_t th_v;

    AVFrame *audio_frame = av_frame_alloc();
    if (!audio_frame) {
        printf("Unable to allocate AVFrame for audio frame\n");
        return -2;
    }
    SwrContext *resample_ctxt =  swr_alloc();
    if (!resample_ctxt) {
        printf("Unable to allocate AVAudioResampleContext\n");
        return -2;
    }
    audio_frame->channel_layout = AV_CH_LAYOUT_STEREO;
    audio_frame->sample_rate = a_cdc->sample_rate;
    audio_frame->format = AV_SAMPLE_FMT_FLT;

    PaStreamParameters pa_stm_param;
    PaStream *stream;
    err = Pa_Initialize();
    if (err != paNoError) {
        printf("PortAudio init error(code: %d).\n", err);
        return -20;
    }
    pa_stm_param.device = Pa_GetDefaultOutputDevice();
    if (pa_stm_param.device == paNoDevice) {
        printf("Can NOT find audio device.\n");
        return -20;
    }
    pa_stm_param.sampleFormat = paFloat32;
    pa_stm_param.channelCount = 2;
    pa_stm_param.suggestedLatency =
        Pa_GetDeviceInfo(pa_stm_param.device)->defaultLowOutputLatency;
    pa_stm_param.hostApiSpecificStreamInfo = NULL;
    err = Pa_OpenStream(&stream, NULL, /* no input */
                        &pa_stm_param, a_cdc->sample_rate, 1024,
                        paClipOff, /* we won't output out of range samples so
                                      don't bother clipping them */
                        audio_callback, &conf);
    if (err != paNoError) {
        printf("Error when opening audio stream.(code %d)\n", err);
        return -20;
    }
    Pa_StartStream(stream);

    int i = 0, started = 0;
    // While not the end of file.
    while (av_read_frame(fmt_ctxt, pckt) >= 0) {
        // If is video stream.
        if (pckt->stream_index == v_idx) {
            err = avcodec_send_packet(v_cdc, pckt);
            if (err < 0) {
                printf(
                    "Error when supplying raw packet data as input to video "
                    "decoder.(code: %d)\n",
                    err);
                return -10;
            }
            while (1) {
                err = avcodec_receive_frame(v_cdc, frame);
                if (err != 0) {
                    if (err == AVERROR(EAGAIN) || err == AVERROR_EOF) {
                        break;
                    }
                    printf("Failed when decoding video(code: %d)\n", err);
                    return -5;
                }
                frame_grey->data[0] =
                    (uint8_t *)av_malloc(av_image_get_buffer_size(
                        AV_PIX_FMT_GRAY8, conf.width, conf.height, 1));
                av_image_fill_arrays(frame_grey->data, frame_grey->linesize,
                                     frame_grey->data[0], AV_PIX_FMT_GRAY8,
                                     conf.width, conf.height, 1);
                sws_scale(sws_ctxt, (const uint8_t *const *)frame->data,
                          frame->linesize, 0, v_cdc->height, frame_grey->data,
                          frame_grey->linesize);
                add_element(conf.video_ch, frame_grey->data[0]);
                av_frame_unref(frame_grey);
                i++;
                if (i == 5) {
                    pthread_create(&th_v, NULL, play_video, &conf);
                }
            }
        } else if (pckt->stream_index == a_idx) {
            err = avcodec_send_packet(a_cdc, pckt);
            if (err < 0) {
                printf(
                    "Error when supplying raw packet data as input to audio "
                    "decoder.(code: %d)\n",
                    err);
                return -10;
            }
            while (1) {
                err = avcodec_receive_frame(a_cdc, frame);
                if (err != 0) {
                    if (err == AVERROR(EAGAIN) || err == AVERROR_EOF) {
                        break;
                    }
                    printf("Failed when decoding audio(code: %d)\n", err);
                    return -5;
                }
                err = swr_convert_frame(resample_ctxt, audio_frame, frame);
                if (err != 0) {
                    print_averror(err);
                    return -20;
                }
                add_element(conf.audio_ch, audio_frame->data[0]);
                audio_frame->data[0] = NULL;
                audio_frame->linesize[0] = 0;
            }
        }
        for (int i = 0;  i < 8; i++) {
            av_buffer_unref(frame->buf + i);
        }
        av_packet_unref(pckt);
        av_frame_unref(frame);
    }

    av_frame_free(&frame_grey);
    av_frame_free(&frame);
    av_packet_free(&pckt);
    avcodec_free_context(&a_cdc);
    avcodec_free_context(&v_cdc);
    avformat_free_context(fmt_ctxt);
}

void print_help() {
    printf(
        "ASCII Player v0.0.1-dev\n"
        "\n"
        "Usage: asciiplayer <file> [-h | --help] [--cache <file>]\n"
        "                          [-n | --no-audio] [-f <fps> | --fps <fps>]\n"
        "\n"
        "       --help -h            Print this help page\n"
        "       --cache -c <file>    Process video into a cached file\n"
        "                            example: $ asciiplayer video.mp4 --cache "
        "cached.apc\n"
        "       --no-audio -n        Play video without playing audio\n"
        "       --fps -f <fps>       Play in <fps> frames per second.\n"
        "                            Note: Please use a number that is a "
        "factor of file fps.\n"
        "\n");
}