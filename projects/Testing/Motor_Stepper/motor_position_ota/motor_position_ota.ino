#include <AccelStepper.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define SSID "Shed45"
#define PASS "test12345"

// Inisialisasi objek stepper
AccelStepper stepper(1, 33, 32); // (tipe driver, pin Step, pin DIR)

void connectToWiFi()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Menghubungkan ke WiFi...");
    WiFi.begin(SSID, PASS);
  }
}

void setup() {
  // Setup pin-mode
  pinMode(33, OUTPUT); // Step
  pinMode(32, OUTPUT); // DIR

  // Set kecepatan maksimum dan percepatan stepper
  stepper.setMaxSpeed(250.0);
  stepper.setAcceleration(500.0);
  
  // stepper.setSpeed(0); // Set kecepatan putaran (dalam langkah per detik)
  // stepper.setCurrentPosition(0);
  ArduinoOTA.setHostname("bdg-045-aktuator-unit-01");
  ArduinoOTA.setPassword("admin");
  ArduinoOTA.begin();
}

void loop() {
  stepper.setSpeed(50);
    stepper.moveTo(0);   // Pergi ke posisi langkah 500
  stepper.runToPosition(); // Jalankan stepper sampai mencapai
  // connectToWiFi();
  // // Putar stepper searah jarum jam
  // stepper.setSpeed(50); // Set kecepatan putaran (dalam langkah per detik)
  // stepper.moveTo(1000);   // Pergi ke posisi langkah 500
  // stepper.runToPosition(); // Jalankan stepper sampai mencapai posisi yang ditentukan

  delay(1000);

// digitalWrite(32, LOW);
// digitalWrite(33, LOW);
  // // Putar stepper berlawanan arah jarum jam
  // stepper.setSpeed(100); // Set kecepatan putaran (dalam langkah per detik)
  // stepper.moveTo(0);     // Pergi ke posisi langkah 0
  // stepper.runToPosition(); // Jalankan stepper sampai mencapai posisi yang ditentukan

  // delay(1000); // Tunda 1 detik

  ArduinoOTA.handle();
}
