void changeBrightness() {
  if (brightness_dir) {
    brightness += 10;
    if (brightness > 255)  brightness = 255;
  } else {
    brightness -= 10;
    if (brightness < 10)   brightness = 10;
  }

  brightness = constrain(brightness, 10, 255);
  FastLED.setBrightness(brightness);
}

void autoLowPass() {
  delay(10);                                // ждём инициализации АЦП
  int thisMax = 0;                          // максимум
  int thisLevel;
  for (byte i = 0; i < 200; i++) {
    thisLevel = analogRead(SOUND_R);        // делаем 200 измерений
    if (thisLevel > thisMax)                // ищем максимумы
      thisMax = thisLevel;                  // запоминаем
    delay(4);                               // ждём 4мс
  }
  LOW_PASS = thisMax + LOW_PASS_ADD;        // нижний порог как максимум тишины + некая величина

  if (LOW_PASS >= 255) {
    Serial.println("ERRRROOR.....   LOW_PASS >= 255");
    LOW_PASS = constrain(LOW_PASS, 0, 255);
  }
}

void animation(int len) {
  if (rainbow_palette == false) {
    curColor = CHSV(ang, 255, 255);
    for (int i = NUM_LEDS / 2 -1; i > NUM_LEDS / 2 - len / 2 -1; i-- )
      leds[i] = curColor;

    for (int i = NUM_LEDS / 2 ; i < NUM_LEDS / 2 + len / 2; i++ ) {
      leds[i] = curColor;
    }
  } else if (rainbow_palette == true)
  { Serial.print("I'm in right way");
    if (millis() - rainbow_timer > 30) {
      rainbow_timer = millis();
      hue = floor((float)hue + RAINBOW_STEP);
    }
    byte count = 0;
    for (int i = NUM_LEDS / 2-1; i > NUM_LEDS / 2 - len / 2 - 1; i--) {
      leds[i] = ColorFromPalette(RainbowColors_p, (count * float_index) / 2 - hue);  // заливка по палитре радуга
      count++;
    }
    count = 0;
    for (int i = NUM_LEDS / 2; i < NUM_LEDS / 2 + len / 2; i++ ) {
      leds[i] = ColorFromPalette(RainbowColors_p, (count * float_index) / 2 - hue); // заливка по палитре радуга
      count++;
    }
  }
}
void fillAll(CRGB color) {
  for (int i = 0; i < NUM_LEDS; i++)
    leds[i] = color;
}
void setPixel(int Pixel, byte red, byte green, byte blue) {
  leds[Pixel].r = red;
  leds[Pixel].g = green;
  leds[Pixel].b = blue;
}
void addglitter( fract8 chanceOfGlitter)
{
  if ( random8() < chanceOfGlitter) {
#if NUM_LEDS < 255
    leds[ random8(NUM_LEDS) ] += CRGB::White;
#else
    leds[ random16(NUM_LEDS) ] += CRGB::White;
#endif
  }
}

void sparkler(uint8_t n)       //Бенгальский огонь
// 0 - нет эффектов
// 1 - Бенгальский огонь
// 2 - 1 яркий светодиод
// 3 - метеорит
// 4 - случайный эффект
{ uint8_t kol = 3;

  if ( StepSize < 10 ) kol = StepSize / 3;
  if (kol >= 2)
  { uint8_t nn = n;
    switch (nn) {
      case 1 :  for ( uint8_t x = 0; x < kol; x++) leds[ StepSize - random8(kol * 2) ] = CRGB::White; break;      //Бенгальский
      case 2 :  leds[StepSize - 1] = CRGB::White; break;                                                          //1 яркий
      case 3 :  leds[StepSize] = CRGB::White; leds[StepSize - 1] = CRGB::Red; leds[StepSize - 2] = CRGB::Violet; break; //Метеорит
    }
  }
}
