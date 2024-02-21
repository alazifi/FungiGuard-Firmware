#include <Arduino.h>
#include <ArduinoJson.h> //library for parsing data json
#include <WiFi.h>        //library for WiFi
#include <ESPmDNS.h>     //library for OTA
#include <WiFiUdp.h>     //library for OTA
#include <ArduinoOTA.h>  //main library for OTA
#include <HTTPClient.h>  //library for HTTP

// WiFi credential
#define SSID "Shed45"
#define PASS "test12345"

// TB server credential
#define TB_SERVER "iotn.abc-server.id"
#define TB_TOKEN "XjsnxxhgwkOcCKIX7yPl"
#define SHED_ID "bdg045"

// Relay pin definitions
#define MISTING_PIN 26
#define EXHAUST_PIN 25
#define BLOWER1_PIN 33
#define BLOWER2_PIN 32

// initiate millis
unsigned long prevMillisWifi = 0;
unsigned long prevMillisUpdateState = 0;

unsigned long delayWifi = 10000;       // connection check every 10 s
unsigned long delayUpdateState = 1000; // update state of relay every 1 s

void setup()
{
  // serial communication for debugging
  Serial.begin(250000);

  // start connect to WiFi
  connectToWiFi();

  // set relay pins mode as output
  pinMode(MISTING_PIN, OUTPUT);
  pinMode(EXHAUST_PIN, OUTPUT);
  pinMode(BLOWER1_PIN, OUTPUT);
  pinMode(BLOWER2_PIN, OUTPUT);

  // initiate work of relay pins as low
  digitalWrite(MISTING_PIN, LOW);
  digitalWrite(EXHAUST_PIN, LOW);
  digitalWrite(BLOWER1_PIN, LOW);
  digitalWrite(BLOWER2_PIN, LOW);

  // OTA credentials
  ArduinoOTA.setHostname("bdg-045-aktuator-unit-02");
  ArduinoOTA.setPassword("admin");

  // initiate OTA
  ArduinoOTA.begin();
}

// function to connect to wifi
void connectToWiFi()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Menghubungkan ke WiFi..."); // debug WiFi state
    WiFi.begin(SSID, PASS);                     // connecting to WiFi
  }
}

// function to get data state data of relays from TB (json package)
String getDataFromTB()
{
  // make object for http
  HTTPClient http;

  // API to get data from Thingsboard
  String url = "https://" + String(TB_SERVER) + "/api/v1/" + String(TB_TOKEN) + "/attributes?clientKeys=" + String(SHED_ID) + "_misting," + String(SHED_ID) + "_exhaust," + String(SHED_ID) + "_blower1," + String(SHED_ID) + "_blower2";

  // start write http
  http.begin(url);

  // add header to http request
  http.addHeader("Content-Type", "application/json");

  // mode http GET
  int httpResponseCode = http.GET();

  // variable for payload
  String payload = "";

  // check http response
  if (httpResponseCode == HTTP_CODE_OK)
  {
    // get string from http response
    payload = http.getString();
  }
  else
  {
    // Serial.print("HTTP Error code: ");
    // Serial.println(httpResponseCode);
    payload = "HTTP Error code: " + String(httpResponseCode);
  }

  // end http connection
  http.end();
  return payload;
}

// function to ensure input state to uppercase
String uppercaseInput(String input)
{
  String result = input;
  for (size_t i = 0; i < result.length(); ++i)
  {
    result[i] = toupper(result[i]);
  }
  return result;
}

// function to control lamp state
void controlLamp()
{
  // make object for parsed document
  DynamicJsonDocument controlDoc(300);

  // deserializeJson
  deserializeJson(controlDoc, getDataFromTB());

  // get state of device
  String mistingState = controlDoc["client"]["" + String(SHED_ID) + "_misting"];
  String exhaustState = controlDoc["client"]["" + String(SHED_ID) + "_exhaust"];
  String blower1State = controlDoc["client"]["" + String(SHED_ID) + "_blower1"];
  String blower2State = controlDoc["client"]["" + String(SHED_ID) + "_blower2"];

  // set state of devices
  if (mistingState != "null" && exhaustState != "null" && blower1State != "null" && blower2State != "null")
  {
    digitalWrite(MISTING_PIN, (uppercaseInput(mistingState) == "ON") ? HIGH : LOW);
    digitalWrite(EXHAUST_PIN, (uppercaseInput(exhaustState) == "ON") ? HIGH : LOW);
    digitalWrite(BLOWER1_PIN, (uppercaseInput(blower1State) == "ON") ? HIGH : LOW);
    digitalWrite(BLOWER2_PIN, (uppercaseInput(blower2State) == "ON") ? HIGH : LOW);
  }
  Serial.println("01." + mistingState + "\t02." + exhaustState + "\t03." + blower1State + "\t04." + blower2State);
}

void loop()
{
  unsigned long currentMillis = millis();

  // handle WiFi connection
  if (currentMillis - prevMillisWifi >= delayWifi)
  {
    connectToWiFi();
    prevMillisWifi = currentMillis;
  }

  // handle movement of motor
  if (currentMillis - prevMillisUpdateState >= delayUpdateState)
  {
    controlLamp();
    prevMillisUpdateState = currentMillis;
  }

  // handle OTA task
  ArduinoOTA.handle();
}
