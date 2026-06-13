/*
 * [Revised] Stage 1 conveyor belt
 * Original behavior preserved; only the following changes applied:
 *   - pulseIn timeout added (prevents up to 1-second stall per sensor on echo loss)
 *   - On measurement failure, belt state is not changed (original code read failure
 *     as distance 0)
 *   - Magic numbers replaced with named constants; duplicated measurement code
 *     extracted into a function; unused variable (pos) removed
 *   - 2 µs LOW settling before trigger added (recommended HC-SR04 procedure);
 *     Serial output format made more readable
 *
 * Hardware: 2× HC-SR04 ultrasonic sensors, 1× continuous-rotation servo (belt drive)
 * Behavior (unchanged from original): when both sensors detect an object within their
 * respective thresholds, servo goes to 0 (belt running); if either is out of range,
 * servo goes to 90 (stop for a continuous-rotation servo).
 */
#include <Servo.h>

const int TRIG1_PIN = 2;
const int ECHO1_PIN = 3;
const int TRIG2_PIN = 4;
const int ECHO2_PIN = 5;
const int SERVO_PIN = 9;

const long DIST1_THRESHOLD_CM = 10;
const long DIST2_THRESHOLD_CM = 20;
const unsigned long ECHO_TIMEOUT_US = 30000UL;  // ~5 m detection range (10 m round-trip path)

Servo beltServo;

long readDistanceCm(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  unsigned long duration = pulseIn(echoPin, HIGH, ECHO_TIMEOUT_US);
  if (duration == 0) return -1;  // timeout: measurement failed
  return (long)(duration / 58.2);  // conversion factor kept from original (exact value ≈ 58.82 µs/cm)
}

void setup() {
  pinMode(TRIG1_PIN, OUTPUT);
  pinMode(ECHO1_PIN, INPUT);
  pinMode(TRIG2_PIN, OUTPUT);
  pinMode(ECHO2_PIN, INPUT);
  beltServo.attach(SERVO_PIN);
  Serial.begin(115200);
}

void loop() {
  delay(100);
  long distance1 = readDistanceCm(TRIG1_PIN, ECHO1_PIN);
  Serial.print("distance1 ");
  Serial.print(distance1);
  Serial.println(" cm");

  delay(100);
  long distance2 = readDistanceCm(TRIG2_PIN, ECHO2_PIN);
  Serial.print("distance2 ");
  Serial.print(distance2);
  Serial.println(" cm");

  if (distance1 < 0 || distance2 < 0) {
    return;  // measurement failed this cycle: keep belt state unchanged
  }

  if (distance1 > DIST1_THRESHOLD_CM || distance2 > DIST2_THRESHOLD_CM) {
    delay(50);
    beltServo.write(90);
    delay(50);
  } else {
    beltServo.write(0);
  }
}
