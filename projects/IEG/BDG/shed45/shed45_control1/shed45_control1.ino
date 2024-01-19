#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <HTTPClient.h>

#define SSID "Shed45"
#define PASS "test12345"

#define TB_SERVER "iot.abc-server.id"
#define TB_TOKEN "XjsnxxhgwkOcCKIX7yPl"
#define SHED_ID "bdg045"

#define MISTING_PIN 26
#define EXHAUST_PIN 25
#define BLOWER1_PIN 33
#define BLOWER2_PIN 32

unsigned long prevMillisWifi = 0;
unsigned long prevMillisUpdateState = 0;

unsigned long delayWifi = 10000;       // cek wifi setiap 10 detik
unsigned long delayUpdateState = 1000;    // update AC setiap 10 menit

void setup() {
  Serial.begin(250000);
  connectToWiFi();
  pinMode(MISTING_PIN, OUTPUT);
  pinMode(EXHAUST_PIN, OUTPUT);
  pinMode(BLOWER1_PIN, OUTPUT);
  pinMode(BLOWER2_PIN, OUTPUT);
  digitalWrite(MISTING_PIN, LOW);
  digitalWrite(EXHAUST_PIN, LOW);
  digitalWrite(BLOWER1_PIN, LOW);
  digitalWrite(BLOWER2_PIN, LOW);
  ArduinoOTA.setHostname("bdg-045-control-unit-01");
  ArduinoOTA.setPassword("admin");
  ArduinoOTA.begin();
}

void connectToWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Menghubungkan ke WiFi...");
    WiFi.begin(SSID, PASS);
  }
}

String getDataFromTB() {
  HTTPClient http;
  String url = "https://" + String(TB_SERVER) + "/api/v1/" + String(TB_TOKEN) + "/attributes?clientKeys="+String(SHED_ID)+"_misting,"+String(SHED_ID)+"_exhaust,"+String(SHED_ID)+"_blower1,"+String(SHED_ID)+"_blower2";
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.GET();
  String payload = "";
  if (httpResponseCode == HTTP_CODE_OK) {
    payload = http.getString();
  } else {
    // Serial.print("HTTP Error code: ");
    // Serial.println(httpResponseCode);
  }
  http.end();
  return payload;
}

String uppercaseInput(String input) {
  String result = input;
  for (size_t i = 0; i < result.length(); ++i) {
    result[i] = toupper(result[i]);
  }
  return result;
}

void controlLamp() {
  String controlData = getDataFromTB();
  DynamicJsonDocument controlDoc(300);
  deserializeJson(controlDoc, controlData);

  String mistingState = controlDoc["client"][""+String(SHED_ID)+"_misting"];
  String exhaustState = controlDoc["client"][""+String(SHED_ID)+"_exhaust"];
  String blower1State = controlDoc["client"][""+String(SHED_ID)+"_blower1"];
  String blower2State = controlDoc["client"][""+String(SHED_ID)+"_blower2"];

  if (mistingState != "null" && exhaustState != "null" && blower1State != "null" && blower2State != "null") {
    digitalWrite(MISTING_PIN, (uppercaseInput(mistingState) == "ON") ? HIGH : LOW);
    digitalWrite(EXHAUST_PIN, (uppercaseInput(exhaustState) == "ON") ? HIGH : LOW);
    digitalWrite(BLOWER1_PIN, (uppercaseInput(blower1State) == "ON") ? HIGH : LOW);
    digitalWrite(BLOWER2_PIN, (uppercaseInput(blower2State) == "ON") ? HIGH : LOW);
  }
  Serial.println("01." + mistingState + "\t02." + exhaustState + "\t03." + blower1State + "\t04." + blower2State);

}


void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - prevMillisWifi >= delayWifi) {
    connectToWiFi();
    prevMillisWifi = currentMillis;
  }

  if (currentMillis - prevMillisUpdateState >= delayUpdateState) {
    controlLamp();
    prevMillisUpdateState = currentMillis;
  }

  ArduinoOTA.handle();
}
