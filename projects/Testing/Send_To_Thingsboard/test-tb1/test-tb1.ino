#include <WiFi.h>
#include <ThingsBoard.h>
#include <Arduino_MQTT_Client.h>

#define SSID "villa"
#define PASS "villavilla"

#define TOKEN_TB "rektoratlantai1"
#define SERVER_TB "iot.camry.um.ac.id"

WiFiClient esp;
Arduino_MQTT_Client mqttClient(esp);
ThingsBoard tb(mqttClient);

void electricityData() {
  tb.sendTelemetryData("voltage1", 32);
  tb.sendTelemetryData("current1", 21);
  tb.sendTelemetryData("power1", 22);
  tb.sendTelemetryData("energy1", 23);
  tb.sendTelemetryData("frequency1", 24);
  tb.sendTelemetryData("pf1", 25);
  tb.sendAttributeData("voltage1", 20);
  tb.sendAttributeData("current1", 21);
  tb.sendAttributeData("power1", 22);
  tb.sendAttributeData("energy1", 23);
  tb.sendAttributeData("frequency1", 24);
  tb.sendAttributeData("pf1", 25);
  delay(1000);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(SSID, PASS);
}

void loop() {
  tb.connect(SERVER_TB, TOKEN_TB);
  electricityData();
  tb.loop();
}
