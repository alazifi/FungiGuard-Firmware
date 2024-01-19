#include <AccelStepper.h>

// Inisialisasi objek stepper
AccelStepper stepper(1, 33, 32); // (tipe driver, pin Step, pin DIR)

void setup() {
  // Setup pin-mode
  pinMode(32, OUTPUT); // Step
  pinMode(33, OUTPUT); // DIR

  // Set kecepatan maksimum dan percepatan stepper
  stepper.setMaxSpeed(250.0);
  stepper.setAcceleration(500.0);
}

void loop() {
  // Putar stepper searah jarum jam
  stepper.setSpeed(50); // Set kecepatan putaran (dalam langkah per detik)
  stepper.moveTo(1000);   // Pergi ke posisi langkah 500
  stepper.runToPosition(); // Jalankan stepper sampai mencapai posisi yang ditentukan

  delay(1000);

// digitalWrite(32, LOW);
// digitalWrite(33, LOW);
  // // Putar stepper berlawanan arah jarum jam
  stepper.setSpeed(100); // Set kecepatan putaran (dalam langkah per detik)
  stepper.moveTo(0);     // Pergi ke posisi langkah 0
  stepper.runToPosition(); // Jalankan stepper sampai mencapai posisi yang ditentukan

  delay(1000); // Tunda 1 detik
}
