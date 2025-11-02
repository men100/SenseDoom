#pragma once

#ifndef __cplusplus

#include "spresense_debug.h"

// コンパイラに対し、printf を spresense_printf に置き換えるよう指示
#define printf spresense_printf

#endif // __cplusplus
