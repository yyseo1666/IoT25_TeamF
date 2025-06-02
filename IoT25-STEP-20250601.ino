#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED 설정
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// 핀 설정
const int trigPin = 12;
const int echoPin = 13;
const int greenLed = 18;  // RGB Green
const int redLed = 5;     // Red LED (보행자 금지용)

void setup() {
  Serial.begin(115200);

  // OLED 초기화
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED 초기화 실패"));
    while (true);  // 멈춤
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);

  digitalWrite(greenLed, LOW);
  digitalWrite(redLed, LOW);
}

long measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2;
  return distance;
}

void loop() {
  long distance = measureDistance();

  // 시리얼 출력
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // OLED 출력
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("거리: ");
  display.print(distance);
  display.println(" cm");

  display.setCursor(0, 20);
  if (distance > 0 && distance <= 100) {
    digitalWrite(greenLed, LOW);    // 켜짐 (공통 애노드일 경우 LOW)
    digitalWrite(redLed, LOW);     // 꺼짐
    display.println("보행자 신호: 켜짐");
  } else {
    digitalWrite(greenLed, HIGH);   // 꺼짐
    digitalWrite(redLed, HIGH);      // 켜짐
    display.println("보행자 신호: 꺼짐");
  }

  display.display();  // OLED에 적용
  delay(500);
}
