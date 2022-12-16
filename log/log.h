#ifndef LOG_H
#define LOG_H

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

typedef struct {
    // Default: stderr
    FILE *file;
    // Default: WARN
    LogLevel log_level;
    // Default: 1
    int has_color;
    // Default: 1
    int has_date;
    // Default: 1
    int has_time;
    // Default: 1
    int has_filename;
    // Default: 1
    int has_linenum;
} Logger;

// Initialize default_logger to default value
extern void logger_init();

Logger logger_get_default();

void logger_set_default(const Logger logger);

extern void logger_log_code(Logger logger, LogLevel ll, int err_code,
                            char *filename, int linenum, ...);

extern void logger_log_code_default(LogLevel ll, int err_code,
                            char *filename, int linenum, ...);

#define logger_log(logger, ll, filename, linenum, ...) \
    logger_log_code(logger, ll, LOG_DEFAULT_CODE, filename, linenum, ...)

#define logger_log_default(ll, filename, linenum, ...) \
    logger_log_code_default(ll, LOG_DEFAULT_CODE, filename, linenum, ...)

#define fatal(err_code, ...) \
    logger_log_code_default(LL_FATAL, err_code, __FILE__, __LINE__, __VA_ARGS__);
#define error(...) \
    logger_log_default(LL_ERROR, __FILE__, __LINE__, __VA_ARGS__);
#define warn(...) \
    logger_log_default(LL_WARN, __FILE__, __LINE__, __VA_ARGS__);
#define info(...) \
    logger_log_default(LL_INFO, __FILE__, __LINE__, __VA_ARGS__);
#define debug(...) \
    logger_log_default(LL_DEBUG, __FILE__, __LINE__, __VA_ARGS__);
#define trace(...) \
    logger_log_default(LL_TRACE, __FILE__, __LINE__, __VA_ARGS__);

#define log_fatal(logger, err_code, ...) \
    logger_log_code(logger, LL_FATAL, err_code, __FILE__, __LINE__, __VA_ARGS__);
#define log_error(logger, ...) \
    logger_log(logger, LL_ERROR, __FILE__, __LINE__, __VA_ARGS__);
#define log_warn(logger, ...) \
    logger_log(logger, LL_WARN, __FILE__, __LINE__, __VA_ARGS__);
#define log_info(logger, ...) \
    logger_log(logger, LL_INFO, __FILE__, __LINE__, __VA_ARGS__);
#define log_debug(logger, ...) \
    logger_log(logger, LL_DEBUG, __FILE__, __LINE__, __VA_ARGS__);
#define log_trace(logger, ...) \
    logger_log(logger, LL_TRACE, __FILE__, __LINE__, __VA_ARGS__);

#endif