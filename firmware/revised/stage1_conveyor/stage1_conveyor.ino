/*
 * [정리본] 1단계 컨베이어 벨트
 * 원본(firmware/original/stage1_conveyor)의 동작 로직을 유지하면서 다음만 적용:
 *   - pulseIn 타임아웃 추가 (에코 유실 시 최대 1초 멈추던 문제 방지)
 *   - 측정 실패 시 벨트 상태를 바꾸지 않음 (원본은 실패가 거리 0으로 읽힘)
 *   - 매직 넘버 상수화, 중복 측정 코드 함수화, 미사용 변수(pos) 제거
 *   - 트리거 전 2µs LOW 정착 추가(HC-SR04 권장 절차), Serial 출력 포맷 가독성 개선
 *
 * 하드웨어: HC-SR04 초음파 센서 x2, 연속회전 서보 x1 (벨트 구동)
 * 동작(원본 그대로): 두 센서가 모두 기준 거리 이내로 물체를 감지하면 서보 0(벨트
 * 구동), 하나라도 기준 밖이면 서보 90(연속회전 서보 기준 정지).
 */
#include <Servo.h>

const int TRIG1_PIN = 2;
const int ECHO1_PIN = 3;
const int TRIG2_PIN = 4;
const int ECHO2_PIN = 5;
const int SERVO_PIN = 9;

const long DIST1_THRESHOLD_CM = 10;
const long DIST2_THRESHOLD_CM = 20;
const unsigned long ECHO_TIMEOUT_US = 30000UL;  // 약 5m 탐지 거리(왕복 경로 10m)에 해당

Servo beltServo;

long readDistanceCm(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  unsigned long duration = pulseIn(echoPin, HIGH, ECHO_TIMEOUT_US);
  if (duration == 0) return -1;  // 타임아웃: 측정 실패
  return (long)(duration / 58.2);  // 원본의 환산 계수 유지 (정확값 ≈ 58.82 µs/cm)
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
    return;  // 측정 실패 사이클: 벨트 상태 유지
  }

  if (distance1 > DIST1_THRESHOLD_CM || distance2 > DIST2_THRESHOLD_CM) {
    delay(50);
    beltServo.write(90);
    delay(50);
  } else {
    beltServo.write(0);
  }
}
