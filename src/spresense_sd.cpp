#include "spresense_sd.h"
#include "spresense_debug.h"
#include <Arduino.h>
#include <SDHCI.h>

SDClass sdCard;

void spresense_sd_init(void) {
  Serial2.println("Initializing SD card...");
  while (!sdCard.begin()) {
    Serial2.println("Insert SD card.");
  }
  Serial2.println("SD card initialized.");
}

boolean spresense_sd_exists(const char *filepath) {
    return sdCard.exists(filepath);
}
