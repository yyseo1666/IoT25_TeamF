#include <Wire.h>
#include <Adafruit_SSD1306.h>

// OLED 설정
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// 핀 정의
const int trigPin = 5;
const int echoPin = 18;
const int buzzerPin = 19;
const int pedRedLed = 21;
const int pedGreenLed = 22;
const int carRedLed = 23;
const int carGreenLed = 4;

// 감지 및 신호 시간 설정
const long detectDistanceCm = 100;  // 감지 거리 기준 (cm)
const int warningDuration = 5000;   // 부저 경고 시간 (ms)
const int walkDuration = 15000;     // 보행자 신호 지속 시간 (ms)

void setup() {
  Serial.begin(115200);

  // 핀 모드 설정
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(pedRedLed, OUTPUT);
  pinMode(pedGreenLed, OUTPUT);
  pinMode(carRedLed, OUTPUT);
  pinMode(carGreenLed, OUTPUT);

  // 초기 신호등 상태 (차량 녹색, 보행자 적색)
  digitalWrite(carGreenLed, HIGH);
  digitalWrite(carRedLed, LOW);
  digitalWrite(pedRedLed, HIGH);
  digitalWrite(pedGreenLed, LOW);
  digitalWrite(buzzerPin, LOW);

  // OLED 초기화
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED init failed"));
    while(true);
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Ready");
  display.display();
}

long measureDistance() {
  // 초음파 거리 측정 함수
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000); // 최대 30ms 대기
  long distanceCm = duration * 0.034 / 2;
  return distanceCm;
}

void loop() {
  long distance = measureDistance();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if(distance > 0 && distance <= detectDistanceCm) {
    // 보행자 감지됨
    warningPhase();
    walkPhase();
  } else {
    // 보행자 미감지 상태, 기본 신호 유지
    defaultPhase();
  }
  delay(200);
}

void warningPhase() {
  unsigned long startTime = millis();
  while(millis() - startTime < warningDuration) {
    // 부저 켜고 끄기 (경고음)
    digitalWrite(buzzerPin, HIGH);
    delay(200);
    digitalWrite(buzzerPin, LOW);
    delay(200);

    // OLED에 "WAIT" 표시 및 남은 시간 표시
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("WAIT");
    int remain = (warningDuration - (millis() - startTime)) / 1000;
    display.setTextSize(3);
    display.setCursor(0, 25);
    display.print(remain);
    display.print("s");
    display.display();
  }
}

void walkPhase() {
  unsigned long startTime = millis();

  // 신호 전환: 차량 빨강, 보행자 초록, 부저 끔
  digitalWrite(carGreenLed, LOW);
  digitalWrite(carRedLed, HIGH);
  digitalWrite(pedRedLed, LOW);
  digitalWrite(pedGreenLed, HIGH);
  digitalWrite(buzzerPin, LOW);

  while(millis() - startTime < walkDuration) {
    // OLED에 "WALK" 표시 및 남은 시간 표시
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println("WALK");
    int remain = (walkDuration - (millis() - startTime)) / 1000;
    display.setTextSize(3);
    display.setCursor(0, 25);
    display.print(remain);
    display.print("s");
    display.display();
    delay(200);
  }
}

void defaultPhase() {
  // 기본 신호: 차량 녹색, 보행자 적색
  digitalWrite(carGreenLed, HIGH);
  digitalWrite(carRedLed, LOW);
  digitalWrite(pedRedLed, HIGH);
  digitalWrite(pedGreenLed, LOW);
  digitalWrite(buzzerPin, LOW);

  // OLED에 기본 대기 상태 표시
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("Ready");
  display.display();
}
