#include "apcache.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
APCache *apcache_alloc() {
    APCache *apc = (APCache *)malloc(sizeof(APCache));
    if (!apc) {
        return NULL;
    }
    apc->version = APCACHE_VERSION;
    apc->fps = 0;
    apc->width = 0;
    apc->height = 0;
    apc->sample_rate = 0;
    apc->file = NULL;
    return apc;
}

/// @brief Free all fields allocated on heap in apc, and free apc it self.
/// @param apc The pointer to the pointer to the APCache object.
/// @note (*apc) will be set to NULL.
void apcache_free(APCache **apc) {
    if (!apc || !(*apc)) {
        return;
    }
    free(*apc);
    *apc = NULL;
}

/// @brief Write meta data to apcache file
/// @param apc APCache struct with fps, width, height, sample_rate set to target
/// number,
///            file pointed to a opened FILE with mode set to "w",
///            version set to target version (1).
/// @return 0 for success, minus number for APCacheErr
int apcache_create(APCache *apc) {
    if (!apc) return APCACHE_ERR_APCACHE_NULL;
    if (!apc->file) return APCACHE_ERR_FILE_NOT_EXIST;
    if (apc->version != APCACHE_VERSION) return APCACHE_ERR_UNKNOWN_VERSION;
    fputs("apcache\n", apc->file);
    fwrite(&apc->version, sizeof(int32_t), 1, apc->file);
    fwrite(&apc->fps, sizeof(uint32_t), 1, apc->file);
    fwrite(&apc->width, sizeof(uint32_t), 1, apc->file);
    fwrite(&apc->height, sizeof(uint32_t), 1, apc->file);
    fwrite(&apc->sample_rate, sizeof(uint32_t), 1, apc->file);
    fflush(apc->file);
    return 0;
}

/// @brief Write a frame into apcache file
/// @param apc The pointer to the APCache object.
/// @param frame Frame to be written to the file
/// @return 0 for success, minus number for APCacheErr
int apcache_write_frame(APCache *apc, APFrame *frame) {
    if (!apc) return APCACHE_ERR_APCACHE_NULL;
    if (!apc->file) return APCACHE_ERR_FILE_NOT_EXIST;
    if (apc->version != APCACHE_VERSION) return APCACHE_ERR_UNKNOWN_VERSION;
    if (!frame) return APCACHE_ERR_FRAME_NOT_EXIST;
    if (frame->type != APAV_AUDIO && frame->type != APAV_VIDEO)
        return APCACHE_ERR_UNKNOWN_FORMAT;
    uint8_t type = frame->type;
    fwrite(&type, sizeof(uint8_t), 1, apc->file);
    fwrite(&frame->bsize, sizeof(uint32_t), 1, apc->file);
    fwrite(frame->data, frame->bsize, 1, apc->file);
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
int is_apcache(char *filename) {
    if (!filename) return APCACHE_ERR_FILE_NOT_EXIST;
    FILE *fp;
    fp = fopen(filename, "r");
    if (!fp) return APCACHE_ERR_PERMISSION_DENIED;
    char buf[8 + 1];
    if (!fgets(buf, 8 + 1, fp)) {
        return APCACHE_ERR_EOF;
    }
    if (strcmp(buf, "apcache\n") != 0) return APCACHE_ERR_UNKNOWN_FORMAT;
    int32_t version;
    if (fread(&version, sizeof(int32_t), 1, fp) == 0) {
        return APCACHE_ERR_EOF;
    }
    if (version != APCACHE_VERSION) return APCACHE_ERR_UNKNOWN_VERSION;
    fclose(fp);
    return 0;
}

/// @brief Open an apcache file in read mode.
/// @param filename path to apcache file
/// @return 0 for success, minus number for APCacheErr
int apcache_open(char *filename, APCache **apcadd) {
    *apcadd = NULL;
    if (!filename) return APCACHE_ERR_FILE_NOT_EXIST;
    FILE *fp;
    fp = fopen(filename, "r");
    if (!fp) return APCACHE_ERR_PERMISSION_DENIED;
    APCache *apc;
    apc = apcache_alloc();
    if (!apc) return APCACHE_ERR_APCACHE_NULL;
    apc->file = fp;
    char buf[8 + 1];
    if (!fgets(buf, 8 + 1, fp)) {
        fclose(fp);
        apcache_free(&apc);
        return APCACHE_ERR_EOF;
    }
    if (strcmp(buf, "apcache\n") != 0) {
        fclose(fp);
        apcache_free(&apc);
        return APCACHE_ERR_UNKNOWN_FORMAT;
    }
    int32_t version;
    if (fread(&version, sizeof(int32_t), 1, fp) == 0) {
        fclose(fp);
        apcache_free(&apc);
        return APCACHE_ERR_EOF;
    }
    if (version != APCACHE_VERSION) {
        fclose(fp);
        apcache_free(&apc);
        return APCACHE_ERR_UNKNOWN_VERSION;
    }
    apc->version = version;
    if (fread(&apc->fps, sizeof(uint32_t), 1, fp) == 0) {
        fclose(fp);
        apcache_free(&apc);
        return APCACHE_ERR_EOF;
    }
    if (fread(&apc->width, sizeof(uint32_t), 1, fp) == 0) {
        fclose(fp);
        apcache_free(&apc);
        return APCACHE_ERR_EOF;
    }
    if (fread(&apc->height, sizeof(uint32_t), 1, fp) == 0) {
        fclose(fp);
        apcache_free(&apc);
        return APCACHE_ERR_EOF;
    }
    if (fread(&apc->sample_rate, sizeof(uint32_t), 1, fp) == 0) {
        fclose(fp);
        apcache_free(&apc);
        return APCACHE_ERR_EOF;
    }
    *apcadd = apc;
    return 0;
}

/// @brief Read a APFrame from APCache
/// @param apc APCache
/// @param frame The pointer to a pointer to APFrame
/// @return 0 for success, minus number for APCacheErr
int apcache_read_frame(APCache *apc, APFrame **frame) {
    if (*frame) {
        apcache_frame_free(frame);
    }
    if (!apc) return APCACHE_ERR_APCACHE_NULL;
    if (!apc->file) return APCACHE_ERR_FILE_NOT_EXIST;
    uint8_t type;
    uint32_t bsize;
    if (fread(&type, sizeof(uint8_t), 1, apc->file) == 0) {
        return APCACHE_ERR_EOF;
    }
    if (fread(&bsize, sizeof(uint32_t), 1, apc->file) == 0) {
        return APCACHE_ERR_EOF;
    }
    APFrame *f = apcache_frame_alloc(type, bsize);
    if (!f) {
        return APCACHE_ERR_FRAME_NOT_EXIST;
    }
    if (fread(f->data, bsize, 1, apc->file) == 0) {
        apcache_frame_free(&f);
        return APCACHE_ERR_EOF;
    }
    *frame = f;
    return 0;
}

int apcache_close(APCache *apc) {
    if (!apc) return APCACHE_ERR_APCACHE_NULL;
    fclose(apc->file);
    return 0;
}