// Default init values
#define RESERVED_VALUES 4
#define DEFAULT_LED_COUNT 50
#define DEFAULT_MODE 0
#define DEFAULT_BRIGHTNESS 255
#define DEFAULT_GLITTER 0

/**
    Reserved EEPROM values: 0 - led count, 1 - mode, 2 - brightness, 3 - glitter
  */

void EEPROM_init(bool forceInit) {
  int valueCount = EEPROM.length();

  for (int i = (forceInit) ? 0 : valueCount; i < RESERVED_VALUES; i++) {  // If forced - EEPROM rewrites all reserved values with default
    switch (i) {
      case 0:
        EEPROM[i] = DEFAULT_LED_COUNT;
        break;
      case 1:
        EEPROM[i] = DEFAULT_MODE;
        break;
      case 2:
        EEPROM[i] = DEFAULT_BRIGHTNESS;
        break;
      case 3:
        EEPROM[i] = DEFAULT_GLITTER;
        break;
      default: Serial.println("Requested default value init for unsupported case: " + i);
    }
  }
}

int EEPROM_getLedCount() {
  return EEPROM[0];
}
int EEPROM_getMode() {
  return EEPROM[1];
}
int EEPROM_getBrightness() {
  return EEPROM[2];
}
int EEPROM_getGlitter() {
  return EEPROM[3];
}

void EEPROM_setLedCount(int ledCount) {
  EEPROM[0] = ledCount;
}
void EEPROM_setMode(int mode) {
  EEPROM[1] = mode;
}
void EEPROM_setBrightness(int brightness) {
  EEPROM[2] = brightness;
}
void EEPROM_setGlitter(int glitterEnabled) {
  EEPROM[3] = glitterEnabled;
}
