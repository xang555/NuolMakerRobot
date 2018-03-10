#include "tracker.h"

tracker::tracker(uint8_t tk1, uint8_t tk2, uint8_t tk3,uint8_t tk4){
    this->_pin_tk1 = tk1;
    this->_pin_tk2 = tk2;
    this->_pin_tk3 = tk3;
    this->_pin_tk4 = tk4;
} // construc

void tracker::initSensorTracker(){
    pinMode(this->_pin_tk1,INPUT);
    pinMode(this->_pin_tk2,INPUT);
    pinMode(this->_pin_tk3,INPUT);
    pinMode(this->_pin_tk4,INPUT);
}

void tracker::initMotorTracker(L298MD& md){
    this->_md = md;
}

void tracker::readSesnor(){

   this->_val_tk1 = digitalRead(this->_pin_tk1);
   this->_val_tk2 = digitalRead(this->_pin_tk2);
   this->_val_tk3 = digitalRead(this->_pin_tk3);
   this->_val_tk4 = digitalRead(this->_pin_tk4);

} //read sensor


bool tracker::_is_wbbw(){ //wbbw
    return (this->_val_tk1 == 1 && this->_val_tk2 == 0 && this->_val_tk3 == 0 && this->_val_tk4 == 1);
}

bool tracker::_is_wwbw(){ // wwbw
    return (this->_val_tk1 == 1 && this->_val_tk2 == 1 && this->_val_tk3 == 0 && this->_val_tk4 == 1);
}

bool tracker::_is_wwwb(){ //wwwb
    return (this->_val_tk1 == 1 && this->_val_tk2 == 1 && this->_val_tk3 == 1 && this->_val_tk4 == 0);
}
bool tracker::_is_wbww(){ //wbww
return (this->_val_tk1 == 1 && this->_val_tk2 == 0 && this->_val_tk3 == 1 && this->_val_tk4 == 1);
}
bool tracker::_is_bwww(){ //bwww
return (this->_val_tk1 == 0 && this->_val_tk2 == 1 && this->_val_tk3 == 1 && this->_val_tk4 == 1);
}

bool tracker::_is_bbbb() { //bbbb
return (this->_val_tk1 == 0 && this->_val_tk2 == 0 && this->_val_tk3 == 0 && this->_val_tk4 == 0);
}

bool tracker::_is_wwww(){ //wwww
return (this->_val_tk1 == 1 && this->_val_tk2 == 1 && this->_val_tk3 == 1 && this->_val_tk4 == 1);
}

bool tracker::_is_wbbb(){ // wbbb
return (this->_val_tk1 == 1 && this->_val_tk2 == 0 && this->_val_tk3 == 0 && this->_val_tk4 == 0);
}

bool tracker::_is_wwbb(){ //wwbb
return (this->_val_tk1 == 1 && this->_val_tk2 == 1 && this->_val_tk3 == 0 && this->_val_tk4 == 0);
}
bool tracker::_is_bwwb(){ //bwwb
return (this->_val_tk1 == 0 && this->_val_tk2 == 1 && this->_val_tk3 == 1 && this->_val_tk4 == 0);
}
bool tracker::_is_bbbw(){ //bbww
return (this->_val_tk1 == 0 && this->_val_tk2 == 0 && this->_val_tk3 == 0 && this->_val_tk4 == 1);
}

bool tracker::_is_bbww(){ //bbww
return (this->_val_tk1 == 0 && this->_val_tk2 == 0 && this->_val_tk3 == 1 && this->_val_tk4 == 1);
}


uint16_t tracker::tracking(){

    static bool robot_is_black_line = false;
    uint8_t positioncount =0 ;

    readSesnor(); //read sensor

    if(_is_wbbw()){ //forward
        this->_md.motorLeftForwardClock(getpwm(60));
        this->_md.motorRightForwardClock(getpwm(60));
        robot_is_black_line = false;
    }else if(_is_wwbw()){ // too right
        this->_md.motorLeftRevertClock(getpwm(50));
        this->_md.motorRightForwardClock(getpwm(50));
        robot_is_black_line = false;
    }else if(_is_wwwb()){ //trun right
        this->_md.motorLeftRevertClock(getpwm(50));
        this->_md.motorRightForwardClock(getpwm(60));
        robot_is_black_line = false;
    }else if(_is_wbww()){ // too left
        this->_md.motorLeftForwardClock(getpwm(50));
        this->_md.motorRightRevertClock(getpwm(50));
        robot_is_black_line = false;
    }else if(_is_bwww()){ // trun left
        this->_md.motorLeftForwardClock(getpwm(50));
        this->_md.motorRightRevertClock(getpwm(60));
        robot_is_black_line = false;
    }else if(_is_wwww()){// trun right
        this->_md.motorLeftRevertClock(getpwm(50));
        this->_md.motorRightForwardClock(getpwm(60));
        robot_is_black_line = false;
    }else if(_is_bbbb()){ // robot counter poit
        this->_md.motorLeftForwardClock(getpwm(60));
        this->_md.motorRightForwardClock(getpwm(60));
        if ( !robot_is_black_line ) {
           positioncount +=1;
           robot_is_black_line = true;
         }  
    }else if(_is_wbbb()){ // trun right
        this->_md.motorLeftRevertClock(getpwm(50));
        this->_md.motorRightForwardClock(getpwm(50));
    }else if(_is_wwbb()){ //trun right
        this->_md.motorLeftRevertClock(getpwm(50));
        this->_md.motorRightForwardClock(getpwm(50));
    }else if(_is_bbww()){ //trun left
        this->_md.motorLeftRevertClock(getpwm(50));
        this->_md.motorRightForwardClock(getpwm(50));
    }else {
        this->_md.motorLeftForwardClock(getpwm(50));
        this->_md.motorRightForwardClock(getpwm(50));
    }

    return positioncount;

} // tracking line
