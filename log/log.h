#ifndef LOG_H
#define LOG_H

#include <stdbool.h>
#include <stdio.h>

typedef enum {
    LL_PANIC,
    LL_ERROR,
    LL_WARN,
    LL_INFO,
    LL_DEBUG,
    LL_TRACE,
} LogLevel;

typedef struct {
    FILE *file;
    LogLevel log_level;
    bool color;
} Logger;

extern Logger default_logger;

extern int logger_init_custom(Logger *logger, FILE *file, LogLevel ll, bool color);
extern int logger_close_custom(Logger *Logger);

extern int logger_init(FILE *file, LogLevel ll, bool color);
extern int logger_close();

extern int logger_log(Logger logger, LogLevel ll, char *filename, int linenum, ...);

#define panic(...) \
    logger_log(default_logger, LL_PANIC, __FILE__, __LINE__, __VA_ARGS__);
#define error(...) \
    logger_log(default_logger, LL_ERROR, __FILE__, __LINE__, __VA_ARGS__);
#define warn(...) \
    logger_log(default_logger, LL_WARN, __FILE__, __LINE__, __VA_ARGS__);
#define info(...) \
    logger_log(default_logger, LL_INFO, __FILE__, __LINE__, __VA_ARGS__);
#define debug(...) \
    logger_log(default_logger, LL_DEBUG, __FILE__, __LINE__, __VA_ARGS__);
#define trace(...) \
    logger_log(default_logger, LL_TRACE, __FILE__, __LINE__, __VA_ARGS__);

#define log_panic(logger, ...) \
    logger_log(logger, LL_PANIC, __FILE__, __LINE__, __VA_ARGS__);
#define log_error(logger, ...) \
    logger_log(logger, LL_PANIC, __FILE__, __LINE__, __VA_ARGS__);
#define log_warn(logger, ...) \
    logger_log(logger, LL_PANIC, __FILE__, __LINE__, __VA_ARGS__);
#define log_info(logger, ...) \
    logger_log(logger, LL_PANIC, __FILE__, __LINE__, __VA_ARGS__);
#define log_debug(logger, ...) \
    logger_log(logger, LL_PANIC, __FILE__, __LINE__, __VA_ARGS__);
#define log_trace(logger, ...) \
    logger_log(logger, LL_PANIC, __FILE__, __LINE__, __VA_ARGS__);

#endif