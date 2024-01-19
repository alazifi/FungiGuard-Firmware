#include <Arduino.h>
#include "MHZ19.h"
#include <LiquidCrystal_I2C.h>  // Remove if using HardwareSerial

#define RX_PIN 16
#define TX_PIN 17
#define BAUDRATE 9600

MHZ19 myMHZ19;
LiquidCrystal_I2C lcd(0x27, 20, 4);  // (Uno example) create device to MH-Z19 serial

unsigned long getDataTimer = 0;

void setup() {
  Serial.begin(9600);  // Device to serial monitor feedback
  Serial2.begin(BAUDRATE, SERIAL_8N1, RX_PIN, TX_PIN);
  myMHZ19.begin(Serial2);  // *Serial(Stream) refence must be passed to library begin().

  myMHZ19.autoCalibration();
  myMHZ19.setRange(10000);
  lcd.init();
  lcd.backlight();
  lcd.clear();  // Turn auto calibration ON (OFF autoCalibration(false))
}

void loop() {
  if (millis() - getDataTimer >= 2000) {
    int CO2;

    /* note: getCO2() default is command "CO2 Unlimited". This returns the correct CO2 reading even 
        if below background CO2 levels or above range (useful to validate sensor). You can use the 
        usual documented command with getCO2(false) */

    CO2 = myMHZ19.getCO2();  // Request CO2 (as ppm)

    Serial.print("CO2 (ppm): ");
    Serial.println(CO2);

    int8_t Temp;
    Temp = myMHZ19.getTemperature();  // Request Temperature (as Celsius)
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
