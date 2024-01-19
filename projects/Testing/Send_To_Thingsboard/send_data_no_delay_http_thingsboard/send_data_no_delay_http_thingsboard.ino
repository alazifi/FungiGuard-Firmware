#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <MHZ19.h>
#include <LiquidCrystal_I2C.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <IRremote.hpp>
#include <DataRemote.h>
#include <HTTPClient.h>

#define SSID "IEG-1"
#define PASS "evergreen999"

#define TB_SERVER "iot.abc-server.id"
#define TB_TOKEN "XjsnxxhgwkOcCKIX7yPl"

#define RX_PIN 16
#define TX_PIN 17
#define BAUDRATE 9600

#define DS18B20_PIN 25
#define DHT_PIN 26
#define DHT_TYPE DHT22

OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);
DeviceAddress probe1 = { 0x28, 0xA1, 0x6, 0x80, 0xE3, 0xE1, 0x3D, 0xE1 };
DeviceAddress probe2 = { 0x28, 0x61, 0xCA, 0x80, 0xE3, 0xE1, 0x3D, 0x7D };
DHT dht(DHT_PIN, DHT_TYPE);
LiquidCrystal_I2C lcd(0x27, 20, 4);
MHZ19 myMHZ19;
WiFiClient espClient;
PubSubClient client(espClient);

DataRemote remote;
IRsend irsend;

unsigned long previousMillisWifi = 0;
unsigned long previousMillisPrintLCD = 0;
unsigned long previousMillisSendTB = 0;

unsigned long delayWifi = 9990;      // cek wifi setiap 9,99 detik
unsigned long delayPrintLCD = 1000;  // print LCD setiap 1 detik
unsigned long delaySendTB = 1000;    // kirim ke TB setiap 10 menit

String prevDataAC1 = "";
String prevDataAC2 = "";
String lastUpdate = "";

float roomTemperature() {
  return dht.readTemperature();
}

float humidity() {
  return dht.readHumidity();
}

float composeTemperature1() {
  sensors.requestTemperatures();
  return sensors.getTempC(probe1);
}

float composeTemperature2() {
  sensors.requestTemperatures();
  return sensors.getTempC(probe2);
}

float composeTemperature3() {
  sensors.requestTemperatures();
  return sensors.getTempC(probe2);
}

int carbonDioxide() {
  return myMHZ19.getCO2();
}

String payloadData() {
  DynamicJsonDocument doc(512);
  doc["bdg_001_u01_compose_temp1"] = String(composeTemperature1(), 2);
  doc["bdg_001_u01_compose_temp2"] = String(composeTemperature2(), 2);
  // doc["bdg_001_u01_compose_temp3"] = String(composeTemperature3(),2);
  doc["bdg_001_u01_room_temp1"] = String(roomTemperature(), 2);
  doc["bdg_001_u01_humidity1"] = String(humidity(), 2);
  doc["bdg_001_u01_carbon_dioxide1"] = carbonDioxide();

  String payload;
  serializeJson(doc, payload);
  return payload;
}

void connectToWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Menghubungkan ke WiFi...");
    WiFi.begin(SSID, PASS);
  }
}

