#include <SoftwareSerial.h>
#include <avr/wdt.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>


static const int CURRENT_SENS_PIN = A0;
static const int BATTERY_PIN = A1;
static const int SOLENOID_PIN = 8;
static const int motion_sensor_in = 9;
static const int motion_i = 0;
static const float UNATTENDED_MAX_TIME = 90.0;
static const float SHUTDOWN_MAX_TIME = 60.0;

Adafruit_MLX90614 temp_sens = Adafruit_MLX90614(); // A4 == SCA, A5 == SCL
SoftwareSerial BT(10,11);

float unattended_timer = UNATTENDED_MAX_TIME;
float shutdown_timer = SHUTDOWN_MAX_TIME;

bool sign_in(String user_name, String password) {
  return false;
}

float getVoltage(int pin) {
  return (float)analogRead(pin) * (5.0/1024);
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

void notifyUser(String msg){
  BT.println(msg);
}

String getResponse(){
  String response = "";
  while(BT.available())
    response += (char)BT.read();
  return response;
}

bool turnOffStove(){
  digitalWrite(SOLENOID_PIN, HIGH); // Will change if normally closed or open
  if(getVoltage(CURRENT_SENS_PIN) > 0)// Will this work? May have to change to interrupt/event
    return false;
  return true;
}

bool turnOnStove(){
  digitalWrite(SOLENOID_PIN, HIGH); // Will change if normally closed or open
  if(getVoltage(CURRENT_SENS_PIN) > 0)
    return true;
  return false;
}

bool restartStove(){
  return false;
}

void motionISR(){
  unattended_timer = UNATTENDED_MAX_TIME;
  shutdown_timer = SHUTDOWN_MAX_TIME;
  Serial.println("Motion Detected");
}

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

  attachInterrupt(digitalPinToInterrupt(motion_sensor_in), motionISR, RISING);
  
  //if(!checkTempSens())
    // handle temp sensor error
  //if(!checkMotionSens())
    // handle motion sensor error
  //if(!checkCurrentSens())
    // handle current sensor error
  Serial.println("System Reset");
}

void loop() {
  wdt_reset();
  //int j = digitalRead(motion_sensor_in);
  //Serial.println(j);
  //delay(200);
  
  if(BT.available()){
    //Serial.println(BT.read());
    Serial.println(getResponse());
    BT.println("Data received");

    
  }
  else{
    BT.println(getTemp());

    delay(400);
  }
  
}
