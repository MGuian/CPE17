void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    pinMode(19, OUTPUT);
    Serial.println("Hello, ESP32!");
}

void loop() {
    // put your main code here, to run repeatedly:
    if(Serial.available()>0){
        char cmd = Serial.read();

        Serial.print("Input received: ");
        Serial.println(cmd);

        if(cmd == '1'){
            digitalWrite(19, HIGH);
            Serial.println("LED ON");
        }

        if(cmd == '0'){
            digitalWrite(19, LOW);
            Serial.println("LED OFF");
        }
    }
}