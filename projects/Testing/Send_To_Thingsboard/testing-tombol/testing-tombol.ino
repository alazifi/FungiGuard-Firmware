const int tombolPin1 = 5;
const int tombolPin2 = 18;
const int tombolPin3 = 19;
const int tombolPin4 = 21;

void setup() {
  Serial.begin(115200);

  // Setiap pin tombol sebagai input
  pinMode(tombolPin1, INPUT_PULLUP);
  pinMode(tombolPin2, INPUT_PULLUP);
  pinMode(tombolPin3, INPUT_PULLUP);
  pinMode(tombolPin4, INPUT_PULLUP);
}

void loop() {
  // Membaca status tombol 1
  if (digitalRead(tombolPin1) == LOW) {
    Serial.println("Tombol 1 ditekan");
  }

  // Membaca status tombol 2
  if (digitalRead(tombolPin2) == LOW) {
    Serial.println("Tombol 2 ditekan");
  }

  // Membaca status tombol 3
  if (digitalRead(tombolPin3) == LOW) {
    Serial.println("Tombol 3 ditekan");
  }

  // Membaca status tombol 4
  if (digitalRead(tombolPin4) == LOW) {
    Serial.println("Tombol 4 ditekan");
  }

  delay(100); // Delay untuk mengurangi polling yang terlalu cepat
}
