#include <MD_TCS230.h>
#include <FreqCount.h>
#include <L298MD.h>

//define pin motor
#define ENA 7
#define N1  40
#define N2  38
#define N3  36
#define N4  34
#define ENB 6

//define pin hand and hand up
#define HAU_ENA 4
#define HAU_N1  50
#define HAU_N2  48
#define HAU_N3  46
#define HAU_N4  44
#define HAU_ENB 5

//define pin led state run
#define LED1  28
#define LED2  26
#define LED3  24
#define LED4  22

//#define MIN_BLACK_VALUE_SENSOR  201
//#define MIN_WHITE_VALUE_SENSOR  200
#define MIN_BLACK_VALUE_SENSOR  501
#define MIN_WHITE_VALUE_SENSOR  500
#define STD_PWM getpwm(60);

#define  BLACK_CAL 0
#define WHITE_CAL 1
#define READ_VAL  2

//define pin for push button
#define BUTTON 9

//define pin for LED
#define LED_BACK  10
#define LED_WHITE 11 

// Pin definitions
#define  S2_OUT  12
#define  S3_OUT  13
#define  OE_OUT   8    // LOW = ENABLED 

#define UP_PIN  52
#define DOWN_PIN  53

uint8_t pwm_l;
uint8_t pwm_r;
int s1,s2,s3,s4;
uint8_t postion_zone_f=0;
uint8_t  bookmark_position_zone_r[4];
uint8_t bookmark_position_zone_l[4];
uint8_t sidenumberOfZoneL[4];
uint8_t valcol = 4;
uint8_t colstore[4] = {2,4,3,1};
bool revest_zone_f = false;

L298MD md = L298MD(ENA,N1,N2,N3,N4,ENB);
L298MD hau_md = L298MD(HAU_ENA,HAU_N1,HAU_N2,HAU_N3,HAU_N4,HAU_ENB);

MD_TCS230	CS(S2_OUT, S3_OUT, OE_OUT);

void setup() 
{
  Serial.begin(250000);
  Serial.print(F("\n[TCS230 Calibrator Example]"));
  pinMode(BUTTON,INPUT);
  pinMode(LED_BACK,OUTPUT);
  pinMode(LED_WHITE,OUTPUT);
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  pinMode(LED3,OUTPUT);
  pinMode(LED4,OUTPUT);
  pinMode(UP_PIN,INPUT);
  pinMode(DOWN_PIN,INPUT);
  md.initmotor();
  hau_md.initmotor();
  
  CS.begin();
  
}

void waitforpushbutton() {

  while (1){

    uint8_t count_button = 0;
     bool ispush = false;
     
     if (digitalRead(BUTTON) == 1) {
      
      if (!ispush){
        count_button ++;
        ispush = true;
      }
      
     }

      if (count_button == 1) {
        break;
      }
     
  }

} // for push button

uint8_t fsmReadValue(uint8_t state, uint8_t valType, uint8_t maxReads)
{
	static	uint8_t	selChannel;
	static	uint8_t	readCount;
	static	sensorData	sd;

	switch(state)
	{
	case 0:	// Prompt for the user to start
		Serial.print(F("\n\nReading value for "));
		switch(valType)
		{
		case BLACK_CAL:	Serial.print(F("BLACK calibration"));	break;
		case WHITE_CAL:	Serial.print(F("WHITE calibration"));	break;
		case READ_VAL:	Serial.print(F("DATA"));				break;
		default:		Serial.print(F("??"));					break;
		}
		
		Serial.print(F("\nPress any key to start ..."));
		state++;
		break;

	case 1:	// Wait for user input
    waitforpushbutton();    
		state++;
		break;

	case 2:	// start the reading process
		CS.read();
		state++;
		break;

	case 3:	// wait for a read to complete
		if (CS.available()) 
		{
			sensorData	sd;
			colorData	rgb;

			switch(valType)
			{
			case BLACK_CAL:	
				CS.getRaw(&sd);	
				CS.setDarkCal(&sd);		
        digitalWrite(LED_BACK,HIGH);
				break;

			case WHITE_CAL:	
				CS.getRaw(&sd);	
				CS.setWhiteCal(&sd);	
        digitalWrite(LED_WHITE,HIGH);
				break;

			case READ_VAL:	
				CS.getRGB(&rgb);
				Serial.print(F("\nRGB is ["));
				Serial.print(rgb.value[TCS230_RGB_R]);
				Serial.print(F(","));
				Serial.print(rgb.value[TCS230_RGB_G]);
				Serial.print(F(","));
				Serial.print(rgb.value[TCS230_RGB_B]);
				Serial.print(F("]"));

       float colornumber = convert(rgb.value[TCS230_RGB_R],rgb.value[TCS230_RGB_G],rgb.value[TCS230_RGB_B]);

        Serial.println(colornumber);
       
				break;
			}
			state++;
		}
		break;

	default:	// reset fsm
		state = 0;
		break;
	}

	return(state);
}


