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


#define SSID "IEG-1"
#define PASS "evergreen999"

#define TB_SERVER "iot.abc-server.id"
#define TB_PORT 1883
#define TB_TOKEN "XjsnxxhgwkOcCKIX7yPl"

#define RX_PIN 16
#define TX_PIN 17
#define BAUDRATE 9600

#define DS18B20_PIN 25
#define DHT_PIN 26
#define DHT_TYPE DHT22

const int IR_PIN_1 = 18;
const int IR_PIN_2 = 27;

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
unsigned long delaySendTB = 10000;   // kirim ke TB setiap 10 menit

String teks = "MODE DISINI";
String receivedPayloadValue = "";

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
  client.setServer(TB_SERVER, TB_PORT);
  client.setCallback(callback);
  // pinMode(IR_PIN_1, OUTPUT);
  // pinMode(IR_PIN_2, OUTPUT);
  ArduinoOTA.setHostname("bdg-001-sensor-unit-01");
  ArduinoOTA.setPassword("admin");
  ArduinoOTA.begin();
}

void loop() {
  unsigned long currentMillis = millis();

  // if (currentMillis - previousMillisWifi >= delayWifi) {
  //   connectToWiFi();
  //   previousMillisWifi = currentMillis;
  // }

  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  if (currentMillis - previousMillisPrintLCD >= delayPrintLCD) {
    printLCD();
    previousMillisPrintLCD = currentMillis;
  }

  if (currentMillis - previousMillisSendTB >= delaySendTB) {
    sendToThingsboard();
    previousMillisSendTB = currentMillis;
  }
  ArduinoOTA.handle();
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

String getPayload() {
  return receivedPayloadValue;
}

String commandFromTB() {
  DynamicJsonDocument doc(512);
  DeserializationError error = deserializeJson(doc, getPayload());
  if (!error) {
    if (doc.containsKey("modeAC1") && doc["modeAC1"].is<String>()) {
      return doc["modeAC1"].as<String>();
    }

    if (doc.containsKey("modeAC2") && doc["modeAC2"].is<String>()) {
      return doc["modeAC2"].as<String>();
    }
  }
  return "none";
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
  int irPin = 0;

  if (deviceToControl() != "none") {

    irPin = (deviceToControl() == "ac01") ? IR_PIN_1 : IR_PIN_2;

    IrSender.setSendPin(irPin);
    remote.dataInput(modeControl());
    Serial.println(irPin);
    irsend.sendPronto(F(remote.getProntoData()), 3U);
    ledcDetachPin(IrSender.sendPin);
  }
}

void sendToThingsboard() {
  String topicTelemetry = "v1/devices/me/telemetry";
  if (client.publish(topicTelemetry.c_str(), payloadData().c_str())) {
    Serial.println("Data terkirim ke Thingsboard");
  } else {
    Serial.println("Gagal kirim data ke Thingsboard");
  }
}

void reconnect() {
  if (client.connect("device-1-pangalengan", TB_TOKEN, NULL)) {
    Serial.println("Terhubung ke Thingsboard");
    const char* topicAttributes = "v1/devices/me/attributes";
    subscribeToTopic(topicAttributes);
  }
}

void subscribeToTopic(const char* topic) {
  client.subscribe(topic);
  Serial.print("Subscribed to topic: ");
  Serial.println(topic);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Pesan diterima dari topik: ");
  Serial.println(topic);

  receivedPayloadValue = "";  // Reset nilai receivedPayloadValue sebelum digunakan kembali
  for (int i = 0; i < length; i++) {
    receivedPayloadValue += (char)payload[i];
  }
  Serial.print("Nilai payload: " + String(receivedPayloadValue));
  Serial.println();
}

void connectToWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Menghubungkan ke WiFi...");
    WiFi.begin(SSID, PASS);
  }
}

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

void printLCD() {
  lcd.setCursor((20 - String(teks).length()) / 2, 0);
  lcd.print(teks);

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
