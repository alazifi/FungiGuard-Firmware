#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);
LiquidCrystal_I2C lcd2(0x26, 20, 4);

unsigned long getDataTimer = 0;

void setup() {

  lcd.init();
  lcd.backlight();
  lcd.clear();

    lcd2.init();
  lcd2.backlight();
  lcd2.clear();
}

void loop() {
  if (millis() - getDataTimer >= 2000) {

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("CO2: 56");
    lcd.print("ppm");

        lcd2.clear();

    lcd2.setCursor(0, 0);
    lcd2.print("CO2: 34");
    lcd2.print("ppm");

    getDataTimer = millis();
  }
}
