#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <HTTPClient.h>
#include <AccelStepper.h>

#define SSID "Shed45"
#define PASS "test12345"

#define TB_SERVER "iot.abc-server.id"
#define TB_TOKEN "XjsnxxhgwkOcCKIX7yPl"
#define SHED_ID "bdg001"

#define DIR 32
#define STEP 33

WiFiClient espClient;
PubSubClient client(espClient);
AccelStepper stepper(1, STEP, DIR);

unsigned long prevMillisWifi = 0;
unsigned long prevMillisSendSerial = 0;
unsigned long prevMillisAction = 0;
unsigned long prevMillisUpdateAC = 0;

unsigned long delayWifi = 10000;       // cek wifi setiap 10 detik
unsigned long delaySendSerial = 1000;  // send serial setiap 1 detik
unsigned long delayAction = 2000;       // kirim ke TB setiap 10 menit

int currentPosition = 0;
int prevPosition = 0;

void setup() {
  Serial.begin(250000);
  pinMode(STEP, OUTPUT);
  pinMode(DIR, OUTPUT);
  digitalWrite(STEP, LOW);
  digitalWrite(DIR, LOW);
  stepper.setMaxSpeed(250.0);
  stepper.setAcceleration(500.0);
  connectToWiFi();
  ArduinoOTA.setHostname("bdg-001-aktuator-unit-01");
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
  String url = "https://" + String(TB_SERVER) + "/api/v1/" + String(TB_TOKEN) + "/attributes?clientKeys=" + String(SHED_ID) + "_vent1";
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.GET();
  String payload = "";
  if (httpResponseCode == HTTP_CODE_OK) {
    String payload = http.getString();
    return payload;
  } else {
    // Serial.print("HTTP Error code: ");
    // Serial.println(httpResponseCode);
    return "HTTP Error code: " + String(httpResponseCode);
  }
  http.end();
}

String commandFromTB() {
  DynamicJsonDocument doc(375);
  String result;
  DeserializationError error = deserializeJson(doc, getDataFromTB());

  if (!error) {
    result = doc["client"]["" + String(SHED_ID) + "_vent1"].as<String>();
  }
  return result;
}

int percentCommand() {
  return commandFromTB().substring(0, commandFromTB().length() - 1).toInt() * 300;
}

void stepMotor(int position) {
  stepper.setSpeed(-50);
  stepper.moveTo(position);
  stepper.runToPosition();
}

void movement(){
  stepMotor(percentCommand());
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - prevMillisWifi >= delayWifi) {
    connectToWiFi();
    prevMillisWifi = currentMillis;
  }

  if (currentMillis - prevMillisAction >= delayAction) {
    movement();
    prevMillisAction = currentMillis;
  }

  ArduinoOTA.handle();
}
