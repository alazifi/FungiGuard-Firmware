#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <HTTPClient.h>
#include <AccelStepper.h>
#include <EEPROM.h>

#define SSID "Shed45"
#define PASS "test12345"

#define TB_SERVER "iotn.abc-server.id"
#define TB_TOKEN "XjsnxxhgwkOcCKIX7yPl"
#define SHED_ID "bdg045"

#define DIR 32
#define STEP 33

WiFiClient espClient;
PubSubClient client(espClient);
AccelStepper stepper(1, STEP, DIR);

unsigned long prevMillisWifi = 0;
unsigned long prevMillisSendSerial = 0;
unsigned long prevMillisAction = 0;
unsigned long prevMillisUpdateAC = 0;

unsigned long delayWifi = 10000;      // cek wifi setiap 10 detik
unsigned long delaySendSerial = 1000; // send serial setiap 1 detik
unsigned long delayAction = 2000;     // kirim ke TB setiap 10 menit



void setup()
{
  Serial.begin(250000);
  EEPROM.begin(512);
  pinMode(STEP, OUTPUT);
  pinMode(DIR, OUTPUT);
  digitalWrite(STEP, LOW);
  digitalWrite(DIR, LOW);
  connectToWiFi();
  ArduinoOTA.setHostname("bdg-045-aktuator-unit-02");
  ArduinoOTA.setPassword("admin");
  ArduinoOTA.begin();
  stepper.setMaxSpeed(1500.0);
  stepper.setAcceleration(3000.0);
  // Serial.println(percentCommand());
  stepper.setCurrentPosition(readValue());
}

void connectToWiFi()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Menghubungkan ke WiFi...");
    WiFi.begin(SSID, PASS);
  }
}

String getDataFromTB()
{
  HTTPClient http;
  String url = "https://" + String(TB_SERVER) + "/api/v1/" + String(TB_TOKEN) + "/attributes?clientKeys=" + String(SHED_ID) + "_vent2";
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.GET();
  String payload = "";
  if (httpResponseCode == HTTP_CODE_OK)
  {
    String payload = http.getString();
    return payload;
  }
  else
  {
    // Serial.print("HTTP Error code: ");
    // Serial.println(httpResponseCode);
    return "HTTP Error code: " + String(httpResponseCode);
  }
  http.end();
}

String commandFromTB()
{
  DynamicJsonDocument doc(375);
  String result;
  DeserializationError error = deserializeJson(doc, getDataFromTB());

  if (!error)
  {
    result = doc["client"]["" + String(SHED_ID) + "_vent2"].as<String>();
  }
  return result;
}

void saveToMemory(int position){
  EEPROM.put(10, position);
  EEPROM.commit();
}

int readValue(){
  int value;
  EEPROM.get(10, value);
  return value;
}

int percentCommand()
{
  int position = commandFromTB().substring(0, commandFromTB().length() - 1).toInt() * 300;
  if(position != 0){
    saveToMemory(position);
    return position;
  }
}

void stepMotor(int position)
{
  stepper.setSpeed(200);
  stepper.moveTo(position);
  stepper.runToPosition();
}

void movement()
{
  stepMotor(percentCommand());
}

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis - prevMillisWifi >= delayWifi)
  {
    connectToWiFi();
    prevMillisWifi = currentMillis;
  }

  if (currentMillis - prevMillisAction >= delayAction)
  {
    percentCommand();
    // movement();
  stepper.setSpeed(200);
  stepper.moveTo(readValue());
  stepper.runToPosition();
  // Serial.println(readValue());
  // percentCommand();
  // Serial.println("Persen " + String(percentCommand()));
  // stepper.setCurrentPosition(percentCommand());
  // movement();
  
    prevMillisAction = currentMillis;
  }

  ArduinoOTA.handle();
}
