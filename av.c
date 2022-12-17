#include "av.h"

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/error.h>

#include "config.h"

int find_codec_context(config *conf, AVFormatContext **p_fmt_ctxt,
                       AVCodecContext **p_a_cdc, AVCodecContext **p_v_cdc,
                       int *p_a_idx, int *p_v_idx) {
    AVFormatContext *fmt_ctxt = NULL;
    AVCodecContext *a_cdc = NULL, *v_cdc = NULL;
    *p_a_idx = *p_v_idx = -1;

    // Try to open input.
    int err_code = avformat_open_input(&fmt_ctxt, conf->filename, NULL, NULL);
    // Error occurred.
    if (err_code != 0) {
        print_averror(err_code);
        return -2;
    }
    if (!fmt_ctxt) {
        printf("AVFormatContext is NULL\n");
        return -2;
    }

    // Try to get stream info from input.
    err_code = avformat_find_stream_info(fmt_ctxt, NULL);
    // Handle error.
    if (err_code < 0) {
        print_averror(err_code);
        return -2;
    }

    for (int i = 0; i < fmt_ctxt->nb_streams; i++) {
        AVCodecParameters *codec_param = fmt_ctxt->streams[i]->codecpar;
        const AVCodec *codec = avcodec_find_decoder(codec_param->codec_id);
        if (!codec) {
            continue;
        }
        AVCodecContext *cdc = NULL;
        cdc = avcodec_alloc_context3(codec);
        if (!cdc) {
            printf("Unable to allocate AVCodecContext\n");
            return -2;
        }
        int err = avcodec_parameters_to_context(cdc, codec_param);
        if (err < 0) {
            print_averror(err);
            return -2;
        }
        if (avcodec_open2(cdc, codec, NULL) < 0) {
            printf("Unable to initialize AVCodecContext\n");
            return -2;
        }
        switch (codec->type) {
            case AVMEDIA_TYPE_VIDEO:
                v_cdc = cdc;
                *p_v_idx = i;
                break;
            case AVMEDIA_TYPE_AUDIO:
                a_cdc = cdc;
                *p_a_idx = i;
                break;
            default:
                avcodec_free_context(&cdc);
                break;
        }
    }

    // Handle excpetions.
    if (!v_cdc) {
        printf("No video stream track found\n");
        return -3;
    }
    if (!a_cdc) {
        printf("No audio stream track found\n");
        conf->no_audio = 1;
        printf("--no-audio has been set to true\n");
    }

    (*p_fmt_ctxt) = fmt_ctxt;
    (*p_a_cdc) = a_cdc;
    (*p_v_cdc) = v_cdc;
    return 0;
}

void print_averror(int code) {
    char err[64];
    if (av_strerror(code, err, 64 - 1) < 0) {
        printf("Unknown AV Error(code: 0x%X)\n", code);
        return;
    }
    printf("AV Error: %s(code: 0x%X)\n", err, code);
}