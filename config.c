#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "args/args.h"

static char *str_rev(char *str) {
    for (int i = 0, j = strlen(str) - 1; i < j; i++, j--) {
        char tmp = str[i];
        str[i] = str[j];
        str[j] = tmp;
    }
    return str;
}

static config default_config() {
    config conf;
    conf.cache = NULL;
    conf.fps = 0;
    conf.filename = NULL;
    conf.help = 0;
    conf.license = 0;
    conf.no_audio = 0;
    conf.logfile = NULL;
    conf.log_level = LL_WARN;
    conf.height = conf.width = 100;
    conf.width--;
    char s[] = " .:-=+*#%%@";
    strcpy(conf.grey_ascii, s);
    conf.grey_ascii_step = (strlen(conf.grey_ascii) - 1) / 255.0;
    conf.video_ch = NULL;
    conf.audio_ch = NULL;
    conf.video_ch_status.lock = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    conf.video_ch_status.has_data = 0;
    conf.video_ch_status.drain_cond = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
    return conf;
}

config parse_config(int argc, char *argv[]) {
    if (argc < 2) {
        printf("No enough arguments\n");
        exit(1);
    }
    config conf = default_config();
    conf.filename = argv[1];

    arg_list al = new_arg_list();

    arg_list_add(&al, ARG_TYPE_FLAG, "help", 'h', "Print this help page");
    arg_list_add(&al, ARG_TYPE_FLAG, "license", 'l',
                 "Show license and author info");
    arg_list_add(&al, ARG_TYPE_STRING, "cache", 'c',
                 "Process video into a cached file");
    arg_list_add(&al, ARG_TYPE_FLAG, "no-audio", 'n',
                 "Play video without playing audio");
    arg_list_add(&al, ARG_TYPE_STRING, "grayscale", 'g', "Grayscale string");
    arg_list_add(&al, ARG_TYPE_FLAG, "reverse", 'r',
                 "Reverse grayscale string");
    arg_list_add(&al, ARG_TYPE_STRING, "log", '\0', "Path to log file");
    arg_list_add(&al, ARG_TYPE_NUMBER, "loglevel", '\0', "Log level");

    int err = parse_args(&al, argc, argv);
    if (err < 0) {
        printf("Arg error: %d %s\n", err, parse_args_err(err));
        exit(-1);
    }

    arg *a;
    if ((a = arg_list_search(&al, "help"))->set) conf.help = a->value.number;
    if ((a = arg_list_search(&al, "license"))->set)
        conf.license = a->value.number;
    if ((a = arg_list_search(&al, "cache"))->set) conf.cache = a->value.str;
    if ((a = arg_list_search(&al, "no-audio"))->set)
        conf.no_audio = a->value.number;
    if ((a = arg_list_search(&al, "grayscale"))->set) {
        strncpy(conf.grey_ascii, a->value.str, 256);
        conf.grey_ascii_step = (strlen(conf.grey_ascii) - 1) / 255.0;
    }
    conf.grey_ascii[256] = '\0';
    if ((a = arg_list_search(&al, "reverse"))->set && a->value.number)
        str_rev(conf.grey_ascii);
    if ((a = arg_list_search(&al, "log"))->set) conf.logfile = a->value.str;
    if ((a = arg_list_search(&al, "loglevel"))->set)
        conf.log_level = a->value.number;

    free_arg_list(&al);
    return conf;
}
