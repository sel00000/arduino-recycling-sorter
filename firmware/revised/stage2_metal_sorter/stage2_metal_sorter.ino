/*
 * [Revised] Stage 2 metal separation (inductive proximity sensor)
 * The original draft (firmware/original/stage2_metal_sorter) did not compile;
 * the following fixes were applied:
 *   - Mixed identifiers senor / sensor / senor_pin → unified as SENSOR_PIN constant
 *   - Calling .attach() on pin macro tap_servo → split into Servo object (tapServo)
 *   - pulseIn timeout added; on timeout the door stays closed
 *     (original code read failure as distance 0, leaving door stuck open)
 *   - Two separate if(val==0)/if(val==1) blocks → equivalent if/else for clarity
 *   - Removed writing LOW to echo pin (unnecessary operation on an INPUT pin)
 * All other gate/diverter logic and angle/delay values kept from the draft.
 *
 * Hardware: 1× HC-SR04 ultrasonic (inlet detection), 1× inductive proximity sensor
 *           (digital output), 2× servos
 * Behavior (unchanged from draft): object detected within 10 cm of inlet → door servo
 * to 100° (open), otherwise 180° (closed). Proximity sensor LOW (metal detected) →
 * diverter servo to 0°, HIGH → 180°.
 */
#include <Servo.h>

const int TRIG_PIN = 7;
const int ECHO_PIN = 6;
const int SENSOR_PIN = 4;      // inductive proximity sensor digital output (NPN: LOW when metal detected)
const int DOOR_SERVO_PIN = 2;  // inlet door
const int TAP_SERVO_PIN = 5;   // metal / non-metal diverter

const long DOOR_OPEN_DISTANCE_CM = 10;
const unsigned long ECHO_TIMEOUT_US = 30000UL;  // ~5 m detection range (10 m round-trip path)

const int DOOR_OPEN_ANGLE = 100;
const int DOOR_CLOSED_ANGLE = 180;
const int TAP_METAL_ANGLE = 0;
const int TAP_NONMETAL_ANGLE = 180;

Servo doorServo;
Servo tapServo;

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(SENSOR_PIN, INPUT);
  doorServo.attach(DOOR_SERVO_PIN);
  tapServo.attach(TAP_SERVO_PIN);
}

void loop() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration = pulseIn(ECHO_PIN, HIGH, ECHO_TIMEOUT_US);
  long distance = (long)(((float)(340UL * duration) / 10000.0f) / 2.0f);  // us -> cm

  int doorAngle;
  if (duration > 0 && distance < DOOR_OPEN_DISTANCE_CM) {
    doorAngle = DOOR_OPEN_ANGLE;
    Serial.println("문이 열렸습니다.");  // Output string kept as-is to preserve original behavior ("The door is open")
    delay(1000);
  } else {
    doorAngle = DOOR_CLOSED_ANGLE;
    delay(500);
  }
  doorServo.write(doorAngle);

  int val = digitalRead(SENSOR_PIN);
  if (val == LOW) {
    tapServo.write(TAP_METAL_ANGLE);
    delay(1000);
  } else {
    tapServo.write(TAP_NONMETAL_ANGLE);
    delay(1000);
  }
}