void loop() 
{
  static uint8_t	runState = 0;		
  static uint8_t	readState = 0;
  static  bool  iscallup  = false;
  static bool isup = false;

  if (!isup){
    isup = true;
        while(1){
      if (!iscallup) {
         Serial.println("UP");
        hau_md.revertclockA(255);
        Serial.println(digitalRead(UP_PIN));
        iscallup = true;
      }
      if (digitalRead(UP_PIN) == 1) {
        hau_md.motorAstop();
        break;
      }
      
    }
  }


  switch(runState)
  {
  case 0:	// calibrate black
	  readState = fsmReadValue(readState, BLACK_CAL, 2);
	  if (readState == 0) runState++;
	  break;

  case 1:	// calibrate white
	  readState = fsmReadValue(readState, WHITE_CAL, 2);
	  if (readState == 0) runState++;
	  break;

  case 2:	// robot run 
//readState = fsmReadValue(readState, READ_VAL, 1);
    autoRobotRun(); //auto robot run
	  break;

  default:
	  runState = 0;	// start again if we get here as something is wrong
  }

  
} // loop function


//================================================= auto robot run ==================================================================//

  void autoRobotRun() {

    
    static uint8_t zoneState = 0;
    static uint8_t readState = 0;
    
    switch (zoneState){

      case 0 : // forward stange line zone
       readState = robot_do_in_f_zone(readState);
       if (readState == 1 ) zoneState = 1; // R zone 
        if ( readState == 2 ) zoneState = 2; // L zone
         break;
        
      case 1 : // take a box zone or zone R
        readState = robot_do_in_r_zone(readState);
        if ( readState == 3 ) zoneState = 0;
        break;
        
      case 2 : // take out a box zone or zone L
            readState = robot_do_in_l_zone(readState);
            if ( readState == 3 ) zoneState = 0;    
        break;
        
    }

  
  }

 //================================================= F Zone ====================================================//
 
uint8_t robot_do_in_f_zone (uint8_t readState) {

  while (1) {
    
        pid_process(); // pid 

      if ( !revest_zone_f ){
         postion_zone_f +=track_line_run();
         Serial.println("F");
         Serial.println(postion_zone_f);
         if (postion_zone_f == 6) {
           delay(500);
           readState = 1;
            postion_zone_f ++;
            revest_zone_f = true;
            break;
         
         } // switch to zone R

    
      }else {
        
         postion_zone_f -= track_line_run();     
         Serial.println("-L");
         Serial.println(postion_zone_f);    
         
      if ( postion_zone_f == 1 )
      {
         trun_to_L_zone();  
         postion_zone_f = 1;
         revest_zone_f = false;
         readState = 2;
        break;
      } //switch to zone L
                 
      } // revert zone
   
   
 } //while

return (readState);
  
} // robot in the zone F

  
 void trun_to_L_zone () {

   bool call = false;
   bool exitloop = false;
   delay(200);
   md.motorAbreak(200);
   md.motorBbreak(200);
   delay(100);
   while (!exitloop) {
    
    s3 = analogRead(A2); //sensor 4
    if ( !(s3 >=MIN_BLACK_VALUE_SENSOR ) ) {

      if (!call) {
        
       pwm_l = getpwm(40);
       pwm_r = getpwm(50);
       md.forwardclockA(pwm_r);
       md.revertclockB(pwm_l); 
       
       call =true;
       
      }
    
    }else {
      delay(100);
      md.motorAbreak(200);
      md.motorBbreak(200);
     delay(100);
      exitloop =true;
    }
        
   }
      
    
 } // robot trun to zone l
 


