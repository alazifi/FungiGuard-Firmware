#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

const int ledPin1 = 32;
const int ledPin2 = 33;

#define SSID "SpeedTest"
#define PASS "Test12345"

const char *mqtt_server = "iot.abc-server.id";  // Alamat server Thingsboard
const char *mqtt_username = "XjsnxxhgwkOcCKIX7yPl"; // Ganti dengan akses token dari Thingsboard
const char *mqtt_client_id = "Test Millis";      // Nama client untuk MQTT

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long previousMillisWifi = 0;
unsigned long intervalWifi = 5000;  // Check Wi-Fi every 5 seconds

unsigned long previousMillis1 = 0;
unsigned long interval1 = 2000;
int ledState1 = LOW;

unsigned long previousMillis2 = 0;
unsigned long interval2 = 500;
int ledState2 = LOW;

void setup() {
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  Serial.begin(115200);
  connectToWiFi();  // Connect to Wi-Fi

  client.setServer(mqtt_server, 1883);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillisWifi >= intervalWifi) {
    connectToWiFi();  // Check Wi-Fi connection
    previousMillisWifi = currentMillis;
  }

  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  if (currentMillis - previousMillis1 >= interval1) {
    ledState1 = (ledState1 == LOW) ? HIGH : LOW;
    digitalWrite(ledPin1, ledState1);

    sendToThingsboard("LED1", ledState1);

    previousMillis1 = currentMillis;
  }

  if (currentMillis - previousMillis2 >= interval2) {
    ledState2 = (ledState2 == LOW) ? HIGH : LOW;
    digitalWrite(ledPin2, ledState2);

    sendToThingsboard("LED2", ledState2);

    previousMillis2 = currentMillis;
  }
}

void sendToThingsboard(const char *attribute, int value) {
  String topic = "v1/devices/me/telemetry";
  String payload = "{\"" + String(attribute) + "\":" + String(value) + "}";

  if (client.publish(topic.c_str(), payload.c_str())) {
    Serial.println("Data sent to Thingsboard");
  } else {
    Serial.println("Failed to send data to Thingsboard");
  }
}

void reconnect() {
  if (client.connect(mqtt_client_id, mqtt_username, NULL)) {
    Serial.println("Connected to Thingsboard");
  }
}

void connectToWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(SSID, PASS);
  }
}
