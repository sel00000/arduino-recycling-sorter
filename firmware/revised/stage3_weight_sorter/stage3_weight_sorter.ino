/*
 * [Revised] Stage 3 weight sorting (HX711 load cell)
 * The original (firmware/original/stage3_weight_sorter) had its record cut off with
 * loop()'s closing brace missing. The missing brace was restored; threshold values
 * replaced with named constants; servo variable names changed
 * (myservo/myservo2 → rotateServo/dumpServo). All other logic — including the chained
 * delays for servo jitter buffering — is kept from the original.
 *
 * Required library: HX711 (install "HX711" by bogde from the Library Manager)
 * Hardware: HX711 + load cell, 2× servos (rotate, dump)
 * Behavior (unchanged from original): weight > 1.0 → rotate servo to 90° then eject
 * (heavy = glass bin); weight > 0.5 and ≤ 1.0 → rotate servo to 0° then eject
 * (light = plastic bin); weight ≤ 0.5 → no action (known limitation — see
 * docs/code-review.md).
 *
 * Calibration: SCALE_FACTOR (23600) was tuned for the load cell used at the time
 * without a reference/calibration weight, so the measurement unit cannot be guaranteed
 * to be kg (judges' Q7). Procedure: ① call tare() with empty scale ② place reference
 * weight (e.g. 1.000 kg) ③ adjust SCALE_FACTOR until get_units() reads 1.000.
 */
#include "HX711.h"
#include <Servo.h>

const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;
const int ROTATE_SERVO_PIN = 5;
const int DUMP_SERVO_PIN = 6;

const float SCALE_FACTOR = 23600.0f;  // load-cell calibration factor
const float HEAVY_THRESHOLD = 1.0f;   // above this → heavy (glass) bin
const float LIGHT_THRESHOLD = 0.5f;   // above this → light (plastic) bin

HX711 scale;
Servo rotateServo;  // sorting direction rotation
Servo dumpServo;    // eject (dump)

void setup() {
  Serial.begin(38400);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(SCALE_FACTOR);
  scale.tare();
  rotateServo.attach(ROTATE_SERVO_PIN);
  dumpServo.attach(DUMP_SERVO_PIN);
  rotateServo.write(0);
  dumpServo.write(0);
}

void loop() {
  float weight = scale.get_units(5);  // 5-sample average (~0.5 s at 10 SPS)
  Serial.println(weight);

  // Chained delay(1000) calls are an intentional buffer for servo jitter (original design)
  if (weight > HEAVY_THRESHOLD) {
    delay(1000);
    rotateServo.write(90);
    delay(1000);
    dumpServo.write(90);
    delay(1000);
    dumpServo.write(0);
    delay(1000);
    rotateServo.write(0);
    delay(1000);
  } else if (weight > LIGHT_THRESHOLD) {
    delay(1000);
    rotateServo.write(0);
    delay(1000);
    dumpServo.write(90);
    delay(1000);
    dumpServo.write(0);
    delay(1000);
  }

  delay(100);
}
