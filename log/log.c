#include "log.h"
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

static Logger default_logger = {NULL, LL_WARN, 1, 1, 1, 1, 1};

extern void logger_init(){
    Logger logger_init={stderr,LL_WARN,1,1,1,1,1};
}

Logger logger_get_default(){
    return default_logger;
}

void logger_set_default(const Logger logger){
    Logger *p;
    *p=logger;
    p->file=logger_get_default().file;
    p->log_level=logger_get_default().log_level;
    p->has_color=logger_get_default().has_color;
    p->has_date=logger_get_default().has_date;
    p->has_filename=logger_get_default().has_filename;
    p->has_linenum=logger_get_default().has_linenum;
    p->has_time=logger_get_default().has_time;
}


extern void logger_log_code(Logger logger, LogLevel ll, int err_code,
                            char *filename, int linenum, ...){
FILE *fp;
time_t now_time;
time(&now_time);

if(ll==default_logger.log_level){
    if((fp=fopen(logger.file,"r+"))==NULL)
    printf("File open error");
    else 
    fprintf(fp,"[%s] WARN %d:%d:...",ctime(&now_time),logger.has_filename,logger.has_linenum);
    fclose(fp);
}

if(ll==default_logger.log_level+1){
    if((fp=fopen(logger.file,"r+"))==NULL)
    printf("File open error");
    else 
    fprintf(fp,"[%s] ERROR %d:%d:...",ctime(&now_time),logger.has_filename,logger.has_linenum);
    fclose(fp);
}

if(ll==default_logger.log_level+2){
    if((fp=fopen(logger.file,"r+"))==NULL)
    printf("File open error");
    else 
    fprintf(fp,"[%s] FATAL %d:%d:...",ctime(&now_time),logger.has_filename,logger.has_linenum);
    fclose(fp);
    exit(err_code);
}
                            }

extern void logger_log_code_default(LogLevel ll, int err_code,
                            char *filename, int linenum, ...){
logger_log_code(default_logger,ll,err_code,filename,linenum);
                            }

