#include <stdio.h>
#include <stdlib.h>
#include "config.h"

config default_config() {
    config conf;
    conf.cache = 0;
    conf.fps = 0;
    conf.filename = NULL;
    conf.help = 0;
    conf.no_audio = 0;
    conf.height = 0;
    conf.width = 0;
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