#include <SoftwareSerial.h> // remove the inverted commas after you copy the code to the IDE
SoftwareSerial BT(10, 11); 
// creates a "virtual" serial port/UART
// connect BT module TX to D10
// connect BT module RX to D11
// connect BT Vcc to 5V, GND to GND
void setup()  
{
  Serial.begin(9600);
  // set the data rate for the SoftwareSerial port
  BT.begin(9600);
  // Send test message to other device
  BT.println("Hello from Arduino");
}
void loop() 
{
  if (BT.available()) {
    Serial.println(BT.read());
    BT.println("Data received");
  } else {
    BT.println("Hello from Arduino");
    delay(500);
  }
}
