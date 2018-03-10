#include <Arduino.h>
#include <L298MD.h>

/*----- l298md pin ---*/
#define A_ENA 2
#define A_N1  3
#define A_N2  4
#define A_N3  5
#define A_N4  6
#define A_ENB 7
/*----- detect hand pin ----*/
#define HAND_DOWN 8
#define HAND_UP 9
#define HAND_TAKE_STAND_BY 10
#define HAND_TAKEOFF_BOX 11

/*------comunication pin ----*/
#define CMM_ONE 12
#define CMM_TWO 13
/*----- command ----*/
#define HAND_STAND_BY  100
#define HAND_UP_COMMAND  200
#define HAND_TAKEOFF_COMMAND  300
#define COMMAND_SUCESS 400

L298MD md(A_ENA,A_N1,A_N2,A_N3,A_N4,A_ENB);

void handStandBy(){

bool hdcall = false;
bool hdtcall = false;
bool sucess_one = false;
bool sucess_two = false;

while(1){

    if(digitalRead(HAND_DOWN) !=1){
      if(!hdcall) {
          md.motorLeftRevertClock(255);
          hdcall = true;
      }
    }else {
        md.motorLeftstop();
        sucess_one = true;
        if(sucess_one && sucess_two){
            return;
        }    
    }

    if(digitalRead(HAND_TAKE_STAND_BY) != 1){
        if(!hdtcall) {
            md.motorRightForwardClock(255);
        }
    }else{
        md.motorRightstop();
        sucess_two = true;
        if(sucess_one && sucess_two){
            return;
        }  
    }

}

} //hand down

void handUp(){

digitalWrite(A0,LOW);

bool call = false;
    while(1){
        if(digitalRead(HAND_UP) !=1){
            if(!call) {
                md.motorLeftForwardClock(255);
                call = true;
            }
        }else {
            md.motorLeftstop();
            digitalWrite(A0,HIGH);
            return;
        }

    }

} //hand up


void handTakeOff(){

digitalWrite(A0,LOW);
bool call = false;

    while(1){
        if(digitalRead(HAND_TAKEOFF_BOX) !=1){
            if(!call) {
                md.motorRightRevertClock(255);
                call = true;
            }
        }else {
            md.motorRightstop();
           digitalWrite(A0,HIGH);
            return;
        }

    }

} // hand takeoff box


void setup() {
   Serial.begin(9600);
    pinMode(HAND_DOWN,INPUT);
    pinMode(HAND_UP,INPUT);
    pinMode(HAND_STAND_BY,INPUT);
    pinMode(HAND_TAKEOFF_BOX,INPUT);
    pinMode(CMM_ONE,INPUT);
    pinMode(CMM_TWO,INPUT);
    pinMode(A0,OUTPUT);
    md.initmotor();
}

uint8_t signal_one = 0;
uint8_t signal_two = 0;

void loop() {

     signal_one = digitalRead(CMM_ONE);
     signal_two = digitalRead(CMM_TWO);
     
     if(signal_one == 1 && signal_two == 0) { //hand stand by
        handStandBy();
     }else if(signal_one == 1 && signal_two == 1){ //hand up
        handUp();
     }else if(signal_one == 0 && signal_two == 1){ //hand takeoff
        handTakeOff();
     }

}