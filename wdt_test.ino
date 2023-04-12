#include <avr/wdt.h>

void setup() {
  Serial.begin(9600);
  Serial.println("System Reset");
  wdt_disable();
  delay(3000);
  wdt_enable(WDTO_2S);
  Serial.println("Setup Completed");
}

void loop() {
  Serial.println("In Loop");
  for(int i = 0; i < 10; i++) {
    wdt_reset();
    Serial.println("WDT Reset");
    delay(250);
  }
  while(1);
}
