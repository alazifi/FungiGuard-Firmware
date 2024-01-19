#include <Arduino.h>

#include "PinDefinitionsAndMore.h"

#define RAW_BUFFER_LENGTH 750

#define MARK_EXCESS_MICROS 0
#include <IRremote.hpp>

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
}

void loop() {
    decode_results results;  
  if (IrReceiver.decode()) {
    IrReceiver.printIRSendUsage(&Serial);
    Serial.println("Raw data:");
    IrReceiver.compensateAndPrintIRResultAsCArray(&Serial, true);
    IrReceiver.compensateAndPrintIRResultAsPronto(&Serial);
    Serial.println("==========================================================================================");
    IrReceiver.resume();
  }
}