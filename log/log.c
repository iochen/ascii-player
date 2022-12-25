#include "log.h"

#include <ncurses.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static Logger default_logger = {NULL, LL_WARN, 1, 1,
                                1,    1,       1, PTHREAD_MUTEX_INITIALIZER};

static const char *LogLevelStr[] = {"TRACE", "DEBUG", "INFO",
                                    "WARN",  "ERROR", "FATAL"};

void logger_init() {
    default_logger =
        (Logger){stderr, LL_WARN, 1, 1, 1, 1, 1, PTHREAD_MUTEX_INITIALIZER};
}

Logger logger_get_default() { return default_logger; }

void logger_set_default(const Logger logger) { default_logger = logger; }

extern void logger_log_code(Logger *logger, LogLevel ll, int err_code,
                            char *filename, int linenum, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    logger_log_code_va_list(logger, ll, err_code, filename, linenum, fmt, ap);
    va_end(ap);
}

extern void logger_log_code_default(LogLevel ll, int err_code, char *filename,
                                    int linenum, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    logger_log_code_va_list(&default_logger, ll, err_code, filename, linenum,
                            fmt, ap);
    va_end(ap);
}

void logger_log_code_va_list(Logger *logger, LogLevel ll, int err_code,
                             char *filename, int linenum, char *fmt,
                             va_list ap) {
    if (!logger || !logger->file) {
        return;
    }
    if (ll < logger->log_level) {
        return;
    }
    pthread_mutex_lock(&logger->lock);

    time_t now;
    struct tm now_info;
    now = time(&now);
    localtime_r(&now, &now_info);

    char buf[32];
    int buf_len = 0;
    buf[buf_len++] = '[';
    if (logger->has_date) {
        sprintf(buf + buf_len, "%04d-%02d-%02d", now_info.tm_year + 1900,
                now_info.tm_mon + 1, now_info.tm_mday);
        buf_len += 10;
    }
    if (logger->has_date && logger->has_time) {
        buf[buf_len++] = ' ';
    }
    if (logger->has_time) {
        sprintf(buf + buf_len, "%02d:%02d:%02d", now_info.tm_hour,
                now_info.tm_min, now_info.tm_sec);
        buf_len += 8;
    }
    buf[buf_len++] = ']';
    sprintf(buf + buf_len, " %5s ", LogLevelStr[ll]);
    buf_len += 7;
    fwrite(buf, buf_len, 1, logger->file);

    if (logger->has_filename) {
        fputs(filename, logger->file);
        if (logger->has_linenum) {
            fprintf(logger->file, ":%d", linenum);
        }
    }
    fputs(": ", logger->file);
    vfprintf(logger->file, fmt, ap);
    fputc('\n', logger->file);
    fflush(logger->file);
    pthread_mutex_unlock(&logger->lock);
    if (ll == LL_FATAL) {
        endwin();
        exit(err_code);
    }
}