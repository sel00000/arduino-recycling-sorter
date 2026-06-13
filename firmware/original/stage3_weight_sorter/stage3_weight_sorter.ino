/*
 * [Original record] Stage 3 weight sorting — code as submitted to the Nano Day
 * competition. The source record was cut off before loop() was closed, losing the
 * closing brace; preserved in that truncated state (this file does not compile).
 * The Korean comments below are part of the historical record.
 * Restored/cleaned-up version: firmware/revised/stage3_weight_sorter/
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
