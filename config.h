#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    // filename can NOT be NULL or empty
    char *filename;
    // as a bool value
    int help;
    // NULL for argument not supplied
    char *cache;
    // as a bool value
    int no_audio;
    // fps <= 0 for srgument not supplied
    int fps;
    int width;
    int height;
} config;

config parse_config(int argc, char *argv[]);

#endif