void setup() {
  // initialize LED_BUILTIN as an output pin.
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on
  Serial.println("HIGH");
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off
  Serial.println("LOW");
  delay(1000);
}
