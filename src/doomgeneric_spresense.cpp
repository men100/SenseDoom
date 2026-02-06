#include <arch/chip/gnssram.h>
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_seesaw.h>
#include <doomtype.h>
#include <doomgeneric.h>
#include "spresense_debug.h"
#include "spresense_sd.h"
#include "LGFX_SPRESENSE.hpp"
#include "doomkeys.h"

// USE_RGB565 のとき、pixel_t は uint16_t
pixel_t* DG_ScreenBuffer = nullptr;

// Screen Buffer (320 x 240 x 16bit = 125KiB)
#define bufferArrayNum (DOOMGENERIC_RESX * DOOMGENERIC_RESY)
static pixel_t screen_buffer_array[bufferArrayNum];

LGFX gfx;
Adafruit_seesaw gamepad;
bool gamepad_found = false;

extern "C" void Z_GetFreeMemory(int* main_free, int* sec_free);

// Button mapping for Adafruit Gamepad QT
#define BUTTON_A      5
#define BUTTON_B      1
#define BUTTON_X      6
#define BUTTON_Y      2
#define BUTTON_START  16
#define BUTTON_SELECT 0

// Joystick Logic
#define JOY_ADDR       0x50
#define JOYSTICK_X_PIN 14
#define JOYSTICK_Y_PIN 15
#define JOY_DEADZONE   200
#define JOY_CENTER     512

uint32_t last_buttons = 0;
int last_joy_x = 0; // -1, 0, 1
int last_joy_y = 0; // -1, 0, 1

