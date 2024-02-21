#include <Arduino.h>
#include <OneWire.h>           //library for One Wire
#include <DallasTemperature.h> //library for DS18B20 - Compose temperature
#include <DHT.h>               //library for DHT22 - Room temperature and humidity
#include <MHZ19.h>             //library for MHZ19 - CO2
#include <ArduinoJson.h>       //library for parsing data json
#include <WiFi.h>              //library for WiFi
#include <HTTPClient.h>        //library for HTTP
#include <ESPmDNS.h>           //library for OTA
#include <WiFiUdp.h>           //library for OTA
#include <ArduinoOTA.h>        //main library for OTA
#include <SoftwareSerial.h>    //library for software serial
#include <IRremote.hpp>        //library for IR remote
#include <DataRemote.h>        //library for all remote commands

// WiFi credential
#define SSID "Shed45"
#define PASS "test12345"

// TB server credential
#define TB_SERVER "iotn.abc-server.id"
#define TB_TOKEN "XjsnxxhgwkOcCKIX7yPl"
#define SHED_ID "bdg045"

// CO2 pin definitions
#define RX_CO2_PIN 16
#define TX_CO2_PIN 17
#define BAUDRATE 9600

// DS18B20 pin definition
#define DS18B20_PIN 25

// DHT22 configuration
#define DHT_PIN 26
#define DHT_TYPE DHT22

// RS485 mode control pin
#define RS485_ENABLE_PIN 14

// RS485 communication
#define RX_SERIAL_PIN 33
#define TX_SERIAL_PIN 32

// make object for serial RS485 communication
SoftwareSerial serialData(RX_SERIAL_PIN, TX_SERIAL_PIN);

// make object for DS18B20
OneWire oneWire(DS18B20_PIN);

// point DS18B20 sensor
DallasTemperature sensors(&oneWire);

// addresses of DS18B20s
DeviceAddress probe1 = {0x28, 0x63, 0x43, 0x80, 0xE3, 0xE1, 0x3D, 0x2B};
DeviceAddress probe2 = {0x28, 0x92, 0xE8, 0x80, 0xE3, 0xE1, 0x3D, 0x2D};
DeviceAddress probe3 = {0x28, 0xE0, 0x35, 0x80, 0xE3, 0xE1, 0x3D, 0x69};
DeviceAddress probe4 = {0x28, 0x6D, 0xFA, 0x80, 0xE3, 0xE1, 0x3D, 0x84};
DeviceAddress probe5 = {0x28, 0x49, 0xAB, 0x80, 0xE3, 0xE1, 0x3D, 0xB6};

// make object of DHT
DHT dht(DHT_PIN, DHT_TYPE);

// make object of MHZ19
MHZ19 myMHZ19;

// make object of IR sender
IRsend irsend;

// make object of IR commands
DataRemote remote;

// initiate millis
unsigned long prevMillisWifi = 0;
unsigned long prevMillisSendSerial = 0;
unsigned long prevMillisSendTB = 0;
unsigned long prevMillisUpdateAC = 0;

unsigned long delayWifi = 300000;     // connection check every 5 m
unsigned long delaySendSerial = 1000; // send serial setiap 1 detik
unsigned long delaySendTB = 600000;   // kirim ke TB setiap 10 menit
unsigned long delayUpdateAC = 60000;  // update AC setiap 1 menit

// variables for ACs
String prevdataAC3 = "";
String prevdataAC4 = "";
String lastUpdate = "";

void setup()
{
  // serial communication for debugging
  Serial.begin(250000);

  // serial communication between ESP32 and MHZ19
  Serial2.begin(BAUDRATE, SERIAL_8N1, RX_CO2_PIN, TX_CO2_PIN);

  // initiate MHZ19 reading
  myMHZ19.begin(Serial2);

  // set auto calibration for MHZ19 (may be irrelevant for extreme conditions, see documentation https://www.winsen-sensor.com/d/files/infrared-gas-sensor/mh-z19b-co2-ver1_0.pdf)
  myMHZ19.autoCalibration();

  // set range of MHZ19
  myMHZ19.setRange(10000);

  // initiate DS18B20 reading
  sensors.begin();

  // initiate DHT reading
  dht.begin();

  // serial communication for RS485
  // serialData.begin(250000);

  // set RS485 pin mode as output
  pinMode(RS485_ENABLE_PIN, OUTPUT);

  // initiate work of RS485 pins as high condition (transmitter mode)
  digitalWrite(RS485_ENABLE_PIN, HIGH);

  // start connect to WiFi
  connectToWiFi();

  // OTA credentials
  ArduinoOTA.setHostname("bdg-045-actuator-unit-02");
  ArduinoOTA.setPassword("admin");

  // initiate OTA
  ArduinoOTA.begin();
}

