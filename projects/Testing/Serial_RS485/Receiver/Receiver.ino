#include <Arduino.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

#define RS485_ENABLE_PIN 14

#define RX_PIN 33
#define TX_PIN 32

SoftwareSerial mySerial(RX_PIN, TX_PIN);
String nilai1, nilai2, nilai3, nilai4, nilai5, nilai6, nilai7, nilai8, nilai9, nilai10, nilai11, nilai12;
int id;

void setup()
{
  Serial.begin(250000);
  mySerial.begin(115200);
  // Serial2.begin(115200);
  pinMode(RS485_ENABLE_PIN, OUTPUT);
  digitalWrite(RS485_ENABLE_PIN, LOW); // Mode receiver
}

void loop()
{
  // Membaca data dari ESP Transmitter
  if (mySerial.available() > 0)
  {
    String receivedData = mySerial.readString();
    // DynamicJsonDocument doc(512);
    // DeserializationError error = deserializeJson(doc, receivedData);

    //   if (!error) {
    //     id = doc["id"];
    //     if (id == 1) {
    //       nilai1 = doc["bwi_001_u01_compose_temp1"].as<String>();
    //       nilai2 = doc["bwi_001_u01_compose_temp2"].as<String>();
    //       nilai3 = doc["bwi_001_u01_compose_temp3"].as<String>();
    //       nilai4 = doc["bwi_001_u01_room_temp1"].as<String>();
    //       nilai5 = doc["bwi_001_u01_humidity1"].as<String>();
    //       nilai6 = doc["bwi_001_u01_carbon_dioxide1"].as<String>();
    //     } else if (id == 2) {
    //       nilai7 = doc["bwi_001_u02_compose_temp4"].as<String>();
    //       nilai8 = doc["bwi_001_u02_compose_temp5"].as<String>();
    //       nilai9 = doc["bwi_001_u02_compose_temp6"].as<String>();
    //       nilai10 = doc["bwi_001_u02_room_temp2"].as<String>();
    //       nilai11 = doc["bwi_001_u02_humidity2"].as<String>();
    //       nilai12 = doc["bwi_001_u02_carbon_dioxide2"].as<String>();
    //     }
    //     Serial.println(" " + nilai1 + " " + nilai2 + " " + nilai3 + " " + nilai4 + " " + nilai5 + " " + nilai6 + " " + nilai7 + " " + nilai8 + " " + nilai9 + " " + nilai10 + " " + nilai11 + " " + nilai12 + " ");
    //   }
    // }
    Serial.println(receivedData);

    delay(1000);
  }
}