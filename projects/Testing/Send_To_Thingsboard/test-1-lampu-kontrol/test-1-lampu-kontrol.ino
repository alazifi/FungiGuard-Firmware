#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>

#define WIFI_SSID "SpeedTest"
#define WIFI_PASS "test12345"

#define TOKEN "tRBRIOUt3H10IgX9ppsM"

#define BELL_PIN 32
#define NAME_PIN "bell1"

char thingsboardServer[] = "iot.abc-server.id";

WiFiClient wifiClient;

PubSubClient client(wifiClient);

int status = WL_IDLE_STATUS;

// We assume that all GPIOs are LOW
boolean gpioState = false;

void setStatus(String pin, boolean enabled) {
  if (pin == NAME_PIN) {
    digitalWrite(BELL_PIN, enabled ? HIGH : LOW);
    gpioState = enabled;
}
}
String getStatus() {
  StaticJsonDocument<200> doc;
  doc[NAME_PIN] = gpioState ? true : false;
  String payload;
  serializeJson(doc, payload);
  Serial.println(payload);
  return payload;
}

// The callback for when a PUBLISH message is received from the server.
void on_message(const char* topic, byte* payload, unsigned int length) {

  Serial.println("On message");

  char json[length + 1];
  strncpy (json, (char*)payload, length);
  json[length] = '\0';

  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  Serial.println(json);

  // Decode JSON request
  StaticJsonDocument<200> doc;
  //JsonObject& data = jsonBuffer.parseObject((char*)json);
  DeserializationError error = deserializeJson(doc, json);
  if (error)
  {
    Serial.println("deserialize failed");
    return;
  }

  // Check request method
  String methodName = String((const char*)doc["method"]);

  if (methodName.equals("getGpioStatus")) {
    // Reply with GPIO status
    String responseTopic = String(topic);
    responseTopic.replace("request", "response");
    client.publish(responseTopic.c_str(), getStatus().c_str());
  } else if (methodName.equals("setGpioStatus")) {
    // Update GPIO status and reply
    setStatus(doc["params"]["pin"], doc["params"]["enabled"]);
    String responseTopic = String(topic);
    responseTopic.replace("request", "response");
    client.publish(responseTopic.c_str(), getStatus().c_str());
    client.publish("v1/devices/me/attributes", getStatus().c_str());
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      WiFi.begin(WIFI_SSID, WIFI_PASS);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Connected to AP");
    }
    Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect("ESP8266 Device", TOKEN, NULL) ) {
      Serial.println( "[DONE]" );
      // Subscribing to receive RPC requests
      client.subscribe("v1/devices/me/rpc/request/+");
      // Sending current GPIO status
      Serial.println("Sending current GPIO status ...");
      client.publish("v1/devices/me/attributes", getStatus().c_str());
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );
      delay( 5000 );
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(BELL_PIN, OUTPUT);
  delay(10);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  client.setServer( thingsboardServer, 1883 );
  client.setCallback(on_message);
}

void loop() {
  if ( !client.connected() ) {
    reconnect();
  }

  client.loop();
}