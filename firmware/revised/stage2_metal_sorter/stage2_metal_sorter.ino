/*
 * [정리본] 2단계 금속 분리 (유도형 근접센서)
 * 원본 초안(firmware/original/stage2_metal_sorter)은 컴파일되지 않아 다음을 수정:
 *   - senor / sensor / senor_pin 혼용 → SENSOR_PIN 상수로 통일
 *   - 핀 매크로 tap_servo에 .attach()를 호출하던 오류 → Servo 객체(tapServo)로 분리
 *   - pulseIn 타임아웃 추가. 타임아웃 시 문 닫힘 유지
 *     (원본은 측정 실패가 거리 0으로 읽혀 문이 열린 상태로 고정됨)
 *   - val==0 / val==1 두 if문 → 등가인 if/else로 명확화
 *   - 원본의 echo 핀 LOW 쓰기 제거 (INPUT 핀에는 불필요한 동작)
 * 그 외 게이트/분기 로직과 각도·딜레이 값은 초안 그대로.
 *
 * 하드웨어: HC-SR04 초음파 x1(투입 감지), 유도형 근접센서 x1(디지털 출력), 서보 x2
 * 동작(초안 그대로): 투입구 10cm 이내 감지 시 문 서보 100도(열림), 아니면 180도(닫힘).
 * 근접센서 LOW(금속 감지) 시 분기 서보 0도, HIGH 시 180도.
 */
#include <Servo.h>

const int TRIG_PIN = 7;
const int ECHO_PIN = 6;
const int SENSOR_PIN = 4;      // 유도형 근접센서 디지털 출력 (NPN 기준: 감지 시 LOW)
const int DOOR_SERVO_PIN = 2;  // 투입구 문
const int TAP_SERVO_PIN = 5;   // 금속/비금속 분기

const long DOOR_OPEN_DISTANCE_CM = 10;
const unsigned long ECHO_TIMEOUT_US = 30000UL;  // 약 5m 탐지 거리(왕복 경로 10m)에 해당

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
    Serial.println("문이 열렸습니다.");
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
