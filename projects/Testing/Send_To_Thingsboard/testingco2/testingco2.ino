#include <Arduino.h>
#include <MHZ19.h>

#define RX_PIN 16
#define TX_PIN 17
#define BAUDRATE 115200

MHZ19 myMHZ19;

void setup() {
  Serial.begin(115200);
  Serial2.begin(BAUDRATE, SERIAL_8N1, RX_PIN, TX_PIN);
  myMHZ19.begin(Serial2);
  myMHZ19.autoCalibration();
}

int carbonDioxide() {
  return myMHZ19.getCO2();
}

void loop() {
  Serial.println(carbonDioxide());
  delay(500);
}