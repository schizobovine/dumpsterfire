/*
 * dumpsterfire.ino
 *
 * Arduino-controlled desktop dumpster fire.
 *
 */

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#include <Bounce2.h>
//#include <Servo.h>

////////////////////////////////////////////////////////////////////////
// PIN ASSIGNMENT
////////////////////////////////////////////////////////////////////////

//#define SERVO_PIN  5
#define TRIMPOT    A0
#define DISP_RST   12

#define ANGLE_MIN 30
#define ANGLE_MAX 150

////////////////////////////////////////////////////////////////////////
// GLOBAL STATE VARIABLES
////////////////////////////////////////////////////////////////////////

// Display
Adafruit_SSD1306 display(DISP_RST);

// Servo
//Servo srvo;

// Holds the current servo angle
uint8_t srv_angle = ANGLE_MIN;
uint8_t old_angle = ANGLE_MIN;

////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
////////////////////////////////////////////////////////////////////////

void printStatus() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(srv_angle, DEC);
  display.display();
}

uint8_t readTrimpot() {
  uint16_t reading = analogRead(TRIMPOT);
  return map(reading, 0, 1023, ANGLE_MIN, ANGLE_MAX);
}

////////////////////////////////////////////////////////////////////////
// STARTUP CODE
////////////////////////////////////////////////////////////////////////

void setup() {

  delay(100);

  // Setup display
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setTextWrap(true);
  display.println(F("DUMPSTER"));
  display.println(F("  FIRE  "));
  display.display();
  display.setTextWrap(false);

  // Used as servo on status indicator
  //pinMode(LED_BUILTIN, OUTPUT);
  //digitalWrite(LED_BUILTIN, LOW);

  // Configure pins
  pinMode(TRIMPOT, INPUT);
  //pinMode(SERVO_PIN, INPUT);

}

////////////////////////////////////////////////////////////////////////
// MAIN LOOP
////////////////////////////////////////////////////////////////////////

void loop() {

  srv_angle = readTrimpot();

  if (srv_angle != old_angle) {
    old_angle = srv_angle;
    printStatus();
  } else {
    delay(100);
  }

  delay(100);

}

// vi: syntax=arduino
