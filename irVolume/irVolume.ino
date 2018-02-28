/*-----------------------------------------
 "irVolume.ino"
Arduino controlled volume dial that transmits IR codes to a regular hifi-amplifier.
The dial also have some haptic and light feedback.
-----------------------------------------*/

#include <IRLib.h>
#include <Encoder.h>
#include "FastLED.h"

Encoder myEnc(2, 4);
IRsend My_Sender;

const byte numPixelLeds = 12;
const byte pixelLedPin =  6;
const byte motorPin = 9;
const byte piezoPin = 11;
const byte buttonPin = 5;

CRGB leds[numPixelLeds];

int volPixelNum = 0;
long oldPosition  = -999;
unsigned long previousMillis = 0;
unsigned long currentMillis;
long newPosition;
bool trigger = false;
int count = 0;
int button = LOW;

void setup() {
  FastLED.addLeds<WS2812, pixelLedPin, GRB>(leds, numPixelLeds);
  FastLED.clear();

  pinMode(motorPin, OUTPUT);
  pinMode(piezoPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  calibrateReset();
}

void loop() {
  newPosition = myEnc.read();
  currentMillis = millis();

  if ((newPosition != oldPosition) && (currentMillis - previousMillis >= 50) && (trigger == false)) {
    if (((newPosition - oldPosition) > 3) || ((oldPosition - newPosition) > 3)) {
      previousMillis = currentMillis;
      if (newPosition < oldPosition) {
        My_Sender.send(RC5, 0x1C11, 13);
        leds[volPixelNum] = CRGB::Black;
        volPixelNum--;
        if (volPixelNum <= 0) volPixelNum = 0;
      } else {
        My_Sender.send(RC5, 0x1C10, 13);
        if (volPixelNum >= 11) volPixelNum = 11;
        volPixelNum++;
        leds[volPixelNum] = CHSV(213, 200, 20);
      }
      FastLED.show();
      oldPosition = newPosition;
      trigger = true;
    }
  }
  if (trigger == true) {
    if (count <= 1) {
      digitalWrite(piezoPin, HIGH);
      //digitalWrite(motorPin, HIGH);
    }
    if (count >= 50) {
      digitalWrite(piezoPin, LOW);
    }
    if (count >= 1000) {
      //digitalWrite(motorPin, LOW);
      trigger = false;
      count = 0;
    }
    count++;
  }

  button = digitalRead(buttonPin);
  if (button == LOW) {
    calibrateReset();
    delay(50);
    My_Sender.send(RC5, 0x1C0C, 13);
    digitalWrite(motorPin, HIGH);
    digitalWrite(piezoPin, HIGH);
    delay(1);
    digitalWrite(piezoPin, LOW);
    delay(3);
    digitalWrite(piezoPin, HIGH);
    delay(4);
    digitalWrite(piezoPin, LOW);
    delay(1);
    digitalWrite(piezoPin, HIGH);
    delay(1);
    digitalWrite(piezoPin, LOW);
    delay(100);
    digitalWrite(motorPin, LOW);
    delay(1000);
  }

}

void calibrateReset() {
  for (int j = 0; j <= 30; j++) {
    My_Sender.send(RC5, 0x1C11, 13);
    delay(5);
  }
  volPixelNum = 0;
  FastLED.clear();
  FastLED.show();
}


