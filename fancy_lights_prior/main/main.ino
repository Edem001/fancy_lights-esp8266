
//------------------CONSTANTS--------------------
#define NUM_LEDS 50 // convert to value for flexibility
int brightness = 150;
#define MAX_BRIGHT 255
#define SMOOTH_BRIGHTNESS_ADJUST_FRAME_SKIP  2 // Skips loop iterations to make brightness changes more smooth
#define BTN_PIN 0
#define LED_PIN 5

#define MODE_ENABLE_AUTO_CHANGE false // Auto mode change: true - enable; false - disable 
#define MODE_MAX 16
bool onFlag = true; // Are lights on/off
bool stepFlag = false; //  Is button held down
bool brightDir = true; // Is brightness going to darken or lighten (false for darken)
bool glitter = false;  //  Is glitter effect enabled

//******************VUmeter*************************
int LOW_PASS = 7;         // нижний порог шумов режим VU, ручная настройка
#define LOW_PASS_ADD 0     // "добавочная" величина к нижнему порогу, для надёжности (режим VU)
#define SOUND_R A0
#define MAIN_LOOP 5
#define INPUT_SOUND_COEF 1.2   // amplify analog input data (default 1)
#define SMOOTH 0.6         // Default 0.4
#define MAX_COEF 1.8        // коэффициент громкости (максимальное равно срднему * этот коэф) (по умолчанию 1.8)
#define EXP 1.6           // степень усиления сигнала (для более "резкой" работы) (по умолчанию 1.4)
long loop_counter = 0;
#define AUTO_LOW_PASS_DELAY 10 // Настройка интервала автободбора нижних частот = AUTO_LOW_PASS_DELAY (секунды)

// RAINBOW
#define RAINBOW_STEP 5.00
float float_index = (float)255 / (NUM_LEDS / 2.0);
long rainbow_timer = 0;
float hue_ang = 0.0;
boolean rainbow_palette = false;


//------------------LIBRARIES--------------------
#define FASTLED_ESP8266_RAW_PIN_ORDER
#include "FastLED.h"
CRGB leds[NUM_LEDS];

#include "GyverButton.h"
GButton touch(BTN_PIN, HIGH_PULL, NORM_CLOSE);

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

#include <ESPAsyncTCP.h>
#include <ArduinoJson.h>
//------------------VARIABLES--------------------
//MAIN
byte mode = 7;
int speed_delay = 60;
String value;

//MINOR_1
bool wasStep = false;
bool wasChanged = false;
bool isHue = true;
short ang = 0;
CRGB curColor = CRGB(255, 255, 255);
byte FIRE_PALETTE = 0;
short hue_audio_delay = 100;
long mode_delay = 3 * 60 * 1000;
int amount = 10;

//MINOR_2
unsigned long hue_audio_timer = 0;
unsigned long mode_timer = 0;
byte Rlenght;
float RsoundLevel, RsoundLevel_f;
float averageLevel = 50;
int maxLevel = 100;
unsigned long main_timer = 0;
float averK = 0.006, k = SMOOTH;
int RcurrentLevel;
int prefferedBright = brightness;
int frameSkip = 0;

const char* ssid = "fancy_lights";
const char* password = "testing_server";

// page

void setup() {
  //-------------------------------------------------------------------------------
  //              WiFi
  //-------------------------------------------------------------------------------
  
  Serial.begin(115200);
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

server.on("/mode", HTTP_GET, [] (AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(mode).c_str());
  });
server.on("/brightness", HTTP_GET, [] (AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(FastLED.getBrightness()).c_str());
  });

server.on("/mode", HTTP_POST, [](AsyncWebServerRequest *request){}, NULL, 
[](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t myindex, size_t total) {
    Serial.println("Got request!");

      DynamicJsonDocument doc(50);
      DeserializationError error = deserializeJson(doc,(const char*)data);
      if (!error) {
        if (doc.containsKey("data")) {
          String text = doc["data"].as<String>();
          Serial.println(text); 
          mode = doc["data"];
        }
        request->send(200);
      }
      else {
        Serial.print("Deserialization error code "); 
        Serial.println(error.c_str());
      }
    
  });
    
server.on("/brightness", HTTP_POST, [](AsyncWebServerRequest *request){}, NULL, 
[](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t myindex, size_t total) {
    Serial.println("Got request!");

      DynamicJsonDocument doc(50);
      DeserializationError error = deserializeJson(doc,(const char*)data);
      if (!error) {
        if (doc.containsKey("data")) {
          String text = doc["data"].as<String>();
          prefferedBright = doc["data"];
        }
        request->send(200);
      }
      else {
        Serial.print("Deserialization error code "); 
        Serial.println(error.c_str());
      }
    
  });
  // Start server
  server.begin();
  
//--------------------------------------------------------------------------------------

  Serial.begin(115200);
  randomSeed(analogRead(SOUND_R));

  EEPROM_init(false);

  FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(brightness);
  autoLowPass();
  loop_counter = millis();

  pinMode(LED_PIN, OUTPUT);
  touch.setStepTimeout(200);
  delay(50);
}

