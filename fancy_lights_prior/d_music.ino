
void VUmeter(){
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

      if(!rainbow_palette){
       CRGB curColor = CHSV(ang, 255, Rlenght);
       fillAll(curColor);  
      }else{
        animation(NUM_LEDS);
      }
    }
    main_timer = millis();
  }
}
