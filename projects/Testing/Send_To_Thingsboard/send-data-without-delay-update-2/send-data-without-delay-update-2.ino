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

#define SLAVE_ID 2

OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);
DeviceAddress probe4 = { 0x28, 0x64, 0xA0, 0x80, 0xE3, 0xE1, 0x3D, 0x3A };
DeviceAddress probe5 = { 0x28, 0xBB, 0xFB, 0x80, 0xE3, 0xE1, 0x3D, 0x3F };
DHT dht(DHT_PIN, DHT_TYPE);
LiquidCrystal_I2C lcd(0x27, 20, 4);
MHZ19 myMHZ19;
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long previousMillisWifi = 0;
unsigned long previousMillisPrintLCD = 0;
unsigned long previousMillisSendTB = 0;

unsigned long delayWifi = 9990;  // cek wifi setiap 5 detik
unsigned long delayPrintLCD = 1000; // print LCD setiap 1 detik
unsigned long delaySendTB = 10000;    // kirim ke TB setiap 10 menit

String teks = "MODE DISINI";

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
  ArduinoOTA.setHostname("bdg-001-sensor-unit-02");
  ArduinoOTA.setPassword("admin");
  ArduinoOTA.begin();
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillisWifi >= delayWifi) {
    //connectToWiFi();
    previousMillisWifi = currentMillis;
  }

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

String payloadData(){
  DynamicJsonDocument doc(512);
  doc["bdg_001_u02_compose_temp4"] = String(composeTemperature4(), 2);
  doc["bdg_001_u02_compose_temp5"] = String(composeTemperature5(),2);
  // doc["bdg_001_u02_compose_temp6"] = String(composeTemperature6(),2);
  doc["bdg_001_u02_room_temp2"] = String(roomTemperature(),2);
  doc["bdg_001_u02_humidity2"] = String(humidity(),2);
  doc["bdg_001_u02_carbon_dioxide2"] = carbonDioxide();

  String payload;
  serializeJson(doc, payload);
  return payload;
}

void sendToThingsboard() {
  String topicTelemetry = "v1/devices/me/telemetry";
  String topicAttribute = "v1/devices/me/attributes";
  if (client.publish(topicTelemetry.c_str(), payloadData().c_str())) {
    Serial.println("Data terkirim ke Thingsboard");
    // client.disconnect();
  } else {
    Serial.println("Gagal kirim data ke Thingsboard");
  }
}

void reconnect() {
  if (client.connect("device-2-bdg", TB_TOKEN, NULL)) {
    Serial.println("Terhubung ke Thingsboard");
  }
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

float composeTemperature4(){
  sensors.requestTemperatures();
  return sensors.getTempC(probe4);
}

float composeTemperature5(){
  sensors.requestTemperatures();
  return sensors.getTempC(probe5);
}

// float composeTemperature6(){
//   sensors.requestTemperatures();
//   return sensors.getTempC(probe6);
// }

int carbonDioxide() {
  return myMHZ19.getCO2();
}

void printLCD() {
  lcd.setCursor((20 - String(teks).length())/2,0);
  lcd.print(teks);

  lcd.setCursor(0, 1);
  lcd.print("SK4:" + String(composeTemperature4(), 1) + "C ");

  lcd.setCursor(0, 2);
  lcd.print("SK5:" + String(composeTemperature5(), 1) + "C ");

  // lcd.setCursor(0, 3);
  // lcd.print("SK6:" + String(composeTemperature6(), 1) + "C ");

  lcd.setCursor(10, 1);
  lcd.print("Temp:" + String(roomTemperature(), 1) + "C");

  lcd.setCursor(10, 2);
  lcd.print("Humi:" + String(humidity(), 1) + "%");

  lcd.setCursor(10, 3);
  lcd.print("CO2:" + String(carbonDioxide()) + "pm ");
}