//=============================================== End Code Zone F =========================================================//


// ===================================================== Zone L ===========================================================//

uint8_t robot_do_in_l_zone (uint8_t readState) {

  bool revest_zone_L = false;
  uint8_t pos = 0; 
  bool exitloop = false;
  
  while (!exitloop) {
    
      pid_process(); // pid 
     if ( !revest_zone_L ) {
      Serial.println("L");
     
       pos+=track_line_run();   
       Serial.println(pos);
                         
     if ( iswhereboxstay(valcol , pos) ) {

            robot_takeOut_box();  
            revest_zone_L = true;
                      
            }
            
     }else {
             pos -= track_line_run(); 
             Serial.println("go out from zone L");
             Serial.println(pos);
              if ( pos == 0 ) {
                Serial.println("robot trun");
              robot_trun_out_from_zone_l();
              readState = 3; //trun to F zone
              exitloop = true;
            }
          }

    
  } //while

  return (readState);
  
} //robot in the zone L

bool iswhereboxstay(uint8_t valcol,uint8_t pos ){

  if (pos == 0 ){
    return false;
  }
    if ( colstore[pos - 1] == valcol ){
      return true;
    }

  return false;
}



void robot_takeOut_box() {
   uint8_t pos =0;   
   bool position_one_is_actived = false;
   
    trun_to_inside_of_zone_L();
     while(1) {
        pid_process(); // pid 
        pos += track_line_run();
        if ( pos == 1 ) {
          if (!position_one_is_actived) {
            robot_hand_takeOut_box();
            position_one_is_actived = true;
          }
          
        }else if ( pos == 2 ) {
           robot_Go_OutSide_Of_Zone_L(); 
           break;        
        }
      
    }
  
}


void robot_hand_takeOut_box() {

      bool call = false;
      delay(200);
      md.motorAbreak(200);
      md.motorBbreak(200);  
      delay(100);  
          //code hand take out box
      handtakeoutbox();    

    while ( 1 ) { // trun 180
       
    s4 = analogRead(A3); // sensor 1
    if ( !(s4 >=MIN_BLACK_VALUE_SENSOR ) ) {

      if (!call) {
        pwm_l = getpwm(40);
       pwm_r = getpwm(50);
          
    md.revertclockA(pwm_l);  
    md.forwardclockB(pwm_r);
       call =true;
       
      }
    
    }else {
      delay(100);
      md.motorAbreak(200);
      md.motorBbreak(200);
      break;
    }
        
   }
  
}

 void handtakeoutbox(){

    hau_md.forwardclockB(255);
    delay(5000);
    hau_md.motorBstop(); 
    
   }


 void robot_trun_out_from_zone_l() {

    bool call = false;
    delay(200);
     md.motorAbreak(200);
     md.motorBbreak(200); 
    delay(100);
     while ( 1 ) {
       
    s4 = analogRead(A3); // sensor 1
    if ( !(s4 >=MIN_BLACK_VALUE_SENSOR ) ) {

      if (!call) {
        pwm_l = getpwm(40);
       pwm_r = getpwm(50);
          
    md.revertclockA(pwm_l);  
    md.forwardclockB(pwm_r);
       call =true;
       
      }
    
    }else {
      delay(100);
      md.motorAbreak(200);
      md.motorBbreak(200);
      break;
    }
        
   }
  
} // trun out from zone l

