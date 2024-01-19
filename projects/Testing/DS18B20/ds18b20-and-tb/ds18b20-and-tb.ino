#include <Arduino.h>
#include <OneWire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>

#define SSID "IEG-1"
#define PASS "evergreen999"

#define TB_SERVER "iot.camry.um.ac.id"
#define TB_PORT 1883
#define TB_TOKEN "413bb7MI6GHC68L2BEEe"

#define DS18B20_PIN 25

OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);
DeviceAddress probe1 = { 0x28, 0xA1, 0x6, 0x80, 0xE3, 0xE1, 0x3D, 0xE1 };
DeviceAddress probe2 = { 0x28, 0x61, 0xCA, 0x80, 0xE3, 0xE1, 0x3D, 0x7D };
LiquidCrystal_I2C lcd(0x27, 20, 4);
WiFiClient espClient;
PubSubClient client(espClient);

const int interval1 = 1000;
unsigned long previousMillis1 = 0;
const int interval2 = 1000;
unsigned long previousMillis2 = 0;
const int interval3 = 1000;
unsigned long previousMillis3 = 0;

int status = WL_IDLE_STATUS;

void setup() {
  Serial.begin(115200);

  sensors.begin();
  lcd.init();
  lcd.backlight();
  lcd.clear();
  connectWiFi();
  connectMQTT();
}

void sendData() {
  sensors.requestTemperatures();
  String topicTelemetry = "v1/devices/me/telemetry";
  String topicAttribute = "v1/devices/me/attributes";

  DynamicJsonDocument doc(512);
  doc["compose_temp1"] = "23";
  doc["compose_temp2"] = "24";

  String payload;
  serializeJson(doc, payload);
  client.publish(topicTelemetry.c_str(), payload.c_str());
  // client.publish(topicAttribute.c_str(), payload.c_str());
  // delay(1000);
}

void loop() {
  unsigned long now = millis();

  if (now - previousMillis3 >= interval3) {
    sendData();
    previousMillis3 = now;
  }
  connectMQTT();
  client.loop();
}

void connectWiFi() {
  // Loop until we're connected
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to WiFi");
    WiFi.begin(SSID, PASS);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Connected to WiFi");
  }
}

void connectMQTT() {
  client.setServer(TB_SERVER, TB_PORT);
  // Loop until we're reconnected
  while (!client.connected()) {
    status = WiFi.status();
    if (status != WL_CONNECTED) {
      connectWiFi();  // Reconnect to WiFi if not connected
    }
    Serial.print("Connecting to ThingsBoard node ...");
    if (client.connect(TB_SERVER, TB_TOKEN, NULL)) {
      Serial.println("[DONE]");
    } else {
      Serial.print("[FAILED]");
      Serial.println(" : retrying in 5 seconds]");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}