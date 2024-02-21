#include <ArduinoJson.h>  //library for parsing data json
#include <WiFi.h>         //library for WiFi
#include <ESPmDNS.h>      //library for OTA
#include <WiFiUdp.h>      //library for OTA
#include <ArduinoOTA.h>   //main library for OTA
#include <HTTPClient.h>   //library for HTTP
#include <AccelStepper.h> //library for determining position of stepper motor
#include <EEPROM.h>       //library for memory non volatile

// WiFi credential
#define SSID "Shed45"
#define PASS "test12345"

// TB server credential
#define TB_SERVER "iotn.abc-server.id"
#define TB_TOKEN "XjsnxxhgwkOcCKIX7yPl"
#define SHED_ID "bdg045"

// motor pin definitions
#define DIR 32
#define STEP 33

// make object for motor stepper
AccelStepper stepper(1, STEP, DIR);

// initiate millis
unsigned long prevMillisWifi = 0;
unsigned long prevMillisSendSerial = 0;
unsigned long prevMillisAction = 0;

unsigned long delayWifi = 10000;      // connection check every 10 s
unsigned long delaySendSerial = 1000; // send serial every 1 s
unsigned long delayAction = 2000;     // motor response to work every 2 s

void setup()
{
  // serial communication for debugging
  Serial.begin(250000);

  // initiate memory non-volatile
  EEPROM.begin(512);

  // set motor pin mode as output
  pinMode(STEP, OUTPUT);
  pinMode(DIR, OUTPUT);

  // initiate work of motor pins
  digitalWrite(STEP, LOW);
  digitalWrite(DIR, LOW);

  // start connect to WiFi
  connectToWiFi();

  // OTA credentials
  ArduinoOTA.setHostname("bdg-045-aktuator-unit-02");
  ArduinoOTA.setPassword("admin");

  // initiate OTA
  ArduinoOTA.begin();

  // set max speed, acceleration, and set last percentage as current position
  stepper.setMaxSpeed(1500.0);
  stepper.setAcceleration(3000.0);
  stepper.setCurrentPosition(readValue());

  // debugging percentage
  //  Serial.println(percentCommand());
}

// function to connect to wifi
void connectToWiFi()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Menghubungkan ke WiFi..."); // debug WiFi state
    WiFi.begin(SSID, PASS);                     // connecting to WiFi
  }
}

// function to get data percentage of ventilation from TB (json package)
String getDataFromTB()
{
  // make object for http
  HTTPClient http;

  // API to get data from Thingsboard
  String url = "https://" + String(TB_SERVER) + "/api/v1/" + String(TB_TOKEN) + "/attributes?clientKeys=" + String(SHED_ID) + "_vent2";

  // start write http
  http.begin(url);

  // add header to http request
  http.addHeader("Content-Type", "application/json");

  // mode http GET
  int httpResponseCode = http.GET();

  // variable for payload
  String payload = "";

  // check http response
  if (httpResponseCode == HTTP_CODE_OK)
  {
    // get string from http response
    String payload = http.getString();
    return payload;
  }
  else
  {
    // code for debugging HTTP response code
    //  Serial.print("HTTP Error code: ");
    //  Serial.println(httpResponseCode);
    return "HTTP Error code: " + String(httpResponseCode);
  }

  // end http connection
  http.end();
}

// function to get exact percentage value from TB
String commandFromTB()
{
  // make object for parsed document
  DynamicJsonDocument doc(375);

  // variable for parsed document
  String result;

  // error handler
  DeserializationError error = deserializeJson(doc, getDataFromTB());

  // return result if not error
  if (!error)
  {
    result = doc["client"]["" + String(SHED_ID) + "_vent2"].as<String>();
  }
  return result;
}

// function to save percentage of position to memory EEPROM after get percentage value
void saveToMemory(int position)
{
  // save value to memory
  EEPROM.put(10, position);

  // commit value, if not committed the value will lost after reboot
  EEPROM.commit();
}

// function to read percentage value (this is useful to avoid device position become 0)
int readValue()
{
  int value;

  // read value
  EEPROM.get(10, value);
  return value;
}

// get final value of percentage from TB
int percentCommand()
{
  // convert percentage to actual linear value
  int position = commandFromTB().substring(0, commandFromTB().length() - 1).toInt() * 300;
  if (position != 0)
  {
    saveToMemory(position);
    return position;
  }
}

// set motor to desired position
void stepMotor(int position)
{
  stepper.setSpeed(200);
  stepper.moveTo(position);
  stepper.runToPosition();
}

// run motor
void movement()
{
  stepMotor(percentCommand());
}

void loop()
{
  unsigned long currentMillis = millis();

  // handle WiFi connection
  if (currentMillis - prevMillisWifi >= delayWifi)
  {
    connectToWiFi();
    prevMillisWifi = currentMillis;
  }

  // handle movement
  if (currentMillis - prevMillisAction >= delayAction)
  {
    percentCommand();

    //choose one of these functions:

    //function 1
    //  movement();

    //function 2
    stepper.setSpeed(200);
    stepper.moveTo(readValue());
    stepper.runToPosition();

    prevMillisAction = currentMillis;
  }

  // handle OTA task
  ArduinoOTA.handle();
}