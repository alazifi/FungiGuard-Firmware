#include <Arduino.h>
#include <SoftwareSerial.h>
#include "MHZ19.h"

#define RX_PIN 33
#define TX_PIN 32
#define BAUDRATE 9600

SoftwareSerial mySerial(RX_PIN, TX_PIN);  // RX, TX

MHZ19 myMHZ19;  // Create device to MH-Z19 serial

unsigned long getDataTimer = 0;

void setup()
{
    Serial.begin(115200); // Device to serial monitor feedback
    mySerial.begin(BAUDRATE); // Initialize SoftwareSerial

    myMHZ19.begin(mySerial); // Pass the SoftwareSerial object to MH-Z19
    myMHZ19.autoCalibration();
    myMHZ19.setRange(20000); // Set the range (adjust to your needs)
}

void loop()
{
    if (millis() - getDataTimer >= 2000)
    {
        int CO2;

        CO2 = myMHZ19.getCO2(); // Request CO2 (as ppm)

        Serial.print("CO2 (ppm): ");
        Serial.println(CO2);

        int8_t Temp;
        Temp = myMHZ19.getTemperature(); // Request Temperature (as Celsius)
        Serial.print("Temperature (C): ");
        Serial.println(Temp);
        Serial.print(myMHZ19.getRange());

        getDataTimer = millis();
    }
}
