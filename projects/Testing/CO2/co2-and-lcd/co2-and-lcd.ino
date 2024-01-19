#include <Arduino.h>
#include "MHZ19.h"
#include <LiquidCrystal_I2C.h>

#define RX_PIN 16
#define TX_PIN 17
#define BAUDRATE 9600

MHZ19 myMHZ19;
LiquidCrystal_I2C lcd(0x27, 20, 4);

unsigned long getDataTimer = 0;

void setup() {
  Serial.begin(9600);
  Serial2.begin(BAUDRATE, SERIAL_8N1, RX_PIN, TX_PIN);
  myMHZ19.begin(Serial2);

  myMHZ19.autoCalibration();
  myMHZ19.setRange(10000);
  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void loop() {
  if (millis() - getDataTimer >= 2000) {
    int CO2;
    CO2 = myMHZ19.getCO2(); //Nilai CO2 ppm

    Serial.print("CO2 (ppm): ");
    Serial.println(CO2);

    int8_t Temp;
    Temp = myMHZ19.getTemperature(); //Nilai suhu Celcius
    Serial.print("Temperature (C): ");
    Serial.println(Temp);
    Serial.print(myMHZ19.getRange());
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("CO2: ");
    lcd.setCursor(4, 0);
    lcd.print(myMHZ19.getCO2());
    lcd.setCursor(9, 0);
    lcd.print("ppm");

    getDataTimer = millis();
  }
}
