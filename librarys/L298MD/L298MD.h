
#ifndef	MAX_PWM_SPEED
#define	MAX_PWM_SPEED	255

#include <Arduino.h>

class L298MD {
	
	
	public :
	   L298MD(uint8_t ena,uint8_t n1,uint8_t n2,uint8_t n3,uint8_t n4,uint8_t enb); // init pin l298 motor driver
		void initmotor();
		void forwardclockA(uint8_t PWM);
		void revertclockA(uint8_t PWM);
		void forwardclockB(uint8_t PWM);
		void revertclockB(uint8_t PWM);
		void motorAstop();
		void motorBstop();
		void motorAbreak(uint8_t PWM);
		void motorBbreak(uint8_t PWM);
		
	private :
		uint8_t ENA;
		uint8_t N1;
		uint8_t N2;
		uint8_t N3;
		uint8_t N4;
		uint8_t ENB; 
	
};

#endif
