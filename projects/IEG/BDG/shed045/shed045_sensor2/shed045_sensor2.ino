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

#define TB_SERVER "iotn.abc-server.id"
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
DeviceAddress probe1 = {0x28, 0x63, 0x43, 0x80, 0xE3, 0xE1, 0x3D, 0x2B};
DeviceAddress probe2 = {0x28, 0x92, 0xE8, 0x80, 0xE3, 0xE1, 0x3D, 0x2D};
DeviceAddress probe3 = {0x28, 0xE0, 0x35, 0x80, 0xE3, 0xE1, 0x3D, 0x69};
DeviceAddress probe4 = {0x28, 0x6D, 0xFA, 0x80, 0xE3, 0xE1, 0x3D, 0x84};
DeviceAddress probe5 = {0x28, 0x49, 0xAB, 0x80, 0xE3, 0xE1, 0x3D, 0xB6};
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

unsigned long delayWifi = 300000;     // cek wifi setiap 10 detik
unsigned long delaySendSerial = 1000; // send serial setiap 1 detik
unsigned long delaySendTB = 600000;   // kirim ke TB setiap 10 menit
unsigned long delayUpdateAC = 60000;  // update AC setiap 10 menit

String prevdataAC3 = "";
String prevdataAC4 = "";
String lastUpdate = "";

void setup()
{
  Serial.begin(250000);
  Serial2.begin(BAUDRATE, SERIAL_8N1, RX_CO2_PIN, TX_CO2_PIN);
  myMHZ19.begin(Serial2);
  myMHZ19.autoCalibration();
  myMHZ19.setRange(10000);
  sensors.begin();
  dht.begin();

  // serialData.begin(250000);
  pinMode(RS485_ENABLE_PIN, OUTPUT);
  digitalWrite(RS485_ENABLE_PIN, HIGH); // Mode transmitter

  connectToWiFi();
  ArduinoOTA.setHostname("bdg-045-sensor-unit-02");
  ArduinoOTA.setPassword("admin");
  ArduinoOTA.begin();
}

float composeTemperature1()
{
  sensors.requestTemperatures();
  return sensors.getTempC(probe1);
}

float composeTemperature2()
{
  sensors.requestTemperatures();
  return sensors.getTempC(probe2);
}

float composeTemperature3()
{
  sensors.requestTemperatures();
  return sensors.getTempC(probe3);
}

float composeTemperature4()
{
  sensors.requestTemperatures();
  return sensors.getTempC(probe4);
}

float composeTemperature5()
{
  sensors.requestTemperatures();
  return sensors.getTempC(probe5);
}

float roomTemperature()
{
  return dht.readTemperature();
}

float humidity()
{
  return dht.readHumidity();
}

int carbonDioxide()
{
  return myMHZ19.getCO2();
}

String payloadData()
{
  DynamicJsonDocument doc(1024);
  // doc["bdg_001_u01_compose_temp1"] = String(composeTemperature1(), 2);
  // doc["bdg_001_u01_compose_temp2"] = String(composeTemperature2(), 2);
  // // doc["bdg_001_u01_compose_temp3"] = String(composeTemperature3(),2);
  // doc["bdg_001_u01_room_temp1"] = String(roomTemperature(), 2);
  // doc["bdg_001_u01_humidity1"] = String(humidity(), 2);
  // doc["bdg_001_u01_carbon_dioxide1"] = String(carbonDioxide());

  doc["" + String(SHED_ID) + "_comTemp6"] = String(composeTemperature1(), 2);
  doc["" + String(SHED_ID) + "_comTemp7"] = String(composeTemperature2(), 2);
  doc["" + String(SHED_ID) + "_comTemp8"] = String(composeTemperature3(), 2);
  doc["" + String(SHED_ID) + "_comTemp9"] = String(composeTemperature4(), 2);
  doc["" + String(SHED_ID) + "_comTemp10"] = String(composeTemperature5(), 2);
  doc["" + String(SHED_ID) + "_roomTemp2"] = String(roomTemperature(), 2);
  doc["" + String(SHED_ID) + "_humi2"] = String(humidity(), 2);
  doc["" + String(SHED_ID) + "_CO22"] = String(carbonDioxide());

  String payload;
  serializeJson(doc, payload);
  return payload;
}

// String statusFromTB() {
//   DynamicJsonDocument doc(375);
//   String status = "";
//   DeserializationError error = deserializeJson(doc, getDataFromTB());

//   if (!error) {
//     status = doc["shared"]["fase"].as<String>();
//   }
//   return status;
// }

