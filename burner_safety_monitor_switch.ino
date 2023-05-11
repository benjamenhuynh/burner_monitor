#include <SoftwareSerial.h>
#include <avr/wdt.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>


static const int CURRENT_SENS_PIN = A0;
static const int BATTERY_PIN = A1;
static const int SOLENOID_PIN = 8;
static const int motion_sensor_in = 9;
static const int HIGH_SWITCH = 6;
static const int FLEX_SWITCH = 7;
static const unsigned long UNATTENDED_MAX_TIME = 5000;
static const unsigned long SHUTDOWN_MAX_TIME = 5000;

bool error_flag = false;

Adafruit_MLX90614 temp_sens = Adafruit_MLX90614(); // A4 == SDA, A5 == SCL
SoftwareSerial BT(10,11);// TX == D10, RX == D11

float getVoltage(int pin) {
  return (float)analogRead(pin) * (5.0/1024); 
}

float getTemp() {
  return temp_sens.readObjectTempF();
}

bool isStoveOn() {
  if(checkSwitch())//if(getTemp() > 100)
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
  notifyUser("Please move in front of the sensor");
  for(int i = 0; i < 5000; i++) {
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
  if(j > 0)
    return true; //if j returns a 1 that means we detect motion
  return false; //return false if nobody is there
}

bool checkCurrentSens() {
  return true;
}

bool isBatteryCharged() {
  if(getVoltage(BATTERY_PIN) < 3.0)
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

bool checkSwitch() {
  int j = digitalRead(FLEX_SWITCH); //performs a digital read on the button pin
  if(j == 0)
    return true; //if j returns a 1 that means stove is on
  return false; //return false if the stove is off
}

void error_state() {
  notifyUser("Error Detected: Unable to continue operation\nPlease shutdown system and contact support");
  while(true)
    wdt_reset();
}

void setup() {
  Serial.begin(9600);
  temp_sens.begin();
  BT.begin(9600);
  wdt_disable();
  delay(2000);
  wdt_enable(WDTO_2S);
  pinMode(motion_sensor_in, INPUT_PULLUP);// Motion Sensor
  pinMode(CURRENT_SENS_PIN, INPUT);// Current Sensor
  pinMode(BATTERY_PIN, INPUT);// Battery Voltage
  pinMode(SOLENOID_PIN, OUTPUT);// Output to solenoid
  pinMode(HIGH_SWITCH, OUTPUT);
  pinMode(FLEX_SWITCH, INPUT_PULLUP);

  digitalWrite(HIGH_SWITCH, LOW);
  digitalWrite(SOLENOID_PIN, HIGH);
  notifyUser(" ");
  /*
  if(!checkTempSens())
    notifyUser("Temperature Sensor Error: Contact Support");
  else
    notifyUser("Temperature Sensor Check Passed!");
  */
  if(!checkMotionSens())
    notifyUser("Motion Sensor Error: Contact Support");
  else
    notifyUser("Motion Sensor Check Passed!");
  if(!checkCurrentSens())
    notifyUser("Current Sensor Error: Contact Support");
  if(!isBatteryCharged())
    notifyUser("Battery is low!");
  else
    notifyUser("Battery Check Passed!");
  //notifyUser(String(getVoltage(BATTERY_PIN)));
  if(error_flag)
    error_state();
  //creating a login admin and pass for temporary use
  notifyUser("Username: ");
  String user_name = "";
  while(user_name != "admin\r\n"){
    if(user_name != "")
      notifyUser("Incorrect username, please try again");
    user_name = getResponse();
    wdt_reset();
  }
  notifyUser("Password: ");
  String password = "";
  while(password != "pass\r\n"){
    if(password != "")
      notifyUser("Incorrect password, please try again");
    password = getResponse();
    wdt_reset();
  }

}

void loop() {
  unsigned long unattendedDelayStart;
  unsigned long shutdownDelayStart;
  while(isStoveOn()) {
    unattendedDelayStart = millis();
    wdt_reset();
    while(!getMotion() && isStoveOn()){
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
}
