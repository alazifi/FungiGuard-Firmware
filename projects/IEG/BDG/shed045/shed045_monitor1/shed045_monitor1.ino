#include <Arduino.h>
#include <ArduinoJson.h>       //library for parsing data json
#include <LiquidCrystal_I2C.h> //library for LCD I2C
#include <SoftwareSerial.h>    //library for software serial

#define EN_PIN_1 18      // mode control pin for RS485 use serial2
#define EN_PIN_2 19      // mode control pin for RS485 use softwareserial
#define SHED_ID "bdg045" // id for shed

#define RX_SEC 33 // pin for RS485 use softwareserial
#define TX_SEC 32 // pin for RS485 use softwareserial

// make object for software serial
SoftwareSerial secSerial(RX_SEC, TX_SEC);

// make object for LCD I2C
LiquidCrystal_I2C lcd(0x26, 20, 4);
LiquidCrystal_I2C lcd2(0x27, 20, 4);

// make object for parsed document
DynamicJsonDocument doc1(512);
DynamicJsonDocument doc2(512);

// initiate millis
unsigned long prevMillisPrintLCD = 0;
unsigned long prevMillisDebug = 0;

unsigned long delayPrintLCD = 1000; // print LCD every 1 s
unsigned long delayDebug = 1000;    // print serial monitor every 1 s

// variables definitions
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
  // serial communication RS485 on serial2
  Serial2.begin(115200);

  // serial communication RS485 on softwareserial
  secSerial.begin(250000);

  // serial communication for debugging
  Serial.begin(500000);

  // set RS485 pin mode as output
  pinMode(EN_PIN_1, OUTPUT);
  pinMode(EN_PIN_2, OUTPUT);

  // initiate work of RS485 pins as low condition (receiver)
  digitalWrite(EN_PIN_1, LOW);
  digitalWrite(EN_PIN_2, LOW);

  // initiate LCD 1 work
  lcd.init();
  lcd.backlight();
  lcd.clear();

  // initiate LCD 2 work
  lcd2.init();
  lcd2.backlight();
  lcd2.clear();
}

// function for getting compose temperature 1
String composeTemperature1()
{
  String result = lastComTemp1;
  if (doc1.containsKey("" + String(SHED_ID) + "_comTemp1"))
  {
    result = doc1["" + String(SHED_ID) + "_comTemp1"].as<String>();
    lastComTemp1 = result;
  }
  return result;
}

// function for getting compose temperature 2
String composeTemperature2()
{
  String result = lastComTemp2;
  if (doc1.containsKey("" + String(SHED_ID) + "_comTemp2"))
  {
    result = doc1["" + String(SHED_ID) + "_comTemp2"].as<String>();
    lastComTemp2 = result;
  }
  return result;
}

// function for getting compose temperature 3
String composeTemperature3()
{
  String result = lastComTemp3;
  if (doc1.containsKey("" + String(SHED_ID) + "_comTemp3"))
  {
    result = doc1["" + String(SHED_ID) + "_comTemp3"].as<String>();
    lastComTemp3 = result;
  }
  return result;
}

// function for getting compose temperature 4
String composeTemperature4()
{
  String result = lastComTemp4;
  if (doc1.containsKey("" + String(SHED_ID) + "_comTemp4"))
  {
    result = doc1["" + String(SHED_ID) + "_comTemp4"].as<String>();
    lastComTemp4 = result;
  }
  return result;
}

// function for getting compose temperature 5
String composeTemperature5()
{
  String result = lastComTemp5;
  if (doc1.containsKey("" + String(SHED_ID) + "_comTemp5"))
  {
    result = doc1["" + String(SHED_ID) + "_comTemp5"].as<String>();
    lastComTemp5 = result;
  }
  return result;
}

// function for getting room temperature 1
String roomTemperature1()
{
  String result = lastRoomTemp1;
  if (doc1.containsKey("" + String(SHED_ID) + "_roomTemp1"))
  {
    result = doc1["" + String(SHED_ID) + "_roomTemp1"].as<String>();
    lastRoomTemp1 = result;
  }
  return result;
}

// function for getting humidity 1
String humidity1()
{
  String result = lastHumi1;
  if (doc1.containsKey("" + String(SHED_ID) + "_humi1"))
  {
    result = doc1["" + String(SHED_ID) + "_humi1"].as<String>();
    lastHumi1 = result;
  }
  return result;
}

// function for getting carbon dioxide 1
String carbonDioxide1()
{
  String result = lastCo21;
  if (doc1.containsKey("" + String(SHED_ID) + "_CO21"))
  {
    result = doc1["" + String(SHED_ID) + "_CO21"].as<String>();
    lastCo21 = result;
  }
  return result;
}

// function for getting plantation phase
String phase()
{
  String result = lastPhase;
  if (doc1.containsKey("fase"))
  {
    result = doc1["fase"].as<String>();
    lastPhase = result;
  }
  return result;
}

// function for formatting plantation phase
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

// function for getting compose temperature 6
String composeTemperature6()
{
  String result = lastComTemp6;
  if (doc2.containsKey("" + String(SHED_ID) + "_comTemp6"))
  {
    result = doc2["" + String(SHED_ID) + "_comTemp6"].as<String>();
    lastComTemp6 = result;
  }
  return result;
}

