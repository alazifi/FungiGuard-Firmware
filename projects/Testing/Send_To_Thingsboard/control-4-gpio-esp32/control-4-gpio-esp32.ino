#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>

#define WIFI_SSID "IEG-1"
#define WIFI_PASS "evergreen999"

#define TOKEN "vz7dNmziBqIbhyTa9gvH"

char thingsboardServer[] = "iot.abc-server.id";

WiFiClient wifiClient;

PubSubClient client(wifiClient);

int status = WL_IDLE_STATUS;

#define NUM_LAMPS 4
int lampPins[NUM_LAMPS] = {33, 25, 26, 27};
const char* lampNames[NUM_LAMPS] = {"lamp1", "lamp2", "lamp3", "lamp4"};

boolean lampStates[NUM_LAMPS] = {false, false, false, false};

void setStatus(String lampName, boolean enabled) {
  for (int i = 0; i < NUM_LAMPS; i++) {
    if (lampName.equals(lampNames[i])) {
      digitalWrite(lampPins[i], enabled ? HIGH : LOW);
      lampStates[i] = enabled;
    }
  }
}

String getStatus() {
  StaticJsonDocument<200> doc;
  for (int i = 0; i < NUM_LAMPS; i++) {
    doc[lampNames[i]] = lampStates[i];
  }
  String payload;
  serializeJson(doc, payload);
  Serial.println(payload);
  return payload;
}

void on_message(const char* topic, byte* payload, unsigned int length) {
  char json[length + 1];
  strncpy(json, (char*)payload, length);
  json[length] = '\0';

  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  Serial.println(json);

  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, json);
  if (error) {
    Serial.println("deserialize failed");
    return;
  }

  String methodName = String((const char*)doc["method"]);

  if (methodName.equals("getGpioStatus")) {
    String responseTopic = String(topic);
    responseTopic.replace("request", "response");
    client.publish(responseTopic.c_str(), getStatus().c_str());
  } else if (methodName.equals("setGpioStatus")) {
 //   String lampName = String(doc["params"]["lamp"]);
    setStatus(doc["params"]["lamp"], doc["params"]["enabled"]);
    String responseTopic = String(topic);
    responseTopic.replace("request", "response");
    client.publish(responseTopic.c_str(), getStatus().c_str());
    client.publish("v1/devices/me/attributes", getStatus().c_str());
  }
}

void reconnect() {
  while (!client.connected()) {
    status = WiFi.status();
    if (status != WL_CONNECTED) {
      WiFi.begin(WIFI_SSID, WIFI_PASS);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Connected to AP");
    }

    Serial.print("Connecting to ThingsBoard node ...");
    if (client.connect("ESP8266 Device", TOKEN, NULL)) {
      Serial.println("[DONE]");
      client.subscribe("v1/devices/me/rpc/request/+");
      Serial.println("Sending current GPIO status ...");
      client.publish("v1/devices/me/attributes", getStatus().c_str());
      delay(500);
    } else {
      Serial.print("[FAILED] [ rc = ");
      Serial.print(client.state());
      Serial.println(" : retrying in 5 seconds]");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < NUM_LAMPS; i++) {
    pinMode(lampPins[i], OUTPUT);
  }

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  client.setServer(thingsboardServer, 1883);
  client.setCallback(on_message);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();
}
