//----------------------------------------------------------------
/*
LED Controller
Designed by: Damian
*/

//----------------------------------------------------------------
#include <FastLED.h>  //libraries

#define NUM_LEDS 100  //pins & led strip
#define LED_PIN 13
#define PALETTE_BUTTON 3
#define PATTERN_BUTTON 2
#define BRIGHTNESS_POTENTIOMETER A3 // REVERSED ON LIGHT UNIT
#define BATTERY_DIVIDER A3

CRGB leds[NUM_LEDS];  //led strip array

uint8_t rando[6];  //randomizer variables for color setting of some patterns
uint8_t wavy;      //function variable for position setting of some patterns

uint8_t currentPaletteCase[2];  //input value holders
uint8_t currentPatternCase;
volatile uint8_t buttonState[2] = { 0, 0 };
uint8_t currentBrightness;

CRGBPalette16 currentPalette[2];

unsigned long lastDebounceTime = 0;  //timing gimmicks
unsigned int debounceDelay = 200;
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
  127, 255, 23, 0,
  231, 255, 0, 0,
  255, 255, 0, 0
};

DEFINE_GRADIENT_PALETTE(yellow){
  0, 255, 255, 0,
  127, 255, 127, 0,
  255, 255, 255, 0
};

DEFINE_GRADIENT_PALETTE(green){
  0, 0, 255, 0,
  127, 95, 255, 0,
  255, 0, 255, 0
};

DEFINE_GRADIENT_PALETTE(blue){
  0, 0, 0, 255,
  127, 0, 255, 255,
  255, 0, 0, 255
};

DEFINE_GRADIENT_PALETTE(purple){
  0, 63, 0, 255,
  127, 255, 0, 83,
  255, 63, 0, 255
};

DEFINE_GRADIENT_PALETTE(white){
  0, 255, 255, 255,
  127, 63, 63, 63,
  255, 255, 255, 255
};

DEFINE_GRADIENT_PALETTE(battery){
  0, 255, 0, 0,
  10, 255, 0, 0,
  140, 255, 255, 0,
  150, 255, 255, 0,
  233, 0, 255, 0,
  255, 0, 255, 0
};

//----------------------------------------------------------------
void setup() {  //code to run once
  delay(500);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(currentBrightness);

  pinMode(PALETTE_BUTTON, INPUT_PULLUP);
  pinMode(PATTERN_BUTTON, INPUT_PULLUP);

  pinMode(6, OUTPUT);
  digitalWrite(6, HIGH);

  attachInterrupt(digitalPinToInterrupt(PALETTE_BUTTON), pressPalette, FALLING);
  attachInterrupt(digitalPinToInterrupt(PATTERN_BUTTON), pressPattern, FALLING);

  Serial.begin(9600);

  currentPaletteCase[0] = 1;
  currentPaletteCase[1] = 0;
  currentPatternCase = 4;

  setColorPalette();
  setOtherPalette();
}


//----------------------------------------------------------------

void loop() {  //code to run continuously
  setBrightness();
  getButtonInput();

  getPattern();
  FastLED.show();
}

//----------------------------------------------------------------
void setBrightness() { // brightness setting
  EVERY_N_MILLISECONDS(100) {
    int brightnessVar = analogRead(BRIGHTNESS_POTENTIOMETER);
    currentBrightness = 5 + map(brightnessVar, 0, 1023, 0, 250);
    FastLED.setBrightness(currentBrightness);
  }
}

//----------------------------------------------------------------
void getButtonInput() {
  uint8_t inputState = buttonState[0] + buttonState[1];

  if (inputState == 2) {  //simultaneous press + release
    currentPalette[1] = currentPalette[0];    
    currentPaletteCase[1] = 1;

    resetButtonState();
    resetButtonState();
  } else if (inputState == 1 && (millis() - lastDebounceTime) > debounceDelay) {  //single button press + release
    if (buttonState[0] == 1) {  //press + release on palette button
      currentPaletteCase[0] = currentPaletteCase[0] % 7 + 1;

      setColorPalette();
      setOtherPalette();

    } else {  //press + release on pattern button
      currentPatternCase = currentPatternCase % 7 + 1;
    }
    resetButtonState();
  }
}

