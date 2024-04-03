//----------------------------------------------------------------
/*
LED Controller
Designed by: Damian
*/

//----------------------------------------------------------------
#include <FastLED.h>  //libraries

#define NUM_LEDS 32  //pins & led strip
#define LED_PIN A4
#define LED_BRIGHTNESS 15
#define PALETTE_BUTTON 3
#define PATTERN_BUTTON 2

CRGB leds[NUM_LEDS];  //led strip array

uint8_t currentPaletteCase = 0;  //buttons increment
uint8_t currentPatternCase = 0;

uint8_t rand1;  //randomizer variables for color setting
uint8_t rand2;
uint8_t rand3;
uint8_t wave1;  //function variables for position setting
uint8_t wave2;
uint8_t wave3;

uint8_t index = 0;  //periodic incrementing value for some patterns

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
  FastLED.setBrightness(LED_BRIGHTNESS);

  pinMode(PALETTE_BUTTON, INPUT_PULLUP);
  pinMode(PATTERN_BUTTON, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(PALETTE_BUTTON), nextPalette, FALLING);
  attachInterrupt(digitalPinToInterrupt(PATTERN_BUTTON), nextPattern, FALLING);

  Serial.begin(9600);

  currentPaletteCase = 0;
}


//----------------------------------------------------------------

void loop() {                    //code to run continuously
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
    case 1:
      flowPattern();
      break;
    case 2:
      sawtoothPattern(60, 60, 60);
      break;
    case 3:
      wavePattern(30, 30, 30);
      break;
    case 4:
      sawtoothPattern(60, 81, 121);
      wavePattern(30, 31, 41);
      break;
    case 5:
      sparklePattern();
      break;
  }

  EVERY_N_MILLISECONDS(20) {
    index += 16;
  };

  FastLED.show();
}

//----------------------------------------------------------------
void blinkPattern() { //pattern setting
  fill_palette(leds, NUM_LEDS, index, 16, currentPalette, 255, LINEARBLEND);
  delay(250);
  FastLED.show();
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  delay(250);
}

void flowPattern() {
  // if (currentPaletteCase == 0) {
  //   currentPaletteCase = 1;
  // }
  fill_palette(leds, NUM_LEDS, index, 16, currentPalette, 255, LINEARBLEND);
}

void sawtoothPattern(uint8_t bpm1, uint8_t bpm2, uint8_t bpm3) {
  wave1 = map(beat8(bpm1, 0), 0, 255, 0, NUM_LEDS - 1);
  wave2 = map(beat8(bpm2, 333), 0, 255, 0, NUM_LEDS - 1);
  wave3 = map(beat8(bpm3, 666), 0, 255, 0, NUM_LEDS - 1);

  colorPicker(wave1, wave2, wave3, 0);
  colorPicker(wave1, wave2, wave3, NUM_LEDS - 1);

  leds[wave1] = ColorFromPalette(currentPalette, rand1);
  leds[wave2] = ColorFromPalette(currentPalette, rand2);
  leds[wave3] = ColorFromPalette(currentPalette, rand3);

  fadeToBlackBy(leds, NUM_LEDS, 8);
}

void wavePattern(uint8_t bpm1, uint8_t bpm2, uint8_t bpm3) {
  wave1 = beatsin8(bpm1, 0, NUM_LEDS - 1, 0, 0);
  wave2 = beatsin8(bpm2, 0, NUM_LEDS - 1, 0, 85);
  wave3 = beatsin8(bpm3, 0, NUM_LEDS - 1, 0, 170);

  colorPicker(wave1, wave2, wave3, 0);
  colorPicker(wave1, wave2, wave3, NUM_LEDS - 1);

  leds[wave1] = ColorFromPalette(currentPalette, rand1);
  leds[wave2] = ColorFromPalette(currentPalette, rand2);
  leds[wave3] = ColorFromPalette(currentPalette, rand3);

  fadeToBlackBy(leds, NUM_LEDS, 8);
}

void sparklePattern() {
  if (millis() - tick > 25) {
    leds[random8(NUM_LEDS - 1)] = ColorFromPalette(currentPalette, random8());
    tick = millis();
  }
  fadeToBlackBy(leds, NUM_LEDS, 1);
}

void cacPattern() {
}


//--------------------------------
void colorPicker(uint8_t wav1, uint8_t wav2, uint8_t wav3, uint8_t i) { //pattern setting helpers
  if (wav1 == i) {
    rand1 = slowRandom8(rand1);
  } else if (wav2 == i) {
    rand2 = slowRandom8(rand2);
  } else if (wav3 == i) {
    rand3 = slowRandom8(rand3);
  }
}

uint8_t slowRandom8(uint8_t rando) {
  if (millis() - tick > 300) {
    tick = millis();
    return random8();
  } else {
    return rando;
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

    currentPatternCase = (currentPatternCase + 1) % 6;
  }
}