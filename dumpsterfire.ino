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
#include <Bounce2.h>
//#include <Servo.h>

////////////////////////////////////////////////////////////////////////
// PIN ASSIGNMENT
////////////////////////////////////////////////////////////////////////

#define ENC_A      2
#define ENC_B      3
#define SERVO_GO   12
#define SERVO_PIN  5
#define ENC_RST    A0
#define DISP_RST   A3

#define ANGLE_MIN 0
#define ANGLE_MAX 180

////////////////////////////////////////////////////////////////////////
// GLOBAL STATE VARIABLES
////////////////////////////////////////////////////////////////////////

// Display
Adafruit_SSD1306 display(DISP_RST);

// Buttons
Bounce butt_reset;

// Servo
//Servo lifter;

// Holds the current servo angle
volatile uint8_t angle = ANGLE_MIN;
uint8_t old_angle = ANGLE_MAX;

// Servo on/off state via switch
boolean servo_go = false;

// Encoder variables
volatile uint8_t reading = 0;
boolean seen_a = false;
boolean seen_b = false;

////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
////////////////////////////////////////////////////////////////////////

void printStatus() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(angle, DEC);
  display.display();
}

////////////////////////////////////////////////////////////////////////
// INTERRUPT HANDLERS
////////////////////////////////////////////////////////////////////////

void intr_enc_a() {
  cli();
  reading = PIND & 0x0C;
  if ((reading == 0x0C) && seen_a) {
    if (angle > ANGLE_MIN)
      angle--;
    seen_a = false;
    seen_b = false;
  } else if (reading == 0x04) {
    seen_b = true;
  }
  sei();
}

void intr_enc_b() {
  cli();
  reading = PIND & 0x0C;
  if ((reading == 0x0C) && seen_b) {
    if (angle < ANGLE_MAX)
      angle++;
    seen_a = false;
    seen_b = false;
  } else if (reading == 0x08) {
    seen_a = true;
  }
  sei();
}

////////////////////////////////////////////////////////////////////////
// STARTUP CODE
////////////////////////////////////////////////////////////////////////

void setup() {

  // Setup display
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setTextWrap(false);
  display.println("DUMPSTRFIR");
  display.display();

  // Configure rotary enccoder pins as inputs
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  pinMode(SERVO_GO, INPUT_PULLUP);

  // Configure butt(ons)
  butt_reset.attach(ENC_RST, INPUT_PULLUP, 10);

  // Used as servo on status indicator
  //pinMode(LED_BUILTIN, OUTPUT);
  //digitalWrite(LED_BUILTIN, LOW);

  // Configure servo controller
  //pinMode(SERVO_PIN, OUTPUT);

  // Attach interrupt handlers
  attachInterrupt(digitalPinToInterrupt(ENC_A), intr_enc_a, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_B), intr_enc_b, RISING);

}

////////////////////////////////////////////////////////////////////////
// MAIN LOOP
////////////////////////////////////////////////////////////////////////

void loop() {

  butt_reset.update();
  if (butt_reset.read() == LOW) {
    angle = 0;
  }

  servo_go = (digitalRead(SERVO_GO) == LOW);

  if (!servo_go) {
    //digitalWrite(LED_BUILTIN, HIGH);
  } else {
    //digitalWrite(LED_BUILTIN, LOW);
    //lifter.attach(SERVO_PIN);
    //lifter.write(angle);
    //lifter.detach();
  }

  if (angle != old_angle) {
    old_angle = angle;
    printStatus();
  } else {
    delay(100);
  }

}

// vi: syntax=arduino
