#ifndef LOG_H
#define LOG_H

#include <pthread.h>
#include <stdio.h>

#define LOG_DEFAULT_CODE -1

typedef enum {
    LL_TRACE,
    LL_DEBUG,
    LL_INFO,
    LL_WARN,
    LL_ERROR,
    LL_FATAL,
} LogLevel;

// [2022-12-30 13:23:45.123] WARN test1.c:123: Example message.
// [2022-12-30 13:23:47.456] WARN test2.c:123: Example message.
// [2022-12-30 13:23:50] WARN test3.c:123: Example message.
// [2022-12-30 13:23:55] WARN test4.c:123: Example message.
typedef struct {
    // Default: stderr
    FILE *file;
    // Default: WARN
    LogLevel log_level;
    // Default: 1
    // TODO: Add this feature in the future
    int has_color;
    // Default: 1
    int has_date;
    // Default: 1
    int has_time;
    // Default: 1
    int has_filename;
    // Default: 1
    int has_linenum;

    pthread_mutex_t lock;
} Logger;

// Initialize default_logger to default value
extern void logger_init();

extern Logger logger_get_default();

extern void logger_set_default(const Logger logger);

void logger_log_code_va_list(Logger *logger, LogLevel ll, int err_code,
                             char *filename, int linenum, char *fmt,
                             va_list ap);

extern void logger_log_code(Logger *logger, LogLevel ll, int err_code,
                            char *filename, int linenum, char *fmt, ...);

extern void logger_log_code_default(LogLevel ll, int err_code, char *filename,
                                    int linenum, char *fmt, ...);

#define logger_log(logger, ll, filename, linenum, fmt, ...)               \
    logger_log_code(logger, ll, LOG_DEFAULT_CODE, filename, linenum, fmt, \
                    ##__VA_ARGS__)

#define logger_log_default(ll, filename, linenum, fmt, ...)               \
    logger_log_code_default(ll, LOG_DEFAULT_CODE, filename, linenum, fmt, \
                            ##__VA_ARGS__)

#define lfatal(err_code, fmt, ...)                                       \
    logger_log_code_default(LL_FATAL, err_code, __FILE__, __LINE__, fmt, \
                            ##__VA_ARGS__)
#define lerror(fmt, ...) \
    logger_log_default(LL_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define lwarn(fmt, ...) \
    logger_log_default(LL_WARN, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define linfo(fmt, ...) \
    logger_log_default(LL_INFO, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define ldebug(fmt, ...) \
    logger_log_default(LL_DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define ltrace(fmt, ...) \
    logger_log_default(LL_TRACE, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define log_fatal(logger, err_code, fmt, ...)                            \
    logger_log_code(logger, LL_FATAL, err_code, __FILE__, __LINE__, fmt, \
                    ##__VA_ARGS__)
#define log_error(logger, fmt, ...) \
    logger_log(logger, LL_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define log_warn(logger, fmt, ...) \
    logger_log(logger, LL_WARN, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define log_info(logger, fmt, ...) \
    logger_log(logger, LL_INFO, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define log_debug(logger, fmt, ...) \
    logger_log(logger, LL_DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define log_trace(logger, fmt, ...) \
    logger_log(logger, LL_TRACE, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#endif