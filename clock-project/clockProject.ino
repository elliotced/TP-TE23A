/*
* Name: Clock and temp project
* Author: Elliot Cederlund
* Date: 2024-11-12
* Description: This project uses a ds3231 to measure time and displays the time to an 1306 oled display, 
* it also displays a time determinted by a potentiometer on the display. Further, it measures temprature with a analog 
* temprature module and displays a mapped value to a 9g-servo-motor. If the current time is higher than 
* the time set by the potentiometer the 9h-servo-motor instead signals the alarm.
*/

// include L´libraries
#include "U8glib.h"
#include <Servo.h>
#include <RTClib.h>
#include <Wire.h>

// init constants
byte tempPin = A0;
byte potePin = A1;
int servoPin = 2;
// init global variables

// construct objects
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);
Servo serv;
RTC_DS3231 rtc;

void setup() {
  // init communication
  Serial.begin(9600);
  Wire.begin();
  // init hardware
  pinMode(A0, INPUT);
  u8g.setFont(u8g_font_courB10);
  serv.attach(servoPin);
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));
}

void loop() {
  float potentio = map(analogRead(potePin),0,1023,0,24);
  oledWrite("Time: " + getTime(),"Alarm: " + String(potentio));
  if(getAlarm(potentio) == true){
    servoWrite(20);
    delay(100);
    servoWrite(30);
    delay(100);
  }
  else{
    servoWrite(getTemp());
    delay(100);
  }

}

/*
*This function converts a potentiometers output from 0-24 to determine if its value is lower than the current time and returns true/false.
*Parameters: value: potentiometer value
*Returns: true or false
*/

bool getAlarm(float value){
  if(value <= rtc.now().hour()){
    return(true);
  }
  else{
    return(false);
  }
}

/*
*This function reads time from an ds3231 module and package the time as a String
*Parameters: Void
*Returns: time in hh:mm:ss as String
*/

String getTime(){
  DateTime now = rtc.now();
  return(String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()));
}

/*
* This function reads an analog pin connected to an analog temprature sensor and calculates the corresponding temp
*Parameters: Void
*Returns: temprature as float
*/
float getTemp(){
  int Vo;
  float R1 = 10000; // value of R1 on board
  float logR2, R2, T;
  float c1 = 0.001129148, c2 = 0.000234125, c3 = 0.0000000876741; //steinhart-hart coeficients for thermistor
  Vo = analogRead(tempPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0); //calculate resistance on thermistor
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2)); // temperature in Kelvin
  T = T - 273.15; //convert Kelvin to Celcius
  return T;
}

/*
* This function takes a string and draws it to an oled display
*Parameters: - text1, text2: Time and Alarm
*Returns: void
*/
void oledWrite(String text1,String text2){
  u8g.firstPage();  
  do {
    u8g.drawStr(0,20,text1.c_str());
    u8g.drawStr(0,60,text2.c_str());
  } while(u8g.nextPage());
}

/*
* takes a temprature value and maps it to corresppnding degree on a servo or signals the alarm
*Parameters: - value: temprature or alarm sequence
*Returns: void
*/
void servoWrite(float value){
  serv.write(map(value,20,30,90,0));
}