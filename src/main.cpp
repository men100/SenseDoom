#include <Arduino.h>
#include <doomgeneric.h>
#include "LGFX_SPRESENSE.hpp"

// 準備したクラスのインスタンスを作成します。
LGFX gfx;

const char* argv[] = {
  "sensedoom",      // argv[0]
  "-iwad",          // argv[1]
  "doom1.wad",      // argv[2]
  "-gfxmode",       // argv[3]
  "rgb565"          // argv[4]
};

void setup() {
  // SPIバスとパネルの初期化を実行すると使用可能になります。
  gfx.init();

  gfx.setBrightness(128); // バックライトの明るさ (0-255)
  gfx.setRotation(1);     // ディスプレイの向き (0=縦, 1=横, 2=逆縦, 3=逆横)
  
  int argc = sizeof(argv) / sizeof(argv[0]);
  doomgeneric_Create(argc, argv);
}

void loop() {
  doomgeneric_Tick();
}
