#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <MHZ19.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <SoftwareSerial.h>

#include <IRremote.hpp>
#include <DataRemote.h>
#include <HTTPClient.h>

#define SSID "Shed45"
#define PASS "test12345"

#define TB_SERVER "iot.abc-server.id"
#define TB_TOKEN "XjsnxxhgwkOcCKIX7yPl"
#define SHED_ID "bdg045"

#define RX_CO2_PIN 16
#define TX_CO2_PIN 17
#define BAUDRATE 9600

#define DS18B20_PIN 25
#define DHT_PIN 26
#define DHT_TYPE DHT22

#define RS485_ENABLE_PIN 14

#define RX_SERIAL_PIN 33
#define TX_SERIAL_PIN 32

SoftwareSerial serialData(RX_SERIAL_PIN, TX_SERIAL_PIN);

OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);
DeviceAddress probe1 = { 0x28, 0x81, 0x83, 0x80, 0xE3, 0xE1, 0x3D, 0x4E };
DeviceAddress probe2 = { 0x28, 0x11, 0xC3, 0x80, 0xE3, 0xE1, 0x3D, 0x16 };
DeviceAddress probe3 = { 0x28, 0x61, 0xCA, 0x80, 0xE3, 0xE1, 0x3D, 0x7D };
DeviceAddress probe4 = { 0x28, 0x1A, 0x62, 0x80, 0xE3, 0xE1, 0x3D, 0x10 };
DeviceAddress probe5 = { 0x28, 0xE5, 0x29, 0x80, 0xE3, 0xE1, 0x3D, 0xDF };
DHT dht(DHT_PIN, DHT_TYPE);
MHZ19 myMHZ19;
WiFiClient espClient;
PubSubClient client(espClient);

DataRemote remote;
IRsend irsend;

unsigned long prevMillisWifi = 0;
unsigned long prevMillisSendSerial = 0;
unsigned long prevMillisSendTB = 0;
unsigned long prevMillisUpdateAC = 0;

unsigned long delayWifi = 300000;      // cek wifi setiap 5 menit
unsigned long delaySendSerial = 1000;  // send serial setiap 10 detik
unsigned long delaySendTB = 600000;    // kirim ke TB setiap 10 menit
unsigned long delayUpdateAC = 60000;   // update AC setiap 1 menit

String prevDataAC1 = "";
String prevDataAC2 = "";
String lastUpdate = "";

void setup() {
  Serial.begin(115200);
  Serial2.begin(BAUDRATE, SERIAL_8N1, RX_CO2_PIN, TX_CO2_PIN);
  myMHZ19.begin(Serial2);
  myMHZ19.autoCalibration();
  myMHZ19.setRange(10000); 
  sensors.begin();
  dht.begin();

  // serialData.begin(115200);
  pinMode(RS485_ENABLE_PIN, OUTPUT);
  digitalWrite(RS485_ENABLE_PIN, HIGH);  // Mode transmitter

  connectToWiFi();
  ArduinoOTA.setHostname("bdg-045-sensor-unit-01");
  ArduinoOTA.setPassword("admin");
  ArduinoOTA.begin();
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
  return sensors.getTempC(probe3);
}

float composeTemperature4() {
  sensors.requestTemperatures();
  return sensors.getTempC(probe4);
}

float composeTemperature5() {
  sensors.requestTemperatures();
  return sensors.getTempC(probe5);
}

float roomTemperature() {
  return dht.readTemperature();
}

float humidity() {
  return dht.readHumidity();
}

int carbonDioxide() {
  return myMHZ19.getCO2();
}

String payloadData() {
  DynamicJsonDocument doc(1024);
  // doc["bdg_001_u01_compose_temp1"] = String(composeTemperature1(), 2);
  // doc["bdg_001_u01_compose_temp2"] = String(composeTemperature2(), 2);
  // // doc["bdg_001_u01_compose_temp3"] = String(composeTemperature3(),2);
  // doc["bdg_001_u01_room_temp1"] = String(roomTemperature(), 2);
  // doc["bdg_001_u01_humidity1"] = String(humidity(), 2);
  // doc["bdg_001_u01_carbon_dioxide1"] = String(carbonDioxide());

  doc[""+String(SHED_ID)+"_comTemp1"] = String(composeTemperature1(), 2);
  doc[""+String(SHED_ID)+"_comTemp2"] = String(composeTemperature2(), 2);
  doc[""+String(SHED_ID)+"_comTemp3"] = String(composeTemperature3(), 2);
  doc[""+String(SHED_ID)+"_comTemp4"] = String(composeTemperature4(), 2);
  doc[""+String(SHED_ID)+"_comTemp5"] = String(composeTemperature5(), 2);
  doc[""+String(SHED_ID)+"_roomTemp1"] = String(roomTemperature(), 2);
  doc[""+String(SHED_ID)+"_humi1"] = String(humidity(), 2);
  doc[""+String(SHED_ID)+"_CO21"] = String(carbonDioxide());

  String payload;
  serializeJson(doc, payload);
  return payload;
}

