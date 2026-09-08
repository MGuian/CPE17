#define POT_PIN 32
#define LED_PIN 19

void setup() {
  // Set ADC attenuation for the potentiometer pin
  analogSetAttenuation(ADC_11db);

  // Configure LED PWM
  // GPIO 19, 5 kHz frequency, 8-bit resolution
  ledcAttach(LED_PIN, 5000, 8);

  Serial.begin(115200);
}

void loop() {
  // Read potentiometer
  int potValue = analogRead(POT_PIN);

  // Convert ADC value (0-4095) to PWM value (0-255)
  int pwmValue = map(potValue, 0, 4095, 0, 255);

  // Set LED brightness
  ledcWrite(LED_PIN, pwmValue);

  // Display values
  Serial.print("Potentiometer: ");
  Serial.print(potValue);
  Serial.print(" | PWM Brightness: ");
  Serial.println(pwmValue);

  delay(20);
}