const int DIR = 33;
const int STEP = 32;
const int steps_per_rev = 200;
const int step_delay = 1000; // in microseconds
unsigned long previousMillis = 0;
const long interval = 1000; // interval between steps in milliseconds

void setup()
{
  Serial.begin(115200);
  pinMode(STEP, OUTPUT);
  pinMode(DIR, OUTPUT);
}

void stepMotor(int dir, int steps)
{
  digitalWrite(DIR, dir);

  for (int i = 0; i < steps; i++)
  {
    digitalWrite(STEP, HIGH);
    delayMicroseconds(step_delay);
    digitalWrite(STEP, LOW);
    delayMicroseconds(step_delay);
  }
}

void loop()
{
  unsigned long currentMillis = millis();

  Serial.println("Spinning Clockwise...");

  if (currentMillis - previousMillis >= interval)
  {
    stepMotor(LOW, steps_per_rev);
    previousMillis = currentMillis;
  }

}