void robot_Go_OutSide_Of_Zone_L(){

    bool call = false;
    bool exitloop = false;
    delay(300);
    md.motorAbreak(200);
     md.motorBbreak(200); 
      delay(100);
     
   while (!exitloop) {
      
    s3 = analogRead(A2); //sensor 4
    if ( !(s3 >=MIN_BLACK_VALUE_SENSOR ) ) {

      if (!call) {
        pwm_l = getpwm(40);
       pwm_r = getpwm(50);
          
      md.forwardclockA(pwm_r);
      md.revertclockB(pwm_l); 
       call =true;
       
      }
    
    }else {
      delay(100);
      md.motorAbreak(200);
      md.motorBbreak(200);
      delay(100);
      exitloop = true;
    }
        
   }
     
}

  void trun_to_inside_of_zone_L() {

  bool call = false;
   bool exitloop = false;
    delay(200);
   md.motorAbreak(200);
   md.motorBbreak(200);
   delay(100);
   
  while (!exitloop) {
      
    s4 = analogRead(A3); // sensor 1
    if ( !(s4 >=MIN_BLACK_VALUE_SENSOR ) ) {

      if (!call) {
        pwm_l = getpwm(40);
       pwm_r = getpwm(50);
          
    md.revertclockA(pwm_l);  
    md.forwardclockB(pwm_r);
       call =true;
       
      }
    
    }else {
      delay(100);
      md.motorAbreak(200);
      md.motorBbreak(200);
      delay(100);
      exitloop = true;
    }
        
   }
    
  }

  
 //=================================== END CODE OF ZONE L ===============================================//  

  
 //==================================== R Zone ============================================================//

 uint8_t robot_do_in_r_zone (uint8_t readState) {

  uint8_t pos = 0; 
  bool revest_zone_r = false;
   
  while (1) {
        
        pid_process(); // pid 

      if ( !revest_zone_r ) {
          Serial.println("R");
          pos += track_line_run();
          Serial.println(pos);
        if (getactivePositionInZoneR(pos)) {
          //position is active for take a box
            if (robotgiveBox()){
              Serial.println("robot do in position active");
              Serial.print(pos);
              Serial.println(" ");
              addbookmakposition(pos);
              revest_zone_r = true;        
            }
     
        }
        
      }else {
           Serial.println("-R");
        pos -=track_line_run();
        if ( pos == 1 ) {
        readState =3; //trun to zone F
        break;
                
      }
      
      }

        
  }

  return (readState);
  
} // robot in the zone R

  bool robotgiveBox(){
   uint8_t pos; 
   bool do_work_success = false;
   bool isactiveposition_one = false;
   
      trun_to_inside_box();
      while (1) {
         pid_process(); // pid 
        pos += track_line_run();
        if ( pos == 1 ) {
          if (!isactiveposition_one) {
            Serial.println(" in side of R zone position");
            Serial.print(pos);
            robot_Step_GeveBox(); // robot gave a box
            isactiveposition_one = true;
          }
          
          } else if ( pos == 2) {
            Serial.println(" in side of R zone position");
            Serial.print(pos);
          robot_Step_Go_Outside(); //robot go outside 
          do_work_success =true ;
          break;
          
        }
        
      }
    return (do_work_success);
  } // robot give a box in zone R

void trun_to_inside_box () {
      bool call = false;
        delay(100);
          md.motorAbreak(200);
         md.motorBbreak(200);
        delay(100);
     while (1) {
        
    s3 = analogRead(A2); //sensor 4
    if ( !(s3 >=MIN_BLACK_VALUE_SENSOR ) ) {

      if (!call) {
       pwm_l = getpwm(40);
       pwm_r = getpwm(50);
          
      md.forwardclockA(pwm_r);
      md.revertclockB(pwm_l); 
       call =true;
       
      }
    
    }else {
      delay(100);
      md.motorAbreak(200);
      md.motorBbreak(200);
      break;
    }
        
   }

}

void robot_Step_Go_Outside () {

      bool call = false;
      delay(400);
       md.motorAbreak(200);
      md.motorBbreak(200);
     delay(100);
     
     while ( 1 ) {
      
    s4 = analogRead(A3); // sensor 1
    if ( !(s4 >=MIN_BLACK_VALUE_SENSOR ) ) {

      if (!call) {
       pwm_l = getpwm(50);
       pwm_r = getpwm(40);
          
      md.revertclockA(pwm_l);  
      md.forwardclockB(pwm_r); 
       call =true;
       
      }
    
    }else {
      delay(100);
      md.motorAbreak(200);
      md.motorBbreak(200);
      break;
    }
        
   }
   
     
} // robot go out side

