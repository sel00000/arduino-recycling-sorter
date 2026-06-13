/*
 * [정리본] 3단계 무게 분류 (HX711 로드셀)
 * 원본(firmware/original/stage3_weight_sorter)은 기록이 끊겨 loop()의 닫는
 * 중괄호가 유실된 상태였습니다. 유실 부분 복원, 임계값 상수화, 서보 변수명 변경
 * (myservo/myservo2 → rotateServo/dumpServo) 외에는 당시 로직(연쇄 delay로
 * 서보 떨림 완충 포함)을 그대로 유지합니다.
 *
 * 필요 라이브러리: HX711 (라이브러리 매니저에서 "HX711" — bogde 작성 — 설치)
 * 하드웨어: HX711 + 로드셀, 서보 x2 (회전용, 배출용)
 * 동작(원본 그대로): 측정값 > 1.0 → 회전 서보 90도 방향에서 배출(무거움=유리 칸),
 * 0.5 초과 ~ 1.0 이하 → 회전 서보 0도 방향에서 배출(가벼움=플라스틱 칸),
 * 0.5 이하 → 동작 없음 (당시 로직의 한계 — docs/code-review.md 참고).
 *
 * 캘리브레이션: SCALE_FACTOR(23600)는 당시 로드셀에 맞춰 둔 값으로, 기준 분동
 * 캘리브레이션을 거치지 않아 측정값의 단위가 kg이라고 보장할 수 없습니다(심사 Q7).
 * 절차: 빈 저울에서 tare() 후 기준 분동(예: 1.000kg)을 올리고 get_units() 출력이
 * 1.000이 되도록 SCALE_FACTOR를 조정.
 */
#include "HX711.h"
#include <Servo.h>

const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;
const int ROTATE_SERVO_PIN = 5;
const int DUMP_SERVO_PIN = 6;

const float SCALE_FACTOR = 23600.0f;  // 로드셀별 캘리브레이션 계수
const float HEAVY_THRESHOLD = 1.0f;   // 초과 시 무거움(유리) 칸
const float LIGHT_THRESHOLD = 0.5f;   // 초과 시 가벼움(플라스틱) 칸

HX711 scale;
Servo rotateServo;  // 분류 방향 회전
Servo dumpServo;    // 배출(투척)

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
  float weight = scale.get_units(5);  // 5회 평균 (10SPS 기준 약 0.5초 소요)
  Serial.println(weight);

  // 연쇄 delay(1000)는 당시 서보 떨림을 완화하기 위한 의도적 완충 (원본 유지)
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
