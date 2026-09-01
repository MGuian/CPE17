#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

const int LED_PIN = 2; // Built-in LED

void setup() {
    Serial.begin(115200);

    // Initialize Bluetooth in Slave mode
    SerialBT.begin("ESP32_LED_Receiver");
    Serial.println("Receiver ready. Waiting for Master...");

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
}

void loop() {
    if (SerialBT.available()) {
        char receivedChar = SerialBT.read();

        if (receivedChar == '1') {
            digitalWrite(LED_PIN, HIGH);
            Serial.println("LED ON");
        }
        else if (receivedChar == '0') {
            digitalWrite(LED_PIN, LOW);
            Serial.println("LED OFF");
        }
    }
}