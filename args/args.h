#ifndef ARGS_H
#define ARGS_H

typedef enum { ARG_TYPE_FLAG, ARG_TYPE_NUMBER, ARG_TYPE_STRING } arg_type;

typedef union {
    int number;
    char *str;
} arg_value;

typedef struct {
    char *name;
    char short_name;
    char *description;
    int set;
    arg_type type;
    arg_value value;
} arg;

typedef struct {
    int arg_num;
    arg *args;
} arg_list;

extern arg_list new_arg_list();
extern void free_arg_list(arg_list *al);
extern void arg_list_add(arg_list *al, arg_type type, char *name,
                         char short_name, char *description);
extern arg *arg_list_search(arg_list *al, char *key);
extern int parse_args(arg_list *al, int argc, char *argv[]);
extern char *parse_args_err(int code);

#endif