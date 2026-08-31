// Setup
int ledPin = 19; // Choose any PWM-capable GPIO
int channel = 0; // 0-15 (ESP32 supports 16 PWM channels)
int freq = 5000; // PWM frequency in Hz
int resolution = 8; // 8-bit resolution (0-255 duty cycle)
int dutyCycle = 0;

void setup() {
    ledcAttachChannel(ledPin, freq, resolution, channel);
    Serial.begin(115200);
    //ledcSetup(channel, freq, resolution);  // Configure PWM channel
    //ledcAttachPin(ledPin, channel);       // Attach pin to channel
}

void loop() {
    while (dutyCycle < 255){
        ledcWrite(ledPin, dutyCycle);
        Serial.print("Forward with duty cycle: ");
        Serial.println(dutyCycle);
        dutyCycle = dutyCycle + 5;
        delay(500);
    }
    while (dutyCycle > 0){
        ledcWrite(ledPin, dutyCycle);
        Serial.print("Forward with duty cycle: ");
        Serial.println(dutyCycle);
        dutyCycle = dutyCycle - 5;
        delay(500);
    }
}