#include <FastLED.h>

#define NUM_LEDS 32
#define LED_PIN A4
#define LED_BRIGHTNESS 50

CRGB leds[NUM_LEDS];

uint8_t index = 0;
uint8_t currentPaletteCase = 0;
uint8_t currentPatternCase = 0;

uint8_t rand1 = random8();
uint8_t rand2 = random8();
uint8_t rand3 = random8();
uint8_t wave1;
uint8_t wave2;
uint8_t wave3;

unsigned long lastDebounceTime = 0;
unsigned int debounceDelay = 250;
unsigned long tick = 0;

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
  127, 255, 63, 0,
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
  0, 63, 0, 255,
  127, 255, 0, 63,
  255, 63, 0, 255
};

CRGBPalette16 currentPalette(
  CRGB::White,
  CRGB::Black,
  CRGB::White,
  CRGB::Black,
  CRGB::White,
  CRGB::Black,
  CRGB::White,
  CRGB::Black,
  CRGB::White,
  CRGB::Black,
  CRGB::White,
  CRGB::Black,
  CRGB::White,
  CRGB::Black,
  CRGB::White,
  CRGB::Black);

void setup() {
  // put your setup code here, to run once:
  delay(500);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(LED_BRIGHTNESS);

  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(3), nextPalette, FALLING);
  attachInterrupt(digitalPinToInterrupt(2), nextPattern, FALLING);

  Serial.begin(9600);

  currentPaletteCase = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  switch (currentPaletteCase) {
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

  switch (currentPatternCase) {
    case 0:
      blinkPattern();
      break;
    case 1:
      flowPattern();
      break;
    case 2:
      sawtoothPattern();
      break;
    case 3:
      wavePattern();
      break;
    case 4:
      sparklePattern();
      break;
  }

  EVERY_N_MILLISECONDS(20) {
    index += 16;
  };

  FastLED.show();
}

void blinkPattern() {
  fill_palette(leds, NUM_LEDS, index, 16, currentPalette, 255, LINEARBLEND);
  delay(400);
  FastLED.show();
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  delay(400);
}

void flowPattern() {
  if (currentPaletteCase == 0) {
    currentPaletteCase = 1;
  }
  fill_palette(leds, NUM_LEDS, index, 16, currentPalette, 255, LINEARBLEND);
}

void sawtoothPattern() {
  if (millis() - tick >= 2000) {
    rand1 = random8();
    tick = millis();
  } else if (millis() - tick == 1333) {
    rand2 = random8();
  } else if (millis() - tick == 666) {
    rand3 = random8();
  }

  wave1 = map(beat8(60, 0), 60, 255, 0, NUM_LEDS - 1);
  wave2 = map(beat8(60, 333), 60, 255, 0, NUM_LEDS - 1);
  wave3 = map(beat8(60, 666), 60, 255, 0, NUM_LEDS - 1);

  leds[wave1] = ColorFromPalette(currentPalette, rand1);
  leds[wave2] = ColorFromPalette(currentPalette, rand2);
  leds[wave3] = ColorFromPalette(currentPalette, rand3);

  fadeToBlackBy(leds, NUM_LEDS, 10);
}

void wavePattern() {
  if (millis() % 499 <= 2) {
    tick = millis() % 500;
  } else if (tick == 333) {
    rand3 = random8();
  } else if (tick == 171) {
    rand2 = random8();
  } else if (tick == 2) {
    rand1 = random8();
  }

    wave1 = beatsin8(30, 0, NUM_LEDS - 1, 0, 0);
    wave2 = beatsin8(30, 0, NUM_LEDS - 1, 0, 85);
    wave3 = beatsin8(30, 0, NUM_LEDS - 1, 0, 170);

    leds[wave1] = ColorFromPalette(currentPalette, rand1);
    //leds[wave2] = ColorFromPalette(currentPalette, rand2);
    //leds[wave3] = ColorFromPalette(currentPalette, rand3);

    fadeToBlackBy(leds, NUM_LEDS, 6);
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

  void nextPalette() {
    if ((millis() - lastDebounceTime) > debounceDelay) {
      lastDebounceTime = millis();

      currentPaletteCase = currentPaletteCase % 6 + 1;
    }
  }

  void nextPattern() {
    if ((millis() - lastDebounceTime) > debounceDelay) {
      lastDebounceTime = millis();

      currentPatternCase = (currentPatternCase + 1) % 5;
    }
  }