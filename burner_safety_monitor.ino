#include <SoftwareSerial.h>
#include <avr/wdt.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <string>
#include <millisDelay.h>


Adafruit_MLX90614 temp_sens = Adafruit_MLX90614();
SoftwareSerial BT(10,11);
CURRENT_SENS_PIN = A0;
BATTERY_PIN = A1;
SOLENOID_PIN = D0;

int motion_sensor_in = 9;
int motion_i = 0;

unsigned long delayStart = 0;
bool delayRunning = false;

char * typeNamep[] = {"Object", "Ambient"};

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
  return false;
}

void notifyUser(string msg){
  BT.println(msg);
}

string getResponse(int timeout){
  string response = "";
  while(BT.available())
    response += (char)BT.read();
  return response;
}

bool turnOffStove(){
  
  return false;
}

bool turnOnStove(){
  return false;
}

bool restartStove(){
  //not sure what we can do here. 
  return false;
}

void motionISR(){
  
}

void setup() {
  Serial.begin(9600);
  wdt.disable();
  delay(2000);
  wdt.enable(WDTO_2S);
  temp_sens.begin();
  BT.begin(9600);
  pinMode(sensor_in, INPUT_PULLUP);
  pinMode(CURRENT_SENS_PIN, INPUT);// Current Sensor
  pinMode(BATTERY_PIN, INPUT);// Battery Voltage
  pinMode(SOLENOID_PIN, OUTPUT;// Output to solenoid

  delayStart =  millis();
  delayRunning = true;

  Serial.println("Robojax MLX90614 test");
  if(!checkTempSens())
    // handle temp sensor error
  if(!checkMotionSens())
    // handle motion sensor error
  if(!checkCurrentSens())
    // handle current sensor error
}

void loop() {
  wdt_reset();
  //int j = digitalRead(motion_sensor_in);
  //check if temp is on

  if(isStoveOn())
  {
    if(!checkMotionSens()){
        notifyUser("Stove is on, there is no motion detected. Should we turn the stove off? Reply with Y/N");
        response = getReponse(2000);
        while(response == NULL)
        {
          notifyUser("Stove is on, there is no motion detected. Should we turn the stove off? Reply with Y/N");
          response = getReponse(2000);
        }
        if(response == "Y")
        {
            turnOffStove();
            notifyUser("Stove has been turned off, thanks!");
        }
        else
        {
           notifyUser("Stove will be kept on");
        }
    }
    else
    {
      notifyUser("Presence detected and stove is on.");
    }
  }
  else
  {
    notifyUser("Stove is not on");
  }
  //we need to check motion sensor
  //then check if BT is available then send message
  //if(BT.available()){
    //Serial.println(BT.read());
    //BT.println("Data received");

    
  //}
  //BT works, so does getTemp, motion sensor needs to interact with ISR. 
  //Y/N feature for arduino to know if somebody is nearby.
  /*else{
    BT.println("Hello from arduino");

    if(motion_i != j){
      Serial.println(j);
    }
    motion_i = j; 
    delay(500);
  }*/

}
