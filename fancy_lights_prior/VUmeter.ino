//****************************** VUmeter ******************************
#define SOUND_R A5


// отрисовка
#define MAIN_LOOP 5         
#define SMOOTH 0.4          
#define MAX_COEF 1.8        // коэффициент громкости (максимальное равно срднему * этот коэф) (по умолчанию 1.8)

// сигнал
#define EXP 1.4             // степень усиления сигнала (для более "резкой" работы) (по умолчанию 1.4)


byte Rlenght;
float RsoundLevel, RsoundLevel_f;
float averageLevel = 50;
int maxLevel = 100;
unsigned long main_timer = 0;
float averK = 0.006, k = SMOOTH;  
int RcurrentLevel;


void VUmeter(){
   if(millis() - hue_audio_timer > 100){
     if(++ang > 255) ang = 0; 
     
     CRGB newColor = CHSV(ang, 255, 255);
     setColourRgb(newColor);
       
     hue_audio_timer = millis();
   }
  
  
   if (millis() - main_timer > MAIN_LOOP) {
    FastLED.clear();
    RsoundLevel = 0;
    
    for (byte i = 0; i < 100; i ++) {
      RcurrentLevel = analogRead(SOUND_R);
      if (RsoundLevel < RcurrentLevel) 
        RsoundLevel = RcurrentLevel;
    }
    // фильтруем по нижнему порогу шумов
    RsoundLevel = map(RsoundLevel, LOW_PASS, 1023, 0, 500);
    RsoundLevel = constrain(RsoundLevel, 0, 500);
    // возводим в степень (для большей чёткости работы)
    RsoundLevel = pow(RsoundLevel, EXP);     

    // фильтр
    RsoundLevel_f = RsoundLevel * k + RsoundLevel_f * (1 - k);
    // если значение выше порога - начинаем самое интересное
    if (RsoundLevel_f > 15) {
      // Фильтр очень медленный, сделано специально для автогромкости
      averageLevel = (float)(RsoundLevel_f) * averK + averageLevel * (1 - averK);

      // принимаем максимальную громкость шкалы как среднюю, умноженную на некоторый коэффициент MAX_COEF
      maxLevel = (float)averageLevel * MAX_COEF;

      // преобразуем сигнал в длину ленты
      Rlenght = map(RsoundLevel_f, 0, maxLevel, 0, NUM_LEDS);
      Rlenght = constrain(Rlenght, 0, NUM_LEDS);
      
      animation(Rlenght);      
    }
    main_timer = millis();
  }
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
  if (EEPROM_LOW_PASS && !AUTO_LOW_PASS) 
    EEPROM.updateInt(0, LOW_PASS);
}




/*****************VIBRATION*************/
void vibration(){
  if (millis() - main_timer > MAIN_LOOP) {
    FastLED.clear();
    RsoundLevel = 0;
    for (byte i = 0; i < 100; i ++) {
      RcurrentLevel = analogRead(SOUND_R);
      if (RsoundLevel < RcurrentLevel) 
        RsoundLevel = RcurrentLevel;
    }
    RsoundLevel = map(RsoundLevel, LOW_PASS, 1023, 0, 500);
    RsoundLevel = constrain(RsoundLevel, 0, 500);
    RsoundLevel = pow(RsoundLevel, EXP);
    RsoundLevel_f = RsoundLevel * k + RsoundLevel_f * (1 - k);
    if (RsoundLevel_f > 15) {
      averageLevel = (float)(RsoundLevel_f) * averK + averageLevel * (1 - averK);
      maxLevel = (float)averageLevel * MAX_COEF;
      Rlenght = map(RsoundLevel_f, 0, maxLevel, 0, 255);
      Rlenght = constrain(Rlenght, 0, 255);

      
       CRGB newColor = CHSV(ang, 255, Rlenght);
       fillAll(newColor);  
    }
    main_timer = millis();
  }
}