void loop() {
  touch.tick();

  if (touch.isSingle()) {
    onFlag = !onFlag;
    FastLED.setBrightness(onFlag ? brightness : 0);
    FastLED.show();
  }

  //for brightness and mode change
  if (touch.isDouble()) { // Mode forward
    wasChanged = true;
    changeMode();
    //changeEffects();
    if (++mode > MODE_MAX) mode = 0;
    
  }
  if (touch.isTriple()) { // Mode back
    if (--mode < 0) mode = MODE_MAX;
    wasChanged = true;
    changeMode();
  }
  if (touch.hasClicks()) {
    if (touch.getClicks() == 4) glitter = !glitter;

    if (touch.getClicks() == 5) {autoLowPass();} // 5 clicks --------------------------------
    }
  if (stepFlag && touch.isRelease()) {
    stepFlag = false;
    brightDir = !brightDir;
  }

  if (prefferedBright != brightness && frameSkip++ == SMOOTH_BRIGHTNESS_ADJUST_FRAME_SKIP){
    if(prefferedBright > brightness)
      {
        brightness++;
        FastLED.setBrightness(brightness);
      }
    else
      {
        brightness--;
        FastLED.setBrightness(brightness);
      }
      frameSkip = 0;
  }

  if (touch.isStep()) {
    stepFlag = true;
    
    changeBrightness(brightDir);
  }

  //mode automatic changing
  if (millis() - mode_timer > mode_delay && MODE_ENABLE_AUTO_CHANGE) {
    if (++mode > 20) mode = 0;
    wasChanged = true;
    mode_timer = millis();
  }

  // hue ang
  if (isHue && (millis() - hue_audio_timer > hue_audio_delay)) {
    if (++ang > 255) ang = 0;

    CRGB newColor = CHSV(ang, 255, 255);
    curColor = newColor;

    hue_audio_timer = millis();
  }
  switch (mode) {
    case 0: lighter();
      break;
    case 1: all_hue();
      break;
    case 2: randomColors();
      break;
    case 3: rainbow();
      break;
    case 4: sparkles();
      break;
    case 5: random_lights();
      break;
    case 6:
    rainbow_palette = false; 
    VUmeter();
    break;
    case 7:
    rainbow_palette = true; 
    VUmeter();
      break;
    case 8: 
    rainbow_palette = false;
    vibration();
      break;
    case 9: 
    rainbow_palette = true;
    vibration();
      break;
    case 10: twinkle();
      break;
    case 11: fire_5();
      break;
    case 12: theaterChase();
      break;
    case 13: theaterChaseRainbow();
      break;
    case 14: SnowSparkle();
      break;
    case 15: twinkleRandom();
      break;
    case 16: rainbow_butterfly();
      break;
    default:
      FastLED.clear();
      Serial.println(mode);
      leds[0] = CRGB::Green;
      leds[NUM_LEDS - 1] = CRGB::Green;
      break;
  }

  //if(glitter) TODO!!!
    //addglitter(10);

  FastLED.show();
}

void changeMode(){
  if (wasChanged) {
    wasChanged = false;
    switch (mode) {
      case 0:
        isHue = true;
        speed_delay = 40;
        lighter();
        break;
      case 1:
        isHue = true;
        speed_delay = 60;
        all_hue();
        break;
      case 2:
        isHue = false;
        randomColors();
        break;
      case 3:
        isHue = true;
        speed_delay = 40;
        rainbow();
        break;
      case 4:
        isHue = false;
        speed_delay = 10;
        sparkles();
        break;
      case 5:
        isHue = true;
        speed_delay = 100;
        curColor = CHSV(ang, 255, 255);
        random_lights();
        break;
      case 6:
        isHue = true;
        rainbow_palette = false;
        VUmeter();
        break;
      case 7:
        isHue = false;
        rainbow_palette = true;
        VUmeter();
        break;
      case 8:
        isHue = true;
        rainbow_palette = false;
        curColor = CHSV(ang, 255, 255);
        vibration();
        break;
      case 9:
        isHue = false;
        rainbow_palette = true;
        curColor = CHSV(ang, 255, 255);
        vibration();
        break;
      case 10:
        isHue = true;
        speed_delay = 160;
        amount = 8;
        twinkle();
        break;
      case 11:
        isHue = false;
        speed_delay = 15;
        fire_5();
        break;
      case 12:
        isHue = true;
        speed_delay = 50;
        theaterChase();
        break;
      case 13:
        isHue = true;
        speed_delay = 100;
        theaterChaseRainbow();
        break;
      case 14:
        isHue = false;
        speed_delay = random(100, 800);
        amount = 30;      //like effect_timer_2
        SnowSparkle();
        break;
      case 15:
        isHue = false;
        speed_delay = random(100, 800);
        amount = 20;
        twinkleRandom();
        break;
      case 16:
        speed_delay = 100;
        rainbow_butterfly();
        break;
      default:
        FastLED.clear();
        Serial.println(mode);
        leds[0] = CRGB::Green;
        leds[NUM_LEDS - 1] = CRGB::Green;
        break;
    }
  }
}

/* changeEffect(){
      EVERY_N_MILLIS_I(thistimer, thisdelay) {                                    // Sets the original delay time.
      thistimer.setPeriod(thisdelay);                                           // This is how you update the delay value on the fly.
      KolLed = NUM_LEDS;                                                        // Выводим Эффект на все светодиоды
      strobe_mode(ledMode, 0);                                                  // отобразить эффект;

      if (StepMode < NUM_LEDS) {                                                // требуется наложить новый эффект
        KolLed = StepMode;                                                      // Выводим Эффект на все светодиоды
        if (StepMode > 10)  strobe_mode(newMode, 0);                            // отобразить эффект;
        sparkler(CHANGE_SPARK);                                                             // бенгальский огонь
      }
    }
    EVERY_N_MILLISECONDS(CHANGE_TIME * 1000 / NUM_LEDS) {                      // Движение плавной смены эффектов
      if ( StepMode < NUM_LEDS)
      { StepMode++;
        if (StepMode == 10) strobe_mode(newMode, 1);
        if (StepMode >= NUM_LEDS)
        { ledMode = newMode;
          StepMode = MAX_LEDS;
        }
        //nblendPaletteTowardPalette(gCurrentPalette, gTargetPalette, NUM_LEDS);
      }
    }
}*/
