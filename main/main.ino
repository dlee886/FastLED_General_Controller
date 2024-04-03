//----------------------------------------------------------------
/*
LED Controller
Designed by: Damian
*/

//----------------------------------------------------------------
#include <FastLED.h>  //libraries

#define NUM_LEDS 32  //pins & led strip
#define LED_PIN A4
#define PALETTE_BUTTON 3
#define PATTERN_BUTTON 2
#define BRIGHTNESS_POTENTIOMETER A3

CRGB leds[NUM_LEDS];  //led strip array

uint8_t rando[6];  //randomizer variables for color setting of some patterns
uint8_t wavy;  //function variable for position setting of some patterns

uint8_t currentPaletteCase = 0;  //input value holders
uint8_t currentPatternCase = 0;
uint8_t currentBrightness = 50;

unsigned long lastDebounceTime = 0;  //timing gimmicks
unsigned int debounceDelay = 250;
unsigned long tick = 0;

//----------------------------------------------------------------
//color definition
DEFINE_GRADIENT_PALETTE(rainbow){
  0, 255, 0, 0,
  31, 255, 191, 0,
  63, 127, 255, 0,
  79, 0, 255, 0,
  95, 0, 255, 63,
  127, 0, 255, 255,
  152, 0, 0, 255,
  159, 63, 0, 255,
  191, 128, 0, 255,
  223, 255, 0, 191,
  255, 255, 0, 0
};

DEFINE_GRADIENT_PALETTE(red){
  0, 255, 0, 0,
  27, 255, 0, 0,
  127, 255, 83, 0,
  211, 255, 0, 0,
  255, 255, 0, 0
};

DEFINE_GRADIENT_PALETTE(yellow){
  0, 255, 255, 0,
  127, 255, 63, 0,
  255, 255, 255, 0
};

DEFINE_GRADIENT_PALETTE(green){
  0, 0, 255, 0,
  127, 255, 255, 0,
  255, 0, 255, 0
};

DEFINE_GRADIENT_PALETTE(blue){
  0, 0, 0, 255,
  127, 0, 255, 255,
  255, 0, 0, 255
};

DEFINE_GRADIENT_PALETTE(purple){
  0, 71, 0, 255,
  127, 255, 0, 83,
  255, 71, 0, 255
};

CRGBPalette16 currentPalette(
  CRGB::White,
  CRGB::White,
  CRGB::White,
  CRGB::Black,
  CRGB::White,
  CRGB::White,
  CRGB::White,
  CRGB::Black,
  CRGB::White,
  CRGB::White,
  CRGB::White,
  CRGB::Black,
  CRGB::White,
  CRGB::White,
  CRGB::White,
  CRGB::Black);

//----------------------------------------------------------------
void setup() {  //code to run once
  delay(500);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(currentBrightness);

  pinMode(PALETTE_BUTTON, INPUT_PULLUP);
  pinMode(PATTERN_BUTTON, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(PALETTE_BUTTON), nextPalette, FALLING);
  attachInterrupt(digitalPinToInterrupt(PATTERN_BUTTON), nextPattern, FALLING);

  Serial.begin(9600);

  currentPaletteCase = 0;
}


//----------------------------------------------------------------

void loop() {                    //code to run continuously

  int brightnessPot = analogRead(BRIGHTNESS_POTENTIOMETER);
  currentBrightness = map(brightnessPot, 0, 1023, 0, 240) + 15;
  FastLED.setBrightness(currentBrightness);

  switch (currentPaletteCase) {  //color palette picker
    case 1:
      currentPalette = rainbow;
      break;
    case 2:
      currentPalette = red;
      break;
    case 3:
      currentPalette = yellow;
      break;
    case 4:
      currentPalette = green;
      break;
    case 5:
      currentPalette = blue;
      break;
    case 6:
      currentPalette = purple;
      break;
  }

  switch (currentPatternCase) {  //pattern picker
    case 0:
      blinkPattern();
      break;
    case 1: //old pattern, now just a forced color switcher
    if (currentPaletteCase == 0) {
      currentPaletteCase = 1;
    }
    currentPatternCase++;
    case 2:
      sawtoothPattern(60, 0, 0);
      sawtoothPattern(60, 333, 1);
      sawtoothPattern(60, 666, 2);

      fadeToBlackBy(leds, NUM_LEDS, 8);
      break;
    case 3:
      wavePattern(30, 0, 0);
      wavePattern(30, 85, 1);
      wavePattern(30, 170, 2);
      wavePattern(40, 0, 3);

      fadeToBlackBy(leds, NUM_LEDS, 8);
      break;
    case 4:
      sawtoothPattern(120, 0, 0);
      break;
    case 5:
      wavePattern(50, 0, 0);
      break;
    case 6:
      sawtoothPattern(60, 0, 0);
      sawtoothPattern(71, 0, 1);
      sawtoothPattern(111, 0, 2);
      sawtoothPattern(63, 333, 3);
      sawtoothPattern(67, 666, 4);    

      fadeToBlackBy(leds, NUM_LEDS, 18);
      break;
    case 7:
      sparklePattern();
      fadeToBlackBy(leds, NUM_LEDS, 1);
      break;
  }

  FastLED.show();
}

//----------------------------------------------------------------
void blinkPattern() { //pattern setting
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  delay(250);
  FastLED.show();
  fill_palette(leds, NUM_LEDS, millis() / 500 * 16, 16, currentPalette, 255, LINEARBLEND);
  delay(250);
}

void sawtoothPattern(uint8_t bpm, int offset, uint8_t rand_index) {
  wavy = map(beat8(bpm, offset), 0, 255, 0, NUM_LEDS - 1);

  colorPicker(rand_index, wavy, 0);

  leds[wavy] = ColorFromPalette(currentPalette, rando[rand_index]);
}

void wavePattern(uint8_t bpm, uint8_t phase, uint8_t rand_index) {
  wavy = beatsin8(bpm, 0, NUM_LEDS - 1, 0, phase);

  colorPicker(rand_index, wavy, 0);
  colorPicker(rand_index, wavy, NUM_LEDS - 1);

  leds[wavy] = ColorFromPalette(currentPalette, rando[rand_index]);
}

void sparklePattern() {
  if (millis() - tick > 25) {
    leds[random8(NUM_LEDS - 1)] = ColorFromPalette(currentPalette, random8());
    tick = millis();
  }
}

void cacPattern() {
}


//--------------------------------
void colorPicker(uint8_t rand_index, uint8_t wav, uint8_t i) { //pattern setting helpers
  if (wav == i) {
    rando[rand_index] = slowRandom8(rando[rand_index]);
  }
}

uint8_t slowRandom8(uint8_t randy) {
  if (millis() - tick > 300) {
    tick = millis();

    uint8_t temp = random8();
    if (abs(temp - randy) < 16) {
      return random8();
    } else {
      return temp;
    }
  } else {
    return randy;
  }
}

//input gimmicks
void nextPalette() {
  if ((millis() - lastDebounceTime) > debounceDelay) {
    lastDebounceTime = millis();

    currentPaletteCase = currentPaletteCase % 6 + 1;
  }
}

void nextPattern() {
  if ((millis() - lastDebounceTime) > debounceDelay) {
    lastDebounceTime = millis();

    currentPatternCase = (currentPatternCase + 1) % 8;
  }
}