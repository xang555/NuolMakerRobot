#include <L298MD.h>

//pin A
#define A_ENA 2
#define A_N1  3
#define A_N2  4
#define A_N3  5 
#define A_N4  6
#define A_ENB 7

//pin B
#define B_ENA 8
#define B_N1  9
#define B_N2  10
#define B_N3  11
#define B_N4  12
#define B_ENB 13

#define MAX_ROBOT_SPEED getpwm(100)

#define SPPED_MOTOR_HANDUP_IS_DWON  getpwm(80)
#define SPPED_MOTOR_HANDUP_IS_UP  getpwm(100)

#define SPEED_MOTOR_BREAK getpwm(50)
#define SPEED_MOTOR_TRUN_LEFT getpwm(80)
#define SPEED_MOTOR_TRUN_RIGTH getpwm(80)

 L298MD mdA(A_ENA,A_N1,A_N2,A_N3,A_N4,A_ENB);
 L298MD mdB(B_ENA,B_N1,B_N2,B_N3,B_N4,B_ENB);

char junk;
String inputString="";
 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
  mdA.initmotor();
  mdB.initmotor();

 Serial.println("start robot ... ");
  
}

void loop() {

  if(Serial.available()){
  while(Serial.available())
    {
      char inChar = (char)Serial.read(); //read the input
      inputString += inChar;        //make a string of the characters coming on serial
    }
    Serial.println(inputString);
    while (Serial.available() > 0)  
    { junk = Serial.read() ; }      // clear the serial buffer

      if ( inputString == "1"){
         // robot up
        mdA.forwardclockA(MAX_ROBOT_SPEED);
        mdA.forwardclockB(MAX_ROBOT_SPEED);    
      
    } else if ( inputString == "2" ){
      //robot down
         mdA.motorAbreak(SPEED_MOTOR_BREAK);
         mdA.motorBbreak(SPEED_MOTOR_BREAK);
         
          mdA.revertclockA(MAX_ROBOT_SPEED);
          mdA.revertclockB(MAX_ROBOT_SPEED );
    }else if ( inputString == "3") {
       // robot trun left 
      mdA.motorAbreak(SPEED_MOTOR_BREAK);
      mdA.motorBbreak(SPEED_MOTOR_BREAK);
      
       mdA.forwardclockB(SPEED_MOTOR_TRUN_LEFT); 
       mdA.revertclockA(MAX_ROBOT_SPEED);
       
    }else if ( inputString == "4" ){
      //robot trun rigth 
       mdA.motorAbreak(SPEED_MOTOR_BREAK);
       mdA.motorBbreak(SPEED_MOTOR_BREAK);
       
       mdA.forwardclockA(SPEED_MOTOR_TRUN_RIGTH); 
       mdA.revertclockB(MAX_ROBOT_SPEED);
      
    }else if ( inputString == "5"){
      //robot stop
          mdA.motorAbreak(SPEED_MOTOR_BREAK);
          mdA.motorBbreak(SPEED_MOTOR_BREAK);
          
          mdA.motorAstop();
          mdA.motorBstop();
          
    }else if ( inputString == "6"){
            // hand take the ball
              mdB.forwardclockA(MAX_ROBOT_SPEED);
                            
    }else if ( inputString == "7"){
        // hand take out the ball
          mdB.revertclockA(MAX_ROBOT_SPEED);
                   
    }else if ( inputString == "8" ){
            //hand-up is up
              mdB.forwardclockB(SPPED_MOTOR_HANDUP_IS_UP);
            
    }else if ( inputString == "9") {
        // hand-up is down     
          mdB.revertclockB(SPPED_MOTOR_HANDUP_IS_DWON);
          
    }else if (inputString == "0"){
      // motor b stop
          mdA.motorAbreak(SPEED_MOTOR_BREAK);
          mdA.motorBbreak(SPEED_MOTOR_BREAK);
          
          mdB.motorAstop();
          mdB.motorBstop();
      
    }

    inputString = "";
  }


} //loop

uint8_t getpwm(uint8_t pwmpersen) {
return (pwmpersen * 255) / 100;
}