void robot_Step_GeveBox () {

    bool call = false;
     delay(200);
      md.motorAbreak(200);
      md.motorBbreak(200);
      delay(100);
     // code run a hand for get a box here
      giveboxAndDetacColor();

      Serial.println("step give box");
         while ( 1 ) {
       
    s3 = analogRead(A2); //sensor 4
    Serial.println(s3);
    if ( !(s3 >=MIN_BLACK_VALUE_SENSOR ) ) {

      if (!call) {
       pwm_l = getpwm(50);
       pwm_r = getpwm(40);
          
      md.forwardclockA(pwm_r);
      md.revertclockB(pwm_l); 
       call =true;
       
      }
    
    }else {
      delay(100);
      md.motorAbreak(200);
      md.motorBbreak(200);
      break;
    }
        
   }
   
  
} // robot give box

  void giveboxAndDetacColor() {
    
    bool iscalldown = false;
    bool  iscallup  = false;   
      
   while(1) {
    if (!iscalldown){
      hau_md.forwardclockA(255); // down
      iscalldown = true;
    }
    if (digitalRead(DOWN_PIN) == 1) {
      hau_md.motorAstop();
      break;
    }
    
   }
         
    hau_md.revertclockB(255); 
    delay(6000);
    hau_md.motorBstop();
    detaccolor();
    delay(100);
    while(1){
      if (!iscallup) {
        hau_md.revertclockA(255);
        iscallup = true;
      }
      if (digitalRead(UP_PIN) == 1) {
        hau_md.motorAstop();
        break;
      }
      
    }
        
  }


  void detaccolor() {

    CS.read();

    while (1) {
   if (CS.available()) 
    {
     colorData rgb;
      CS.getRGB(&rgb);
      float colornumber = convert(rgb.value[TCS230_RGB_R],rgb.value[TCS230_RGB_G],rgb.value[TCS230_RGB_B]);
      Serial.println(colornumber);
      valcol = colornumber;
       digitalWrite(LED_BACK,LOW);
        digitalWrite(LED_WHITE,LOW);
      if ( valcol == 1 ){
        digitalWrite(LED_BACK,HIGH);
        digitalWrite(LED_WHITE,HIGH);
        delay(100);
        digitalWrite(LED_BACK,LOW);
        digitalWrite(LED_WHITE,LOW);
      }else if (valcol == 2) {

         digitalWrite(LED_BACK,HIGH);
        digitalWrite(LED_WHITE,HIGH);
        delay(100);
        digitalWrite(LED_BACK,LOW);
        digitalWrite(LED_WHITE,LOW);
        delay(100);
        digitalWrite(LED_BACK,HIGH);
        digitalWrite(LED_WHITE,HIGH);
        delay(100);
        digitalWrite(LED_BACK,LOW);
        digitalWrite(LED_WHITE,LOW);
      }else if (valcol ==3) {
          digitalWrite(LED_BACK,HIGH);
        digitalWrite(LED_WHITE,HIGH);
        delay(100);
        digitalWrite(LED_BACK,LOW);
        digitalWrite(LED_WHITE,LOW);
        delay(100);
        digitalWrite(LED_BACK,HIGH);
        digitalWrite(LED_WHITE,HIGH);
        delay(100);
        digitalWrite(LED_BACK,LOW);
        digitalWrite(LED_WHITE,LOW);
        delay(100);
        digitalWrite(LED_BACK,HIGH);
        digitalWrite(LED_WHITE,HIGH);
        delay(100);
        digitalWrite(LED_BACK,LOW);
        digitalWrite(LED_WHITE,LOW);
        
      }else if ( valcol == 4 ) {

           digitalWrite(LED_BACK,HIGH);
        digitalWrite(LED_WHITE,HIGH);
        delay(100);
        digitalWrite(LED_BACK,LOW);
        digitalWrite(LED_WHITE,LOW);
        delay(100);
        digitalWrite(LED_BACK,HIGH);
        digitalWrite(LED_WHITE,HIGH);
        delay(100);
        digitalWrite(LED_BACK,LOW);
        digitalWrite(LED_WHITE,LOW);
        delay(100);
        digitalWrite(LED_BACK,HIGH);
        digitalWrite(LED_WHITE,HIGH);
        delay(100);
        digitalWrite(LED_BACK,LOW);
        digitalWrite(LED_WHITE,LOW);
        delay(100);
        digitalWrite(LED_BACK,HIGH);
        digitalWrite(LED_WHITE,HIGH);
        delay(100);
        digitalWrite(LED_BACK,LOW);
        digitalWrite(LED_WHITE,LOW);
        
      }
      
      break;
    }
    
    }
    
  }
    

 bool getactivePositionInZoneR(uint8_t positionzoneR){

  if (positionzoneR == 0) {
    return false;
  }
  
  for (uint8_t i=0 ; i < sizeof (bookmark_position_zone_r) -1 ; i ++ ) {
        if ( positionzoneR != 0 && bookmark_position_zone_r[i] == positionzoneR){
          return false;
        }
  }

 return true;
  
}

 void addbookmakposition(uint8_t pos){

      for (uint8_t i =0 ; i < sizeof(bookmark_position_zone_r) - 1 ; i ++) {
          
        if ( bookmark_position_zone_r[i] == 0 ){
          bookmark_position_zone_r[i] = pos;
          return;
        }
        
      }
          
  } //add book maks position

  //========================================= End Code Zoon R ============================================================//