// String enrichedData() {
//   String sensorData = payloadData();
//   sensorData.remove(0, 1);
//   sensorData.remove(sensorData.length() - 1);
//   // String payload = "{\"id\":\"" + String(SLAVE_ID) + "\"," + sensorData + ", \"fase\":\"" + String(statusFromTB()) + "\"}";
//   String payload = "{\"id\":\"" + String(SLAVE_ID) + "\"," + sensorData + "}";
//   return payload;
// }

void connectToWiFi()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Menghubungkan ke WiFi...");
    WiFi.begin(SSID, PASS);
  }
}

void sendDataToTB()
{
  HTTPClient http;
  String url = "https://" + String(TB_SERVER) + "/api/v1/" + String(TB_TOKEN) + "/telemetry";
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(String(payloadData()));
  // debug data disini
  //  if (httpResponseCode > 0) {
  //    Serial.print("HTTP Response code: ");
  //    Serial.println(httpResponseCode);
  //  } else {
  //    Serial.print("HTTP Error code: ");
  //    Serial.println(httpResponseCode);
  //  }
  http.end();
}

void sendDataToAttributeTB()
{
  HTTPClient http;
  String url = "https://" + String(TB_SERVER) + "/api/v1/" + String(TB_TOKEN) + "/attributes";
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(String(payloadData()));
  // debug data disini
  //  if (httpResponseCode > 0) {
  //    Serial.print("HTTP Response code: ");
  //    Serial.println(httpResponseCode);
  //  } else {
  //    Serial.print("HTTP Error code: ");
  //    Serial.println(httpResponseCode);
  //  }
  http.end();
}

String getDataFromTB()
{
  HTTPClient http;
  String url = "https://" + String(TB_SERVER) + "/api/v1/" + String(TB_TOKEN) + "/attributes?clientKeys=" + String(SHED_ID) + "_modeAC3," + String(SHED_ID) + "_modeAC4";
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
  String dataAC3 = "";
  String dataAC4 = "";
  String result;
  DeserializationError error = deserializeJson(doc, getDataFromTB());

  if (!error)
  {
    dataAC3 = doc["client"]["" + String(SHED_ID) + "_modeAC3"].as<String>();
    dataAC4 = doc["client"]["" + String(SHED_ID) + "_modeAC4"].as<String>();

    if (dataAC3 != prevdataAC3)
    {
      prevdataAC3 = dataAC3;
      lastUpdate = "modeAC3";
    }
    else if (dataAC4 != prevdataAC4)
    {
      prevdataAC4 = dataAC4;
      lastUpdate = "modeAC4";
    }

    if (lastUpdate == "modeAC3")
    {
      result = prevdataAC3;
    }
    else if (lastUpdate == "modeAC4")
    {
      result = prevdataAC4;
    }
  }
  return result;
}

String deviceToControl()
{
  String device = commandFromTB();
  return device.substring(0, 4);
}

String modeControl()
{
  String mode = commandFromTB();
  return mode.substring(5);
}

void controlAC()
{
  int irPin;
  const int IR_PIN_1 = 18;
  const int IR_PIN_2 = 19;

  if (deviceToControl() == "ac03")
  {
    irPin = IR_PIN_1;
  }
  else if (deviceToControl() == "ac04")
  {
    irPin = IR_PIN_2;
  }
  else
  {
    irPin = 0;
  }

  IrSender.setSendPin(irPin);
  remote.dataInput(modeControl());
  irsend.sendPronto(F(remote.getProntoData()), 3U);
  ledcDetachPin(IrSender.sendPin);
  // Serial.println(irPin);
  // Serial.println(F(remote.getProntoData()));
}

void sendToSerial()
{
  // serialData.println(payloadData());     //DIPAKAI
  // serialData.flush();                    //DIPAKAI
  // serialData.println(enrichedData());
  // Serial.println(enrichedData());
  // Serial.println(getDataFromTB());
  // Serial.flush();
  Serial.println(payloadData());
}

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis - prevMillisWifi >= delayWifi)
  {
    connectToWiFi();
    prevMillisWifi = currentMillis;
  }

  if (currentMillis - prevMillisSendSerial >= delaySendSerial)
  {
    sendToSerial();
    sendDataToAttributeTB();
    // Serial.println(commandFromTB());
    // Serial.println(deviceToControl());
    // Serial.println(modeControl());
    prevMillisSendSerial = currentMillis;
  }

  if (currentMillis - prevMillisSendTB >= delaySendTB)
  {
    sendDataToTB();
    prevMillisSendTB = currentMillis;
  }

  if (currentMillis - prevMillisUpdateAC >= delayUpdateAC)
  {
    controlAC();
    prevMillisUpdateAC = currentMillis;
  }

  ArduinoOTA.handle();
}
