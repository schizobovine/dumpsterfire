/*
 * dumpsterfire.ino
 *
 * Arduino-controlled desktop dumpster fire.
 *
 */

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include <Bounce2.h>
//#include <Metro.h>
//#include <Adafruit_TiCoServo.h>
#include <SD.h>
#include <TMRpcm.h>

////////////////////////////////////////////////////////////////////////
// PIN ASSIGNMENT
////////////////////////////////////////////////////////////////////////

#define PIN_BUTT       2
#define PIN_SPEAKER    9
#define PIN_PIXELS     4
#define PIN_AMP_EN     7
#define PIN_SD_CS      8
//#define PIN_SERVO      9

#define DEBOUNCE_MS    50
#define SERVO_MS      500

#define PIXEL_NUM  (30)
#define PIXEL_MODE ((NEO_GRB)+(NEO_KHZ800))

#define SERVO_PULSE_CLOSED 2300
#define SERVO_PULSE_OPEN   1700

////////////////////////////////////////////////////////////////////////
// GLOBAL STATE VARIABLES
////////////////////////////////////////////////////////////////////////

// Audio
TMRpcm audio;

// Servo
//Adafruit_TiCoServo srv;
//Metro srv_timer;
bool lid_closed = true;
bool in_motion = false;

// NeoPixels
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXEL_NUM, PIN_PIXELS, PIXEL_MODE);

// Butt(ons)
Bounce butt;

////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
////////////////////////////////////////////////////////////////////////

void lidOpen() {
  lid_closed = false;
  //srv.write(SERVO_PULSE_OPEN);
}

void lidClosed() {
  lid_closed = true;
  //srv.write(SERVO_PULSE_CLOSED);
}

void startFlipLid() {
  in_motion = true;
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(PIN_AMP_EN, HIGH);
  //srv_timer.reset();
  if (lid_closed) {
    lidOpen();
  } else {
    lidClosed();
  }
  audio.play("fire.wav");
}

void stopFlipLid() {
  in_motion = false;
  digitalWrite(LED_BUILTIN, LOW);
  audio.stopPlayback();
  digitalWrite(PIN_AMP_EN, LOW);
}

// Lock up the chip and flash built-in LED as a distress beacon
void error(int period=250) {
  bool flip = true;
  while (true) {
    digitalWrite(LED_BUILTIN, (flip ? HIGH : LOW));
    flip = !flip;
    delay(period);
  }
}

////////////////////////////////////////////////////////////////////////
// STARTUP CODE
////////////////////////////////////////////////////////////////////////

void setup() {

  // Setup SD card
  if (!SD.begin(PIN_SD_CS)) {
    error();
  }

  // Signal for when we're in process
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // Setup butt(on)
  butt.attach(PIN_BUTT, INPUT_PULLUP, DEBOUNCE_MS);

  // Attach servo & setup servo timer
  //srv_timer.interval(SERVO_MS);
  //srv.attach(PIN_SERVO, SERVO_PULSE_OPEN, SERVO_PULSE_CLOSED);
  lidClosed();

  // Startup NeoPixels
  pixels.begin();
  for (uint8_t i=0; i<pixels.numPixels(); i++) {
    pixels.setPixelColor(i, 255 - ((i * 5) % 256), 0, 0);
  }
  pixels.show();

  // Setup audio
  audio.speakerPin = PIN_SPEAKER;
  pinMode(PIN_AMP_EN, OUTPUT);
  digitalWrite(PIN_AMP_EN, LOW);

}

////////////////////////////////////////////////////////////////////////
// MAIN LOOP
////////////////////////////////////////////////////////////////////////

void loop() {

  // Check button and if we're not currently moving, setup to do so
  if (butt.update()) {
    if (butt.fell() && !in_motion) {
      startFlipLid();
    }
  }

  // If we're in motion the timer has finished, set state to no longer in
  // motion
  //if (in_motion && srv_timer.check()) {
  if (in_motion && !audio.isPlaying()) {
  //if (in_motion) {
    stopFlipLid();
  }

}

// vi: syntax=arduino
