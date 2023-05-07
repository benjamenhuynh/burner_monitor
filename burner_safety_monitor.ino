#include <SoftwareSerial.h>
#include <avr/wdt.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>


static const int CURRENT_SENS_PIN = A0;
static const int BATTERY_PIN = A1;
static const int SOLENOID_PIN = 8;
static const int motion_sensor_in = 9;
static const int motion_i = 0;
static const unsigned long UNATTENDED_MAX_TIME = 10000;
static const unsigned long SHUTDOWN_MAX_TIME = 10000;

Adafruit_MLX90614 temp_sens = Adafruit_MLX90614(); // A4 == SCA, A5 == SCL
SoftwareSerial BT(10,11);

long long unattended_timer = UNATTENDED_MAX_TIME;
long long shutdown_timer = SHUTDOWN_MAX_TIME;
unsigned long delayStart = 0;

float getVoltage(int pin) {
  return (float)analogRead(pin) * (5.0/1024);
}

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
  unattended_timer = 0;
  notifyUser("Please move in front of the sensor");
  for(int i = 0; i < 4000; i++) {
    delay(1);
    wdt_reset();
    if(getMotion())
      return true;
  }
  return false;
}

bool getMotion() {
  //https://www.forward.com.au/pfod/ArduinoProgramming/TimingDelaysInArduino.html
  int j = digitalRead(motion_sensor_in); //performs a digital read on the motion sensor pin 
  if(j > 0) {
    unattended_timer = UNATTENDED_MAX_TIME;
    shutdown_timer = SHUTDOWN_MAX_TIME;
    return true; //if j returns a 1 that means we detect motion
  }
  return false; //return false if nobody is there
}

bool checkCurrentSens() {
  return true;
}

bool isBatteryCharged() {
  if(getVoltage(BATTERY_PIN) < 2.59)
      return false;
  return true;
}

void notifyUser(String msg){
  BT.println(msg);
}

String getResponse(){
  String response = "";
  delay(20);
  while(BT.available())
    response += (char)BT.read();
  return response;
}

bool turnOffStove(){
  digitalWrite(SOLENOID_PIN, HIGH); // Will change if normally closed or open
  //if(getVoltage(CURRENT_SENS_PIN) > 0)// Will this work? May have to change to interrupt/event
  //  return false;
  return true;
}

bool turnOnStove(){
  digitalWrite(SOLENOID_PIN, LOW); // Will change if normally closed or open
  //if(getVoltage(CURRENT_SENS_PIN) > 0)
  //  return true;
  return true;//false;
}

bool restartStove(){
  notifyUser("The stove has been shut off. Please make sure the burners are turned off and select \"Ok\"");
  String response = "";
  while(response != "Ok\r\n")
    response = getResponse();
  return turnOnStove();
}

/*
void motionISR(){
  unattended_timer = UNATTENDED_MAX_TIME;
  shutdown_timer = SHUTDOWN_MAX_TIME;
  Serial.println("Motion Detected");
}
*/

void setup() {
  Serial.begin(9600);
  wdt_disable();
  delay(2000);
  wdt_enable(WDTO_2S);
  temp_sens.begin();
  BT.begin(9600);
  pinMode(motion_sensor_in, INPUT_PULLUP);// Motion Sensor
  pinMode(CURRENT_SENS_PIN, INPUT);// Current Sensor
  pinMode(BATTERY_PIN, INPUT);// Battery Voltage
  pinMode(SOLENOID_PIN, OUTPUT);// Output to solenoid

  //attachInterrupt(digitalPinToInterrupt(motion_sensor_in), motionISR, RISING);
  delayStart = millis();
  
  if(!checkTempSens())
    notifyUser("Temperature Sensor Error: Contact Support Temp Temp Temp");
  if(!checkMotionSens())
    notifyUser("Motion Sensor Error: Contact Support");
  //if(!checkCurrentSens())
    //notifyUser("Stove is on, there is no motion detected. Should we turn the stove off? Reply with Y/N");
  Serial.println("System Reset");
}

void loop() {
  wdt_reset();
  if(isStoveOn())
  {
    if(unattended_timer - (millis() - delayStart) <= 0) {
      if(!getMotion()){
          String response = "";
          do {
            notifyUser("Stove is on, there is no motion detected. Should we turn the stove off? Reply with Y/N");
            while(response != "Y\r\n" && response != "N\r\n"){
              response = getResponse();
              if(response != "")
                Serial.println(response);
              wdt_reset();
            }
  
            if(response == "Y\r\n")
            {
              turnOffStove();
              notifyUser("Stove has been turned off, thanks!");
            }
            else if(response == "N\r\n"){
               notifyUser("Stove will be kept on");
            }
          } while(response == NULL);
      }
      else
      {
        notifyUser("Presence detected and stove is on.");
      }
    }
    }
   else
   {
     notifyUser("Stove is not on");
     while(!isStoveOn()) {
       wdt_reset();
       delayStart = millis();
     }
   }
}
