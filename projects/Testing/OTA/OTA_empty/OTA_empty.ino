#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define SSID "Shed45"
#define PASS "test12345"

void setup() {
  // put your setup code here, to run once:
  connectToWiFi();
  ArduinoOTA.setHostname("bdg-045-sensor-unit-05");
  ArduinoOTA.setPassword("admin");
  ArduinoOTA.begin();
}

void connectToWiFi()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Menghubungkan ke WiFi...");
    WiFi.begin(SSID, PASS);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  ArduinoOTA.handle();
}
