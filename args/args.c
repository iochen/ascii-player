#include "args.h"

#include <stdlib.h>
#include <string.h>

#include "parse.h"

arg_list new_arg_list() {
    arg_list al;
    al.arg_num = 0;
    al.args = NULL;
    return al;
}

void arg_list_add(arg_list *al, arg_type type, char *name, char short_name,
                  char *description) {
    al->args = realloc(al->args, ++al->arg_num * sizeof(arg));
    al->args[al->arg_num - 1] =
        (arg){name, short_name, description, 0, type, NULL};
}

arg *arg_list_search(arg_list *al, char *key) {
    // validate params
    if (!key || !al) {
        return NULL;
    }

    int len = al->arg_num, key_len = strlen(key);

    // trim '-'(s) in the leading of the key
    while (key_len > 0 && key[0] == '-') {
        key++;
        key_len--;
    }
    // if key is empty
    if (key_len < 1) {
        return NULL;
    }

    for (int i = 0; i < len; i++) {
        if (al->args[i].short_name == key[0] ||
            strcasecmp(al->args[i].name, key) == 0) {
            return &al->args[i];
        }
    }
}

int parse_args(arg_list *al, int argc, char *argv[]) {
    int parsed = 0;
    // validate param
    if (!argc || !argv) {
        return 0;
    }
    // skip program name
    argv++;
    argc--;
    for (int i = 0; i < argc; i++) {
        if (argv[i][0] == '-') {
            arg *ag = arg_list_search(al, argv[i]);
            // if arg type is a flag
            if (ag->type == FLAG) {
                // if has value
                if (i < argc - 1 && argv[i + 1][0] != '-') {
                    int b_value = parse_bool(argv[i + 1]);
                    if (b_value < 0) {
                        return -1;
                    }
                    ag->set = 1;
                    ag->value = (arg_value)b_value;
                    i++;
                } else {
                    ag->set = 1;
                    ag->value = (arg_value)1;
                }
            } else if (ag->type == NUMBER) {
                if (i < argc - 1) {
                    return -2;
                }
                int succ = 0;
                int i_value = parse_int(argv[i + 1], &succ);
                if (!succ) {
                    return -3;
                }
                ag->set = 1;
                ag->value = (arg_value)i_value;
                i++;
            } else if (ag->type == STRING) {
                if (i < argc - 1) {
                    return -4;
                }
                ag->set = 1;
                ag->value = (arg_value)argv[i + 1];
                i++;
            } else {
                return -5;
            }
            parsed++;
        }
    }
    return parsed;
};

char *parse_args_err(int code) {
    switch (code) {
        case -1:
            return "Bool typed arg has an unknown value";
        case -2:
            return "No value after a number typed arg";
        case -3:
            return "Unable to parse int";
        case -4:
            return "No value after a number typed arg";
        case -5:
            return "Unknown arg type";
        default:
            return "Undefined error";
    }
}