void sendDataToThingsboard() {
  HTTPClient http;
  String url = "https://" + String(TB_SERVER) + "/api/v1/" + String(TB_TOKEN) + "/telemetry";
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(String(payloadData()));
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("HTTP Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

String getDataFromThingsboard() {
  HTTPClient http;
  String url = "https://" + String(TB_SERVER) + "/api/v1/" + String(TB_TOKEN) + "/attributes";
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.GET();
  String payload = "";
  if (httpResponseCode == HTTP_CODE_OK) {
    String payload = http.getString();
    return payload;
  } else {
    Serial.print("HTTP Error code: ");
    Serial.println(httpResponseCode);
    return "HTTP Error code: " + String(httpResponseCode);
  }
  http.end();
}

String commandFromTB() {
  DynamicJsonDocument doc(1024);
  String dataAC1 = "";
  String dataAC2 = "";
  String result;
  DeserializationError error = deserializeJson(doc, getDataFromThingsboard());

  if (!error) {
    dataAC1 = doc["shared"]["modeAC1"].as<String>();
    dataAC2 = doc["shared"]["modeAC2"].as<String>();

    if (dataAC1 != prevDataAC1) {
      prevDataAC1 = dataAC1;
      lastUpdate = "modeAC1";
    } else if (dataAC2 != prevDataAC2) {
      prevDataAC2 = dataAC2;
      lastUpdate = "modeAC2";
    }

    if (lastUpdate == "modeAC1") {
      result = prevDataAC1;
    } else if (lastUpdate == "modeAC2") {
      result = prevDataAC2;
    }
  }
  return result;
}

String deviceToControl() {
  String device = commandFromTB();
  return device.substring(0, 4);
}

String modeControl() {
  String mode = commandFromTB();
  return mode.substring(5);
}

void controlAC() {
  int irPin;
  const int IR_PIN_1 = 18;
  const int IR_PIN_2 = 27;

  if (deviceToControl() == "ac01") {
    irPin = IR_PIN_1;
  } else if (deviceToControl() == "ac02") {
    irPin = IR_PIN_2;
  } else {
    irPin = 0;
  }

  IrSender.setSendPin(irPin);
  remote.dataInput(modeControl());
  irsend.sendPronto(F(remote.getProntoData()), 3U);
  Serial.println(irPin);
  Serial.println(F(remote.getProntoData()));
  ledcDetachPin(IrSender.sendPin);
}

String statusFromTB() {
  DynamicJsonDocument doc(1024);
  String status = "";
  String result = "";
  DeserializationError error = deserializeJson(doc, getDataFromThingsboard());

  if (!error) {
    status = doc["shared"]["mode"].as<String>();
  }

  int pad = (20 - status.length());
  int lpad = pad / 2;
  int rpad = pad - lpad;

  for (int i = 0; i < lpad; i++) {
    result += " ";
  }

  result += status;

  for (int i = 0; i < rpad; i++) {
    result += " ";
  }
  return result;
}

void printLCD() {

  lcd.setCursor(0, 0);
  lcd.print(statusFromTB());

  lcd.setCursor(0, 1);
  lcd.print("SK1:" + String(composeTemperature1(), 1) + "C ");

  lcd.setCursor(0, 2);
  lcd.print("SK2:" + String(composeTemperature2(), 1) + "C ");

  // lcd.setCursor(0, 3);
  // lcd.print("SK3:" + String(composeTemperature3(), 1) + "C ");

  lcd.setCursor(10, 1);
  lcd.print("Temp:" + String(roomTemperature(), 1) + "C");

  lcd.setCursor(10, 2);
  lcd.print("Humi:" + String(humidity(), 1) + "%");

  lcd.setCursor(10, 3);
  lcd.print("CO2:" + String(carbonDioxide()) + "pm ");
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(BAUDRATE, SERIAL_8N1, RX_PIN, TX_PIN);
  myMHZ19.begin(Serial2);
  myMHZ19.autoCalibration();
  sensors.begin();
  dht.begin();
  lcd.init();
  lcd.backlight();
  lcd.clear();
  connectToWiFi();
  ArduinoOTA.setHostname("bdg-001-sensor-unit-01");
  ArduinoOTA.setPassword("admin");
  ArduinoOTA.begin();
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillisWifi >= delayWifi) {
    connectToWiFi();
    previousMillisWifi = currentMillis;
  }

  if (currentMillis - previousMillisPrintLCD >= delayPrintLCD) {
    printLCD();
    previousMillisPrintLCD = currentMillis;
  }

  if (currentMillis - previousMillisSendTB >= delaySendTB) {
    sendDataToThingsboard();
    previousMillisSendTB = currentMillis;
  }


  ArduinoOTA.handle();
}
