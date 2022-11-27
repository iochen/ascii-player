#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

static config default_config() {
    config conf;
    conf.cache = 0;
    conf.fps = 0;
    conf.filename = NULL;
    conf.help = 0;
    conf.no_audio = 0;
    getmaxyx(stdscr, conf.height, conf.width);
    conf.width--;
    strcpy(conf.grey_ascii, " .:-=+*#%@");
    conf.grey_ascii_step = strlen(conf.grey_ascii) / 256.0;
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
    // TODO: Parse other args
    return conf;
}