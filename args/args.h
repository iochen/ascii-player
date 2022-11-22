#ifndef ARGS_H
#define ARGS_H

typedef enum { FLAG, NUMBER, STRING } arg_type;

typedef union {
    int flag;
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
extern void arg_list_add(arg_list *al, arg_type type, char *name,
                         char short_name, char *description);
extern arg *arg_list_search(arg_list *al, char *key);
extern int parse_args(arg_list *al, int argc, char *argv[]);
extern char *parse_args_err(int code);

#endif