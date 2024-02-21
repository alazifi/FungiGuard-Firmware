#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define SSID "SpeedTest"
#define PASS "Test12345"

#define TB_SERVER "iot.abc-server.id"
#define TB_PORT 1883
#define TB_TOKEN "tRBRIOUt3H10IgX9ppsM"

#define BELL_PIN 33
#define NAME_PIN "bwi_001_u02_bell2"

#define TRANSMISSION_DELAY 1000

WiFiClient wifiClient;
PubSubClient client(wifiClient);

boolean gpioState = false;

// TaskHandle_t process;
TaskHandle_t transmission;

void setup()
{
  Serial.begin(115200);
  pinMode(BELL_PIN, OUTPUT);
  delay(10);
  connectWiFi();

  ArduinoOTA.setHostname("bwi-001-controller-unit-02");
  ArduinoOTA.setPassword("admin");
  ArduinoOTA.begin();

  // client.setServer(TB_SERVER, TB_PORT);
  // client.setCallback(onMessage);
  // xTaskCreatePinnedToCore(dataProcessing, "process", 20000, NULL, 1, &process, 0);
  // delay(500);

  xTaskCreatePinnedToCore(dataTransmission, "transmission", 20000, NULL, 1, &transmission, 1);
  delay(500);
}

void setStatus(String pin, boolean enabled)
{
  if (pin == NAME_PIN)
  {
    digitalWrite(BELL_PIN, enabled ? HIGH : LOW);
    gpioState = enabled;
  }
}
String getStatus()
{
  StaticJsonDocument<200> doc;
  doc[NAME_PIN] = gpioState ? true : false;
  String payload;
  serializeJson(doc, payload);
  Serial.println(payload);
  return payload;
}

// The callback for when a PUBLISH message is received from the server.
void onMessage(const char *topic, byte *payload, unsigned int length)
{

  Serial.println("On message");

  char json[length + 1];
  strncpy(json, (char *)payload, length);
  json[length] = '\0';

  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  Serial.println(json);

  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, json);
  if (error)
  {
    Serial.println("deserialize failed");
    return;
  }

  String methodName = String((const char *)doc["method"]);

  if (methodName.equals("getGpioStatus"))
  {
    // Reply with GPIO status
    String responseTopic = String(topic);
    responseTopic.replace("request", "response");
    client.publish(responseTopic.c_str(), getStatus().c_str());
  }
  else if (methodName.equals("setGpioStatus"))
  {
    // Update GPIO status and reply
    setStatus(doc["params"]["pin"], doc["params"]["enabled"]);
    String responseTopic = String(topic);
    responseTopic.replace("request", "response");
    client.publish(responseTopic.c_str(), getStatus().c_str());
    client.publish("v1/devices/me/attributes", getStatus().c_str());
    // client.publish("v1/devices/me/telemetry", getStatus().c_str());
  }
}

void connectWiFi()
{
  WiFi.begin(SSID, PASS);
}

void connectMQTT()
{
  client.setServer(TB_SERVER, TB_PORT);
  client.setCallback(onMessage);
  // while (!client.connected()) {
  //   status = WiFi.status();
  //   if ( status != WL_CONNECTED) {
  //    WiFi.begin(SSID, PASS);
  //     while (WiFi.status() != WL_CONNECTED) {
  //       delay(500);
  //       Serial.print(".");
  //     }
  //     Serial.println("Connected to AP");
  //   }
  //   Serial.print("Connecting to ThingsBoard node ...");
  //   // Attempt to connect (clientId, username, password)
  if (client.connect("Unit bwi-001-controller-unit-02", TB_TOKEN, NULL))
  {
    Serial.println("Connected to MQTT");
    // Subscribing to receive RPC requests
    client.subscribe("v1/devices/me/rpc/request/+");
    // Sending current GPIO status
    Serial.println("Sending current GPIO status ...");
    client.publish("v1/devices/me/attributes", getStatus().c_str());
    // client.publish("v1/devices/me/telemetry", getStatus().c_str());
  }
  else
  {
    Serial.print("Failed, rc=");
    Serial.print(client.state());
    Serial.println(" Retrying in 3 seconds...");
    delay(3000);
  }
}
// }

void dataProcessing(void *parameters)
{
  for (;;)
  {
  }
}

void dataTransmission(void *parameters)
{
  for (;;)
  {
    if (!client.connected())
    {
      connectMQTT();
    }
    client.loop();
    delay(TRANSMISSION_DELAY);
  }
}

void loop()
{
  ArduinoOTA.handle();
}