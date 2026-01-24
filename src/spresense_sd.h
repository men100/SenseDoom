#pragma once

#include <doomtype.h>

#ifdef __cplusplus
extern "C" {
#endif

void spresense_sd_init(void);
boolean spresense_sd_exists(const char *filepath);

#ifdef __cplusplus
} // extern "C"
#endif
