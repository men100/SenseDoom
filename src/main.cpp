#include <Arduino.h>
#include <doomgeneric.h>
#include "LGFX_SPRESENSE.hpp"

// 準備したクラスのインスタンスを作成します。
LGFX display;

void setup() {
  // SPIバスとパネルの初期化を実行すると使用可能になります。
  display.init();

  display.setBrightness(128); // バックライトの明るさ (0-255)
  display.setRotation(1);     // ディスプレイの向き (0=縦, 1=横, 2=逆縦, 3=逆横)
  
  display.fillScreen(TFT_BLACK); // 画面を黒で塗りつぶし

  // テキストを描画
  display.setCursor(10, 10);
  display.setFont(&fonts::Font2); // フォントを設定
  display.setTextColor(TFT_WHITE, TFT_BLACK); // 文字色:白, 背景色:黒
  
  display.println("Hello Spresense!");
  display.println("LovyanGFX + ILI9341");

  // 図形を描画
  display.drawRect(10, 60, 100, 50, TFT_RED);
  display.fillRect(120, 60, 100, 50, TFT_GREEN);
  display.drawCircle(60, 150, 30, TFT_BLUE);

  doomgeneric_Create(0, NULL);
}

void loop() {
  doomgeneric_Tick();
}
