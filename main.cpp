#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// WiFi 설정
const char* WIFI_SSID = "최준서의 iPhone";
const char* WIFI_PASS = "1223334444";

// MQTT 설정
const char* MQTT_BROKER = "broker.hivemq.com";
const int   MQTT_PORT   = 1883;
const char* MQTT_TOPIC  = "drum/hit";  // 데이터 보낼 주제

// 피에조 설정
const int PIEZO_PIN = 4;
const int THRESHOLD = 100;

WiFiClient espClient;
PubSubClient mqtt(espClient);

void connectWiFi() {
  Serial.print("WiFi 연결 중");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi 연결 완료");
  Serial.println(WiFi.localIP());
}

void connectMQTT() {
  while (!mqtt.connected()) {
    Serial.print("MQTT 연결 중...");
    if (mqtt.connect("ESP32DrumPad")) {
      Serial.println("완료");
    } else {
      Serial.print("실패, 재시도 중... rc=");
      Serial.println(mqtt.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  connectWiFi();
  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
}

void loop() {
  if (!mqtt.connected()) connectMQTT();
  mqtt.loop();

  int val = analogRead(PIEZO_PIN);
  if (val > THRESHOLD) {
    char msg[10];
    itoa(val, msg, 10);  // 숫자 → 문자열 변환
    mqtt.publish(MQTT_TOPIC, msg);
    Serial.print("전송: ");
    Serial.println(val);
    delay(50);
  }
}