// function to get compose temperature, probe 1
float composeTemperature1()
{
  sensors.requestTemperatures();
  return sensors.getTempC(probe1);
}

// function to get compose temperature, probe 2
float composeTemperature2()
{
  sensors.requestTemperatures();
  return sensors.getTempC(probe2);
}

// function to get compose temperature, probe 3
float composeTemperature3()
{
  sensors.requestTemperatures();
  return sensors.getTempC(probe3);
}

// function to get compose temperature, probe 4
float composeTemperature4()
{
  sensors.requestTemperatures();
  return sensors.getTempC(probe4);
}

// function to get compose temperature, probe 5
float composeTemperature5()
{
  sensors.requestTemperatures();
  return sensors.getTempC(probe5);
}

// function to get room temperature
float roomTemperature()
{
  return dht.readTemperature();
}

// function to get humidity
float humidity()
{
  return dht.readHumidity();
}

// function to get carbon dioxide
int carbonDioxide()
{
  return myMHZ19.getCO2();
}

// function for assembling all data
String payloadData()
{
  // make object for serialized document
  DynamicJsonDocument doc(1024);

  // serialized documents as payload
  doc["" + String(SHED_ID) + "_comTemp6"] = String(composeTemperature1(), 2);
  doc["" + String(SHED_ID) + "_comTemp7"] = String(composeTemperature2(), 2);
  doc["" + String(SHED_ID) + "_comTemp8"] = String(composeTemperature3(), 2);
  doc["" + String(SHED_ID) + "_comTemp9"] = String(composeTemperature4(), 2);
  doc["" + String(SHED_ID) + "_comTemp10"] = String(composeTemperature5(), 2);
  doc["" + String(SHED_ID) + "_roomTemp2"] = String(roomTemperature(), 2);
  doc["" + String(SHED_ID) + "_humi2"] = String(humidity(), 2);
  doc["" + String(SHED_ID) + "_CO22"] = String(carbonDioxide());

  // variable for payload
  String payload;

  // put payload to variable
  serializeJson(doc, payload);

  // return the payload
  return payload;
}

//uncomment this if you set this sensor unit 2 to get phase of plantation
// function for getting plantation phase from Thingsboard
String statusFromTB()
{
  // make object for parsed document
  DynamicJsonDocument doc(375);

  // variable for plantation phase
  String status = "";

  // error handler
  DeserializationError error = deserializeJson(doc, getDataFromTB());

  // return result if not error
  if (!error)
  {
    status = doc["client"]["" + String(SHED_ID) + "_fase"].as<String>();
  }
  return status;
}

//uncomment this if you set this sensor unit 2 to get phase of plantation
// function for assembling all data (phase + sensors)
// String enrichedData()
// {
//   // take all sensor data
//   String sensorData = payloadData();

//   // sensor data formatting
//   sensorData.remove(0, 1);
//   sensorData.remove(sensorData.length() - 1);

//   // combine sensor data and phase
//   String payload = "{" + sensorData + ", \"fase\":\"" + String(statusFromTB()) + "\"}";
//   return payload;
// }

// function to connect to wifi
void connectToWiFi()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Menghubungkan ke WiFi...");
    WiFi.begin(SSID, PASS);
  }
}

void sendDataToTelemetryTB()
{
  HTTPClient http;
  String url = "https://" + String(TB_SERVER) + "/api/v1/" + String(TB_TOKEN) + "/telemetry";
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(String(payloadData()));
  // debug data disini
  //  if (httpResponseCode > 0) {
  //    Serial.print("HTTP Response code: ");
  //    Serial.println(httpResponseCode);
  //  } else {
  //    Serial.print("HTTP Error code: ");
  //    Serial.println(httpResponseCode);
  //  }
  http.end();
}

// function to send sensors data to Thingsboard Attribute
void sendDataToAttributeTB()
{
  // make object for http
  HTTPClient http;

  // API to send data to Thingsboard Attribute
  String url = "https://" + String(TB_SERVER) + "/api/v1/" + String(TB_TOKEN) + "/attributes";
    
  // start write http
  http.begin(url);

  // add header to http request
  http.addHeader("Content-Type", "application/json");

  // mode http post with sensors data as payload
  int httpResponseCode = http.POST(String(payloadData()));

  // code for debugging HTTP response code
  //  if (httpResponseCode > 0) {
  //    Serial.print("HTTP Response code: ");
  //    Serial.println(httpResponseCode);
  //  } else {
  //    Serial.print("HTTP Error code: ");
  //    Serial.println(httpResponseCode);
  //  }

  // end http connection
  http.end();
}

