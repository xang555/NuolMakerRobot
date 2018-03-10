#include "L298MD.h"
#ifndef	LOW_PWM	
#define	LOW_PWM	0

L298MD::L298MD(){}

L298MD::L298MD(uint8_t ena,uint8_t n1,uint8_t n2,uint8_t n3,uint8_t n4,uint8_t enb){
	this->ENA = ena;
	this->ENB = enb;
	this->N1 = n1;
	this->N2 = n2;
	this->N3 = n3;
	this->N4 = n4;		
}


void L298MD::initmotor(){
		pinMode(ENA,OUTPUT);
		pinMode(ENB,OUTPUT);
		pinMode(N1,OUTPUT);
		pinMode(N2,OUTPUT);
		pinMode(N3,OUTPUT);
		pinMode(N4,OUTPUT);
	}
	
void L298MD::motorLeftForwardClock(uint8_t PWM){
	analogWrite(ENA,PWM);
	digitalWrite(N1,HIGH);
	digitalWrite(N2,LOW);
}	

void L298MD::motorRightForwardClock(uint8_t PWM){
	analogWrite(ENB,PWM);
	digitalWrite(N3,HIGH);
	digitalWrite(N4,LOW);
}

void L298MD::motorLeftRevertClock(uint8_t PWM){
	analogWrite(ENA,PWM);
	digitalWrite(N1,LOW);
	digitalWrite(N2,HIGH);
}

void L298MD::motorRightRevertClock(uint8_t PWM){
	analogWrite(ENB,PWM);
	digitalWrite(N3,LOW);
	digitalWrite(N4,HIGH);
}

void L298MD::motorLeftbreak(uint8_t PWM){
	analogWrite(ENA,PWM);
	digitalWrite(N1,HIGH);
	digitalWrite(N2,HIGH);
}

void L298MD::motorRightbreak(uint8_t PWM){
	analogWrite(ENB,PWM);
	digitalWrite(N3,HIGH);
	digitalWrite(N4,HIGH);
}

void L298MD::motorLeftstop(){
	analogWrite(ENA,LOW_PWM);
	digitalWrite(N1,LOW);
	digitalWrite(N2,LOW);
}

void L298MD::motorRightstop(){
	analogWrite(ENB,LOW_PWM);
	digitalWrite(N3,LOW);
	digitalWrite(N4,LOW);
}

#endif



