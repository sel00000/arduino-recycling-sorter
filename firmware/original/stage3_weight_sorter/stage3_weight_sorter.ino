/*
 * [원본 기록] 3단계 무게 분류 — 나노데이 경진대회 제출 당시 코드
 * 기록 원문이 loop()가 닫히기 전에 끊겨 닫는 중괄호가 유실된 상태 그대로 보존합니다.
 * (이 파일은 컴파일되지 않습니다.) 복원·정리본: firmware/revised/stage3_weight_sorter/
 */
#include "HX711.h"
#include <Servo.h>

Servo myservo;
Servo myservo2;

// HX711 회로구성
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;

HX711 scale;
float value = 23600.f;  // 무게에 대한 값 조절 (각자 자기의 로드셀의 맞는 값)

void setup() {
    Serial.begin (38400);

    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

    scale.set_scale(value);
    scale.tare();
    myservo.attach(5);
    myservo2.attach(6);

    myservo.write(0);
    myservo2.write(0);

}
float weight = 0;   //변수

void loop() {

    weight = scale.get_units(5);     //변수 받고, 얘가 있어야 조건 걸 수 있음
    //int deg = map (weight,0,6,0,180);  //0kg,6kg,0도,180도
    Serial.println(weight);

    if (weight > 1.0 ) {
        //1kg 이상의 조건
    delay (1000);    //1초
    myservo.write(90);   //회전
    delay (1000);
    myservo2.write(90);  //갖다 버리기
    delay (1000);
    myservo2.write(0);
    delay (1000);
    myservo.write(0);
    delay (1000);
    }else if (weight > 0.5) {
        //0.5~1.0kg
    delay (1000);
    myservo.write(0);   //회전
    delay (1000);
    myservo2.write(90);  //갖다 버리기
    delay (1000);
    myservo2.write(0);
    delay (1000);
    }

    delay(100);
