// Koding untuk mengendalikan dua lampu LED pada ESP32 agar berkedip menggunakan millis dengan interval berbeda

// Library yang diperlukan
#include <Arduino.h>
#include <WiFi.h>

// Pin yang digunakan untuk lampu LED
const int ledPin1 = 25;
const int ledPin2 = 26;

#define SSID "SpeedTest"
#define PASS "Test12345"

// Variabel untuk menyimpan waktu terakhir kedipan
unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;

// Interval antara kedipan (dalam milidetik)
const long interval1 = 2000; // Interval lampu 1
const long interval2 = 500;  // Interval lampu 2

// Status lampu (HIGH atau LOW)
int ledState1 = LOW;
int ledState2 = LOW;

void setup() {
  // Mengatur pin sebagai output
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  WiFi.begin(SSID, PASS);
}

void loop() {
  // Mendapatkan waktu sekarang
  unsigned long currentMillis = millis();

  // Memeriksa apakah sudah waktunya untuk mengubah status lampu 1
  if (currentMillis - previousMillis1 >= interval1) {
    // Menyimpan waktu terakhir lampu 1 diubah

    // Mengubah status lampu 1
    ledState1 = (ledState1 == LOW) ? HIGH : LOW;

    // Menyalakan atau mematikan lampu 1 sesuai dengan status terbaru
    digitalWrite(ledPin1, ledState1);
    previousMillis1 = currentMillis;
  }

  // Memeriksa apakah sudah waktunya untuk mengubah status lampu 2
  if (currentMillis - previousMillis2 >= interval2) {
    // Menyimpan waktu terakhir lampu 2 diubah

    // Mengubah status lampu 2
    ledState2 = (ledState2 == LOW) ? HIGH : LOW;

    // Menyalakan atau mematikan lampu 2 sesuai dengan status terbaru
    digitalWrite(ledPin2, ledState2);
    previousMillis2 = currentMillis;
  }
}