String statusFromTB() {
  DynamicJsonDocument doc(375);
  String status = "";
  DeserializationError error = deserializeJson(doc, getDataFromTB());

  if (!error) {
    status = doc["client"][""+String(SHED_ID)+"_fase"].as<String>();
  }
  return status;
}

String enrichedData() {
  String sensorData = payloadData();
  sensorData.remove(0, 1);
  sensorData.remove(sensorData.length() - 1);
  // String payload = "{\"id\":\"" + String(SLAVE_ID) + "\"," + sensorData + ", \"fase\":\"" + String(statusFromTB()) + "\"}";
  String payload = "{" + sensorData + ", \"fase\":\"" + String(statusFromTB()) + "\"}";
  return payload;
}

void connectToWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Menghubungkan ke WiFi...");
    WiFi.begin(SSID, PASS);
  }
}

void sendDataToTB() {
  HTTPClient http;
  String url = "https://" + String(TB_SERVER) + "/api/v1/" + String(TB_TOKEN) + "/telemetry";
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(String(payloadData()));
  //debug data disini
  // if (httpResponseCode > 0) {
  //   Serial.print("HTTP Response code: ");
  //   Serial.println(httpResponseCode);
  // } else {
  //   Serial.print("HTTP Error code: ");
  //   Serial.println(httpResponseCode);
  // }
  http.end();
}

void sendDataToAttributeTB() {
  HTTPClient http;
  String url = "https://" + String(TB_SERVER) + "/api/v1/" + String(TB_TOKEN) + "/attributes";
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(String(payloadData()));
  //debug data disini
  // if (httpResponseCode > 0) {
  //   Serial.print("HTTP Response code: ");
  //   Serial.println(httpResponseCode);
  // } else {
  //   Serial.print("HTTP Error code: ");
  //   Serial.println(httpResponseCode);
  // }
  http.end();
}

String getDataFromTB() {
  HTTPClient http;
  String url = "https://" + String(TB_SERVER) + "/api/v1/" + String(TB_TOKEN) + "/attributes?clientKeys="+String(SHED_ID)+"_modeAC1,"+String(SHED_ID)+"_modeAC2,"+String(SHED_ID)+"_fase";
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
  String dataAC1 = "";
  String dataAC2 = "";
  String result;
  DeserializationError error = deserializeJson(doc, getDataFromTB());

  if (!error) {
    dataAC1 = doc["client"][""+String(SHED_ID)+"_modeAC1"].as<String>();
    dataAC2 = doc["client"][""+String(SHED_ID)+"_modeAC2"].as<String>();

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
  const int IR_PIN_2 = 19;

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
  ledcDetachPin(IrSender.sendPin);
  // Serial.println(irPin);
  // Serial.println(F(remote.getProntoData()));
}

void sendToSerial() {
  // serialData.println(enrichedData());    //DIPAKAI
  // serialData.flush();                    //DIPAKAI
  // serialData.println(payloadData());
  Serial.println(enrichedData());
  // Serial.flush();
  // Serial.println(getDataFromTB());
  // Serial.println(enrichedData());
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - prevMillisWifi >= delayWifi) {
    connectToWiFi();
    prevMillisWifi = currentMillis;
  }

  if (currentMillis - prevMillisSendSerial >= delaySendSerial) {
    sendToSerial();
    sendDataToAttributeTB();
    prevMillisSendSerial = currentMillis;
  }

  if (currentMillis - prevMillisSendTB >= delaySendTB) {
    sendDataToTB();
    prevMillisSendTB = currentMillis;
  }

  if (currentMillis - prevMillisUpdateAC >= delayUpdateAC) {
    controlAC();
    // Serial.println(modeControl());
    prevMillisUpdateAC = currentMillis;
  }

  ArduinoOTA.handle();
}
