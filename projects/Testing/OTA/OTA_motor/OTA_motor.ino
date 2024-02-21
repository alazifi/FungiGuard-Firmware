#include <AccelStepper.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define SSID "Shed45"
#define PASS "test12345"

// Inisialisasi objek stepper
AccelStepper stepper(1, 33, 32); // (tipe driver, pin Step, pin DIR)

void setup() {
  // put your setup code here, to run once:
  // Setup pin-mode
  pinMode(32, OUTPUT); // Step
  pinMode(33, OUTPUT); // DIR

  // Set kecepatan maksimum dan percepatan stepper
  stepper.setMaxSpeed(1500.0);
  stepper.setAcceleration(3000.0);
  stepper.setCurrentPosition(0);

  connectToWiFi();
  
  // stepper.runToPosition();
  ArduinoOTA.setHostname("bdg-045-aktuator-unit-02");
  ArduinoOTA.setPassword("admin");
  ArduinoOTA.begin();
}

void connectToWiFi()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Menghubungkan ke WiFi...");
    WiFi.begin(SSID, PASS);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
    stepper.setSpeed(200); // Set kecepatan putaran (dalam langkah per detik)
  stepper.moveTo(-30000);   // Pergi ke posisi langkah 500
  stepper.runToPosition(); // Jalankan stepper sampai mencapai posisi yang ditentukan

  delay(5000);
  ArduinoOTA.handle();
}
