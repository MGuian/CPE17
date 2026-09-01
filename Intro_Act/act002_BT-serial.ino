#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

const int LED_PIN = 2; // Built-in LED on most ESP32 boards

void setup() {
    // Start Serial Monitor
    Serial.begin(115200);
    // Start Bluetooth with a device name
    SerialBT.begin("ESP32_LED_Control");
    Serial.println("Bluetooth Device is Ready to Pair");
    // Set LED pin as output
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
}

void loop() {
    // Check if data is received via Bluetooth
    if (SerialBT.available()) {
        char receivedChar = SerialBT.read();

        if (receivedChar == '1') {
            digitalWrite(LED_PIN, HIGH);
            Serial.println("LED ON");
            SerialBT.println("LED is ON");
        }
        else if (receivedChar == '0') {
                digitalWrite(LED_PIN, LOW);
                Serial.println("LED OFF");
                SerialBT.println("LED is OFF");
        }
    }
}