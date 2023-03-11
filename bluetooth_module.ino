char val; // variable to receive data from the serial port
int ledpin = 8; //LED connected to pin 8

int sensorpin = 0;  //Analog pin


unsigned long lastConnectionTime = 0;
const unsigned long postingInterval = 1000;

void setup() {
  pinMode(ledpin, OUTPUT); //pin 8 as OUTPUT
  Serial.begin(9600);
  // put your setup code here, to run once:

}

void loop() {
  if(Serial.available()){
    val = Serial.read();
  }
  if (val == 'H'){
    digitalWrite(ledpin, HIGH);
  }
  else{
    digitalWrite(ledpin, LOW);
  }

  if((millis()-lastConnectionTime > postinginterval)){
int ana_A = analogRead(sensorpin);
Serial.print("Sensor reading: ");
Serial.println(ana_A);
lastConnectionTime = millis();

  }
  // put your main code here, to run repeatedly:

}
