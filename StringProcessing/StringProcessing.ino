char *b;
String a;
void setup() {
  a = "dsadas";
  a += "123";

  b = (char *)a.c_str();
  Serial.begin(9600);
}

void loop() {
  Serial.println(b);
  delay(500);
}
