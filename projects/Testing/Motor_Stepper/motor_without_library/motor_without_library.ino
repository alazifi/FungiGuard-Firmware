#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define SSID "Shed45"
#define PASS "test12345"

const int DIR = 32;
const int STEP = 33;
const int steps_per_rev = 0;
const int step_delay = 1000; // in microseconds
unsigned long previousMillis = 0;
const long interval = 1000; // interval between steps in milliseconds

void setup()
{
  Serial.begin(115200);
  pinMode(STEP, OUTPUT);
  pinMode(DIR, OUTPUT);
  connectToWiFi();
  ArduinoOTA.setHostname("bdg-045-aktuator-unit-02");
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

void stepMotor(int dir, int steps)
{
  digitalWrite(DIR, dir);

  for (int i = 0; i < steps; i++)
  {
    digitalWrite(STEP, HIGH);
    delayMicroseconds(step_delay);
    digitalWrite(STEP, LOW);
    delayMicroseconds(step_delay);
  }
}

void loop()
{
  unsigned long currentMillis = millis();

  Serial.println("Spinning Clockwise...");

  if (currentMillis - previousMillis >= interval)
  {
    stepMotor(LOW, steps_per_rev);
    previousMillis = currentMillis;
  }
  ArduinoOTA.handle();

}
