#include <softwareserial.h>
#include <avr/wdt.h>
#include <wire.h>


bool sign_in(String user_name, String password) {
  return false;
}

bool send_response(){
  return false;
}

//read_response

int getTemp() {
  return 0;
}

bool isStoveOn() {
  return false;
}

bool checkTempSens() {
  return false;
}

bool checkMotionSens() {
  return false;
}

bool checkInductiveSens() {
  return false;
}

bool isBatteryCharged() {
  return false;
}

bool notifyUser(String msg){
  return false;
}

String getResponse(int timeout){
  return NULL;
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
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
