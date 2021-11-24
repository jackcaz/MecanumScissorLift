#include <PS2X_lib.h>
#include <Servo.h>

#define PWMA 12    
#define DIRA1 34 
#define DIRA2 35  
#define PWMB 8    
#define DIRB1 37 
#define DIRB2 36  
#define PWMC 9   
#define DIRC1 43 
#define DIRC2 42 
#define PWMD 5   
#define DIRD1 A4 
#define DIRD2 A5 

#define PWMRELAY1 45  //PL4
#define PWMRELAY2 44 //PL5

#define PWMRELAY3 4  //PL4
#define PWMRELAY4 6 //PL5

//Servo
Servo myservo;
int pos = 0;
int r =0;
//PS2
#define PS2_DAT        52  //14    
#define PS2_CMD        51  //15
#define PS2_SEL        53  //16
#define PS2_CLK        50  //17

char speed;
// #define pressures   true
#define pressures   false
// #define rumble      true
#define rumble      false
PS2X ps2x; // create PS2 Controller Class

int error = 0;
byte type = 0;
byte vibrate = 0;

void (* resetFunc) (void) = 0;

#define MOTORA_FORWARD(pwm)    do{digitalWrite(DIRA1,LOW); digitalWrite(DIRA2,HIGH);analogWrite(PWMA,pwm);}while(0)
#define MOTORA_STOP(x)         do{digitalWrite(DIRA1,LOW); digitalWrite(DIRA2,LOW); analogWrite(PWMA,0);}while(0)
#define MOTORA_BACKOFF(pwm)    do{digitalWrite(DIRA1,HIGH);digitalWrite(DIRA2,LOW); analogWrite(PWMA,pwm);}while(0)

#define MOTORB_FORWARD(pwm)    do{digitalWrite(DIRB1,HIGH); digitalWrite(DIRB2,LOW);analogWrite(PWMB,pwm);}while(0)
#define MOTORB_STOP(x)         do{digitalWrite(DIRB1,LOW); digitalWrite(DIRB2,LOW); analogWrite(PWMB,0);}while(0)
#define MOTORB_BACKOFF(pwm)    do{digitalWrite(DIRB1,LOW);digitalWrite(DIRB2,HIGH); analogWrite(PWMB,pwm);}while(0)

#define MOTORC_FORWARD(pwm)    do{digitalWrite(DIRC1,LOW); digitalWrite(DIRC2,HIGH);analogWrite(PWMC,pwm);}while(0)
#define MOTORC_STOP(x)         do{digitalWrite(DIRC1,LOW); digitalWrite(DIRC2,LOW); analogWrite(PWMC,0);}while(0)
#define MOTORC_BACKOFF(pwm)    do{digitalWrite(DIRC1,HIGH);digitalWrite(DIRC2,LOW); analogWrite(PWMC,pwm);}while(0)

#define MOTORD_FORWARD(pwm)    do{digitalWrite(DIRD1,HIGH); digitalWrite(DIRD2,LOW);analogWrite(PWMD,pwm);}while(0)
#define MOTORD_STOP(x)         do{digitalWrite(DIRD1,LOW); digitalWrite(DIRD2,LOW); analogWrite(PWMD,0);}while(0)
#define MOTORD_BACKOFF(pwm)    do{digitalWrite(DIRD1,LOW);digitalWrite(DIRD2,HIGH); analogWrite(PWMD,pwm);}while(0)

#define SERIAL  Serial

#define LOG_DEBUG

#ifdef LOG_DEBUG
#define M_LOG SERIAL.print
#else
#define M_LOG 
#endif

#define MAX_PWM   200
#define MIN_PWM   130
int Motor_PWM = 130;


//    ↑A-----B↑   
//     |  ↑  |
//     |  |  |
//    ↑C-----D↑
void ADVANCE()
{
  MOTORA_FORWARD(Motor_PWM);MOTORB_FORWARD(Motor_PWM);    
  MOTORC_FORWARD(Motor_PWM);MOTORD_FORWARD(Motor_PWM);    
}

//    ↓A-----B↓   
//     |  |  |
//     |  ↓  |
//    ↓C-----D↓
void BACK()
{
  MOTORA_BACKOFF(Motor_PWM);MOTORB_BACKOFF(Motor_PWM);
  MOTORC_BACKOFF(Motor_PWM);MOTORD_BACKOFF(Motor_PWM);
}
//    =A-----B↑   
//     |   ↖ |
//     | ↖   |
//    ↑C-----D=
void LEFT_1()
{
  MOTORA_STOP(Motor_PWM);MOTORB_FORWARD(Motor_PWM);
  MOTORC_FORWARD(Motor_PWM);MOTORD_STOP(Motor_PWM);
}

