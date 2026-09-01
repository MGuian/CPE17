#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

// Replace with the MAC address of the Receiver ESP32
uint8_t receiverAddress[6] = {0x24, 0x6F, 0x28, 0x12, 0x34, 0x56};

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