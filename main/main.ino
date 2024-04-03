#include <FastLED.h>

#define NUM_LEDS 32
#define LED_PIN A4

CRGB leds[NUM_LEDS];

uint8_t index = 0;
uint8_t casePalette = 0;

DEFINE_GRADIENT_PALETTE (rainbow) {
  0,   255,  0, 0,
  128,   0, 255, 0,
  255, 255,  0, 0
};

DEFINE_GRADIENT_PALETTE (red) {
  0,   255,  0, 0,
  128, 255, 64, 0,
  255, 255,  0, 0
};

DEFINE_GRADIENT_PALETTE (green) {
  0,   64,  255, 0,
  128, 0, 255, 32,
  255, 64,  255, 0
};

CRGBPalette16 currentPalette = green;

void setup() {
  // put your setup code here, to run once:
  delay(500);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);

  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);

  Serial.begin(9600);

  attachInterrupt(digitalPinToInterrupt(2), nextPalette, FALLING);

}

void loop() {
  // put your main code here, to run repeatedly:
  fill_palette(leds, NUM_LEDS, index, 8, currentPalette, 255, LINEARBLEND);  
  

  EVERY_N_MILLISECONDS(16) {
    index += 8;
    FastLED.show();
  };
}

void nextPalette() {
  if (casePalette == 3) {
    casePalette = 0;
  } else {
    casePalette++;
  }

  switch (casePalette) {
    case 0:
    currentPalette = rainbow;
    break;
    case 1:
    currentPalette = red;
    break;
    case 2:
    currentPalette = green;
  }
}