extern "C" {

void DG_Init() {
  spresense_debug_init();
  spresense_sd_init();
  up_gnssram_initialize();
  
  gfx.init();
  gfx.setBrightness(128); // バックライトの明るさ (0-255)
  gfx.setRotation(1);     // ディスプレイの向き (0=縦, 1=横, 2=逆縦, 3=逆横)

  // 出力バッファは Litte Endian な一方、ILI9341 への入力は Big Endian である必要がある
  // ここで Byte Swap して、色化けしないようにする
  gfx.setSwapBytes(true);

  DG_ScreenBuffer = screen_buffer_array;
  if (DG_ScreenBuffer == nullptr) {
    spresense_printf("DG_Init: Failed to allocate screen buffer!\n");
    while(1);
  }

  // Init Gamepad
  Wire.begin();
  
  if (!gamepad.begin(JOY_ADDR)) {
    spresense_printf("Seesaw gamepad not found! Check wiring.\n");
    gamepad_found = false;
  } else {
    gamepad_found = true;
  }

  if (gamepad_found) {
      spresense_printf("Seesaw gamepad started at 0x%02X!\n", JOY_ADDR);
      uint32_t mask = (1UL << BUTTON_A) | (1UL << BUTTON_B) | (1UL << BUTTON_X) | (1UL << BUTTON_Y) |
                      (1UL << BUTTON_START) | (1UL << BUTTON_SELECT);
      gamepad.pinModeBulk(mask, INPUT_PULLUP);
  }
}

void DG_DrawFrame() {
  // USE_RGB565 により RGB565 バッファが来るので、直接入力できる
  gfx.pushImage(0, 40, DOOMGENERIC_RESX, DOOMGENERIC_RESY, (uint16_t*)DG_ScreenBuffer);

  static uint32_t lastMillis = 0;
  static int frameCount = 0;
  static float fps = 0.0f;

  frameCount++;
  uint32_t currentMillis = millis();
  if (currentMillis - lastMillis >= 1000) {
    fps = frameCount * 1000.0f / (currentMillis - lastMillis);
    frameCount = 0;
    lastMillis = currentMillis;

    // FPS 描画
    gfx.fillRect(0, 0, 320, 40, TFT_BLACK); 
    gfx.setTextColor(TFT_WHITE, TFT_BLACK);
    gfx.setFont(&fonts::Font2); // Use a standard font
    
    int main_free = 0;
    int sec_free = 0;
    Z_GetFreeMemory(&main_free, &sec_free);
    
    gfx.setCursor(10, 5);
    gfx.printf("Mem: %dKiB (Main) / %dKiB (GNSS)", main_free/1024, sec_free/1024);
    gfx.setCursor(10, 20);
    gfx.printf("FPS: %.1f", fps);
  }
}

void DG_SleepMs(uint32_t ms) {
  delay(ms);
}

uint32_t DG_GetTicksMs() {
  return millis();
}

int DG_GetKey(int* pressed, unsigned char* key) {
  if (!gamepad_found) {
    return 0;
  }

  // 理想的には Queue などで複数入力を溜め込むのが望ましいが、
  // シンプルに1回の読み出しで1つの変更のみを処理する

  // Read Buttons
  uint32_t buttons = gamepad.digitalReadBulk(
    (1UL << BUTTON_A) | (1UL << BUTTON_B) | (1UL << BUTTON_X) | (1UL << BUTTON_Y) | 
    (1UL << BUTTON_START) | (1UL << BUTTON_SELECT));
  
  // Invert logic (INPUT_PULLUP: 0 is pressed)
  buttons = ~buttons; 

  // Check changes
  uint32_t changed = buttons ^ last_buttons;
  
  if (changed) {
      // Find the first changed bit
      for (int i = 0; i < 32; i++) {
          if (changed & (1UL << i)) {
              *pressed = (buttons & (1UL << i)) ? 1 : 0;
              
              // Map to Doom Keys
              switch(i) {
                  case BUTTON_A:      *key = KEY_USE; break;      // A -> Use
                  case BUTTON_B:      *key = KEY_FIRE; break;     // B -> Fire
                  case BUTTON_X:      *key = KEY_STRAFE_R; break; // X -> Strafe Right
                  case BUTTON_Y:      *key = KEY_STRAFE_L; break; // Y -> Strafe Left
                  case BUTTON_START:  *key = KEY_ENTER; break;    // START -> Enter
                  case BUTTON_SELECT: *key = KEY_TAB; break;      // SELECT -> Tab (Map)
                  default: *key = 0; break;
              }
              
              if (*key != 0) {
                  // 他の bit は次回処理
                  last_buttons ^= (1UL << i); 
                  return 1; // Event generated
              }
          }
      }
      // 変更があったが 0 に map された
      last_buttons = buttons; 
  }

  // アナログパッドの読み込み、20ms に一回
  static uint32_t last_joy_read = 0;
  if (millis() - last_joy_read > 20) {
      last_joy_read = millis();
      int x_val = gamepad.analogRead(JOYSTICK_X_PIN);
      int y_val = gamepad.analogRead(JOYSTICK_Y_PIN);
      
      int new_joy_x = 0;
      if (x_val < JOY_CENTER - JOY_DEADZONE) new_joy_x = -1; // Left
      else if (x_val > JOY_CENTER + JOY_DEADZONE) new_joy_x = 1; // Right
      
      int new_joy_y = 0;
      if (y_val < JOY_CENTER - JOY_DEADZONE) new_joy_y = 1; // Up 
      else if (y_val > JOY_CENTER + JOY_DEADZONE) new_joy_y = -1; // Down
      
      // Process X change
      if (new_joy_x != last_joy_x) {
          if (last_joy_x != 0) {
              // Release old direction
              *pressed = 0;
              *key = (last_joy_x == -1) ? KEY_RIGHTARROW : KEY_LEFTARROW; // ロジック反転 (うまく動くので)
              last_joy_x = 0; // Intermediate state
              return 1;
          }
          if (new_joy_x != 0) {
              // Press new direction
              *pressed = 1;
              *key = (new_joy_x == -1) ? KEY_RIGHTARROW : KEY_LEFTARROW; // ロジック反転 (うまく動くので)
              last_joy_x = new_joy_x;
              return 1;
          }
      }

      // Process Y change
      if (new_joy_y != last_joy_y) {
          if (last_joy_y != 0) {
              // Release old
              *pressed = 0;
              *key = (last_joy_y == 1) ? KEY_UPARROW : KEY_DOWNARROW;
              last_joy_y = 0;
              return 1;
          }
          if (new_joy_y != 0) {
              // Press new
              *pressed = 1;
              *key = (new_joy_y == 1) ? KEY_UPARROW : KEY_DOWNARROW;
              last_joy_y = new_joy_y;
              return 1;
          }
      }
  }

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
