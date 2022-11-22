#include <stdio.h>
#include <libavformat/avformat.h>
#include <libavutil/error.h>

#include "config.h"

void print_help();
void print_averror(int code);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_help();
        return -1;
    }
    config conf = parse_config(argc, argv);
    if (conf.help) {
        print_help();
        return 0;
    }

    // create and allocate AVFormatContext to store avformat info
    AVFormatContext *fmt_ctxt = avformat_alloc_context();
    if (!fmt_ctxt) {
        printf("Error allocating av format context\n");
        return -2;
    }

    // try to open file
    int err_code = avformat_open_input(&fmt_ctxt, conf.filename, NULL, NULL);
    if (err_code != 0) {
        print_averror(err_code);
        return -2;
    }

    // get stream info from file
    err_code = avformat_find_stream_info(fmt_ctxt, NULL);
    if (err_code < 0) {
        print_averror(err_code);
        return -2;
    }

    

    avformat_free_context(fmt_ctxt);
}

void print_averror(int code) {
    char err[64];
    int result = av_strerror(code, err, 63);
    err[63] = '\0';
    if (result < 0) {
        printf("Unknown av error(code: 0x%X)\n", code);
        return;
    }
    printf("%s(code: 0x%X)\n", code);
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
        "                            example: $ asciiplayer video.mp4 --cache cached.apc\n"
        "       --no-audio -n        Play video without playing audio\n"
        "       --fps -f <fps>       Play in <fps> frames per second."
        "                            Note: Please use a number that is a factor of file fps."
        "\n"
        );
}