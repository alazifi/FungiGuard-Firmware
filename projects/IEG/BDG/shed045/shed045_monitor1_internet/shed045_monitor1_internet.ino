#include <Arduino.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <SoftwareSerial.h>

#define EN_PIN_1 18 // serial2
#define EN_PIN_2 19 // softwareserial

#define SSID "Shed45"
#define PASS "test12345"

#define TB_SERVER "iotn.abc-server.id"
#define TB_TOKEN "XjsnxxhgwkOcCKIX7yPl"
#define SHED_ID "bdg045"

#define RX_SEC 33
#define TX_SEC 32

SoftwareSerial secSerial(RX_SEC, TX_SEC);

LiquidCrystal_I2C lcd(0x26, 20, 4);
LiquidCrystal_I2C lcd2(0x27, 20, 4);

DynamicJsonDocument doc1(512);
DynamicJsonDocument doc2(512);

unsigned long prevMillisWifi = 0;
// unsigned long prevMillisSendSerial = 0;
unsigned long prevMillisGetTB = 0;

unsigned long delayWifi = 10000; // cek wifi setiap 5 menit
// unsigned long delaySendSerial = 1000;  // send serial setiap 10 detik
unsigned long delayGetTB = 1000; // kirim ke TB setiap 10 menit

String lastComTemp1 = "";
String lastComTemp2 = "";
String lastComTemp3 = "";
String lastComTemp4 = "";
String lastComTemp5 = "";
String lastRoomTemp1 = "";
String lastHumi1 = "";
String lastCo21 = "";
String lastComTemp6 = "";
String lastComTemp7 = "";
String lastComTemp8 = "";
String lastComTemp9 = "";
String lastComTemp10 = "";
String lastRoomTemp2 = "";
String lastHumi2 = "";
String lastCo22 = "";
String lastPhase = "";

void setup()
{
  // Serial2.begin(115200);
  // secSerial.begin(250000);
  Serial.begin(500000);

  pinMode(EN_PIN_1, OUTPUT);
  pinMode(EN_PIN_2, OUTPUT);

  digitalWrite(EN_PIN_1, LOW);
  digitalWrite(EN_PIN_2, LOW);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  connectToWiFi();

  lcd2.init();
  lcd2.backlight();
  lcd2.clear();
  ArduinoOTA.setHostname("bdg-045-monitor-unit-01");
  ArduinoOTA.setPassword("admin");
  ArduinoOTA.begin();
}

void connectToWiFi()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Menghubungkan ke WiFi...");
    WiFi.begin(SSID, PASS);
  }
}

String composeTemperature1()
{
  String result = lastComTemp1;
  if (doc1.containsKey("client"))
  {
    result = doc1["client"]["" + String(SHED_ID) + "_comTemp1"].as<String>();
    lastComTemp1 = result;
  }
  return result;
}

String composeTemperature2()
{
  String result = lastComTemp2;
  if (doc1.containsKey("client"))
  {
    result = doc1["client"]["" + String(SHED_ID) + "_comTemp2"].as<String>();
    lastComTemp2 = result;
  }
  return result;
}

String composeTemperature3()
{
  String result = lastComTemp3;
  if (doc1.containsKey("client"))
  {
    result = doc1["client"]["" + String(SHED_ID) + "_comTemp3"].as<String>();
    lastComTemp3 = result;
  }
  return result;
}

String composeTemperature4()
{
  String result = lastComTemp4;
  if (doc1.containsKey("client"))
  {
    result = doc1["client"]["" + String(SHED_ID) + "_comTemp4"].as<String>();
    lastComTemp4 = result;
  }
  return result;
}

String composeTemperature5()
{
  String result = lastComTemp5;
  if (doc1.containsKey("client"))
  {
    result = doc1["client"]["" + String(SHED_ID) + "_comTemp5"].as<String>();
    lastComTemp5 = result;
  }
  return result;
}

String roomTemperature1()
{
  String result = lastRoomTemp1;
  if (doc1.containsKey("client"))
  {
    result = doc1["client"]["" + String(SHED_ID) + "_roomTemp1"].as<String>();
    lastRoomTemp1 = result;
  }
  return result;
}

