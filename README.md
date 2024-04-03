----------------------------------------------------------------
# FastLED_General_Controller
Arduino code for controlling WS2812B LED strip, using FastLED library
Plays various animations by toggling various inputs
Author: Damian Lee

----------------------------------------------------------------
Arduino Circuit:
Arduino Nano or Arduino Pro Mini 5V was used
-digital pin 2 (digital pin with interrupt) -> button to ground
-digital pin 3 (digital pin with interrupt) -> button to ground
-analogue pin 3 (any analogue pin) -> middle of 10k potentiometer
-analogue pin 4 (any digital pin) -> data out to LED strip

----------------------------------------------------------------
Inputs:
5V power to the Arduino Nano/Arduino Pro Mini controller and LED strip
2 buttons for toggling animations
	-color palette picker
	-animation picker
potentiometer for adjusting brightness