uint8_t track_line_run(){ //line flow

      static bool robot_is_black_line = false;
       uint8_t positioncount =0 ;
      
    if ( (s4 <=MIN_WHITE_VALUE_SENSOR ) && (s1 >=MIN_BLACK_VALUE_SENSOR ) && (s2 >=MIN_BLACK_VALUE_SENSOR) && (s3 <= MIN_WHITE_VALUE_SENSOR ) ){ // 0 1 1 0
            //go a head
            digitalWrite(LED1,LOW);
            digitalWrite(LED2,HIGH);
            digitalWrite(LED3,HIGH);
            digitalWrite(LED4,LOW);
            
            pwm_l = STD_PWM;
            pwm_r = STD_PWM;
            
            md.forwardclockA(pwm_l);
            md.forwardclockB(pwm_r);

            robot_is_black_line =false;
            
       } else if ( (s4 <=MIN_WHITE_VALUE_SENSOR ) && (s1 >=MIN_BLACK_VALUE_SENSOR ) && (s2 <= MIN_WHITE_VALUE_SENSOR) && (s3 <= MIN_WHITE_VALUE_SENSOR ) ){ // 0 1 0 0     
          //trun left
           digitalWrite(LED1,LOW);
            digitalWrite(LED2,HIGH);
            digitalWrite(LED3,LOW);
            digitalWrite(LED4,LOW);
          pwm_l = getpwm(50);
          pwm_r = getpwm(50);
          
           md.revertclockA(pwm_l);  
           md.forwardclockB(pwm_r);

           robot_is_black_line =false;
           
     } else if ( (s4 <=MIN_WHITE_VALUE_SENSOR ) && (s1 <= MIN_WHITE_VALUE_SENSOR ) && (s2 >= MIN_BLACK_VALUE_SENSOR) && (s3 <= MIN_WHITE_VALUE_SENSOR ) ){ // 0 0 1 0
           //trun rigth

            digitalWrite(LED1,LOW);
            digitalWrite(LED2,LOW);
            digitalWrite(LED3,HIGH);
            digitalWrite(LED4,LOW);
            
           pwm_l = getpwm(50);
          pwm_r = getpwm(50);
          
           md.forwardclockA(pwm_l);
           md.revertclockB(pwm_r);     

           robot_is_black_line =false;
              
    }else if ( (s4 <=MIN_WHITE_VALUE_SENSOR ) && (s1 <= MIN_WHITE_VALUE_SENSOR ) && (s2 >= MIN_BLACK_VALUE_SENSOR) && (s3 >= MIN_BLACK_VALUE_SENSOR ) ){ // 0 0 1 1

          // rigth

           digitalWrite(LED1,LOW);
            digitalWrite(LED2,LOW);
            digitalWrite(LED3,HIGH);
            digitalWrite(LED4,HIGH);
         pwm_l = getpwm(50);
          pwm_r = getpwm(50);
          
          md.forwardclockA(pwm_r);
           md.revertclockB(pwm_l); 
           robot_is_black_line =false;
           
    } else if ( (s4 >= MIN_BLACK_VALUE_SENSOR ) && (s1 >= MIN_BLACK_VALUE_SENSOR ) && (s2 <= MIN_WHITE_VALUE_SENSOR) && (s3 <= MIN_WHITE_VALUE_SENSOR ) ) { // 1 1 0 0

        //left
         digitalWrite(LED1,HIGH);
            digitalWrite(LED2,HIGH);
            digitalWrite(LED3,LOW);
            digitalWrite(LED4,LOW);
          pwm_l = getpwm(50);
          pwm_r = getpwm(50);
          
           md.revertclockA(pwm_l);  
           md.forwardclockB(pwm_r);
           robot_is_black_line =false;
           
    } else if ( (s4 >= MIN_BLACK_VALUE_SENSOR ) && (s1 <= MIN_WHITE_VALUE_SENSOR ) && (s2 <= MIN_WHITE_VALUE_SENSOR) && (s3 <= MIN_WHITE_VALUE_SENSOR ) ) { // 1 0 0 0

        //too left
            digitalWrite(LED1,HIGH);
            digitalWrite(LED2,LOW);
            digitalWrite(LED3,LOW);
            digitalWrite(LED4,LOW);
         pwm_l = getpwm(50);
          pwm_r = getpwm(60);
          
           md.revertclockA(pwm_l);  
           md.forwardclockB(pwm_r);
           robot_is_black_line =false;
            
    } else if ( (s4 <= MIN_WHITE_VALUE_SENSOR ) && (s1 <= MIN_WHITE_VALUE_SENSOR ) && (s2 <= MIN_WHITE_VALUE_SENSOR) && (s3 >= MIN_BLACK_VALUE_SENSOR ) ) { // 0 0 0 1
      //too rigth
       digitalWrite(LED1,LOW);
            digitalWrite(LED2,LOW);
            digitalWrite(LED3,LOW);
            digitalWrite(LED4,HIGH);
           pwm_l = getpwm(60);
          pwm_r = getpwm(50);
          
           md.forwardclockA(pwm_r);
           md.revertclockB(pwm_l); 
            robot_is_black_line =false;
            
    } else if ( (s4 <= MIN_WHITE_VALUE_SENSOR ) && (s1 <= MIN_WHITE_VALUE_SENSOR ) && (s2 <= MIN_WHITE_VALUE_SENSOR) && (s3 <= MIN_WHITE_VALUE_SENSOR ) ) { // 0 0 0 0
         digitalWrite(LED1,LOW);
            digitalWrite(LED2,LOW);
            digitalWrite(LED3,LOW);
            digitalWrite(LED4,LOW);
        md.motorAstop();
        md.motorBstop();
        robot_is_black_line =false;
        
    } else if ( (s4 >=MIN_BLACK_VALUE_SENSOR ) && (s1 >=MIN_BLACK_VALUE_SENSOR ) && (s2 >=MIN_BLACK_VALUE_SENSOR) && (s3 >=MIN_BLACK_VALUE_SENSOR ) ) { // 1 1 1 1

           digitalWrite(LED1,HIGH);
            digitalWrite(LED2,HIGH);
            digitalWrite(LED3,HIGH);
            digitalWrite(LED4,HIGH);
             pwm_l = STD_PWM;
            pwm_r = STD_PWM;
            
            md.forwardclockA(pwm_l);
            md.forwardclockB(pwm_r);

        if ( !robot_is_black_line ) {
           positioncount +=1;
           robot_is_black_line = true;
        }
                 

    } else if ( (s4 <=MIN_WHITE_VALUE_SENSOR ) && (s1 >=MIN_BLACK_VALUE_SENSOR ) && (s2 >=MIN_BLACK_VALUE_SENSOR) && (s3 >=MIN_BLACK_VALUE_SENSOR ) ) { //0 1 1 1

            digitalWrite(LED1,LOW);
            digitalWrite(LED2,HIGH);
            digitalWrite(LED3,HIGH);
            digitalWrite(LED4,HIGH);

//            pwm_l = getpwm(50);
//            pwm_r = getpwm(40);
//          
//           md.forwardclockA(pwm_r);
//           md.revertclockB(pwm_l); 
//           robot_is_black_line =false;
//           delay(200);
      
    }else if ( (s4 >=MIN_BLACK_VALUE_SENSOR ) && (s1 >=MIN_BLACK_VALUE_SENSOR ) && (s2 >=MIN_BLACK_VALUE_SENSOR) && (s3 <=MIN_WHITE_VALUE_SENSOR ) ) { // 1 1 1 0

            digitalWrite(LED1,HIGH);
            digitalWrite(LED2,HIGH);
            digitalWrite(LED3,HIGH);
            digitalWrite(LED4,LOW);
//            
//          pwm_l = getpwm(40);
//          pwm_r = getpwm(50);
//          
//           md.revertclockA(pwm_l);  
//           md.forwardclockB(pwm_r);
//           robot_is_black_line =false;
//           delay(200);
           
    }

    return (positioncount);

}



