#include <Arduino.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

#define RS485_ENABLE_PIN 14  // Contoh, sesuaikan dengan konfigurasi Anda
#define SLAVE_ID 2

#define RX_PIN 33
#define TX_PIN 32

SoftwareSerial mySerial(RX_PIN, TX_PIN);

void setup() {
  Serial.begin(250000);
  mySerial.begin(115200);
  // Serial2.begin(115200);
  pinMode(RS485_ENABLE_PIN, OUTPUT);
  digitalWrite(RS485_ENABLE_PIN, HIGH);  // Mode transmitter
}

String data(){
  
  DynamicJsonDocument doc(512);
  doc["bwi_001_u02_compose_temp4"] = "nilai7";
  doc["bwi_001_u02_compose_temp5"] = "nilai8";
  doc["bwi_001_u02_compose_temp6"] = "nilai9";
  doc["bwi_001_u02_room_temp2"] = "nilai10";
  doc["bwi_001_u02_humidity2"] = "nilai11";
  doc["bwi_001_u02_carbon_dioxide2"] = "nilai12";

  String payload;
  serializeJson(doc, payload);
  return payload;
}

void loop() {
  String sensorData = data();
  sensorData.remove(0,1);
  sensorData.remove(sensorData.length()-1);
  String payload = "{\"id\":" + String(SLAVE_ID) + "," + sensorData + "}";
  mySerial.print(payload);
  // Serial2.println("Ini Serial 2");
  Serial.println(payload);
  delay(1000);
}
