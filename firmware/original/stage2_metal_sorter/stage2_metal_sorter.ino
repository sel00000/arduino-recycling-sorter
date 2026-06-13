/*
 * [Original record] Stage 2 metal separation — my draft. (The version that actually
 * ran at the competition was completed by a teammate; that record is lost.)
 * Preserved as-is: it does NOT compile, due to mismatched identifiers
 * (senor/sensor/senor_pin/tap_servo_pin) and calling .attach() on the tap_servo
 * macro. The Korean comments below are part of the historical record.
 * Cleaned-up version: firmware/revised/stage2_metal_sorter/
 */
#include<Servo.h> // 서보모터 동작을 위한 서보 라이브러리 불러오기

Servo servo; // 객체문 설정

#define trig 7 // 7번핀 초음파 센서 정의
#define echo 6 // 6번핀 초음파 센서 분리수거 함 들어갈때 우노 핀 정의
int angle = 0; // 서보 모터 각도 선언

#define senor 4 // 4번핀 유도형 근접센서 정의
#define tap_servo 5 // 5번핀 서보모터 정의   유도형 근접센서 알루미늄캔 들어갈때 우노 핀 선언문 지정
int val; //

void setup() {
  servo.attach(2); // 객체문
  Serial.begin(115200);  // 시리얼포트 115200 값 설정
  pinMode(trig, OUTPUT);  // 7번핀 초음파센서 핀모드 인풋과 아웃풋 지정  (송신부)
  pinMode(echo, INPUT);  //  6번핀 인풋 지정  (수신부)

  pinMode(senor_pin, INPUT);   // 유도형 근접센서 핀모드 인풋 지정
  tap_servo.attach(tap_servo_pin);
}

void loop()     // 반복문
{
 long duration, distance;

   digitalWrite(trig, LOW);
   digitalWrite(echo, LOW);
   delayMicroseconds(2); // 2us 딜레이
   digitalWrite(trig, HIGH); // Trig 7번핀 High
   delayMicroseconds(10); //10us 딜레이
   digitalWrite(trig, LOW); // Trig 7번핀 Low

   duration = pulseIn(echo, HIGH);  //duration 에 pulseln 값 저장
   distance = ((float)(340 * duration) / 10000) /2;  // 거리 측정 & cm 으로 나타내기 위해 10000으로 나눠줌

  if ( distance < 10 ) {   // 만약  10cm 보다 가까운 거리 (이하) 일때 서보모터를 100만큼 시리얼포트에 문자를 보여주고 딜레이 1000뒤에 움직여라
     angle = 100;
     Serial.println("문이 열렸습니다.");
     delay(1000);
  }
  else {      // 아니면  소보모터를 180 만큼 딜레이 500 으로 움직여라
    angle = 180;
    delay(500);
  }
  servo.write(angle);

  val = digitalRead(sensor); //

  if (val==0)
  {tap_servo.write(0);
   delay(1000);
  }
  if (val==1)
  {tap_servo.write(180);
   delay(1000);
  }

}
