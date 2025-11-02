#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void spresense_debug_init(void);
int spresense_printf(const char *format, ...);

#ifdef __cplusplus
} // extern "C"
#endif
