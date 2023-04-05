int sensor_in = 9;
int i = 0;

void setup() {
  Serial.begin(9600);
  pinMode(sensor_in, INPUT_PULLUP);//vjj

}

void loop() {
  int j = digitalRead(sensor_in);
  if(i != j)
    Serial.println(j);
  i =j;
}
