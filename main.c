#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/error.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

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

    // Initialize libavformat and register all the muxers, demuxers and
    // protocols.
    av_register_all();
    avcodec_register_all();

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
    pthread_t th_v, th_a;
    int i = 0, started = 0;
    // While not the end of file.
    while (av_read_frame(fmt_ctxt, pckt) >= 0) {
        // If is video stream.
        if (pckt->stream_index == v_idx) {
            err = avcodec_send_packet(v_cdc, pckt);
            if (err < 0) {
                printf(
                    "Error when supplying raw packet data as input to a "
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
                // printf("added i = %d\n", i++);
                i++;
                if (i == 5) {
                    pthread_create(&th_v, NULL, play_video, &conf);
                }
            }
        } else if (pckt->stream_index == a_idx) {
        }
        av_packet_unref(pckt);
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