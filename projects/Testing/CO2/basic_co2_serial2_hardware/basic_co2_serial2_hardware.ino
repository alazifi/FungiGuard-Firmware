#include <Arduino.h>
#include "MHZ19.h"

#define RX_PIN 16
#define TX_PIN 17
#define BAUDRATE 9600

MHZ19 myMHZ19;
unsigned long getDataTimer = 0;

void setup()
{
    Serial.begin(115200);
    Serial2.begin(BAUDRATE, SERIAL_8N1, RX_PIN, TX_PIN);
    myMHZ19.begin(Serial2);
    myMHZ19.autoCalibration();
    myMHZ19.setRange(20000);
}

void loop()
{
    if (millis() - getDataTimer >= 2000)
    {
        int CO2;

        CO2 = myMHZ19.getCO2(); // Nilai CO2 ppm

        Serial.print("CO2 (ppm): ");
        Serial.println(CO2);

        int8_t Temp;
        Temp = myMHZ19.getTemperature(); // Nilai suhu Celsius
        Serial.print("Temperature (C): ");
        Serial.println(Temp);
        Serial.print(myMHZ19.getRange());

        getDataTimer = millis();
    }
}