// function for getting compose temperature 7
String composeTemperature7()
{
  String result = lastComTemp7;
  if (doc2.containsKey("" + String(SHED_ID) + "_comTemp7"))
  {
    result = doc2["" + String(SHED_ID) + "_comTemp7"].as<String>();
    lastComTemp7 = result;
  }
  return result;
}

// function for getting compose temperature 8
String composeTemperature8()
{
  String result = lastComTemp8;
  if (doc2.containsKey("" + String(SHED_ID) + "_comTemp8"))
  {
    result = doc2["" + String(SHED_ID) + "_comTemp8"].as<String>();
    lastComTemp8 = result;
  }
  return result;
}

// function for getting compose temperature 9
String composeTemperature9()
{
  String result = lastComTemp9;
  if (doc2.containsKey("" + String(SHED_ID) + "_comTemp9"))
  {
    result = doc2["" + String(SHED_ID) + "_comTemp9"].as<String>();
    lastComTemp9 = result;
  }
  return result;
}

// function for getting compose temperature 10
String composeTemperature10()
{
  String result = lastComTemp10;
  if (doc2.containsKey("" + String(SHED_ID) + "_comTemp10"))
  {
    result = doc2["" + String(SHED_ID) + "_comTemp10"].as<String>();
    lastComTemp10 = result;
  }
  return result;
}

// function for getting room temperature 2
String roomTemperature2()
{
  String result = lastRoomTemp2;
  if (doc2.containsKey("" + String(SHED_ID) + "_roomTemp2"))
  {
    result = doc2["" + String(SHED_ID) + "_roomTemp2"].as<String>();
    lastRoomTemp2 = result;
  }
  return result;
}

// function for getting humidity 2
String humidity2()
{
  String result = lastHumi2;
  if (doc2.containsKey("" + String(SHED_ID) + "_humi2"))
  {
    result = doc2["" + String(SHED_ID) + "_humi2"].as<String>();
    lastHumi2 = result;
  }
  return result;
}

// function for getting carbon dioxide 2
String carbonDioxide2()
{
  String result = lastCo22;
  if (doc2.containsKey("" + String(SHED_ID) + "_CO22"))
  {
    result = doc2["" + String(SHED_ID) + "_CO22"].as<String>();
    lastCo22 = result;
  }
  return result;
}

// function for display value to LCD
void printLCD()
{
  // print to LCD 1
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

  // print to LCD 2
  lcd2.setCursor(0, 0);
  lcd2.print("K1:" + String(composeTemperature1()) + "C ");

  lcd2.setCursor(0, 1);
  lcd2.print("K2:" + String(composeTemperature2()) + "C ");

  lcd2.setCursor(0, 2);
  lcd2.print("K3:" + String(composeTemperature3()) + "C ");

  lcd2.setCursor(0, 3);
  lcd2.print("K4:" + String(composeTemperature4()) + "C ");

  // lcd2.setCursor(0, 3);
  // lcd2.print("K5:" + String(composeTemperature5()) + "C ");

  lcd2.setCursor(10, 0);
  lcd2.print("K6:" + String(composeTemperature6()) + "C ");

  lcd2.setCursor(10, 1);
  lcd2.print("K7:" + String(composeTemperature7()) + "C ");

  lcd2.setCursor(10, 2);
  lcd2.print("K8:" + String(composeTemperature8()) + "C ");

  lcd2.setCursor(10, 3);
  lcd2.print("K9:" + String(composeTemperature9()) + "C ");

  // lcd2.setCursor(10, 3);
  // lcd2.print("K10:" + String(composeTemperature10()) + "C ");
}

void loop()
{
  String receivedData1;
  String receivedData2;
  String id = "";

  // check data on serial2
  if (Serial2.available() > 0)
  {
    receivedData1 = Serial2.readString();
    DeserializationError error1 = deserializeJson(doc1, receivedData1);
  }

  // check data on softwareserial
  if (secSerial.available() > 0)
  {
    receivedData2 = secSerial.readString();
    DeserializationError error2 = deserializeJson(doc2, receivedData2);
  }

  unsigned long currentMillis = millis();

  if (currentMillis - prevMillisPrintLCD >= delayPrintLCD)
  {
    printLCD();
    prevMillisPrintLCD = currentMillis;
  }

  if (currentMillis - prevMillisDebug >= delayDebug)
  {
    // print as parsed value
    Serial.println("01. " + String(composeTemperature1()) + "\t02." + String(composeTemperature2()) + "\t03." + String(composeTemperature3()) + "\t04." + String(composeTemperature4()) + "\t05." + String(composeTemperature5()) + "\t06." + String(roomTemperature1()) + "\t07." + String(humidity1()) + "\t08." + String(carbonDioxide1()) + "\t9. " + String(composeTemperature6()) + "\t10." + String(composeTemperature7()) + "\t11." + String(composeTemperature8()) + "\t12." + String(composeTemperature9()) + "\t13." + String(composeTemperature10()) + "\t14." + String(roomTemperature2()) + "\t15." + String(humidity2()) + "\t16." + String(carbonDioxide2()));

    // print as json package
    //  Serial.println(receivedData1);
    //  Serial.println(receivedData2);
    prevMillisDebug = currentMillis;
  }
}
