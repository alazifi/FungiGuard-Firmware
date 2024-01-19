#include <Arduino.h>
#include <ArduinoJson.h>

#define RS485_ENABLE_PIN 19 // Contoh, sesuaikan dengan konfigurasi Anda
#define SLAVE_ID 1

void setup()
{
  Serial.begin(250000);
  Serial2.begin(115200);
  pinMode(RS485_ENABLE_PIN, OUTPUT);
  digitalWrite(RS485_ENABLE_PIN, HIGH); // Mode transmitter
}

String data()
{

  DynamicJsonDocument doc(512);
  doc["bwi_001_u01_compose_temp1"] = "nilai1";
  doc["bwi_001_u01_compose_temp2"] = "nilai2";
  doc["bwi_001_u01_compose_temp3"] = "nilai3";
  doc["bwi_001_u01_room_temp1"] = "nilai4";
  doc["bwi_001_u01_humidity1"] = "nilai5";
  doc["bwi_001_u01_carbon_dioxide1"] = "nilai6";

  String payload;
  serializeJson(doc, payload);
  return payload;
}

void loop()
{
  String sensorData = data();
  sensorData.remove(0, 1);
  sensorData.remove(sensorData.length() - 1);
  String payload = "{\"id\":" + String(SLAVE_ID) + "," + sensorData + "}";
  Serial2.print(payload);
  Serial.println(payload);
  delay(1000);
}
