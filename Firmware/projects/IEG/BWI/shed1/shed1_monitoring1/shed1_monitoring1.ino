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

#define SSID "SpeedTest"
#define PASS "Test12345"

#define TB_SERVER "iot.abc-server.id"
#define TB_PORT 1883
#define TB_TOKEN "tRBRIOUt3H10IgX9ppsM"

#define RX_PIN 16
#define TX_PIN 17
#define BAUDRATE 9600

#define DS18B20_PIN 25
#define DHT_PIN 26
#define DHT_TYPE DHT22

#define TRANSMISSION_DELAY 600000
#define DISPLAYING_DELAY 5000

OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);
DeviceAddress probe1 = { 0x28, 0xA1, 0x6, 0x80, 0xE3, 0xE1, 0x3D, 0xE1 };
DeviceAddress probe2 = { 0x28, 0x61, 0xCA, 0x80, 0xE3, 0xE1, 0x3D, 0x7D };
DHT dht(DHT_PIN, DHT_TYPE);
LiquidCrystal_I2C lcd(0x27, 20, 4);
MHZ19 myMHZ19;
WiFiClient espClient;
PubSubClient client(espClient);

TaskHandle_t process;
TaskHandle_t transmission;

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
  connectWiFi();

  ArduinoOTA.setHostname("bwi-001-monitoring-unit-01");

  ArduinoOTA.setPassword("admin");
  ArduinoOTA.begin();

  xTaskCreatePinnedToCore(dataProcessing, "process", 20000, NULL, 1, &process, 0);
  delay(500);

  xTaskCreatePinnedToCore(dataTransmission, "transmission", 20000, NULL, 1, &transmission, 1);
  delay(500);
}

void connectWiFi() {
  WiFi.begin(SSID, PASS);
}

void connectMQTT() {
  client.setServer(TB_SERVER, TB_PORT);

  while (!client.connected()) {
    if (client.connect("Unit 1 MU", TB_TOKEN, NULL)) {
      Serial.println("Connected to MQTT");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 3 seconds...");
      delay(3000);
    }
  }
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

void printLCD() {
  sensors.requestTemperatures();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("SK1: ");
  lcd.setCursor(4, 0);
  lcd.print(sensors.getTempC(probe1), 1);
  lcd.setCursor(8, 0);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("SK2: ");
  lcd.setCursor(4, 1);
  lcd.print(sensors.getTempC(probe2), 1);
  lcd.setCursor(8, 1);
  lcd.print("C");

  // lcd.setCursor(0, 2);
  // lcd.print("SK3: ");
  // lcd.setCursor(4, 2);
  // lcd.print(sensors.getTempC(probe3), 1);
  // lcd.setCursor(8, 2);
  // lcd.print("C");

  lcd.setCursor(10, 0);
  lcd.print("Temp:");
  lcd.setCursor(15, 0);
  lcd.print(roomTemperature(), 1);
  lcd.setCursor(19, 0);
  lcd.print("C");

  lcd.setCursor(10, 1);
  lcd.print("Humi:");
  lcd.setCursor(15, 1);
  lcd.print(humidity(), 1);
  lcd.setCursor(19, 1);
  lcd.print("%");

  lcd.setCursor(0, 3);
  lcd.print("Kadar CO2 : ");
  lcd.setCursor(12, 3);
  lcd.print(carbonDioxide());
  lcd.setCursor(17, 3);
  lcd.print("ppm");
  delay(DISPLAYING_DELAY);
}

void sendData() {
  sensors.requestTemperatures();
  String topicTelemetry = "v1/devices/me/telemetry";
  String topicAttribute = "v1/devices/me/attributes";

  DynamicJsonDocument doc(512);
  doc["bwi_001_u01_compose_temp1"] = sensors.getTempC(probe1);
  doc["bwi_001_u01_compose_temp2"] = sensors.getTempC(probe2);
  doc["bwi_001_u01_compose_temp3"] = sensors.getTempC(probe3);
  doc["bwi_001_u01_room_temp1"] = roomTemperature();
  doc["bwi_001_u01_humidity1"] = humidity();
  doc["bwi_001_u01_carbon_dioxide1"] = carbonDioxide();

  String payload;
  serializeJson(doc, payload);
  client.publish(topicTelemetry.c_str(), payload.c_str());
  // client.publish(topicAttribute.c_str(), payload.c_str());
  // delay(1000);
}

void dataProcessing(void* parameters) {

  for (;;) {
    printLCD();
  }
}

void dataTransmission(void* parameters) {
  for (;;) {
    if (!client.connected()) {
      connectMQTT();
    }
    client.loop();
    sendData();
    delay(TRANSMISSION_DELAY);
  }
}

void loop() {
  ArduinoOTA.handle();
}