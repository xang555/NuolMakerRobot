#include <L298MD.h>
#ifndef	LOW_PWM	
#define	LOW_PWM	0

L298MD::L298MD(uint8_t ena,uint8_t n1,uint8_t n2,uint8_t n3,uint8_t n4,uint8_t enb){
	
	ENA = ena;
	ENB = enb;
	N1 = n1;
	N2 = n2;
	N3 = n3;
	N4 = n4;	
	
}


void L298MD::initmotor(){
	
		pinMode(ENA,OUTPUT);
		pinMode(ENB,OUTPUT);
		pinMode(N1,OUTPUT);
		pinMode(N2,OUTPUT);
		pinMode(N3,OUTPUT);
		pinMode(N4,OUTPUT);
		
	}
	
void L298MD::forwardclockA(uint8_t PWM){
	
	digitalWrite(N1,LOW);
	analogWrite(ENA,PWM);
	digitalWrite(N2,HIGH);
		
}	

void L298MD::forwardclockB(uint8_t PWM){
	
	digitalWrite(N3,LOW);
	analogWrite(ENB,PWM);
	digitalWrite(N4,HIGH);
		
	
}

void L298MD::revertclockA(uint8_t PWM){
	
	digitalWrite(N1,HIGH);
	analogWrite(ENA,PWM);
	digitalWrite(N2,LOW);
	
}

void L298MD::revertclockB(uint8_t PWM){
	
	digitalWrite(N3,HIGH);
	analogWrite(ENB,PWM);
	digitalWrite(N4,LOW);
	
}

void L298MD::motorAbreak(uint8_t PWM){
	
	digitalWrite(N1,HIGH);
	analogWrite(ENA,PWM);
	digitalWrite(N2,HIGH);
}

void L298MD::motorBbreak(uint8_t PWM){
	digitalWrite(N3,HIGH);
	analogWrite(ENB,PWM);
	digitalWrite(N4,HIGH);
}

void L298MD::motorAstop(){
	
	digitalWrite(N1,LOW);
	analogWrite(ENA,LOW_PWM);
	digitalWrite(N2,LOW);
	
}

void L298MD::motorBstop(){
	
	digitalWrite(N3,LOW);
	analogWrite(ENB,LOW_PWM);
	digitalWrite(N4,LOW);
	
}

#endif



