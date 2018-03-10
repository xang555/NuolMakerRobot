#include <Arduino.h>
#include <L298MD.h>
#include <Ultrasonic.h>
#include "./track/tracker.h"

#define MAX_STATION_COUNT 4

/*---- define  motor driver pin  ---*/
#define A_ENA 2
#define A_N1  22
#define A_N2  23
#define A_N3  24
#define A_N4  25
#define A_ENB 3
/*--------- define line tracking sensor ------*/
#define TK_S_1 4
#define TK_S_2 5
#define TK_S_3 6
#define TK_S_4 7
/*-------- ROBOT STATE -----------*/
#define STOP_BUS_STATE 0
#define STATION_ONE_STATE 1
#define STATION_TWO_STATE 2
#define STATION_THREE_STATE 3

/*----  comunication pin------*/
#define CMM_ONE 42
#define CMM_TWO 43
#define CMM_SUCESS 44

L298MD md(A_ENA,A_N1,A_N2,A_N3,A_N4,A_ENB);
tracker tk(TK_S_1,TK_S_2,TK_S_3,TK_S_4);
Ultrasonic ultrasonic(8, 9);

 uint8_t looper = 0;
 uint8_t robotState = 0;
 uint8_t robotposition = 3;

void handleTakeoffBox(){ 
  delay(50);
  md.motorLeftbreak(tk.getpwm(100));
  md.motorRightbreak(tk.getpwm(100));
    Serial.println("takeoff box");
 while(digitalRead(CMM_SUCESS) != 1){}

   for(uint8_t i = 0 ; i< 10 ; i++){ 
   digitalWrite(CMM_ONE,LOW);
   digitalWrite(CMM_TWO,HIGH);
   }


 while(digitalRead(CMM_SUCESS) != 1){}
Serial.println("takeoff sucess");
} // handle takeoff box


void gotoStopStation(uint8_t& robotState){

    uint16_t pos = 0;
    uint32_t distance = 0;
    
   for(uint8_t i = 0 ; i< 10 ; i++){
    digitalWrite(CMM_ONE,HIGH);
    digitalWrite(CMM_TWO,LOW);
   } 

    while(1){

      pos += tk.tracking();  
      if(MAX_STATION_COUNT - robotposition == pos){
        delay(60);
        md.motorLeftbreak(tk.getpwm(100));
        md.motorRightbreak(tk.getpwm(100));
        while(1){
            distance = ultrasonic.distanceRead(); 
            if(distance > 0 && distance <= 7){
                delay(10000);
                if(looper == 0){
                    robotState = 2;
                }else if(looper ==1){
                    robotState = 1;
                }else if(looper ==2){
                    robotState = 3;
                }
                break;
            }
        }

        break;

      }  

    }

} //go to stop station 


uint8_t gotoOneStation(uint8_t& robotState) {

    uint16_t pos = 0;

   for(uint8_t i = 0 ; i< 10 ; i++){ 
      digitalWrite(CMM_ONE,HIGH);
      digitalWrite(CMM_TWO,HIGH);
   }

    while(1){

        pos += tk.tracking();
        if(pos == 1){
            handleTakeoffBox(); //handle in takeoff box
            break;
        }
         
    }

    robotState = 0;

    return pos;

} // go to one station

uint8_t gotoStationTwo(uint8_t& robotState) {

    uint16_t pos = 0;
  for(uint8_t i = 0 ; i< 10 ; i++){ 
   digitalWrite(CMM_ONE,HIGH);
   digitalWrite(CMM_TWO,HIGH);
  }

    while(1){

        pos += tk.tracking();
        if(pos == 2){
            handleTakeoffBox(); //handle in takeoff box
            break;
        }
         
    }

    robotState = 0;

    return pos;

} //go to two station 


uint8_t gotoStationThree(uint8_t& robotState) {

    uint16_t pos = 0;
  for(uint8_t i = 0 ; i< 10 ; i++){   
   digitalWrite(CMM_ONE,HIGH);
   digitalWrite(CMM_TWO,HIGH);
    }

    while(1){

        pos += tk.tracking();
        if(pos == 3){
            handleTakeoffBox(); //handle in takeoff box
            break;
        }
         
    }

    robotState = 0;

    return pos;    

} // go to three station


void setup() {
   Serial.begin(9600);
   pinMode(CMM_ONE,OUTPUT);
   pinMode(CMM_TWO,OUTPUT);
   pinMode(CMM_SUCESS,INPUT);
   md.initmotor();
   tk.initSensorTracker();
   tk.initMotorTracker(md);

   digitalWrite(CMM_ONE,LOW);
   digitalWrite(CMM_TWO,LOW);  

}

void loop() {

    switch(robotState){

        case STOP_BUS_STATE:
            //stop station
            Serial.println("go to stop");
            gotoStopStation(robotState);
            break;
        case STATION_ONE_STATE:
            //station one
            Serial.println("go to one");
               robotposition=gotoOneStation(robotState);
                looper = 2;
            break;
        case STATION_TWO_STATE:
            //station two
            Serial.println("go to two");
             robotposition=gotoStationTwo(robotState);
             looper = 1;
            break;
        case STATION_THREE_STATE:
            //station three
            Serial.println("go to three");
            robotposition=gotoStationThree(robotState);
             looper = 0;
            
    }

}