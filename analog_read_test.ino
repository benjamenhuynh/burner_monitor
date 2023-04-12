void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
  
}

void loop() {
  int x = analogRead(A0);
  float y = (float)x * (5.0/1024);
  Serial.print("Analog Reading: ");
  Serial.println(x);
  Serial.print("Voltage Reading: ");
  Serial.print(y);
  Serial.println("V");
  delay(3000);
}
