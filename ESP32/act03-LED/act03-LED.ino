#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLE2902.h>

const int LED_PIN = 19;

#define SERVICE_UUID "0000FFE0-0000-1000-8000-00805F9B34FB"
#define CHAR_UUID    "0000FFE1-0000-1000-8000-00805F9B34FB"

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      String val = pCharacteristic->getValue();
      if (val.length() == 0) return;

      val.trim();
      val.toUpperCase();

      // Switch flipped ON on phone dashboard
      if (val.indexOf("T0:1") != -1 || val.indexOf("C:1") != -1 || val == "1") {
        digitalWrite(LED_PIN, HIGH);
        Serial.println("LED ON");
      } 
      // Switch flipped OFF on phone dashboard
      else if (val.indexOf("T0:0") != -1 || val.indexOf("C:0") != -1 || val == "0") {
        digitalWrite(LED_PIN, LOW);
        Serial.println("LED OFF");
      }
    }
};

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  BLEDevice::init("ESP32_LED");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pChar = pService->createCharacteristic(
    CHAR_UUID, 
    BLECharacteristic::PROPERTY_READ     | 
    BLECharacteristic::PROPERTY_WRITE    | 
    BLECharacteristic::PROPERTY_WRITE_NR | 
    BLECharacteristic::PROPERTY_NOTIFY
  );

  pChar->addDescriptor(new BLE2902());
  pChar->setCallbacks(new MyCallbacks());

  pService->start();
  BLEDevice::getAdvertising()->start();
  Serial.println("BLE Ready to pair!");
}

void loop() {
  delay(1000);
}