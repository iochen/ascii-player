#include "parse.h"

#include <string.h>

int parse_bool(const char *value) {
    int value_len = strlen(value);
    // one char value
    if (value_len == 1) {
        if (value[0] == '1' || value[0] == 't' || value[0] == 'T') {
            return 1;
        } else if (value[0] == '0' || value[0] == 'f' || value[0] == 'F') {
            return 0;
        } else {
            return -1;
        }
        // two char value
    } else if (value_len == 2) {
        if (strcasecmp(value, "on") == 0) {
            return 1;
        } else if (strcasecmp(value, "no") == 0) {
            return 0;
        } else {
            return -1;
        }
        // three char value
    } else if (value_len == 3) {
        if (strcasecmp(value, "yes") == 0) {
            return 1;
        } else if (strcasecmp(value, "off") == 0) {
            return 0;
        } else {
            return -1;
        }
        // four char value
    } else if (value_len == 4) {
        if (strcasecmp(value, "true") == 0) {
            return 1;
        } else {
            return -1;
        }
        // five char value
    } else if (value_len == 5) {
        if (strcasecmp(value, "false") == 0) {
            return 0;
        } else {
            return -1;
        }
    } else {
        return -1;
    }
    return -1;
}

int parse_int(const char *value, int *succ) {
    int num = 0, sgn = 1;
    if (value[0] == '-') {
        sgn = -1;
        value++;
    } else if (value[0] == '+') {
        value++;
    }
    for (int i = 0; value[i] != '\0'; i++) {
        if (value[i] >= '0' && value[i] <= '9')
            num = num * 10 + value[i] - '0';
        else {
            *succ = 0;
            return 0;
        }
    }
    *succ = 1;
    return sgn * num;
}
