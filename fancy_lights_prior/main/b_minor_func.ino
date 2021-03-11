void changeBrightness() {
  if(brightness_dir){
    brightness += 10;
    if(brightness > 255)  brightness = 255;
  }else{
    brightness -= 10;
    if(brightness < 20)   brightness = 20;
  }

  brightness = constrain(brightness, 20, 255);
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

  if(LOW_PASS >= 255){
    Serial.println("ERRRROOR.....   LOW_PASS >= 255");
    LOW_PASS = constrain(LOW_PASS, 0, 255);
  }
}

void animation(int len) {
  short count = 0;
  if (millis() - rainbow_timer > 30 && rainbow_palette == true) {
        rainbow_timer = millis();
        hue_ang = floor((float)hue_ang + RAINBOW_STEP);
      }
  for (int i = NUM_LEDS/2, j = NUM_LEDS/2-1; i < NUM_LEDS/2 + len/2 ; i++,j-- )
    if (rainbow_palette == true){
      leds[i] = ColorFromPalette(RainbowColors_p, (count * float_index) / 2 - hue_ang);  // заливка по палитре радуга
      leds[j] = ColorFromPalette(RainbowColors_p, (count * float_index) / 2 - hue_ang);
        count++;
    }
    else
    {
      leds[i] = curColor;
      leds[j] = curColor;
    }
  
    count = 0;
  
}
void fillAll(CRGB color){
  for(int i = 0; i < NUM_LEDS; i++)
    leds[i] = color;
}
void setPixel(int Pixel, byte red, byte green, byte blue) {
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
}