//    ↓A-----B↑   
//     |  ←  |
//     |  ←  |
//    ↑C-----D↓
void LEFT_2()
{
  MOTORA_BACKOFF(Motor_PWM);MOTORB_FORWARD(Motor_PWM);
  MOTORC_FORWARD(Motor_PWM);MOTORD_BACKOFF(Motor_PWM);
}
//    ↓A-----B=   
//     | ↙   |
//     |   ↙ |
//    =C-----D↓
void LEFT_3()
{
  MOTORA_BACKOFF(Motor_PWM);MOTORB_STOP(Motor_PWM);
  MOTORC_STOP(Motor_PWM);MOTORD_BACKOFF(Motor_PWM);
}
//    ↑A-----B=   
//     | ↗   |
//     |   ↗ |
//    =C-----D↑
void RIGHT_1()
{
  MOTORA_FORWARD(Motor_PWM);MOTORB_STOP(Motor_PWM);
  MOTORC_STOP(Motor_PWM);MOTORD_FORWARD(Motor_PWM);
}
//    ↑A-----B↓   
//     |  →  |
//     |  →  |
//    ↓C-----D↑
void RIGHT_2()
{
  MOTORA_FORWARD(Motor_PWM);MOTORB_BACKOFF(Motor_PWM);
  MOTORC_BACKOFF(Motor_PWM);MOTORD_FORWARD(Motor_PWM);
}
//    =A-----B↓   
//     |   ↘ |
//     | ↘   |
//    ↓C-----D=
void RIGHT_3()
{
  MOTORA_STOP(Motor_PWM);MOTORB_BACKOFF(Motor_PWM);
  MOTORC_BACKOFF(Motor_PWM);MOTORD_STOP(Motor_PWM);
}
// ↓A-----B↑
//  |     |
//  |     |
// ↓C-----D↑
void TURN_LEFT()
{
  MOTORA_BACKOFF(Motor_PWM);MOTORB_FORWARD(Motor_PWM);
  MOTORC_BACKOFF(Motor_PWM);MOTORD_FORWARD(Motor_PWM);  
}
// ↑A-----B↓
//  |     |
//  |     |
// ↑C-----D↓
void TURN_RIGHT()
{
  MOTORA_FORWARD(Motor_PWM);MOTORB_BACKOFF(Motor_PWM);
  MOTORC_FORWARD(Motor_PWM);MOTORD_BACKOFF(Motor_PWM);  
}
//    =A-----B=  
//     |  =  |
//     |  =  |
//    =C-----D=
void STOP()
{
  MOTORA_STOP(Motor_PWM);MOTORB_STOP(Motor_PWM);
  MOTORC_STOP(Motor_PWM);MOTORD_STOP(Motor_PWM);
}
void UART_Control()
{
  char Uart_Date=0;
 if(SERIAL.available())
  {
   Uart_Date = SERIAL.read();
  }
  switch(Uart_Date)
  {
     case 'A':  ADVANCE(); M_LOG("Run!\r\n");        break;
     case 'B':  RIGHT_1();  M_LOG("Right up!\r\n");     break;
     case 'C':  RIGHT_2();  M_LOG("Right!\r\n");        break;
     case 'D':  RIGHT_3();  M_LOG("Right down!\r\n");   break;
     case 'E':  BACK();     M_LOG("Run!\r\n");        break;
     case 'F':  LEFT_3();   M_LOG("Left down!\r\n");    break;
     case 'G':  LEFT_2();   M_LOG("Left!\r\n");       break;
     case 'H':  LEFT_1();   M_LOG("Left up!\r\n");  break;
     case 'Z':  STOP();     M_LOG("Stop!\r\n");       break;
     case 'L':  Motor_PWM = 240;                      break;
     case 'M':  Motor_PWM = 130;                       break;
   }
}
void IO_init()
{
//motor pins
  pinMode(PWMA, OUTPUT);
  pinMode(DIRA1, OUTPUT);
  pinMode(DIRA2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(DIRB1, OUTPUT);
  pinMode(DIRB2, OUTPUT);
  pinMode(PWMC, OUTPUT);
  pinMode(DIRC1, OUTPUT);
  pinMode(DIRC2, OUTPUT);
  pinMode(PWMD, OUTPUT);
  pinMode(DIRD1, OUTPUT);
  pinMode(DIRD2, OUTPUT);

//relay pins
  pinMode(PL1, OUTPUT);
  pinMode(PL2, OUTPUT);

  
  STOP();
}
void setup()
{
  //define servo
  myservo.attach(46);   
  
  Serial.begin(9600);
  delay(300) ;//added delay to give wireless ps2 module some time to startup, before configuring it
  //CHANGES for v1.6 HERE!!! **************PAY ATTENTION*************

  //setup pins and settings: GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);

  if (error == 0) {
    Serial.print("Found Controller, configured successful ");
    Serial.print("pressures = ");
    if (pressures)
      Serial.println("true ");
    else
      Serial.println("false");
    Serial.print("rumble = ");
    if (rumble)
      Serial.println("true)");
    else
      Serial.println("false");
    Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
    Serial.println("holding L1 or R1 will print out the analog stick values.");
    Serial.println("Note: Go to www.billporter.info for updates and to report bugs.");
  }
  else if (error == 1)
  {
    Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");
    resetFunc();
  }

  else if (error == 2)
    Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");

  else if (error == 3)
    Serial.println("Controller refusing to enter Pressures mode, may not support it. ");

//  Serial.print(ps2x.Analog(1), HEX);

  type = ps2x.readType();
  switch (type) {
  case 0:
    Serial.print("Unknown Controller type found ");
    break;
  case 1:
    Serial.print("DualShock Controller found ");
    break;
  case 2:
    Serial.print("GuitarHero Controller found ");
    break;
  case 3:
    Serial.print("Wireless Sony DualShock Controller found ");
    break;
  }
  IO_init();
  
//  SERIAL.print("Start");
}

void loop() {
  /* You must Read Gamepad to get new values and set vibration values
    ps2x.read_gamepad(small motor on/off, larger motor strenght from 0-255)
    if you don't enable the rumble, use ps2x.read_gamepad(); with no values
    You should call this at least once a second
  */
//Relay start values = relay off---------------------------------------------------------------------------------------------
  analogWrite(PWMRELAY1, 255);
  analogWrite(PWMRELAY2, 255);
  analogWrite(PWMRELAY3, 255);
  analogWrite(PWMRELAY4, 255);
//---------------------------------------------------------------------------------------------------------------------
  
  UART_Control();
  if (error == 1) //skip loop if no controller found
    return;

  if (type == 2) { //Guitar Hero Controller
    return;
  }
  else  { //DualShock Controller
    ps2x.read_gamepad(false, vibrate); //read controller and set large motor to spin at 'vibrate' speed

// Stop
    if (ps2x.Button(PSB_SELECT)) {
      Serial.println("stop");
      STOP();
    }

//start
    if (ps2x.Button(PSB_START))  {
      Serial.println("Start is being held");
     Motor_PWM = 90;
      ADVANCE();
    }
    if (ps2x.Button(PSB_PAD_UP)) {
      Serial.println("Up held this hard: ");
      Motor_PWM = 120;
     ADVANCE();
    }

    if (ps2x.Button(PSB_PAD_DOWN)) {
      Serial.println("Down held this hard: ");
       Motor_PWM = 120;
      BACK();
    }

  if (ps2x.Button(PSB_PAD_LEFT)){
    
     Serial.println("motor_pmove_left");
     Motor_PWM = 120;//200
     LEFT_2();
     delay(20);
   // Motor_PWM = 120;//200
   // LEFT_1();
  }
//
  if (ps2x.Button(PSB_PAD_RIGHT)) {

    Serial.println("motor_pmove_right");
    Motor_PWM = 120;//200
    RIGHT_2();
    delay(20);
    //Serial.println("turn right");
    //Motor_PWM = 120;//200
    //RIGHT_1();
  }
if (ps2x.Button(PSB_R2)){
  r = r+1;
}
   if (ps2x.Button(PSB_R2)&& r>20&&ps2x.Button(PSB_L2)) {
      Serial.println("Down held this hard: ");
      analogWrite(PWMRELAY4, 0);
      delay (500);
      analogWrite(PWMRELAY3, 0);
      delay(20);
      analogWrite(PWMRELAY3, 255);
      delay(300);
      r = 0;

  
      
    }


  
//Actuator Up and down-------------------------------------------------------------------------------------------------
// Turn left
    if(ps2x.Button(PSB_GREEN)) {
     analogWrite(PWMRELAY1, 0);
     Serial.println("scissor up");
     
      //Serial.println("turn left");
      //TURN_LEFT();
    }
// Turn right
    if(ps2x.Button(PSB_BLUE)) {
      analogWrite(PWMRELAY2, 0);
      Serial.println("scissor down");
      //Serial.println("turn right");
      //TURN_RIGHT();
    }

//Servo Buttons---------------------------------------------------------------------------------------------
    if (ps2x.Button(PSB_PINK)) {    
      pos = pos + 1;
      myservo.write(0);
      delay(3000);
      Serial.println("servo move backward");
      
      //Serial.println("motor_pmove_left");
      //LEFT_2();
    }
    if (ps2x.Button(PSB_RED)) {
      pos = pos - 1;
      myservo.write(100);
      delay(3000);
      Serial.println("servo move forward");
      
      //Serial.println("motor_pmove_right");
      //RIGHT_2();
     }
    delay(20);

  }

  //-----------------------------------------------------------------------------------------------------

    Serial.print("Stick Values:");
    Serial.print(ps2x.Analog(PSS_LY), DEC); //Left stick, Y axis. Other options: LX, RY, RX
    Serial.print(",");
    Serial.print(ps2x.Analog(PSS_LX), DEC);
    Serial.print(",");
    Serial.print(ps2x.Analog(PSS_RY), DEC);
    Serial.print(",");
    Serial.println(ps2x.Analog(PSS_RX), DEC);

    int LY = ps2x.Analog(PSS_LY);
    int LX = ps2x.Analog(PSS_LX);
    int RY = ps2x.Analog(PSS_RY);
    int RX = ps2x.Analog(PSS_RX);

    if (  (LY<124 && LX>((127/255)*LY+64) && LX < ((-127/255)*LY+191))  ||  (LY<124 && LX==((127/255)*LY+64))   ) 
    {
     Motor_PWM = 0.8 * (127 - LY);
      ADVANCE();
      delay(20);
      Serial.print("1");

    }

    if (   (LY<124 && LX>((-127/255)*LY+191) && LX<((-255/127)*LY+(48705/127)))  ||  (LY<124  &&  LX==((-127/255)*LY+191))   )
    {
     Motor_PWM = 0.8 * (LX-64-LY);
      RIGHT_1();
      delay(20);
      Serial.print("2");
    }
    
    if ( (LX>130 && LX>((-255/127)*LY+(48705/127)) && LX>((255/127)*LY+(-16320/127)))  ||  (LX>130  &&  LX==((-255/127)*LY+(48705/127))) )
    {
     Motor_PWM = 0.8 * (LX-127);
      RIGHT_2();
      delay(20);
      Serial.print("3");
    }

    if (   (LY>130 && LX<((255/127)*LY+(-16320/127)) && LX>((-127/255)*LY+191)) ||   (LY>130   &&   (LX==((255/127)*LY+(-16320/127))))     )
    {
     Motor_PWM = 0.8 * (LY+LX-319);
      RIGHT_3();
      delay(20);
      Serial.print("4");
    }

//-----------------------------------------------------------------------------------------------------------------------------------------

    if (    (LY>130 && LX>((127/255)*LY+64)  &&  LX<((-127/255)*LY+191))  ||   (LY>130   &&   (LX==((127/255)*LY+64)))    ) 
    {
     Motor_PWM = 0.8 * (LY-127);
      BACK();
      delay(20);
      Serial.print("5");
    }

    if (     (LY>130 && LX<((127/255)*LY+64)   &&   LX>((-255/127)*LY+(48705/127)))  ||  (LY>130   &&   (LX==((-127/255)*LY+191)))    )
    {
     Motor_PWM = 0.8 * (LY-64-LX);
      LEFT_3();
      delay(20);
      Serial.print("6");
    }
    
    if (    (LX<124 && LX<((-255/127)*LY+(48705/127))  &&   LX<((255/127)*LY+(-16320/127)))   ||   (LX<124   &&    (LX==((-255/127)*LY+(48705/127))))   )
    {
     Motor_PWM = 0.8 * (127-LX);
      LEFT_2();
      delay(20);
      Serial.print("7");
    }

    if (    (LY<124 && LX>((255/127)*LY+(-16320/127))   &&   LX<((127/255)*LY+64))   ||   (LY<124   &&   (LX==((255/127)*LY+(-16320/127))))     )
    {
     Motor_PWM = 0.8 * (LY+LX+64);
      LEFT_1();
      delay(20);
      Serial.print("8");
    }
    
//Spin controls----------------------------------------------------------------
    if (RX > 128)
    {
      Motor_PWM = 0.8 * (RX - 128);
      TURN_RIGHT();
      delay(20);
    }

    if (RX < 128)
    {
      Motor_PWM = 0.8 * (127 - RX);
       TURN_LEFT();
      delay(20);
    }

//----------------------------------------------------------------------------------
    if ((LY>125&&LY<129)&&(LX>125&&LX<129)&&(RY>125&&RY<129)&&(RX>125&&RX<129))
    {
      STOP();
      delay(20);
      Serial.print("9");
    }
  
  if (ps2x.Button(PSB_L1) && ps2x.Button(PSB_R1)) { //print stick values if b oth are TRUE
    Serial.print("Stick Values:");
    Serial.print(ps2x.Analog(PSS_LY), DEC); //Left stick, Y axis. Other options: LX, RY, RX
    Serial.print(",");
    Serial.print(ps2x.Analog(PSS_LX), DEC);
    Serial.print(",");
    Serial.print(ps2x.Analog(PSS_RY), DEC);
    Serial.print(",");
    Serial.println(ps2x.Analog(PSS_RX), DEC);

    int LY = ps2x.Analog(PSS_LY);
    int LX = ps2x.Analog(PSS_LX);
    int RY = ps2x.Analog(PSS_RY);
    int RX = ps2x.Analog(PSS_RX);


//-----------------------------------------------------------------------------------------------------

    if (  (LY<124 && LX>((127/255)*LY+64) && LX < ((-127/255)*LY+191))  ||  (LY<124 && LX==((127/255)*LY+64))   ) 
    {
     Motor_PWM = 1.5 * (127 - LY);
      ADVANCE();
      delay(20);
      Serial.print("1");

    }

    if (   (LY<124 && LX>((-127/255)*LY+191) && LX<((-255/127)*LY+(48705/127)))  ||  (LY<124  &&  LX==((-127/255)*LY+191))   )
    {
     Motor_PWM = 1.5 * (LX-64-LY);
      RIGHT_1();
      delay(20);
      Serial.print("2");
    }
    
    if ( (LX>130 && LX>((-255/127)*LY+(48705/127)) && LX>((255/127)*LY+(-16320/127)))  ||  (LX>130  &&  LX==((-255/127)*LY+(48705/127))) )
    {
     Motor_PWM = 1.5 * (LX-127);
      RIGHT_2();
      delay(20);
      Serial.print("3");
    }

    if (   (LY>130 && LX<((255/127)*LY+(-16320/127)) && LX>((-127/255)*LY+191)) ||   (LY>130   &&   (LX==((255/127)*LY+(-16320/127))))     )
    {
     Motor_PWM = 1.5 * (LY+LX-319);
      RIGHT_3();
      delay(20);
      Serial.print("4");
    }

//-----------------------------------------------------------------------------------------------------------------------------------------

    if (    (LY>130 && LX>((127/255)*LY+64)  &&  LX<((-127/255)*LY+191))  ||   (LY>130   &&   (LX==((127/255)*LY+64)))    ) 
    {
     Motor_PWM = 1.5 * (LY-127);
      BACK();
      delay(20);
      Serial.print("5");
    }

    if (     (LY>130 && LX<((127/255)*LY+64)   &&   LX>((-255/127)*LY+(48705/127)))  ||  (LY>130   &&   (LX==((-127/255)*LY+191)))    )
    {
     Motor_PWM = 1.5 * (LY-64-LX);
      LEFT_3();
      delay(20);
      Serial.print("6");
    }
    
    if (    (LX<124 && LX<((-255/127)*LY+(48705/127))  &&   LX<((255/127)*LY+(-16320/127)))   ||   (LX<124   &&    (LX==((-255/127)*LY+(48705/127))))   )
    {
     Motor_PWM = 1.5 * (127-LX);
      LEFT_2();
      delay(20);
      Serial.print("7");
    }

    if (    (LY<124 && LX>((255/127)*LY+(-16320/127))   &&   LX<((127/255)*LY+64))   ||   (LY<124   &&   (LX==((255/127)*LY+(-16320/127))))     )
    {
     Motor_PWM = 1.5 * (LY+LX+64);
      LEFT_1();
      delay(20);
      Serial.print("8");
    }

//Spin controls----------------------------------------------------------------
    if (RX > 128)
    {
      Motor_PWM = 1.5 * (RX - 128);
      TURN_RIGHT();
      delay(20);
    }

    if (RX < 128)
    {
      Motor_PWM = 1.5 * (127 - RX);
       TURN_LEFT();
      delay(20);
    }

//----------------------------------------------------------------------------------
    if ((LY>125&&LY<129)&&(LX>125&&LX<129)&&(RY>125&&RY<129)&&(RX>125&&RX<129))
    {
      STOP();
      delay(20);
      Serial.print("9");
    }

//servo control
  }
}
