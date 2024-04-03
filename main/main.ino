#include <FastLED.h>
#include <Button.h>

#define NUM_LEDS 32
#define LED_PIN A4
#define LED_BRIGHTNESS 50

CRGB leds[NUM_LEDS];

uint8_t index = 0;
uint8_t currentPaletteCase = 0;
uint8_t currentPatternCase = 1;

Button nextPalette(3);
Button nextPattern(2);

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
  127, 255, 128, 0,
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

  nextPalette.begin();
  nextPattern.begin();

  Serial.begin(9600);
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
      currentPaletteCase = 0;
  }

  switch (currentPatternCase) {
    case 1:
      blinkPattern();
      break;
    case 2:
      wavePattern();
      break;
    case 3:
      currentPatternCase = 1;
      break;
  }

  EVERY_N_MILLISECONDS(8) {
    if (nextPalette.pressed()) {
      currentPaletteCase++;
    }
    if (nextPattern.pressed()) {
      currentPatternCase++;
    }
  }

  EVERY_N_MILLISECONDS(20) {
    index += 8;
  };

  FastLED.show();
}

void blinkPattern() {
  fill_palette(leds, NUM_LEDS, 0, 16, currentPalette, 255, LINEARBLEND);
  delay(500);
  FastLED.show();
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  delay(500);
  
}

void wavePattern() {
  fill_palette(leds, NUM_LEDS, index, 16, currentPalette, 255, LINEARBLEND);
}
