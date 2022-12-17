#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
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
    conf.cache = 0;
    conf.fps = 0;
    conf.filename = NULL;
    conf.help = 0;
    conf.license = 0;
    conf.no_audio = 0;

    getmaxyx(stdscr, conf.height, conf.width);
    conf.width--;
    char s[] = " .:-=+*#%@";
    strcpy(conf.grey_ascii,s);
    conf.grey_ascii_step = (strlen(conf.grey_ascii) - 1) / 255.0;
    conf.video_ch = NULL;
    conf.audio_ch = NULL;
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
    arg_list_add(&al, ARG_TYPE_FLAG, "license", 'l', "Show license and author info");
    arg_list_add(&al, ARG_TYPE_STRING, "cache", 'c', "Process video into a cached file");
    arg_list_add(&al, ARG_TYPE_FLAG, "no-audio", 'n', "Play video without playing audio");
    int err = parse_args(&al, argc, argv);
    if (err < 0) {
        printf("Arg error: %d %s\n", err, parse_args_err(err));
        exit(-1);
    }

    arg *a;
    if ((a = arg_list_search(&al, "help"))->set) conf.help = a->value.number; 
    if ((a = arg_list_search(&al, "license"))->set) conf.license = a->value.number; 
    if ((a = arg_list_search(&al, "cache"))->set) conf.cache = a->value.str; 
    if ((a = arg_list_search(&al, "no-audio"))->set) conf.no_audio = a->value.number; 
    // endwin();

    // printf("help: %d\n", conf.help);
    //     printf("license: %d\n", conf.license);
    // printf("cache: %s\n", conf.cache);
    // printf("no-audio: %d\n", conf.no_audio);
    // exit(0);
    return conf;
}