void pid_process(){

    s1 = analogRead(A0); // sensor 2
    s2 = analogRead(A1); // sensor 3
    s3 = analogRead(A2); //sensor 4
    s4 = analogRead(A3); // sensor 1
     
}

uint8_t getpwm(uint8_t pwmpersen) {
return (pwmpersen * 255) / 100;
}

//======================================================= END AUTO ROBOT ===================================================//


 // ========================================== Function Convert RGB to Number ============================================ //

float convert(float r,float g,float b) 
{
  float fmax,fmin,l,s,d;
  float h;
 
  r=r/255;
  g=g/255;
  b=b/255;
   fmax=max(r,max(g,b));
   fmin=min(r,min(g,b));
   Serial.print("r:");  Serial.print(r); Serial.print("g:");  Serial.print(g); Serial.print("b:");  Serial.print(b);
  Serial.print("\nmax:");  Serial.print(fmax);
    Serial.print("\nmin:");  Serial.print(fmin);
   l=(fmin+fmax)/2;    
    Serial.print("\nl:");  Serial.print(l);
    if(l<0.5){
      s=(fmax-fmin)/(fmax+fmin);
    }
    else{
        s=(fmax-fmin)/(2.0-fmax+fmin);
    }
       Serial.print("\ns:");  Serial.print(s);
       
    if(fmax==r)
    {  Serial.print("\n if R");
        float t2;   float t1;
     t1=g-b;
     Serial.print("\nfmax:");  Serial.print(fmax);
     Serial.print("\nfmin:");  Serial.print(fmin);
     t2=fmax-fmin;
      Serial.print("\nt1:");  Serial.print(t1);
        Serial.print("\nt2:");  Serial.print(t2);
      h=t1/t2;
          Serial.print("\nsh:");  Serial.print(h);
    }
      else if(fmax==g)
    {
      Serial.print("\n if G");
          h=2+((b-r)/(fmax-fmin));
    }
     else if(fmax==b)
    {
      Serial.print("\n if B");
          h=4+((r-g)/(fmax-fmin));
    }
       Serial.print("\n h:");  Serial.print(h);
    h=h*60;   
    if(h<0){
      h=+360;
      }
    Serial.print("\n h*60:");  Serial.print(h);
    Serial.print("\n\n");

if((0<=h && h<=30) || (300<=h && h<=365))
{
  //red

   Serial.println("Red");
return 1; 
}
else if(30<h&&h<=60)
{
   Serial.println("Yell");
  //yel
return 2; 
}
else if(60<h&&h<170)
{
  Serial.println("Gred");
  //gre
return 3; 
}
else if(170<h&&h<270)
{
   Serial.println("blue");
  //blue
return 4; 
}
else{
   Serial.println("NNNNNNNNNNNNNNNN");
  return 0;
}
  
}

 // ================================================ END FUNCtION =====================================================================//


