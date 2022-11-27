#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "channel/channel.h"
#include "config.h"
void *play_video(void *arg) {
    config *conf = (config *)arg;
    uint8_t *data = NULL;
    int err;
    for (int count = 0;; count++) {
        if ((err = read_element(conf->video_ch, (void **)&data)) != 0) {
            printf("Error reading element(code: %d)\n", err);
            exit(2);
        }
        // printf("Read data = %d\n", count);
        clear();
        for (int i = 0; i < conf->height; i++) {
            for (int j = 0; j < conf->width; j++) {
                addch(conf->grey_ascii[(int)(conf->grey_ascii_step *
                                             data[i * conf->width + j])]);
            }
            addch('\n');
        }
        refresh();
        usleep(30000);
    }
    free(data);
}