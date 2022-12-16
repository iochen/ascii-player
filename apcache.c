#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "apcache.h"

/// @brief Allocate an apcache frame object.
/// @param type APAVType
/// @param bsize size of data array (in bytes)
/// @return The pointer to allocated object.
APFrame *apcache_frame_alloc(APAVType type, unsigned int bsize) {
    void *data = malloc(bsize);
    if (!data) {
        return NULL;
    }
    APFrame *frame = (APFrame *)malloc(sizeof(APFrame));
    if (!frame) {
        free(data);
        return NULL;
    }
    frame->type = type;
    frame->bsize = bsize;
    frame->data = data;
    return frame;
}

/// @brief Free all fields allocated on heap in frame, and free frame it self.
/// @param frame The pointer to the pointer to the frame object.
/// @note (*frame) will be set to NULL.
void apcache_frame_free(APFrame **frame) {
    if (!frame || !(*frame)) {
        return;
    }
    free((**frame).data);
    free(*frame);
    *frame = NULL;
}

/// @brief Allocate an apcache object with all fields set to default.
/// @return The pointer to allocated object.
APCache *apcache_alloc(){
    APCache *apc = (APCache *)malloc(sizeof(APCache));
    if (!apc) {
        free(apc);
        return NULL;
    }
    apc->version = APCACHE_VERSION;
    apc->fps =0;
    apc->width =0;
    apc->height=0;
    apc->sample_rate=0;
    apc->file=NULL;
    return apc;
}

/// @brief Free all fields allocated on heap in apc, and free apc it self.
/// @param apc The pointer to the pointer to the APCache object.
/// @note (*apc) will be set to NULL.
void apcache_free(APCache **apc){
    if (!apc || !(*apc)) {
        return;
    }
    free((**apc).file);
    free(*apc);
    *apc = NULL;
}

/// @brief Write meta data to apcache file
/// @param apc APCache struct with fps, width, height, sample_rate set to target number,
///            file pointed to a opened FILE with mode set to "w",
///            version set to target version (1).
/// @return 0 for success, minus number for APCacheErr
int apcache_create(APCache *apc){
    if(!apc)
        return APCACHE_ERR_APCACHE_NULL;
    if(!apc->file)
        return APCACHE_ERR_FILE_NOT_EXIST;
    if(apc->version!=APCACHE_VERSION)
        return APCACHE_ERR_UNKNOWN_VERSION;
    fputs("apache\n",apc->file);
    fwrite(&apc->version,sizeof(int32_t),1,apc->file);
    fwrite(&apc->fps,sizeof(uint32_t),1,apc->file);
    fwrite(&apc->width,sizeof(uint32_t),1,apc->file);
    fwrite(&apc->height,sizeof(uint32_t),1,apc->file);
    fwrite(&apc->sample_rate,sizeof(uint32_t),1,apc->file);
    fflush(apc->file);
    return 0;
}

/// @brief Write a frame into apcache file
/// @param apc The pointer to the APCache object.
/// @param frame Frame to be written to the file
/// @return 0 for success, minus number for APCacheErr
int apcache_write_frame(APCache *apc, APFrame *frame){
    if(!apc)
        return APCACHE_ERR_APCACHE_NULL;
    if(!apc->file)
        return APCACHE_ERR_FILE_NOT_EXIST;
    if(apc->version!=APCACHE_VERSION)
        return APCACHE_ERR_UNKNOWN_VERSION;
    if(!frame)
        return APCACHE_ERR_FRAME_NOT_EXIST;
    if(frame->type!=APAV_AUDIO&&frame->type!=APAV_VIDEO)
        return APCACHE_ERR_UNKNOWN_FORMAT;
    fwrite(&frame->type,sizeof(uint8_t),1,apc->file);
    fwrite(&frame->bsize,sizeof(uint32_t),1,apc->file);
    fwrite(&frame->data,frame->bsize,1,apc->file);
    fflush(apc->file);
    return 0;
}

/// @brief Check whether is an apcache file.
///        1. Check whether file exists
///        2. Check whether have permission to read
///        3. Check whether file content starts with "apcache\n"
///        4. Check if version number is valid (Only 1 is valid currently)
/// @param filename path to apcache file
/// @return 0 for success, minus number for APCacheErr
int is_apcache(char *filename){
    if(!filename)
        return APCACHE_ERR_FILE_NOT_EXIST;
    FILE *fp;
    char *p;
    fp=fopen(*filename,"r");
    if(!fp)
        return APCACHE_ERR_PERMISSION_DENIED;
    fgets(p,7,fp);
    if(!strcmp(p,"apache\n"))
        return APCACHE_ERR_UNKNOWN_FORMAT;
    int32_t version;
    fread(&version,sizeof(int32_t),1,fp);
    if(version!=APCACHE_VERSION)
        return APCACHE_ERR_UNKNOWN_VERSION;
    return 0;
}

/// @brief Open an apcache file in read mode.
/// @param filename path to apcache file
/// @return 0 for success, minus number for APCacheErr
int apcache_open(char *filename, APCache **apcadd){
    if(!filename)
        return APCACHE_ERR_FILE_NOT_EXIST;
    FILE *fp;
    fp=fopen(*filename,"r");
    if(!fp)
        return APCACHE_ERR_PERMISSION_DENIED;
    APCache *apc;
    apc=apcache_alloc();
    if(!apc)
        return APCACHE_ERR_APCACHE_NULL;
    *apcadd=apc;
    return 0;
}

/// @brief Read a APFrame from APCache
/// @param apc APCache
/// @param frame The pointer to a pointer to APFrame
/// @return 0 for success, minus number for APCacheErr
int apcache_read_frame(APCache *apc, APFrame **frame){
    if(!apc)
        return APCACHE_ERR_APCACHE_NULL;
    if(!apc->file)
        return APCACHE_ERR_FILE_NOT_EXIST;
    int n;
    n=fread();

    if(n==0&&feof(apc->file))
        return APCACHE_ERR_EOF;
    //If the file has come to the end, return -1
}

int apcache_close(APCache *apc){
    if(!apc)
        return APCACHE_ERR_APCACHE_NULL;
    fclose(apc->file);
}