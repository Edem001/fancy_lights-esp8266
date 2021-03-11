unsigned long effect_timer = 0;
unsigned long effect2_timer = 0;
int rainbow_delay = 100;
unsigned long random_lights_delay = 0;
// ****************************** LIGHTER ******************************
byte position = 0;
boolean direction = true;
void lighter() {
  if (millis() - effect_timer > speed_delay) {
    FastLED.clear(); 
    if (direction) {
      position++;
      if (position > NUM_LEDS - 2) {
        direction = false;
      }
    } else {
      position--;
      if (position < 1) {
        direction = true;
      }
    } 
    leds[position] = curColor;
    
    effect_timer = millis();  
  }
}


//****************************** ALL_HUE ******************************

void all_hue(){
  if(millis() - effect_timer > speed_delay){
    FastLED.clear();
    fillAll(curColor);
    effect_timer = millis();
 }
}
// --------------------------------- Random Colors ------------------------------------
short color_scale = 0;
void randomColors() {
  if(random(0, 75) <= 1)
    color_scale = random(0, 100);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(color_scale * 2.5, 255, 255);
  }
}


//****************************** RAINBOW ******************************
void rainbow(){
  if(millis() - effect_timer > speed_delay){
    FastLED.clear();  
   
    for(int i = 0; i < NUM_LEDS; i++)
        leds[i] = CHSV((byte)(ang + i * float(255 / NUM_LEDS)), 255, 255);
    
    effect_timer = millis();
  }
}


//****************************** SPARKLES ******************************
void sparkles() {
  if(millis() - effect_timer > speed_delay){
    int pixel = random(0, NUM_LEDS);
    if (isEmpty(pixel))
      leds[pixel] = CHSV(random(0, 255), 255, 255);
      
    for (int i = 0; i < NUM_LEDS; i++) {
      if (isEmpty(pixel)) continue;
      leds[i].fadeToBlackBy(10);
    }
    effect_timer = millis();
  }
}

boolean isEmpty(int pixel){
  if(leds[pixel].r == 0 && leds[pixel].g == 0 && leds[pixel].b == 0)
    return 1;
  else 
    return 0;
}




// ****************************** FIRE ******************************
#define COOLING 70 //55 
// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120

CRGBPalette16 gPal;

void fire() {
  if (FIRE_PALETTE == 0) gPal = HeatColors_p;
  else if (FIRE_PALETTE == 1) gPal = CRGBPalette16( CRGB::Black, CRGB::Red,  CRGB::Yellow, CRGB::White);
  else if (FIRE_PALETTE == 2) gPal = CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Aqua,   CRGB::White);
  else if (FIRE_PALETTE == 3) gPal = CRGBPalette16( CRGB::Black, CRGB::Red,  CRGB::White);
  
  random16_add_entropy( random8());
  Fire2012WithPalette(); // run simulation frame, using palette colors
}

void Fire2012WithPalette()
{
  // Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
  for ( int i = 0; i < NUM_LEDS; i++) 
    heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for ( int k = NUM_LEDS - 1; k >= 2; k--) 
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if ( random8() < SPARKING ) {
    int y = random8(7);
    heat[y] = qadd8( heat[y], random8(160, 255) );
  }

  // Step 4.  Map from heat cells to LED colors
  for ( int j = 0; j < NUM_LEDS; j++) {
    // Scale the heat value from 0-255 down to 0-240
    // for best results with color palettes.
    byte colorindex = scale8( heat[j], 240);
    CRGB color = ColorFromPalette( gPal, colorindex);
    leds[j] = color;
  }
}



// ****************************** Random Lights ******************************
void random_lights(){
  if(millis() - effect_timer > speed_delay){
    FastLED.clear();
    
       leds[random(NUM_LEDS)] = curColor;
       FastLED.show();
    effect_timer = millis();
 }
}


// ****************************** Twinkle ******************************
byte twinkle_count = 0;
void twinkle(){
  if( twinkle_count >= amount){
    FastLED.clear();
    twinkle_count  = 0;
  }

  if(millis() - effect_timer > speed_delay){
    leds[random(NUM_LEDS)] = CHSV(ang, 255, 255);
    twinkle_count ++;
    effect_timer = millis();
  }
}



// ****************************** TheaterChase ******************************
int th_q = 0;
void theaterChase(){
  if(++th_q > 3) th_q=0;

  if(millis() - effect_timer > speed_delay){
    FastLED.clear();
    for(int i = th_q; i < NUM_LEDS; i+=4)
      leds[i] = CHSV(ang,255, 255);
      
    effect_timer = millis();
  }
}



// ****************************** TheaterChaseRainbow -- RAINBOW_2******************************
void theaterChaseRainbow(){
  if(millis() - effect_timer > speed_delay){
    FastLED.clear();
    if(++th_q > 3) th_q=0;
    for(int i = th_q; i < NUM_LEDS; i+=3)
       leds[i] = CHSV((byte)(ang + i * float(255 / NUM_LEDS/5)), 255, 255);

    effect_timer = millis();
  }
}




// ****************************** SnowSparkle ******************************
void SnowSparkle(){
  if(millis() - effect_timer > speed_delay){
    fillAll(CRGB(10,10,10)); 
    
    leds[random(NUM_LEDS)] = CRGB(255,255,255);

    speed_delay = random(100, 800);
    effect_timer = millis();
  }
 
  if(millis() - effect2_timer > amount){
    fillAll(CRGB(10,10,10)); 
    effect2_timer = millis();
  }
}


// ****************************** TwinkleRandom ******************************
byte twinkleCounter = 0;
void twinkleRandom(){
  if(millis() - effect_timer > speed_delay){
    if(++twinkleCounter > amount){
      twinkleCounter = 0;
      FastLED.clear();
    } 
    leds[random(NUM_LEDS)] = CHSV(random(0,255),255,255);
    
    effect_timer = millis();
  }
}


// ****************************** Fire_5 ******************************
void fire_5(){
  if(millis() - effect_timer > speed_delay){
    FastLED.clear();
    Fire(55,120); 
    effect_timer = millis();
  }
}

void Fire(int Cooling, int Sparking) {
  static byte heat[NUM_LEDS];
  int cooldown;
  
  // Step 1.  Cool down every cell a little
  for( int i = 0; i < NUM_LEDS; i++) {
    cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);
    
    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }
  
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
    
  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160,255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for( int j = 0; j < NUM_LEDS; j++) {
    setPixelHeatColor(j, heat[j] );
  }
}
void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature/255.0)*191);
 
  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252
 
  // figure out which third of the spectrum we're in:
  if( t192 > 0x80) {                     // hottest
    setPixel(Pixel, 255, 255, heatramp);
  } else if( t192 > 0x40 ) {             // middle
    setPixel(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    setPixel(Pixel, heatramp, 0, 0);
  }
}



//****************************** RAINBOW_BUTTERFLY ******************************
int rainbow_couter_k = 0;
int rainbow_dir = 1;
void rainbow_butterfly(){
  if(millis() - effect_timer > speed_delay){
    FastLED.clear();
    
    if(rainbow_couter_k > NUM_LEDS || rainbow_couter_k < 0) rainbow_dir *= -1;
    rainbow_couter_k += rainbow_dir;
   
    for(int i = 0; i < NUM_LEDS; i++)
        if(i <= rainbow_couter_k){
          leds[i] = CHSV((byte)(ang + i * float(255 / NUM_LEDS)), 255, 255);
        }
    
    effect_timer = millis();
  }
}
