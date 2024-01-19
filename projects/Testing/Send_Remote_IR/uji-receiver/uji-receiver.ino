#include <IRremote.h>

int IR_RECEIVE_PIN = 25;

IRrecv IrReceiver(IR_RECEIVE_PIN);

void setup() {
    Serial.begin(115200);
    Serial.println("Enabling IRin");
    IrReceiver.enableIRIn();
    // IrReceiver.blink13(true);

    Serial.print(F("Ready to receive IR signals at pin "));
    Serial.println(IR_RECEIVE_PIN);
}

void loop() {
    if (IrReceiver.decode()) {
        IrReceiver.printResultShort(&Serial);
        Serial.println();

        IrReceiver.resume();
    }
    delay(100);
}
