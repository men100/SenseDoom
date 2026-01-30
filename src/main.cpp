#include <Arduino.h>
#include <doomgeneric.h>

const char* argv[] = {
  "sensedoom",      // argv[0]
  "-iwad",          // argv[1]
  "doom1.wad",      // argv[2]
  "-gfxmode",       // argv[3]
  "rgb565"          // argv[4]
};

void setup() {
  int argc = sizeof(argv) / sizeof(argv[0]);
  doomgeneric_Create(argc, argv);
}

void loop() {
  doomgeneric_Tick();
}