//function to get mode AC data from Thingsboard
String getDataFromTB()
{
  // make object for http
  HTTPClient http;

  // API to get data from Thingsboard
  String url = "https://" + String(TB_SERVER) + "/api/v1/" + String(TB_TOKEN) + "/attributes?clientKeys=" + String(SHED_ID) + "_modeAC3," + String(SHED_ID) + "_modeAC4";
  
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

String commandFromTB()
{
  //make object for parsed document
  DynamicJsonDocument doc(375);

  //variable for setting AC mode
  String dataAC3 = "";
  String dataAC4 = "";
  String result;

  //error handler
  DeserializationError error = deserializeJson(doc, getDataFromTB());

  // return result if not error
  if (!error)
  {
    //AC mode
    dataAC3 = doc["client"]["" + String(SHED_ID) + "_modeAC3"].as<String>();
    dataAC4 = doc["client"]["" + String(SHED_ID) + "_modeAC4"].as<String>();

    //check if there is a change in AC mode
    if (dataAC3 != prevdataAC3)
    {
      prevdataAC3 = dataAC3;
      lastUpdate = "modeAC3";
    }
    else if (dataAC4 != prevdataAC4)
    {
      prevdataAC4 = dataAC4;
      lastUpdate = "modeAC4";
    }

    //set AC mode to the latest setting
    if (lastUpdate == "modeAC3")
    {
      result = prevdataAC3;
    }
    else if (lastUpdate == "modeAC4")
    {
      result = prevdataAC4;
    }
  }
  return result;
}

//function to determine which AC device to be controlled
String deviceToControl()
{
  String device = commandFromTB();
  return device.substring(0, 4);
}

//function to set AC mode to the determined device
String modeControl()
{
  String mode = commandFromTB();
  return mode.substring(5);
}

//function to send command of AC mode to Ac device using IR emitter
void controlAC()
{
  int irPin;
  
  //define pin of IR emitter that will control AC
  const int IR_PIN_1 = 18;
  const int IR_PIN_2 = 19;

  //assign determined device to IR emitter pin
  if (deviceToControl() == "ac03")
  {
    irPin = IR_PIN_1;
  }
  else if (deviceToControl() == "ac04")
  {
    irPin = IR_PIN_2;
  }
  else
  {
    irPin = 0;
  }

  //set IR emitter pin to send IR data
  IrSender.setSendPin(irPin);

  //get mode AC and assign it to relevant pronto hex in library DataRemote.h 
  remote.dataInput(modeControl());

  //get pronto hex from library DataRemote.h, and send it to AC device via IR emitter
  irsend.sendPronto(F(remote.getProntoData()), 3U);

  //unassign pin IR emitter, if this line undefined, the previous pin will always work
  ledcDetachPin(IrSender.sendPin);

  //debug for pronto hex code
  // Serial.println(irPin);
  // Serial.println(F(remote.getProntoData()));
}

//function to send data sensors data + plantation phase to serial (RS485/serial monitor)
void sendToSerial()
{
  //uncomment this to send sensors data + plantation phase to rs485
  serialData.println(enrichedData());
  serialData.flush();

  //uncomment this to send sensors data + plantation phase to serial monitor (debugging)
  // Serial.println(enrichedData());
  // Serial.flush();
}

void loop()
{
  unsigned long currentMillis = millis();

  //handle WiFi connection
  if (currentMillis - prevMillisWifi >= delayWifi)
  {
    connectToWiFi();
    prevMillisWifi = currentMillis;
  }

  //handle sending data to serial RS485 (or serial monitor) and Thingsboard attribute
  if (currentMillis - prevMillisSendSerial >= delaySendSerial)
  {
    sendToSerial();
    sendDataToAttributeTB();
    prevMillisSendSerial = currentMillis;
  }

  //handle sending data to Thingsboard Telemetry
  if (currentMillis - prevMillisSendTB >= delaySendTB)
  {
    sendDataToTelemetryTB();
    prevMillisSendTB = currentMillis;
  }

  //handle setting for AC mode
  if (currentMillis - prevMillisUpdateAC >= delayUpdateAC)
  {
    controlAC();

    // uncomment this to see the pronto hex code
    // Serial.println(modeControl());
    prevMillisUpdateAC = currentMillis;
  }

  //handle OTA task
  ArduinoOTA.handle();
}