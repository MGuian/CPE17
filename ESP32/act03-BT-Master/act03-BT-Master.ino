#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

// Replace with the MAC address of the Receiver ESP32
uint8_t receiverAddress[6] = {0xE0, 0x8C, 0xFE, 0x18, 0xD4, 0x46};

void setup() {
    Serial.begin(115200);
    // Initialize Master mode
    SerialBT.begin("ESP32_Master", true);
    Serial.println("Master initialized, connecting...");
    if (SerialBT.connect(receiverAddress)) {
        Serial.println("Connected to Receiver ESP32!");
    } else {
        Serial.println("Failed to connect. Retrying...");
    }
}

void loop() {
    if (SerialBT.connected()) {
        SerialBT.println("1"); // LED ON
        Serial.println("Sent: 1");
        delay(3000);

        SerialBT.println("0"); // LED OFF
        Serial.println("Sent: 0");
        delay(3000);
    } else {
        Serial.println("Not connected...");
        delay(2000);
    }
}