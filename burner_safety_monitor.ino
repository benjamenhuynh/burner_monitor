#include <SoftwareSerial.h>
#include <avr/wdt.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <string>


Adafruit_MLX90614 temp_sens = Adafruit_MLX90614();
SoftwareSerial BT(10,11);
CURRENT_SENS_PIN = A0;
BATTERY_PIN = A1;
SOLENOID_PIN = D0;



bool sign_in(String user_name, String password) {
  return false;
}

bool send_response(){
  return false;
}

//read_response

float getTemp() {
  return temp_sens.readObjectTempF();
}

bool isStoveOn() {
  if(getTemp() > 100)
    return true;
  return false;
}

bool checkTempSens() {
  float temp = getTemp();// will return NaN if not functioning correctly
  if(temp == temp)// NaN is not equal to NaN
    return true;
  return false;
}

bool checkMotionSens() {
  return true;
}

bool checkCurrentSens() {
  return true;
}

bool isBatteryCharged() {
  return false;
}

void notifyUser(string msg){
  BT.println(msg);
}

string getResponse(int timeout){
  string response = "";
  while(BT.available())
    response += BT.read();
  return response;
}

bool turnOffStove(){
  return false;
}

bool turnOnStove(){
  return false;
}

bool restartStove(){
  return false;
}

void motionISR(){
  
}

void setup() {
  wdt.disable();
  delay(2000);
  wdt.enable(WDTO_2S);
  temp_sens.begin();
  BT.begin(9600);
  pinMode(CURRENT_SENS_PIN, INPUT);// Current Sensor
  pinMode(BATTERY_PIN, INPUT);// Battery Voltage
  pinMode(SOLENOID_PIN, OUTPUT;// Output to solenoid
  
  if(!checkTempSens())
    // handle temp sensor error
  if(!checkMotionSens())
    // handle motion sensor error
  if(!checkCurrentSens())
    // handle current sensor error
}

void loop() {
  wdt_reset();

}
