#include <SoftwareSerial.h>
#include <avr/wdt.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>


static const int CURRENT_SENS_PIN = A0;
static const int BATTERY_PIN = A1;
static const int SOLENOID_PIN = 8;
static const int motion_sensor_in = 9;
static const int motion_i = 0;
static const unsigned long UNATTENDED_MAX_TIME = 5000;
static const unsigned long SHUTDOWN_MAX_TIME = 5000;

bool error_flag = false;

Adafruit_MLX90614 temp_sens = Adafruit_MLX90614(); // A4 == SCA, A5 == SCL
SoftwareSerial BT(10,11);

//unsigned long unattended_timer = UNATTENDED_MAX_TIME;
//unsigned long shutdown_timer = SHUTDOWN_MAX_TIME;
//unsigned long unattendedDelayStart = 0;

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
  if(temp == temp)// NaN is not equal to itself
    return true;
  error_flag = true;
  return false;
}

bool checkMotionSens() {
  //unattended_timer = 0;
  notifyUser("Please move in front of the sensor");
  for(int i = 0; i < 4000; i++) {
    delay(1);
    wdt_reset();
    if(getMotion())
      return true;
  }
  error_flag = true;
  return false;
}

bool getMotion() {
  //https://www.forward.com.au/pfod/ArduinoProgramming/TimingDelaysInArduino.html
  int j = digitalRead(motion_sensor_in); //performs a digital read on the motion sensor pin 
  if(j > 0) {
    //unattended_timer = UNATTENDED_MAX_TIME;
    //shutdown_timer = SHUTDOWN_MAX_TIME;
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
  digitalWrite(SOLENOID_PIN, LOW); // Will change if normally closed or open
  if(getVoltage(CURRENT_SENS_PIN) > 0) {
    error_flag = true;
    return false;
  }
  return true;
}

bool turnOnStove(){
  digitalWrite(SOLENOID_PIN, HIGH); // Will change if normally closed or open
  if(getVoltage(CURRENT_SENS_PIN) > 0)
    return true;
  error_flag = true;
  return false;
}

bool restartStove(){
  notifyUser("The stove has been shut off. Please make sure the burners are turned off before restarting!");
  notifyUser("Enter Y to restart the stove, N to leave it off:");
  String response = "";
  while(response != "Y\r\n" && response != "N\r\n") {
    response = getResponse();
    wdt_reset();
  }
  if(response == "Y\r\n") {
    notifyUser("Turning stove back on...");
    return turnOnStove();
  }else
    return false;
}

void error_state() {
  notifyUser("Error Detected: Unable to continue operation\nPlease shutdown system and contact support");
  while(true)
    wdt_reset();
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
  //delayStart = millis();
  digitalWrite(SOLENOID_PIN, HIGH);
  if(!checkTempSens())
    notifyUser("Temperature Sensor Error: Contact Support");
  if(!checkMotionSens())
    notifyUser("Motion Sensor Error: Contact Support");
  if(!checkCurrentSens())
    notifyUser("Current Sensor Error: Contact Support");
  if(error_flag)
    error_state();
}

void loop() {
  unsigned long unattendedDelayStart;
  unsigned long shutdownDelayStart;
  while(isStoveOn()) {
    unattendedDelayStart = millis();
    wdt_reset();
    while(!getMotion()){
      wdt_reset();
      if((millis() - unattendedDelayStart) >= UNATTENDED_MAX_TIME) {
        shutdownDelayStart = millis();
        String response = "";
        notifyUser("Stove is on, there is no motion detected. Should we turn the stove off? Reply with Y/N");
        while(response != "Y\r\n" && response != "N\r\n"){
          response = getResponse();
          if((millis() - shutdownDelayStart) >= SHUTDOWN_MAX_TIME)
            response = "Y\r\n";
          if(getMotion())
            response = "N\r\n";
          wdt_reset();
        }
        if(response == "Y\r\n") {
          turnOffStove();
          notifyUser("Stove has been turned off, thanks!");
          if(!restartStove())
            break;
          unattendedDelayStart = millis();
        }else if(response == "N\r\n") {
          notifyUser("Stove will be kept on");
          unattendedDelayStart = millis();
        }
      }
    }
  }
  if(error_flag)
    error_state();
  notifyUser("Stove is not on");
  while(!isStoveOn())
    wdt_reset();
  /*
  unsigned long unattendedDelayStart = millis();
  unsigned long shutdownDelayStart = millis();
  wdt_reset();
  if(isStoveOn())
  {
    if(UNATTENDED_MAX_TIME - (millis() - unattendedDelayStart) <= 0) {//if(unattended_timer - (millis() - delayStart) <= 0) {
      if(!getMotion()){
          String response = "";
          //do {
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
          //} while(response == NULL);
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
   */
}