String humidity1()
{
  String result = lastHumi1;
  if (doc1.containsKey("client"))
  {
    result = doc1["client"]["" + String(SHED_ID) + "_humi1"].as<String>();
    lastHumi1 = result;
  }
  return result;
}

String carbonDioxide1()
{
  String result = lastCo21;
  if (doc1.containsKey("client"))
  {
    result = doc1["client"]["" + String(SHED_ID) + "_CO21"].as<String>();
    lastCo21 = result;
  }
  return result;
}

String phase()
{
  String result = lastPhase;
  if (doc1.containsKey("client"))
  {
    result = doc1["client"]["" + String(SHED_ID) + "_fase"].as<String>();
    lastPhase = result;
  }
  return result;
}

String paddedPhase()
{
  String result = "";
  int pad = (20 - phase().length());
  int lpad = pad / 2;
  int rpad = pad - lpad;

  for (int i = 0; i < lpad; i++)
  {
    result += " ";
  }

  result += phase();

  for (int i = 0; i < rpad; i++)
  {
    result += " ";
  }
  return result;
}

String composeTemperature6()
{
  String result = lastComTemp6;
  if (doc2.containsKey("client"))
  {
    result = doc2["client"]["" + String(SHED_ID) + "_comTemp6"].as<String>();
    lastComTemp6 = result;
  }
  return result;
}

String composeTemperature7()
{
  String result = lastComTemp7;
  if (doc2.containsKey("client"))
  {
    result = doc2["client"]["" + String(SHED_ID) + "_comTemp7"].as<String>();
    lastComTemp7 = result;
  }
  return result;
}

String composeTemperature8()
{
  String result = lastComTemp8;
  if (doc2.containsKey("client"))
  {
    result = doc2["client"]["" + String(SHED_ID) + "_comTemp8"].as<String>();
    lastComTemp8 = result;
  }
  return result;
}

String composeTemperature9()
{
  String result = lastComTemp9;
  if (doc2.containsKey("client"))
  {
    result = doc2["client"]["" + String(SHED_ID) + "_comTemp9"].as<String>();
    lastComTemp9 = result;
  }
  return result;
}

String composeTemperature10()
{
  String result = lastComTemp10;
  if (doc2.containsKey("client"))
  {
    result = doc2["client"]["" + String(SHED_ID) + "_comTemp10"].as<String>();
    lastComTemp10 = result;
  }
  return result;
}

String roomTemperature2()
{
  String result = lastRoomTemp2;
  if (doc2.containsKey("client"))
  {
    result = doc2["client"]["" + String(SHED_ID) + "_roomTemp2"].as<String>();
    lastRoomTemp2 = result;
  }
  return result;
}

String humidity2()
{
  String result = lastHumi2;
  if (doc2.containsKey("client"))
  {
    result = doc2["client"]["" + String(SHED_ID) + "_humi2"].as<String>();
    lastHumi2 = result;
  }
  return result;
}

String carbonDioxide2()
{
  String result = lastCo22;
  if (doc2.containsKey("client"))
  {
    result = doc2["client"]["" + String(SHED_ID) + "_CO22"].as<String>();
    lastCo22 = result;
  }
  return result;
}

String getDataFromTB1()
{
  HTTPClient http;
  String url = "https://" + String(TB_SERVER) + "/api/v1/" + String(TB_TOKEN) + "/attributes?clientKeys=" + String(SHED_ID) + "_comTemp1," + String(SHED_ID) + "_comTemp2," + String(SHED_ID) + "_comTemp3," + String(SHED_ID) + "_comTemp4," + String(SHED_ID) + "_comTemp5," + String(SHED_ID) + "_roomTemp1," + String(SHED_ID) + "_humi1," + String(SHED_ID) + "_CO21," + String(SHED_ID) + "_fase";
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.GET();
  String payload = "";
  if (httpResponseCode == HTTP_CODE_OK)
  {
    String payload = http.getString();
    return payload;
  }
  else
  {
    // Serial.print("HTTP Error code: ");
    // Serial.println(httpResponseCode);
    return "HTTP Error code: " + String(httpResponseCode);
    // return url;
  }
  http.end();
}

