#include <FastLED.h>
#include <Button.h>

#define NUM_LEDS 32
#define LED_PIN A4

CRGB leds[NUM_LEDS];

uint8_t index = 0;
uint8_t casePalette = 0;

Button nextPalette(2);

DEFINE_GRADIENT_PALETTE (rainbow) {
  0,   255,  0,  0,
  31,  255, 191, 0,
  63,  127, 255, 0,
  95,    0, 255, 63,
  127,   0, 255, 255,
  159,  63,   0, 255,
  191, 128,   0, 255,
  223, 255,   0, 191,
  255, 255,   0,   0
};

DEFINE_GRADIENT_PALETTE (red) {
  0,   255,   0, 0,
  127, 255, 128,  0,
  255, 255,   0, 0
};

DEFINE_GRADIENT_PALETTE (orange) {
  0,    255, 191, 0,
  127,  127, 255, 0,
  255,  255, 191, 0
};

DEFINE_GRADIENT_PALETTE (yellow) {
  0,   255,  255, 0,
  127, 255, 127, 0,
  255, 255,  255, 0
};

DEFINE_GRADIENT_PALETTE (green) {
  0,   0, 255, 0,
  127, 0, 255, 63,
  255, 0, 255, 0
};

DEFINE_GRADIENT_PALETTE (blue) {
  0,   0,  0, 255,
  127, 0, 255,  255,
  255, 0,  0, 255
};

DEFINE_GRADIENT_PALETTE (purple) {
  0,   127, 0, 255,
  127, 255, 0,  63,
  255, 127, 0, 255
};

DEFINE_GRADIENT_PALETTE (pink) {
  0,   255,  0, 255,
  128, 255, 0,  128,
  255, 255,  0, 255
};
CRGBPalette16 currentPalette = rainbow;

void setup() {
  // put your setup code here, to run once:
  delay(500);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);

  nextPalette.begin();
  
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  EVERY_N_MILLISECONDS(8) {
    if  (nextPalette.pressed()) {
      if (casePalette == 7) {
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
      currentPalette = orange;
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
      case 7:
      currentPalette = pink;
    }
  }
}
  

  EVERY_N_MILLISECONDS(20) {
    fill_palette(leds, NUM_LEDS, index, 16, currentPalette, 255, LINEARBLEND);  
    index += 8;
    FastLED.show();
  };
}
