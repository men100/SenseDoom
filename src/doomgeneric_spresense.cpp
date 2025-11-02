#include <doomtype.h>
#include <doomgeneric.h>
#include "spresense_debug.h"

extern "C" {

void DG_Init() {
  spresense_debug_init();
}

void DG_DrawFrame() {
  // do nothing
}

void DG_SleepMs(uint32_t ms) {
  (void)ms;
  // do nothing
}

uint32_t DG_GetTicksMs() {
  // do nothing
  return 0;
}

int DG_GetKey(int* pressed, unsigned char* key) {
  (void)pressed;
  (void)key;
  // do nothing
  return 0;
}
void DG_SetWindowTitle(const char * title) {
  (void)title;
  // do nothing
}

void I_Endoom(byte *endoom_data) {
  (void)endoom_data;
  // do nothing
}

} // extern "C"