String getDataFromTB2()
{
  HTTPClient http;
  String url = "https://" + String(TB_SERVER) + "/api/v1/" + String(TB_TOKEN) + "/attributes?clientKeys=" + String(SHED_ID) + "_comTemp6," + String(SHED_ID) + "_comTemp7," + String(SHED_ID) + "_comTemp8," + String(SHED_ID) + "_comTemp9," + String(SHED_ID) + "_comTemp10," + String(SHED_ID) + "_roomTemp2," + String(SHED_ID) + "_humi2," + String(SHED_ID) + "_CO22";
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.GET();
  String payload = "";
  if (httpResponseCode == HTTP_CODE_OK)
  {
    String payload = http.getString();
    return payload;
  }
  else
  {
    // Serial.print("HTTP Error code: ");
    // Serial.println(httpResponseCode);
    return "HTTP Error code: " + String(httpResponseCode);
  }
  http.end();
}

void printLCD()
{
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(paddedPhase());

  lcd.setCursor(0, 1);
  lcd.print("R1:" + String(roomTemperature1()) + "C");

  lcd.setCursor(0, 2);
  lcd.print("H1:" + String(humidity1()) + "%");

  lcd.setCursor(0, 3);
  lcd.print("C1:" + String(carbonDioxide1()) + "pm ");

  lcd.setCursor(10, 1);
  lcd.print("R2:" + String(roomTemperature2()) + "C");

  lcd.setCursor(10, 2);
  lcd.print("H2:" + String(humidity2()) + "%");

  lcd.setCursor(10, 3);
  lcd.print("C2:" + String(carbonDioxide2()) + "pm ");
  //=====================
  lcd2.setCursor(0, 0);
  lcd2.print("K1:" + String(composeTemperature1()) + "C ");

  lcd2.setCursor(0, 1);
  lcd2.print("K2:" + String(composeTemperature2()) + "C ");

  lcd2.setCursor(0, 2);
  lcd2.print("K3:" + String(composeTemperature3()) + "C ");

  lcd2.setCursor(0, 3);
  lcd2.print("K4:" + String(composeTemperature4()) + "C ");

  lcd2.setCursor(10, 0);
  lcd2.print("K6:" + String(composeTemperature6()) + "C ");

  lcd2.setCursor(10, 1);
  lcd2.print("K7:" + String(composeTemperature7()) + "C ");

  lcd2.setCursor(10, 2);
  lcd2.print("K8:" + String(composeTemperature8()) + "C ");

  lcd2.setCursor(10, 3);
  lcd2.print("K9:" + String(composeTemperature9()) + "C ");
}

void loop()
{
  // DynamicJsonDocument doc(512);

  String receivedData1;
  String receivedData2;
  String id = "";

  DeserializationError error1 = deserializeJson(doc1, getDataFromTB1());
  DeserializationError error2 = deserializeJson(doc2, getDataFromTB2());

  // if (Serial2.available() > 0) {
  //   receivedData1 = Serial2.readString();
  //   DeserializationError error1 = deserializeJson(doc1, receivedData1);
  // }

  // if (secSerial.available() > 0) {
  //   receivedData2 = secSerial.readString();
  //   DeserializationError error2 = deserializeJson(doc2, receivedData2);
  // }

  unsigned long currentMillis = millis();

  if (currentMillis - prevMillisWifi >= delayWifi)
  {
    connectToWiFi();
    prevMillisWifi = currentMillis;
  }

  if (currentMillis - prevMillisGetTB >= delayGetTB)
  {
    //  Serial.println("01. " + String(composeTemperature1()) + "\t02."+ String(composeTemperature2()) + "\t03." + String(composeTemperature3()) + "\t04." + String(composeTemperature4()) + "\t05." + String(composeTemperature5()) + "\t06." + String(roomTemperature1()) + "\t07." + String(humidity1()) + "\t08." + String(carbonDioxide1()) + "\t9. " + String(composeTemperature6()) + "\t10."+ String(composeTemperature7()) + "\t11." + String(composeTemperature8()) + "\t12." + String(composeTemperature9()) + "\t13." + String(composeTemperature10()) + "\t14." + String(roomTemperature2()) + "\t15." + String(humidity2()) + "\t16." + String(carbonDioxide2()));

    printLCD();
    prevMillisGetTB = currentMillis;
  }

  ArduinoOTA.handle();
}
