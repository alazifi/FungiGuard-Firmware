#include <Arduino.h>
#include <IRremote.hpp>
#include <DataRemote.h>

DataRemote dataRemote;

#define NUMBER_OF_REPEATS 3U

IRsend irsend;

void setup() {
  Serial.begin(115200);
  IrSender.begin(27);           // Start with IR_SEND_PIN as send pin and enable feedback LED at default feedback LED pin
}

void loop() {
//24 cool low on
    dataRemote.dataInput("18 COOL LOW ON");
    // irsend.sendPronto(F(dataRemote.getProntoData()), NUMBER_OF_REPEATS);
    irsend.sendPronto(dataRemote.getProntoData(), 0);
    delay(500);
}
