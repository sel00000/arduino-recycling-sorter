/*
 * [원본 기록] 1단계 컨베이어 벨트 — 나노데이 경진대회 제출 당시 코드
 * 수정 없이 그대로 보존한 기록입니다. (컴파일 가능) 정리본: firmware/revised/stage1_conveyor/
 */
#include <Servo.h>
Servo servo1;

int echo1=3,trig1=2;
int echo2=5,trig2=4;

int pos=0;

void setup(){
  pinMode(trig1,OUTPUT);
  pinMode(echo1,INPUT);

  pinMode(trig2,OUTPUT);
  pinMode(echo2,INPUT);

  servo1.attach(9);
  Serial.begin(115200);
}

void loop(){
  delay(100);
  digitalWrite(trig1,HIGH);
  delayMicroseconds(10);
  digitalWrite(trig1,LOW);

  long distance1=pulseIn(echo1,HIGH)/58.2;

  Serial.print("distance1");
  Serial.print(distance1);
  Serial.println("cm");

  delay(100);
  digitalWrite(trig2,HIGH);
  delayMicroseconds(10);
  digitalWrite(trig2,LOW);

  long distance2=pulseIn(echo2,HIGH)/58.2;

  Serial.print("distance2");
  Serial.print(distance2);
  Serial.println("cm");

  if((distance1>10)||(distance2>20)){
    delay(50);
    servo1.write(90);
    delay(50);
      }else {
        servo1.write(0);
  }
}
