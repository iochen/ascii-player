#ifndef APCACHE_H
#define APCACHE_H

#include <stdint.h>
#include <stdio.h>

#define APCACHE_VERSION 1

/*
||==================================================================================||
||             VALUE             |           TYPE           |          BYTES        ||
||==================================================================================||
||           "apache\n"          |         ASCII string     |            7          ||
||-------------------------------|--------------------------|-----------------------||
||            VERSION            |           int32          |            4          ||
||-------------------------------|--------------------------|-----------------------||
||             FPS               |           uint32         |            4          ||
||-------------------------------|--------------------------|-----------------------||
||            WIDTH              |           uint32         |            4          ||
||-------------------------------|--------------------------|-----------------------||
||            HEIGHT             |           uint32         |            4          ||
||-------------------------------|--------------------------|-----------------------||
||          SAMPLE_RATE          |           uint32         |            4          ||
||-------------------------------|--------------------------|-----------------------||
||          FRAME[0]_TYPE        |           uint8          |            1          ||
||-------------------------------|--------------------------|-----------------------||
||          FRAME[0]_SIZE        |           uint32         |            4          ||
||-------------------------------|--------------------------|-----------------------||
||          FRAME[0]_DATA        | unsigned char[]/ float[] |      FRAME[0]_SIZE    ||
||-------------------------------|--------------------------|-----------------------||
||          FRAME[1]_TYPE        |           uint8          |            1          ||
||-------------------------------|--------------------------|-----------------------||
||          FRAME[1]_SIZE        |           uint32         |            4          ||
||-------------------------------|--------------------------|-----------------------||
||          FRAME[1]_DATA        |  unsigned char / float   |      FRAME[1]_SIZE    ||
||----------------------------------------------------------|-----------------------||
||          FRAME[2]_TYPE        |           uint8          |            1          ||
||-------------------------------|--------------------------|-----------------------||
||          FRAME[2]_SIZE        |           uint32         |            4          ||
||-------------------------------|--------------------------|-----------------------||
||          FRAME[2]_DATA        |  unsigned char / float   |      FRAME[2]_SIZE    ||
||----------------------------------------------------------|-----------------------||
                                             ...
                                             ...
                                             ...
||----------------------------------------------------------|-----------------------||
||          FRAME[n]_TYPE        |           uint8          |            1          ||
||-------------------------------|--------------------------|-----------------------||
||          FRAME[n]_SIZE        |           uint32         |            4          ||
||-------------------------------|--------------------------|-----------------------||
||          FRAME[n]_DATA        |  unsigned char / float   |      FRAME[n]_SIZE    ||
||==================================================================================||
*/

typedef enum {
    APAV_AUDIO,
    APAV_VIDEO
} APAVType;

typedef enum {
    APCACHE_ERR_FILE_NOT_EXIST = -100000,
    APCACHE_ERR_PERMISSION_DENIED,
    APCACHE_ERR_UNKNOWN_FORMAT,
    APCACHE_ERR_UNKNOWN_VERSION,
    APCACHE_ERR_FRAME_NOT_EXIST,
    APCACHE_ERR_IOERROR,
    APCACHE_ERR_APCACHE_NULL,
    APCACHE_ERR_EOF = -1,
} APCacheErr;

typedef struct {
    // Version number of apcache file format.
    int32_t version;
    // Frames per second
    uint32_t fps;
    // Width for each video frame
    uint32_t width;
    // Height for each video frame
    uint32_t height;
    // Sample rate for audio frames
    // When sample_rate is 0, there will be no audio frame in apcache file
    uint32_t sample_rate;
    // Opened apcache file
    // NULL for not initialized
    FILE *file;
} APCache;

typedef struct {
    // Apcache frame type
    APAVType type;
    // Size of data array (in byte)
    uint32_t bsize;
    // For version 1, 
    // when the frame is a video frame, data is an array of type unsigned char;
    // when the frame is an audio frame, data is an array of type float;
    void *data;
} APFrame;

/// @brief Allocate an apcache frame object.
/// @param type APAVType
/// @param bsize size of data array (in bytes)
/// @return The pointer to allocated object.
APFrame *apcache_frame_alloc(APAVType type, unsigned int bsize);

/// @brief Free all fields allocated on heap in frame, and free frame it self.
/// @param frame The pointer to the pointer to the frame object.
/// @note (*frame) will be set to NULL.
void apcache_frame_free(APFrame **frame);

/// @brief Allocate an apcache object with all fields set to default.
/// @return The pointer to allocated object.
APCache *apcache_alloc();

/// @brief Free all fields allocated on heap in apc, and free apc it self.
/// @param apc The pointer to the pointer to the APCache object.
/// @note (*apc) will be set to NULL.
void apcache_free(APCache **apc);

/// @brief Write meta data to apcache file
/// @param apc APCache struct with fps, width, height, sample_rate set to target number,
///            file pointed to a opened FILE with mode set to "w",
///            version set to target version (1).
/// @return 0 for success, minus number for APCacheErr
int apcache_create(APCache *apc);

/// @brief Write a frame into apcache file
/// @param apc The pointer to the APCache object.
/// @param frame Frame to be written to the file
/// @return 0 for success, minus number for APCacheErr
int apcache_write_frame(APCache *apc, APFrame *frame);

/// @brief Check whether is an apcache file.
///        1. Check whether file exists
///        2. Check whether have permission to read
///        3. Check whether file content starts with "apcache\n"
///        4. Check if version number is valid (Only 1 is valid currently)
/// @param filename path to apcache file
/// @return 0 for success, minus number for APCacheErr
int is_apcache(char *filename);

/// @brief Open an apcache file in read mode.
/// @param filename path to apcache file
/// @return 0 for success, minus number for APCacheErr
int apcache_open(char *filename, APCache **apc);

/// @brief Read a APFrame from APCache
/// @param apc APCache
/// @param frame The pointer to a pointer to APFrame
/// @return 0 for success, minus number for APCacheErr
int apcache_read_frame(APCache *apc, APFrame **frame);

int apcache_close(APCache *apc);
#endif