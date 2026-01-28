#include <arch/chip/gnssram.h>
#include <Arduino.h>
#include <doomtype.h>
#include <doomgeneric.h>
#include "spresense_debug.h"
#include "spresense_sd.h"
#include "LGFX_SPRESENSE.hpp"
#include "doomkeys.h"

// USE_RGB565 のとき、pixel_t は uint16_t
pixel_t* DG_ScreenBuffer = nullptr;

extern LGFX gfx;

extern "C" {

void DG_Init() {
  spresense_debug_init();
  spresense_sd_init();
  up_gnssram_initialize();

  // 出力バッファは Litte Endian な一方、ILI9341 への入力は Big Endian である必要がある
  // ここで Byte Swap して、色化けしないようにする
  gfx.setSwapBytes(true);

  size_t bufferSize = DOOMGENERIC_RESX * DOOMGENERIC_RESY * sizeof(pixel_t);
  DG_ScreenBuffer = (pixel_t*)malloc(bufferSize);

  if (DG_ScreenBuffer == nullptr) {
    spresense_printf("DG_Init: Failed to allocate screen buffer!\n");
    while(1);
  }
}

void DG_DrawFrame() {
  // USE_RGB565 により RGB565 バッファが来るので、直接入力できる
  gfx.pushImage(0, 0, DOOMGENERIC_RESX, DOOMGENERIC_RESY, (uint16_t*)DG_ScreenBuffer);
}

void DG_SleepMs(uint32_t ms) {
  delay(ms);
}

uint32_t DG_GetTicksMs() {
  return millis();
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
