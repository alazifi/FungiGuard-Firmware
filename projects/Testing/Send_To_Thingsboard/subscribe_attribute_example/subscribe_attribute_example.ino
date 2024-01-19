#include <WiFi.h>
#include <PubSubClient.h>

// Replace with your network credentials
const char* ssid = "IEG-1";
const char* password = "evergreen999";

// Replace with your MQTT broker address
const char* mqttServer = "iot.abc-server.id";
const int mqttPort = 1883;

const char* deviceToken = "XjsnxxhgwkOcCKIX7yPl";

WiFiClient espClient;
PubSubClient client(espClient);

// Function to handle messages received from the subscribed topic
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);

  Serial.print("Payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void subscribeToTopic(const char* topic){
      client.subscribe(topic);
      Serial.print("Subscribed to topic: ");
      Serial.println(topic);
}

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Set up MQTT client
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  // // Subscribe to a topic
  // const char* topic = "v1/devices/me/attributes";
  // client.subscribe(topic);
  // Serial.print("Subscribed to topic: ");
  // Serial.println(topic);
  //   const char* topic = "v1/devices/me/attributes";
  // subscribeToTopic(topic);
}

void loop() {
  // Reconnect to MQTT broker if necessary
  if (!client.connected()) {
    reconnect();
  }

  // Allow the client to process incoming messages
  client.loop();
}

void reconnect() {
  // Loop until reconnected to MQTT broker
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");

    if (client.connect("ESP32", deviceToken, NULL)) {
      Serial.println("Connected to MQTT broker");
      // const char* topic = "v1/devices/me/attributes";
      // client.subscribe(topic);
      // Serial.print("Subscribed to topic: ");
      // Serial.println(topic);
          const char* topic = "v1/devices/me/attributes";
  subscribeToTopic(topic);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}
