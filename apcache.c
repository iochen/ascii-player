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