//----------------------------------------------------------------
void setColorPalette() {
  switch (currentPaletteCase[0]) {  //color palette picker
    case 1:
      currentPalette[0] = rainbow;
      break;
    case 2:
      currentPalette[0] = red;
      break;
    case 3:
      currentPalette[0] = yellow;
      break;
    case 4:
      currentPalette[0] = green;
      break;
    case 5:
      currentPalette[0] = blue;
      break;
    case 6:
      currentPalette[0] = purple;
      break;
    case 7:
      currentPalette[0] = white;
      break;
  }
}

void setOtherPalette() {
  if (currentPaletteCase[1] == 0) {
    currentPalette[1] = currentPalette[0];
  }
}

//----------------------------------------------------------------
void getPattern() {
  switch (currentPatternCase) {  //pattern picker
    case 0:
      batteryLevel();
      break;
    case 1:
      blinkPattern();
      break;
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
      sawtoothPattern(100, 0, 0);
      break;
    case 5:
      wavePattern(50, 0, 0);
      break;
    case 6:
      sawtoothPattern(60, 0, 0);
      sawtoothPattern(16, 0, 1);
      sawtoothPattern(111, 300, 2);
      sawtoothPattern(18, 80, 3);
      sawtoothPattern(67, 100, 4);
      sawtoothPattern(23, 200, 5);

      fadeToBlackBy(leds, NUM_LEDS, 18);
      break;
    case 7:
      sparklePattern();
      fadeToBlackBy(leds, NUM_LEDS, 1);
      break;
  }
}

//----------------------------------------------------------------
void batteryLevel() {  //pattern setting
  int lvl = analogRead(BATTERY_DIVIDER);
  uint8_t batteryLvl = map (lvl, 720, 1023, 0, NUM_LEDS);

  CRGBPalette16 batteryPalette = battery;

  wavy = map(beat8(30, 500), 0, 255, 0, NUM_LEDS);

  if (wavy < batteryLvl - 1) {
    leds[wavy] = ColorFromPalette(batteryPalette, wavy * 255 / NUM_LEDS);
  }
  leds[NUM_LEDS] = CRGB::Black;

  wavy = map(beat8(30, 1700), 0, 255, 0, NUM_LEDS);
  leds[wavy] = CRGB::Black;
}

void blinkPattern() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  delay(250);
  FastLED.show();
  fill_palette(leds, NUM_LEDS, millis() / 500 * 16, 16, currentPalette[random8() % 2], 255, LINEARBLEND);
  delay(250);
}


void sawtoothPattern(uint8_t bpm, int offset, uint8_t rand_index) {  // bpm, offset from internal timer, index of random variable holder
  wavy = map(beat8(bpm, offset), 0, 255, 0, NUM_LEDS);

  huePicker(rand_index, wavy, 0);

  leds[wavy] = ColorFromPalette(currentPalette[rando[rand_index] % 2], rando[rand_index]);
  leds[NUM_LEDS] = CRGB::Black;
}

void wavePattern(uint8_t bpm, uint8_t phase, uint8_t rand_index) { // bpm, phase offset, index of random variable holder
  wavy = beatsin8(bpm, 0, NUM_LEDS - 1, 0, phase);

  huePicker(rand_index, wavy, 0);
  huePicker(rand_index, wavy, NUM_LEDS - 1);

  leds[wavy] = ColorFromPalette(currentPalette[rando[rand_index] % 2], rando[rand_index]);
}

void sparklePattern() {
  if (millis() - tick > (768 / NUM_LEDS)) {
    leds[random8(NUM_LEDS)] = ColorFromPalette(currentPalette[random8() % 2], random8());
    tick = millis();
  }
}

//--------------------------------
void huePicker(uint8_t rand_index, uint8_t wav, uint8_t i) { 
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

//----------------------------------------------------------------
//input gimmicks
void pressPalette() {
  if (digitalRead(PALETTE_BUTTON) == LOW) {
    buttonState[0] = 1;
    if ((millis() - lastDebounceTime) > debounceDelay) {
      lastDebounceTime = millis();
    }
  }
}

void pressPattern() {
  if (digitalRead(PATTERN_BUTTON) == LOW) {
    buttonState[1] = 1;
    if ((millis() - lastDebounceTime) > debounceDelay) {
      lastDebounceTime = millis();
    }
  }
}

void resetButtonState() {
  buttonState[0] = 0;
  buttonState[1] = 0;
  fill_solid(leds, NUM_LEDS, CRGB(127, 127, 127));
  FastLED.show();
  delay(150);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
  delay(150);
}