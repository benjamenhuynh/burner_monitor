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

Adafruit_MLX90614 temp_sens = Adafruit_MLX90614(); // A4 == SCA, A5 == SCL
SoftwareSerial BT(10,11);

unsigned long unattended_timer = UNATTENDED_MAX_TIME;
unsigned long shutdown_timer = SHUTDOWN_MAX_TIME;
unsigned long prev_time = 0;
unsigned long delayStart = 0;

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
  //https://www.forward.com.au/pfod/ArduinoProgramming/TimingDelaysInArduino.html
  int j = digitalRead(motion_sensor_in); //performs a digital read on the motion sensor pin 
  if(j > 0)
    return true; //if j returns a 1 that means we detect motion
  return false; //return false if nobody is there 
  /*
  unattended_timer = 0;
  notifyUser("Please move in front of the sensor");
  for(int i = 0; i < 10; i++) {
    delay(400);
    wdt_reset();
    if(unattended_timer == UNATTENDED_MAX_TIME)
      return true;
  }
  */
  return false;
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
  bool delayRunning = false;
  
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
  /*
  while(!isStoveOn()) {
    delay(400);
    wdt_reset();
  }
  */
  if(isStoveOn())
  {
    if(!checkMotionSens()){
        //notifyUser("Stove is on, there is no motion detected. Should we turn the stove off? Reply with Y/N");
        //String response = getResponse();
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
  else
  {
    notifyUser("Stove is not on");
    while(!isStoveOn())
      wdt_reset();
  }
  
  /*
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
  */
}
