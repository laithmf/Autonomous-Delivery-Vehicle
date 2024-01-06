/*
//---  Senior Project - Fall 2023  ---
//---  Autonomous Delivery Vehicle  ---
//---  By: Laith Fakhruldin and Spencer Martinez  ---
//---  Dr. Ece Yaprak and prof. Moise Sunda  ---
//---  ET4999  ---
//---  10/28/2023  ---

   This Arduino program is designed for controlling an autonomous miniature delivery vehicle. 
   The vehicle is tasked with picking up and dropping off items at specific locations determined 
   by the color of an RGB color sensor. It communicates with a Python application via Bluetooth 
   to perform three-cycle simulations and receive commands for picking and dropping off items. 

   The vehicle operates on a fixed tracking lane and will stop once a full cycle is complete, 
   awaiting the next command from the Python program. Various functions and sensors are used 
   to achieve precise movements and item handling, including the control of servos, motors, 
   and ultrasonic sensors.

*/

//Libraries to be used to control some of the sensor
#include <Adafruit_TCS34725.h>
#include <Wire.h>
#include <Servo.h>
#include <NewPing.h> // Include the NewPing library
#include <Adafruit_PWMServoDriver.h>
#include <SoftwareSerial.h>


SoftwareSerial HC05(10,11); //(TX, RX) pins for the bluetooth communication

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);// address of the MCP23017 servo motor driver

//some global variables
#define SERVO1MIN 110
#define SERVO1MAX 540
#define SERVO2MIN 135
#define SERVO2MAX 500
#define SERVO3MIN 70
#define SERVO3MAX 535
#define SERVO_FREQ 50
#define LIGHT_OFF 1024
#define PWM_MAX 4096

// Define the pins used for motor control
const int DIRA_PIN = 2;
const int PWMA_PIN = 5;
const int DIRB_PIN = 4;
const int PWMB_PIN = 6;

//what channel each servo at start with 0 up to 15 "16 channels"
uint8_t servonum = 0;

//---- Define the TCS34725 sensor object ----//
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

//define the darkest color black will be minimum value and whitest color white maximum value.
//this will calibrate all colors on these values.
int redMin = 28;
int redMax = 580;
int greenMin = 26;
int greenMax = 592;
int blueMin = 20;
int blueMax = 452;


// ---- defining the ultrasound sensorS for picking and dropping items and avoidance ---- //
#define TRIGGER_PIN A2
#define ECHO_PIN A3
#define TRIGGER_AVOID_PIN 12
#define ECHO_AVOID_PIN 13
#define MAX_DISTANCE 1000// Define a maximum distance value 
NewPing sonarItem(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
NewPing sonarAvoid(TRIGGER_AVOID_PIN, ECHO_AVOID_PIN, MAX_DISTANCE);


// ---- set global inputs and outputs for pins and functions ---- //

bool forward_flag;
bool backward_flag;
bool left_flag;
bool right_flag;
bool jobOne_flag;
bool jobTwo_flag;
bool jobThree_flag;
bool stop_flag;
bool alarm_flag;
bool isColorOrange;
bool isColorGreen;
bool isColorYellow;
bool isColorLightBlue;
bool isColorBlue;
bool isColorDarkBlue;
bool isColorDarkOrange;
bool isColorPurple;
bool isColorPink;
bool isColorRed;
bool isLineTracking;
bool shouldReset = false;
bool orange_orange_PickDrop_flag = false;
bool orange_blue_PickDrop_flag = false;
bool orange_purple_PickDrop_flag = false;
bool blue_blue_PickDrop_flag = false;
bool blue_orange_PickDrop_flag = false;
bool blue_purple_PickDrop_flag = false;
bool purple_purple_PickDrop_flag = false;
bool purple_orange_PickDrop_flag = false;
bool purple_blue_PickDrop_flag = false;
bool orangeDropOff_flag = false;
bool pickUpPerformed = false;
bool resumeCycle = false;

unsigned long pickupStartTime = 0;
const unsigned long pickupDelay = 2000;

static unsigned int jobCycleCounter;
int base_degrees;
int arm_degrees;
int claw_degrees;
int current_claw_degrees;
int colorPassCnt;
int Left_Tra_Value;
int Center_Tra_Value;
int Right_Tra_Value;
int Black_Line;
int distance;
char command;
int currentCycle = 0; 

//struct is used to control the stopping moving of vehicle
enum CarState {
  RUNNING,
  STOPPED
};

CarState carState = RUNNING; 


void alarmColorON() {
  pwm.setPWM(15,0,PWM_MAX);
  delay(500);
  pwm.setPWM(15,PWM_MAX,0);
  delay(500);
  /*for (int i=0;i<4096;i++) {
      pwm.setPWM(15, 0, i);
    }

    for (int i=4096;i>0;i--) {
      pwm.setPWM(15, i, 0);
    }
  */
}

void alarmColorOFF() {
  pwm.setPWM(15, PWM_MAX, 0); //PWM_MA = 4096,
}

// --- pickup and drop off item function  for each distinct color----//
void pick_itemOrange() {
  move_claw(162);
  delay(1000);
  base_movement(161);
  delay(1000);
  move_arm(120);
  delay(1000);
  move_claw(20);
  delay(1000);
  move_arm(30);
  delay(1000);
  base_movement(83);
  
}
void drop_itemOrange() {
  base_movement(161);
  delay(1000);
  move_arm(115);
  delay(1000);
  move_claw(165);
  delay(1000);
  move_arm(30);
  delay(1000);
  base_movement(83);
  delay(1000);
  move_claw(60);
}

void pick_itemBlue() {
  move_claw(159);
  delay(1000);
  base_movement(161);
  delay(1000);
  move_arm(175);
  delay(1000);
  move_claw(18);
  delay(1000);
  move_arm(30);
  delay(1000);
  base_movement(83);
  
}
void drop_itemBlue() {
  base_movement(160);
  delay(1000);
  move_arm(139);
  delay(1000);
  move_claw(165);
  delay(1000);
  move_arm(28);
  delay(1000);
  base_movement(83);
  delay(1000);
  move_claw(60);
}

void pick_itemPurple() {
  move_claw(160);
  delay(1000);
  base_movement(160);
  delay(1000);
  move_arm(123);
  delay(1000);
  move_claw(18);
  delay(1000);
  move_arm(30);
  delay(1000);
  base_movement(83);
 
}
void drop_itemPurple() {
  base_movement(162);
  delay(1000);
  move_arm(123);
  delay(1000);
  move_claw(165);
  delay(1000);
  move_arm(30);
  delay(1000);
  base_movement(83);
  delay(1000);
  move_claw(60);
}


// ---- checking to see if the item is picked or not ---- //
bool isItemPicked() {
  // Check the claw degree to see if an item is picked
  if(current_claw_degrees <= 20){
    return true;
  } else{ 
    return false;
  }
  
}

//check for the item if is dropped
bool isItemDropped() {
  // Check the claw degree to see if an item is picked
  if(current_claw_degrees >= 50){
    return true;
  } else{
    return false;
  }
   
}


//----- set the colors inputs from RGB color sensor ------//

// these settings are to initialize the colors that is picked from reading each color of RGB sensor --- //
bool isRed(unsigned int red, unsigned int green, unsigned int blue) {
  int redMinRed = 115;    
  int redMaxRed = 185;
  int greenMinRed = 30;
  int greenMaxRed = 69;
  int blueMinRed = 40;
  int blueMaxRed = 90;
  // Check if the RGB values fall within the specified red range
  bool isColorInRangeResult = (red >= redMinRed && red <= redMaxRed && green >=greenMinRed && green <= greenMaxRed && blue >= blueMinRed && blue <= blueMaxRed);
  return isColorInRangeResult;
}

bool isBlue(unsigned int red, unsigned int green, unsigned int blue) {
  //set the threshold for each RGB of the sensor
  int redMinRed = 36;    
  int redMaxRed = 74;
  int greenMinRed = 78;
  int greenMaxRed = 111;
  int blueMinRed = 130;
  int blueMaxRed = 203;
  bool isColorInRangeResult = (red >= redMinRed && red <= redMaxRed && green >=greenMinRed && green <= greenMaxRed && blue >= blueMinRed && blue <= blueMaxRed);
  return isColorInRangeResult;
}
bool isOrange(unsigned int red, unsigned int green, unsigned int blue) {
  int redMin = 125;    
  int redMax = 255;
  int greenMin = 71;
  int greenMax = 145;
  int blueMin = 55;
  int blueMax = 95;
  bool isColorInRangeResult = (red >= redMin && red <= redMax && green >=greenMin && green <= greenMax && blue >= blueMin && blue <= blueMax);
  return isColorInRangeResult;
}

bool isPurple(unsigned int red, unsigned int green, unsigned int blue) {
  int redMin = 110;    
  int redMax = 180;
  int greenMin = 60;
  int greenMax = 90;
  int blueMin = 100;
  int blueMax = 140;
  bool isColorInRangeResult = (red >= redMin && red <= redMax && green >=greenMin && green <= greenMax && blue >= blueMin && blue <= blueMax);
  return isColorInRangeResult;
}
bool isDarkOrange(unsigned int red, unsigned int green, unsigned int blue) {
  int redMin = 125;    
  int redMax = 155;
  int greenMin = 57;
  int greenMax = 72;
  int blueMin = 50;
  int blueMax = 80;
  bool isColorInRangeResult = (red >= redMin && red <= redMax && green >=greenMin && green <= greenMax && blue >= blueMin && blue <= blueMax);
  return isColorInRangeResult;
}

bool isDarkBlue(unsigned int red, unsigned int green, unsigned int blue) {
  int redMin = 25;   
  int redMax = 65;
  int greenMin = 45;
  int greenMax = 65;
  int blueMin = 95;
  int blueMax = 145;
  bool isColorInRangeResult = (red >= redMin && red <= redMax && green >=greenMin && green <= greenMax && blue >= blueMin && blue <= blueMax);
  return isColorInRangeResult;
}

bool isLightBlue(unsigned int red, unsigned int green, unsigned int blue) {
  int redMin = 115;    
  int redMax = 170;
  int greenMin = 125;
  int greenMax = 185;
  int blueMin = 140;
  int blueMax = 195;
  bool isColorInRangeResult = (red >= redMin && red <= redMax && green >=greenMin && green <= greenMax && blue >= blueMin && blue <= blueMax);
  return isColorInRangeResult;
}

bool isGreen(unsigned int red, unsigned int green, unsigned int blue) {
  int redMin = 74;    
  int redMax = 100;
  int greenMin = 105;
  int greenMax = 185;
  int blueMin = 110;
  int blueMax = 125;
  bool isColorInRangeResult = (red >= redMin && red <= redMax && green >=greenMin && green <= greenMax && blue >= blueMin && blue <= blueMax);
  return isColorInRangeResult;
}

bool isYellow(unsigned int red, unsigned int green, unsigned int blue) {
  int redMin = 140;    
  int redMax = 255;
  int greenMin = 140;
  int greenMax = 255;
  int blueMin = 90;
  int blueMax = 125;
  bool isColorInRangeResult = (red >= redMin && red <= redMax && green >=greenMin && green <= greenMax && blue >= blueMin && blue <= blueMax);
  return isColorInRangeResult;
}

bool isPink(unsigned int red, unsigned int green, unsigned int blue) {
  int redMin = 145;    
  int redMax = 255;
  int greenMin = 118;
  int greenMax = 205;
  int blueMin = 130;
  int blueMax = 195;
  bool isColorInRangeResult = (red >= redMin && red <= redMax && green >=greenMin && green <= greenMax && blue >= blueMin && blue <= blueMax);
  return isColorInRangeResult;
}

//this is the color of the foam board set as the road, may be needed it in the future
bool isWhite(unsigned int red, unsigned int green, unsigned int blue) {
  int redMin = 430;    
  int redMax = 480;
  int greenMin = 450;
  int greenMax = 550;
  int blueMin = 450;
  int blueMax = 550;
  bool isColorInRangeResult = (red >= redMin && red <= redMax && green >=greenMin && green <= greenMax && blue >= blueMin && blue <= blueMax);
  return isColorInRangeResult;
}

// this is the function will check will bool of the color set to the reading of the sensor the time of pickup or drop off
bool colorMatch() {
  // Read RGB color values from the sensor
  uint16_t red, green, blue, clear;
  tcs.getRawData(&red, &green, &blue, &clear);
  /*
  Serial.print("Red: "); Serial.print(red);
  Serial.print(" - Green: "); Serial.print(green);
  Serial.print(" - Blue: "); Serial.print(blue);
  Serial.println();
  delay(1000);*/

  int redV = map(red,redMin, redMax, 0,255);
  int greenV = map(green, greenMin, greenMax, 0, 255);
  int blueV = map(blue,blueMin,blueMax,0,255);

  //constrain to values between 0-255 not to go over these values
  int redVal = constrain(redV,0,255);
  int greenVal=constrain(greenV, 0,255);
  int blueVal=constrain(blueV,0,255);
  /*
  Serial.print("Red: "); Serial.print(redVal);
  Serial.print(" - Green: "); Serial.print(greenVal);
  Serial.print(" - Blue: "); Serial.print(blueVal);
  Serial.println();
  delay(1000);
  */
  // Check for color matches and update the flags
  isColorOrange = isOrange(redVal, greenVal, blueVal);
  isColorGreen = isGreen(redVal, greenVal, blueVal);
  isColorYellow = isYellow(redVal, greenVal, blueVal);
  isColorLightBlue = isLightBlue(redVal, greenVal, blueVal);
  isColorDarkBlue = isDarkBlue(redVal, greenVal, blueVal);
  isColorBlue = isBlue(redVal, greenVal, blueVal);
  isColorDarkOrange = isDarkOrange(redVal, greenVal, blueVal);
  isColorPurple = isPurple(redVal, greenVal, blueVal);
  isColorPink = isPink(redVal, greenVal, blueVal);
  isColorRed = isRed(redVal, greenVal, blueVal);
  
  //we use these printout to check the values of the RGB color sensor
  /*Serial.print("Red: ");Serial.print(isColorRed);
  Serial.print(" - isOrange: ");Serial.print(isColorOrange);
  Serial.print(" - isGreen: ");Serial.print(isColorGreen);
  Serial.print(" - isYellow: ");Serial.print(isColorYellow);
  Serial.print(" - isDarkBlue: ");Serial.print(isColorDarkBlue);
  Serial.print(" - isPurple: ");Serial.print(isColorPurple);
  Serial.print(" - isPink: ");Serial.print(isColorPink);
  Serial.print(" - isLightBlue: ");Serial.print(isColorLightBlue);
  Serial.print(" - isDarkDarOrange: ");Serial.print(isColorDarkOrange);
  Serial.print(" - isPink: ");Serial.print(isColorPink);
  Serial.print(" - isBlue: ");Serial.print(isColorBlue);
  Serial.println();
  delay(1000);*/
    
  // Return true if any color is detected, otherwise false
  return isColorOrange || isColorGreen || isColorYellow || isColorLightBlue || isColorDarkBlue || isColorDarkOrange || isColorPurple || isColorPink || isColorRed || isColorBlue;
}

//These functions will determine received command from the Python with type of coloring of pickup and drop off
void orange_orange_pick_drop(){
  //activate the while loop for this function
  orange_orange_PickDrop_flag = true;
  //set the alarm off
  alarmColorOFF();
  //different types of variables used for creating operation driving vehicle run smoothly
  bool pickUpPerformed = false; 
  bool dropOffPerformed = false;
  bool isLineTracking = true;
  bool colorMatchStarted = true;
  
  unsigned long currentAvoidTimer = 0;
  unsigned long currentTime = 0;
  unsigned long continueTimer = 0;
  unsigned long pickUpTimer = 0; // Timer for pick-up delay
  unsigned int stopDistance = 15;
  unsigned int  stepCnt= 1;
  //loop through to pick the item from orange station and drops it at orange station  
  while(orange_orange_PickDrop_flag){
    if(carState ==RUNNING){
      //used to get command 'n' for Coninue Cycle from Python run cycle again for this function only
      currentCycle = 1;
      //send a status report to Python program
      HC05.println("ORANGE - ORANGE CYCLE!!");
      //reading the tracking, ultrasonic sensors to see their conditions.
      Left_Tra_Value = digitalRead(7);
      Center_Tra_Value = digitalRead(8);
      Right_Tra_Value = digitalRead(A1);
      unsigned int distance_cm = sonarItem.ping_cm();
      unsigned int distanceAvoid_cm = sonarAvoid.ping_cm();
      //using front ultrasonic sensor to skip to a different route if object on the main route
      if(distanceAvoid_cm <=40 && distanceAvoid_cm > 10){
        if(Left_Tra_Value == Black_Line && Center_Tra_Value == Black_Line && Right_Tra_Value == Black_Line){
          isLineTracking = false;
          Rotate_Left(85);
          delay(200);
          isLineTracking = true;
          Serial.println("if statement <= 40cm > 10cm");
        }
      }  
      //this let the vehicle to move forward on the main route if no obsetruction on the route.
      else if(distanceAvoid_cm > 40|| distanceAvoid_cm == 0){
        if(Left_Tra_Value == Black_Line && Center_Tra_Value == Black_Line && Right_Tra_Value == Black_Line){
          isLineTracking = false;
          Move_Forward(60);
          delay(100);
          isLineTracking = true;
          Serial.println("if statement >40cm");
        } 
      }  
      //added switch statement to add a timer between pickup and drop off stations do to conflict with same color 
      //will be dropped of on the same station of pickup
      switch (stepCnt){
        //pickup station in the orange station.
        case 1:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            colorMatchStarted = true;
            isLineTracking = true;
            if(isColorOrange && (distance_cm <= stopDistance && distance_cm > 0) && current_claw_degrees >=50) {
              isLineTracking = false;
              colorMatchStarted = false; // Reset the flag
              if(isItemDropped()){
                Stop();
                delay(200);
                pick_itemOrange();
                pickUpPerformed = true;
                stepCnt = 2;
              }
            } 
            else if(isItemPicked()){
              stepCnt= 2;
            }
          }
          break;
        //adding timer case condition
        case 2:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            isLineTracking = true;
            colorMatchStarted = true;
            isLineTracking = true;
            if (isItemPicked()) {
              if(pickUpTimer == 0){
                pickUpTimer = millis();
              }
              if (millis() - pickUpTimer >= 3000) {
                pickUpPerformed = true;
                colorMatchStarted = true;
                pickUpTimer = 0;
                stepCnt = 3;
              }
            }
          }
          break;
        case 3:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            colorMatchStarted = true;
            isLineTracking = true;
            if(pickUpPerformed && isColorOrange && current_claw_degrees <= 20 ){
              isLineTracking = false;
              colorMatchStarted = false; // Reset the flag
              if(isItemPicked()){
                Stop();
                delay(200);
                drop_itemOrange();
                dropOffPerformed = true;
                isLineTracking = true;
                colorMatchStarted = true;
                stepCnt = 4;
              }  
            }
          }
          break;
        case 4:
          //this will halt the vehicle if any object reached within 10cm and will move when the object is removed
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            colorMatchStarted = true;
            isLineTracking = true;
          }
          break;  
        
      } 
      //this will end the cycle of the picking and dropping off object condition
      if(isColorRed){
        delay(150);
        Stop();
        delay(800);
        orange_orange_PickDrop_flag = false;
        stepCnt = 1;
        currentCycle =10;
        shouldReset=true;
        break;
      }
      //make the tracking condition true when needed
      if(isLineTracking){
        black_line();
      } 
      //make the color match condition true when required
      if(colorMatchStarted){
        colorMatch();
      }
      //this condition will activate the stop command came from Python program side with 's'
      if( HC05.available()>0){
        command = HC05.read();
        if(command == 's'){
          carState = STOPPED;
          Stop();
          HC05.println("Vehicle has been STOPPED!!");
        }
      }
    } 
    //this will continue the cycle until the red color is reached to end the cycle
    else if(carState == STOPPED){
      if(HC05.available()>0){
        command = HC05.read();
        if(command == 'n'){
          stepCnt = 1;
          currentCycle = 1;
          HC05.println("ORANGE - ORANGE CYCLE!!");
          carState = RUNNING;
        }
      }    
    } 
  }
}


//same as the function above with tackling different colors
void orange_blue_pick_drop(){
  orange_blue_PickDrop_flag = true;
  alarmColorOFF();
  
  bool pickUpPerformed = false; 
  bool dropOffPerformed = false;
  bool isLineTracking = true;
  bool colorMatchStarted = true;
  
  unsigned long currentAvoidTimer = 0;
  unsigned long currentTime = 0;
  unsigned long continueTimer = 0;
  unsigned long pickUpTimer = 0; // Timer for pick-up delay
  unsigned int stopDistance = 15;
  unsigned int  stepCnt= 1;
    
  while(orange_blue_PickDrop_flag){
    currentCycle = 2;
    if(carState ==RUNNING){
      HC05.println("ORANGE - BLUE CYCLE!!");
      Left_Tra_Value = digitalRead(7);
      Center_Tra_Value = digitalRead(8);
      Right_Tra_Value = digitalRead(A1);
      unsigned int distance_cm = sonarItem.ping_cm();
      unsigned int distanceAvoid_cm = sonarAvoid.ping_cm();
    
      if(distanceAvoid_cm <=40 && distanceAvoid_cm > 10){
        if(Left_Tra_Value == Black_Line && Center_Tra_Value == Black_Line && Right_Tra_Value == Black_Line){
          isLineTracking = false;
          Rotate_Left(85);
          delay(200);
          isLineTracking = true;
          Serial.println("if statement <= 40cm > 10cm");
        }
      }  
      else if(distanceAvoid_cm > 40|| distanceAvoid_cm == 0){
        if(Left_Tra_Value == Black_Line && Center_Tra_Value == Black_Line && Right_Tra_Value == Black_Line){
          isLineTracking = false;
          Move_Forward(60);
          delay(100);
          isLineTracking = true;
        } 
      }  
    
      switch (stepCnt){
        case 1:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            colorMatchStarted = true;
            isLineTracking = true;
            if(isColorOrange && (distance_cm <= stopDistance && distance_cm > 0) && current_claw_degrees >=50) {
              isLineTracking = false;
              colorMatchStarted = false; // Reset the flag
              if(isItemDropped()){
                Stop();
                delay(200);
                pick_itemOrange();
                pickUpPerformed = true;
                stepCnt = 2;
              }
            } 
            else if(current_claw_degrees <= 20){
              stepCnt= 2;
            }
          }
          break;
        case 2:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          }  
          else{
            alarmColorOFF();
            isLineTracking = true;
            colorMatchStarted = true;
            isLineTracking = true;
            if (isItemPicked()){
              if(pickUpTimer == 0){
                pickUpTimer = millis();
              }
              if (millis() - pickUpTimer >= 12000) {
                pickUpPerformed = true;
                colorMatchStarted = true;
                pickUpTimer = 0;
                stepCnt = 3;
              }
            }
          }
          break;
        case 3:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            colorMatchStarted = true;
            isLineTracking = true;
            if(pickUpPerformed && isColorBlue  && current_claw_degrees <= 20){ 
              isLineTracking = false; 
              colorMatchStarted = false; // Reset the flag
              if(isItemPicked()){
                Stop();
                delay(200);
                drop_itemBlue();
                dropOffPerformed = true;
                isLineTracking = true;
                colorMatchStarted = true;
                stepCnt = 4;
              }  
            }
          }
          break;
        case 4:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            colorMatchStarted = true;
            isLineTracking = true;
          }
          break;  
      } 
    
      if(isColorRed){
        delay(150);
        //pickUpPerformed = true;
        //dropOffPerformed = true;
        Stop();
        delay(800);
        orange_blue_PickDrop_flag = false;
        stepCnt = 1;
        currentCycle =10;
        shouldReset=true;
        break;
      }
      if(isLineTracking){
        black_line();
      } 
    
      if(colorMatchStarted){
        colorMatch();
      }
      if( HC05.available()>0){
        command = HC05.read();
        if(command == 's'){
          carState = STOPPED;
          Stop();
          HC05.println("Vehicle has been STOPPED!!");
        }
      }
    } 
    else if(carState == STOPPED){
      if(HC05.available()>0){
        command = HC05.read();
        if(command == 'n'){
          currentCycle = 2;
          HC05.println("ORANGE - BLUE CYCLE!!");
          carState = RUNNING;
        }
      }    
    } 
  }
}

//another function with different color
void orange_purple_pick_drop(){
  orange_purple_PickDrop_flag = true;
  alarmColorOFF();
  
  bool pickUpPerformed = false; 
  bool dropOffPerformed = false;
  bool isLineTracking = true;
  bool colorMatchStarted = true;
  
  unsigned long currentAvoidTimer = 0;
  unsigned long currentTime = 0;
  unsigned long continueTimer = 0;
  unsigned long pickUpTimer = 0; // Timer for pick-up delay
  unsigned int stopDistance = 15;
  unsigned int  stepCnt= 1;
    
  while(orange_purple_PickDrop_flag){
    currentCycle = 3;
    if(carState == RUNNING){
      HC05.println("ORANGE - PURPLE CYCLE!!");
      Left_Tra_Value = digitalRead(7);
      Center_Tra_Value = digitalRead(8);
      Right_Tra_Value = digitalRead(A1);
      unsigned int distance_cm = sonarItem.ping_cm();
      unsigned int distanceAvoid_cm = sonarAvoid.ping_cm();
    
      if(distanceAvoid_cm <=40 && distanceAvoid_cm > 10){
        if(Left_Tra_Value == Black_Line && Center_Tra_Value == Black_Line && Right_Tra_Value == Black_Line){
          isLineTracking = false;
          Rotate_Left(85);
          delay(200);
          isLineTracking = true;
          Serial.println("if statement <= 40cm > 10cm");
        }
      }  
      else if(distanceAvoid_cm > 40|| distanceAvoid_cm == 0){
        if(Left_Tra_Value == Black_Line && Center_Tra_Value == Black_Line && Right_Tra_Value == Black_Line){
          isLineTracking = false;
          Move_Forward(60);
          delay(100);
          isLineTracking = true;
          Serial.println("if statement >40cm");
        } 
      }  
    
      switch (stepCnt){
        case 1:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            Serial.print("stepCnt inside switch case 1: "); Serial.println(stepCnt);
            colorMatchStarted = true;
            isLineTracking = true;
            if(isColorOrange && (distance_cm <= stopDistance && distance_cm > 0) && current_claw_degrees >=50) {
              isLineTracking = false;
              colorMatchStarted = false; // Reset the flag
              if(isItemDropped()){
                Stop();
                delay(200);
                pick_itemOrange();
                pickUpPerformed = true;
                stepCnt = 2;
              }
            } 
            else if(current_claw_degrees <= 20){
              stepCnt= 2;
            }
          }
          break;
        case 2:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
            stepCnt = 2;
            if(pickUpTimer == 0){
              pickUpTimer = millis();
            }
          }  
          else{
            alarmColorOFF();
            isLineTracking = true;
            colorMatchStarted = true;
            isLineTracking = true;
            //if (isItemPicked()) {
            if(pickUpTimer == 0){
              pickUpTimer = millis();
            }
            if (millis() - pickUpTimer >= 14000) {
              pickUpPerformed = true;
              colorMatchStarted = true;
              pickUpTimer = 0;
              stepCnt = 3;
            }
            //}
          }
          break;
        case 3:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            colorMatchStarted = true;
            isLineTracking = true;
            if(pickUpPerformed && isColorPurple && current_claw_degrees <= 20){
              isLineTracking = false;
              colorMatchStarted = false; // Reset the flag
              if(isItemPicked()){
                Stop();
                delay(200);
                drop_itemPurple();
                dropOffPerformed = true;
                isLineTracking = true;
                colorMatchStarted = true;
                stepCnt = 4;
              }  
            }
          }
          break;
        case 4:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            colorMatchStarted = true;
            isLineTracking = true;
          }
          break;  
      } 
    
      if(isColorRed){
        delay(150);
        Stop();
        delay(800);
        orange_purple_PickDrop_flag = false;
        stepCnt = 1;
        currentCycle =10;
        shouldReset=true;
        break;
      }
      if(isLineTracking){
        black_line();
      } 
    
      if(colorMatchStarted){
        colorMatch();
      }
      if( HC05.available()>0){
        command = HC05.read();
        if(command == 's'){
          carState = STOPPED;
          Stop();
          HC05.println("Vehicle has been STOPPED!!");
        }
      }
    } 
    else if(carState == STOPPED){
      if(HC05.available()>0){
        command = HC05.read();
        if(command == 'n'){
          currentCycle = 3;
          HC05.println("ORANGE - PURPLE CYCLE!!");
          carState = RUNNING;
        }
      }    
    } 
  }
}


void blue_blue_pick_drop(){
  blue_blue_PickDrop_flag = true;
  alarmColorOFF();
  
  bool pickUpPerformed = false; 
  bool dropOffPerformed = false;
  bool isLineTracking = true;
  bool colorMatchStarted = true;
  
  unsigned long currentAvoidTimer = 0;
  unsigned long currentTime = 0;
  unsigned long continueTimer = 0;
  unsigned long pickUpTimer = 0; // Timer for pick-up delay
  unsigned int stopDistance = 15;
  unsigned int  stepCnt= 1;
    
  while(blue_blue_PickDrop_flag){
    currentCycle = 4;
    if(carState ==RUNNING){
      HC05.println("BLUE - BLUE CYCLE!!");
      Left_Tra_Value = digitalRead(7);
      Center_Tra_Value = digitalRead(8);
      Right_Tra_Value = digitalRead(A1);
      unsigned int distance_cm = sonarItem.ping_cm();
      unsigned int distanceAvoid_cm = sonarAvoid.ping_cm();
    
      if(distanceAvoid_cm <=40 && distanceAvoid_cm > 10){
        if(Left_Tra_Value == Black_Line && Center_Tra_Value == Black_Line && Right_Tra_Value == Black_Line){
          isLineTracking = false;
          Rotate_Left(85);
          delay(200);
          isLineTracking = true;
          Serial.println("if statement <= 40cm > 10cm");
        }
      }  
      else if(distanceAvoid_cm > 40|| distanceAvoid_cm == 0){
        if(Left_Tra_Value == Black_Line && Center_Tra_Value == Black_Line && Right_Tra_Value == Black_Line){
          isLineTracking = false;
          Move_Forward(60);
          delay(100);
          isLineTracking = true;
        } 
      }  
    
      switch (stepCnt){
        case 1:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            colorMatchStarted = true;
            isLineTracking = true;
            if(isColorBlue && (distance_cm <= stopDistance && distance_cm > 0) && current_claw_degrees >=50) {
              isLineTracking = false;
              colorMatchStarted = false; // Reset the flag
              if(isItemDropped()){
                Stop();
                delay(200);
                pick_itemBlue();
                pickUpPerformed = true;
                stepCnt = 2;
              }
            } 
            else if(current_claw_degrees <= 20){
              stepCnt= 2;
            }
          }
          break;
        case 2:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            isLineTracking = true;
            colorMatchStarted = true;
            isLineTracking = true;
            if (isItemPicked()) {
              if(pickUpTimer == 0){
                pickUpTimer = millis();
              }
              if (millis() - pickUpTimer >= 3000) {
                pickUpPerformed = true;
                colorMatchStarted = true;
                pickUpTimer = 0;
                stepCnt = 3;
              }
            }
          }
          break;
        case 3:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            colorMatchStarted = true;
            isLineTracking = true;
            if(pickUpPerformed && isColorBlue && current_claw_degrees <= 20){
              isLineTracking = false;
              colorMatchStarted = false; // Reset the flag
              if(isItemPicked()){
                Stop();
                delay(200);
                drop_itemBlue();
                dropOffPerformed = true;
                isLineTracking = true;
                colorMatchStarted = true;
                stepCnt = 4;
              }  
            }
          }
          break;
        case 4:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            colorMatchStarted = true;
            isLineTracking = true;
          }
          break;  
      } 
    
      if(isColorRed){
        delay(150);
        //pickUpPerformed = true;
        //dropOffPerformed = true;
        Stop();
        delay(800);
        orange_orange_PickDrop_flag = false;
        stepCnt = 1;
        currentCycle =10;
        shouldReset=true;
        break;
      }
      if(isLineTracking){
        black_line();
      } 
    
      if(colorMatchStarted){
        colorMatch();
      }
      if( HC05.available()>0){
        command = HC05.read();
        if(command == 's'){
          carState = STOPPED;
          Stop();
          HC05.println("Vehicle has been STOPPED!!");
        }
      }
    } 
    else if(carState == STOPPED){
      if(HC05.available()>0){
        command = HC05.read();
        if(command == 'n'){
          currentCycle = 4;
          HC05.println("BLUE - BLUE CYCLE!!");
          carState = RUNNING;
        }
      }    
    } 
  }
}

void blue_orange_pick_drop(){
  blue_orange_PickDrop_flag = true;
  alarmColorOFF();
  
  bool pickUpPerformed = false; 
  bool dropOffPerformed = false;
  bool isLineTracking = true;
  bool colorMatchStarted = true;
  
  unsigned long currentAvoidTimer = 0;
  unsigned long currentTime = 0;
  unsigned long continueTimer = 0;
  unsigned long pickUpTimer = 0; // Timer for pick-up delay
  unsigned int stopDistance = 15;
  unsigned int  stepCnt= 1;
    
  while(blue_orange_PickDrop_flag){
    currentCycle = 5;
    if(carState ==RUNNING){
      HC05.println("BLUE - ORANGE CYCLE!!");
      Left_Tra_Value = digitalRead(7);
      Center_Tra_Value = digitalRead(8);
      Right_Tra_Value = digitalRead(A1);
      unsigned int distance_cm = sonarItem.ping_cm();
      unsigned int distanceAvoid_cm = sonarAvoid.ping_cm();
    
      if(distanceAvoid_cm <=40 && distanceAvoid_cm > 10){
        if(Left_Tra_Value == Black_Line && Center_Tra_Value == Black_Line && Right_Tra_Value == Black_Line){
          isLineTracking = false;
          Rotate_Left(85);
          delay(200);
          isLineTracking = true;
        }
      }  
      else if(distanceAvoid_cm > 40|| distanceAvoid_cm == 0){
        if(Left_Tra_Value == Black_Line && Center_Tra_Value == Black_Line && Right_Tra_Value == Black_Line){
          isLineTracking = false;
          Move_Forward(60);
          delay(100);
          isLineTracking = true;
          Serial.println("if statement >40cm");
        } 
      }  
    
      switch (stepCnt){
        case 1:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            colorMatchStarted = true;
            isLineTracking = true;
            if(isColorBlue && (distance_cm <= stopDistance && distance_cm > 0) && current_claw_degrees >=50) {
              isLineTracking = false;
              colorMatchStarted = false; // Reset the flag
              if(isItemDropped()){
                Stop();
                delay(200);
                pick_itemBlue();
                pickUpPerformed = true;
                stepCnt = 2;
              }
            } 
            else if(current_claw_degrees <= 20){
              stepCnt= 2;
            }
          }
          break;
        case 2:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            isLineTracking = true;
            colorMatchStarted = true;
            isLineTracking = true;
            if (isItemPicked()) {
              if(pickUpTimer == 0){
                pickUpTimer = millis();
              }
              if (millis() - pickUpTimer >= 2000) {
                pickUpPerformed = true;
                colorMatchStarted = true;
                pickUpTimer = 0;
                stepCnt = 3;
              }
            }
          }
          break;
        case 3:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            colorMatchStarted = true;
            isLineTracking = true;
            if(pickUpPerformed && isColorOrange  && current_claw_degrees <= 20){ //&& (distance_cm > stopDistance || distance_cm <= 0)
              isLineTracking = false;
              colorMatchStarted = false; // Reset the flag
              if(isItemPicked()){
                Stop();
                delay(200);
                drop_itemOrange();
                dropOffPerformed = true;
                isLineTracking = true;
                colorMatchStarted = true;
                stepCnt = 4;
              }  
            }
          }
          break;
        case 4:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            colorMatchStarted = true;
            isLineTracking = true;
          }
          break;  
      } 
    
      if(isColorRed){
        delay(150);
        //pickUpPerformed = true;
        //dropOffPerformed = true;
        Stop();
        delay(800);
        blue_blue_PickDrop_flag = false;
        stepCnt = 1;
        currentCycle =10;
        shouldReset=true;
        break;
      }
      if(isLineTracking){
        black_line();
      } 

      if(colorMatchStarted){
        colorMatch();
      }
      if( HC05.available()>0){
        command = HC05.read();
        if(command == 's'){
          carState = STOPPED;
          Stop();
          HC05.println("Vehicle has been STOPPED!!");
        }
      }
    } 
    else if(carState == STOPPED){
      if(HC05.available()>0){
        command = HC05.read();
        if(command == 'n'){
          currentCycle =5;
          HC05.println("BLUE - ORANGE CYCLE!!");
          carState = RUNNING;
        }
      }    
    } 
  }
}


void blue_purple_pick_drop(){
  blue_purple_PickDrop_flag = true;
  alarmColorOFF();
  
  bool pickUpPerformed = false; 
  bool dropOffPerformed = false;
  bool isLineTracking = true;
  bool colorMatchStarted = true;
  
  unsigned long currentAvoidTimer = 0;
  unsigned long currentTime = 0;
  unsigned long continueTimer = 0;
  unsigned long pickUpTimer = 0; // Timer for pick-up delay
  unsigned int stopDistance = 15;
  unsigned int  stepCnt= 1;
    
  while(blue_purple_PickDrop_flag){
    currentCycle = 6;
    if(carState == RUNNING){
      HC05.println("BLUE - PURPLE CYCLE!!");
      Left_Tra_Value = digitalRead(7);
      Center_Tra_Value = digitalRead(8);
      Right_Tra_Value = digitalRead(A1);
      unsigned int distance_cm = sonarItem.ping_cm();
      unsigned int distanceAvoid_cm = sonarAvoid.ping_cm();
    
      if(distanceAvoid_cm <=40 && distanceAvoid_cm > 10){
        if(Left_Tra_Value == Black_Line && Center_Tra_Value == Black_Line && Right_Tra_Value == Black_Line){
          isLineTracking = false;
          Rotate_Left(85);
          delay(200);
          isLineTracking = true;
        }
      }  
      else if(distanceAvoid_cm > 40|| distanceAvoid_cm == 0){
        if(Left_Tra_Value == Black_Line && Center_Tra_Value == Black_Line && Right_Tra_Value == Black_Line){
          isLineTracking = false;
          Move_Forward(60);
          delay(100);
          isLineTracking = true;
        } 
      }  
    
      switch (stepCnt){
        case 1:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            colorMatchStarted = true;
            isLineTracking = true;
            if(isColorBlue && (distance_cm <= stopDistance && distance_cm > 0) && current_claw_degrees >=50) {
              isLineTracking = false;
              colorMatchStarted = false; // Reset the flag
              if(isItemDropped()){
                Stop();
                delay(200);
                pick_itemBlue();
                pickUpPerformed = true;
                stepCnt = 2;
              }
            } 
            else if(current_claw_degrees <= 20){
              stepCnt= 2;
            }
          }
          break;
        case 2:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          }  
          else{
            alarmColorOFF();
            isLineTracking = true;
            colorMatchStarted = true;
            isLineTracking = true;
            if (isItemPicked()) {
              if(pickUpTimer == 0){
                pickUpTimer = millis();
              }
              if (millis() - pickUpTimer >= 6000) {
                pickUpPerformed = true;
                colorMatchStarted = true;
                pickUpTimer = 0;
                stepCnt = 3;
              }
            }
          }
          break;
        case 3:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            colorMatchStarted = true;
            isLineTracking = true;
            if(pickUpPerformed && isColorPurple  && current_claw_degrees <= 20){
              isLineTracking = false;
              colorMatchStarted = false; // Reset the flag
              if(isItemPicked()){
                Stop();
                delay(200);
                drop_itemPurple();
                dropOffPerformed = true;
                isLineTracking = true;
                colorMatchStarted = true;
                stepCnt = 4;
              }  
            }
          }
          break;
        case 4:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            colorMatchStarted = true;
            isLineTracking = true;
          }
          break;  
      } 
    
      if(isColorRed){
        delay(150);
        //pickUpPerformed = true;
        //dropOffPerformed = true;
        Stop();
        delay(800);
        blue_purple_PickDrop_flag = false;
        stepCnt = 1;
        currentCycle =10;
        shouldReset=true;
        break;
      }
      if(isLineTracking){
        black_line();
      } 

      if(colorMatchStarted){
        colorMatch();
      }
      if( HC05.available()>0){
        command = HC05.read();
        if(command == 's'){
          carState = STOPPED;
          Stop();
          HC05.println("Vehicle has been STOPPED!!");
        }
      }
    } 
    else if(carState == STOPPED){
      if(HC05.available()>0){
        command = HC05.read();
        if(command == 'n'){
          currentCycle = 6;
          HC05.println("BLUE - PURPLE CYCLE!!");
          carState = RUNNING;
        }
      }    
    } 
  }
}

void purple_purple_pick_drop(){
  purple_purple_PickDrop_flag = true;
  alarmColorOFF();
  bool pickUpPerformed = false; 
  bool dropOffPerformed = false;
  bool isLineTracking = true;
  bool colorMatchStarted = true;
  
  unsigned long currentAvoidTimer = 0;
  unsigned long currentTime = 0;
  unsigned long continueTimer = 0;
  unsigned long pickUpTimer = 0; // Timer for pick-up delay
  unsigned int stopDistance = 15;
  unsigned int  stepCnt= 1;
    
  while(purple_purple_PickDrop_flag){
    if(carState ==RUNNING){
      currentCycle = 7;
      HC05.println("PURPLE - PURPLE CYCLE!!");
      Left_Tra_Value = digitalRead(7);
      Center_Tra_Value = digitalRead(8);
      Right_Tra_Value = digitalRead(A1);
      unsigned int distance_cm = sonarItem.ping_cm();
      unsigned int distanceAvoid_cm = sonarAvoid.ping_cm();
    
      if(distanceAvoid_cm <=40 && distanceAvoid_cm > 10){
        if(Left_Tra_Value == Black_Line && Center_Tra_Value == Black_Line && Right_Tra_Value == Black_Line){
          isLineTracking = false;
          Rotate_Left(85);
          delay(200);
          isLineTracking = true;
        }
      }  
      else if(distanceAvoid_cm > 40|| distanceAvoid_cm == 0){
        if(Left_Tra_Value == Black_Line && Center_Tra_Value == Black_Line && Right_Tra_Value == Black_Line){
          isLineTracking = false;
          Move_Forward(60);
          delay(100);
          isLineTracking = true;
        } 
      }  
    
      switch (stepCnt){
        case 1:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            
            colorMatchStarted = true;
            isLineTracking = true;
            if(isColorPurple && (distance_cm <= stopDistance && distance_cm > 0) && current_claw_degrees >=50) {
              isLineTracking = false;
              colorMatchStarted = false; // Reset the flag
              if(isItemDropped()){
                Stop();
                delay(200);
                pick_itemPurple();
                pickUpPerformed = true;
                stepCnt = 2;
              }
            } 
            else if(isItemPicked()){
              stepCnt= 2;
            }
          }
          break;
        case 2:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            isLineTracking = true;
            colorMatchStarted = true;
            isLineTracking = true;
            if (isItemPicked()) {
              if(pickUpTimer == 0){
                pickUpTimer = millis();
              }
              if (millis() - pickUpTimer >= 2000) {
                pickUpPerformed = true;
                colorMatchStarted = true;
                pickUpTimer = 0;
                stepCnt = 3;
              }
            }
          }
          break;
        case 3:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            
            colorMatchStarted = true;
            isLineTracking = true;
            if(pickUpPerformed && isColorPurple && current_claw_degrees <= 20 ){
              isLineTracking = false;
              colorMatchStarted = false; // Reset the flag
              if(isItemPicked()){
                Stop();
                delay(200);
                drop_itemPurple();
                dropOffPerformed = true;
                isLineTracking = true;
                colorMatchStarted = true;
                HC05.println("Step 3 complete!");
                stepCnt = 4;
              }  
            }
          }
          break;
        case 4:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            colorMatchStarted = true;
            isLineTracking = true;
          }
          break;  
        
      } 
    
      if(isColorRed){
        delay(150);
        //pickUpPerformed = true;
        //dropOffPerformed = true;
        Stop();
        delay(800);
        purple_purple_PickDrop_flag = false;
        stepCnt = 1;
        currentCycle =10;
        shouldReset=true;

        break;
      }
      if(isLineTracking){
        black_line();
      } 
    
      if(colorMatchStarted){
        colorMatch();
      }
      if( HC05.available()>0){
        command = HC05.read();
        if(command == 's'){
          carState = STOPPED;
          Stop();
          HC05.println("Vehicle has been STOPPED!!");
        }
      }
    } 
    else if(carState == STOPPED){
      if(HC05.available()>0){
        command = HC05.read();
        if(command == 'n'){
          stepCnt = 1;
          currentCycle = 7;
          HC05.println("PURPLE - PURPLE CONTINUE CYCLE!!");
          carState = RUNNING;
        }
      }    
    } 
  }
}

void purple_orange_pick_drop(){
  purple_orange_PickDrop_flag = true;
  alarmColorOFF();
  bool pickUpPerformed = false; 
  bool dropOffPerformed = false;
  bool isLineTracking = true;
  bool colorMatchStarted = true;
  
  unsigned long currentAvoidTimer = 0;
  unsigned long currentTime = 0;
  unsigned long continueTimer = 0;
  unsigned long pickUpTimer = 0; // Timer for pick-up delay
  unsigned int stopDistance = 15;
  unsigned int  stepCnt= 1;
    
  while(purple_orange_PickDrop_flag){
    if(carState ==RUNNING){
      currentCycle = 8;
      HC05.println("PURPLE - ORANGE CYCLE!!");
      Left_Tra_Value = digitalRead(7);
      Center_Tra_Value = digitalRead(8);
      Right_Tra_Value = digitalRead(A1);
      unsigned int distance_cm = sonarItem.ping_cm();
      unsigned int distanceAvoid_cm = sonarAvoid.ping_cm();
    
      if(distanceAvoid_cm <=40 && distanceAvoid_cm > 10){
        if(Left_Tra_Value == Black_Line && Center_Tra_Value == Black_Line && Right_Tra_Value == Black_Line){
          isLineTracking = false;
          Rotate_Left(85);
          delay(200);
          isLineTracking = true;
        }
      }  
      else if(distanceAvoid_cm > 40|| distanceAvoid_cm == 0){
        if(Left_Tra_Value == Black_Line && Center_Tra_Value == Black_Line && Right_Tra_Value == Black_Line){
          isLineTracking = false;
          Move_Forward(60);
          delay(100);
          isLineTracking = true;
        } 
      }  
    
      switch (stepCnt){
        case 1:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            
            colorMatchStarted = true;
            isLineTracking = true;
            if(isColorPurple && (distance_cm <= stopDistance && distance_cm > 0) && current_claw_degrees >=50) {
              isLineTracking = false;
              colorMatchStarted = false; // Reset the flag
              if(isItemDropped()){
                Stop();
                delay(200);
                pick_itemPurple();
                pickUpPerformed = true;
                stepCnt = 2;
              }
            } 
            else if(isItemPicked()){
              stepCnt= 2;
            }
          }
          break;
        case 2:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            isLineTracking = true;
            colorMatchStarted = true;
            isLineTracking = true;
            if (isItemPicked()) {
              if(pickUpTimer == 0){
                pickUpTimer = millis();
              }
              if (millis() - pickUpTimer >= 2000) {
                pickUpPerformed = true;
                colorMatchStarted = true;
                pickUpTimer = 0;
                stepCnt = 3;
              }
            }
          }
          break;
        case 3:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            colorMatchStarted = true;
            isLineTracking = true;
            if(pickUpPerformed && isColorOrange  && current_claw_degrees <= 20 ){
              isLineTracking = false;
              colorMatchStarted = false; // Reset the flag
              if(isItemPicked()){
                Stop();
                delay(200);
                drop_itemOrange();
                dropOffPerformed = true;
                isLineTracking = true;
                colorMatchStarted = true;
                stepCnt = 4;
              }  
            }
          }
          break;
        case 4:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            colorMatchStarted = true;
            isLineTracking = true;
          }
          break;  
        
      } 
    
      if(isColorRed){
        delay(150);
        //pickUpPerformed = true;
        //dropOffPerformed = true;
        Stop();
        delay(800);
        purple_orange_PickDrop_flag = false;
        stepCnt = 1;
        currentCycle =10;
        shouldReset=true;

        break;
      }
      if(isLineTracking){
        black_line();
      } 
    
      if(colorMatchStarted){
        colorMatch();
      }
      if( HC05.available()>0){
        command = HC05.read();
        if(command == 's'){
          carState = STOPPED;
          Stop();
          HC05.println("Vehicle has been STOPPED!!");
        }
      }
    } 
    else if(carState == STOPPED){
      if(HC05.available()>0){
        command = HC05.read();
        if(command == 'n'){
          stepCnt = 1;
          currentCycle = 8;
          HC05.println("PURPLE - ORANGE CONTINUE CYCLE!!");
          carState = RUNNING;
        }
      }    
    } 
  }
}

void purple_blue_pick_drop(){
  purple_blue_PickDrop_flag = true;
  alarmColorOFF();
  
  bool pickUpPerformed = false; 
  bool dropOffPerformed = false;
  bool isLineTracking = true;
  bool colorMatchStarted = true;
  
  unsigned long currentAvoidTimer = 0;
  unsigned long currentTime = 0;
  unsigned long continueTimer = 0;
  unsigned long pickUpTimer = 0; // Timer for pick-up delay
  unsigned int stopDistance = 15;
  unsigned int  stepCnt= 1;
    
  while(purple_blue_PickDrop_flag){
    currentCycle = 9;
    if(carState == RUNNING){
      HC05.println("PURPLE - BLUE CYCLE!!");
      Left_Tra_Value = digitalRead(7);
      Center_Tra_Value = digitalRead(8);
      Right_Tra_Value = digitalRead(A1);
      unsigned int distance_cm = sonarItem.ping_cm();
      unsigned int distanceAvoid_cm = sonarAvoid.ping_cm();
    
      if(distanceAvoid_cm <=40 && distanceAvoid_cm > 10){
        if(Left_Tra_Value == Black_Line && Center_Tra_Value == Black_Line && Right_Tra_Value == Black_Line){
          isLineTracking = false;
          Rotate_Left(85);
          delay(200);
          isLineTracking = true;
          Serial.println("if statement <= 40cm > 10cm");
        }
      }  
      else if(distanceAvoid_cm > 40|| distanceAvoid_cm == 0){
        if(Left_Tra_Value == Black_Line && Center_Tra_Value == Black_Line && Right_Tra_Value == Black_Line){
          isLineTracking = false;
          Move_Forward(60);
          delay(100);
          isLineTracking = true;
          Serial.println("if statement >40cm");
        } 
      }  
    
      switch (stepCnt){
        case 1:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            colorMatchStarted = true;
            isLineTracking = true;
            if(isColorPurple && (distance_cm <= stopDistance && distance_cm > 0) && current_claw_degrees >=50) {
              isLineTracking = false;
              colorMatchStarted = false; // Reset the flag
              if(isItemDropped()){
                Stop();
                delay(200);
                pick_itemPurple();
                pickUpPerformed = true;
                stepCnt = 2;
              }
            } else if(current_claw_degrees <= 20){
              stepCnt= 2;
            }
          }
          break;
        case 2:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            isLineTracking = true;
            colorMatchStarted = true;
            isLineTracking = true;
            if (isItemPicked()) {
              if(pickUpTimer == 0){
                pickUpTimer = millis();
              }
              if (millis() - pickUpTimer >= 2000) {
                pickUpPerformed = true;
                colorMatchStarted = true;
                pickUpTimer = 0;
                stepCnt = 3;
              }
            }
          }
          break;
        case 3:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            colorMatchStarted = true;
            isLineTracking = true;
            if(pickUpPerformed && isColorBlue && current_claw_degrees <= 20){
              isLineTracking = false;
              colorMatchStarted = false; // Reset the flag
              if(isItemPicked()){
                Stop();
                delay(200);
                drop_itemBlue();
                dropOffPerformed = true;
                isLineTracking = true;
                colorMatchStarted = true;
                stepCnt = 4;
              }  
            }
          }
          break;
        case 4:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            colorMatchStarted = true;
            isLineTracking = true;
          }
          break;  
      } 
    
      if(isColorRed){
        delay(150);
        //pickUpPerformed = true;
        //dropOffPerformed = true;
        Stop();
        delay(800);
        purple_blue_PickDrop_flag = false;
        stepCnt = 1;
        currentCycle =10;
        shouldReset=true;
        break;
      }
      if(isLineTracking){
        black_line();
      } 
    
      if(colorMatchStarted){
        colorMatch();
      }
      if( HC05.available()>0){
        command = HC05.read();
        if(command == 's'){
          carState = STOPPED;
          Stop();
          HC05.println("Vehicle has been STOPPED!!");
        }
      }
    } else if(carState == STOPPED){
      if(HC05.available()>0){
        command = HC05.read();
        if(command == 'n'){
          currentCycle = 9;
          HC05.println("PURPLE - BLUE CYCLE!!");
          carState = RUNNING;
        }
      }    
    } 
  }
}

//These functions jobOne, jobTwo, and jobThree will be towards the simulation cycle that start with joOne() and ends at jobThree().
//these functions are similar to the multicolor function with only thing that must loop through all three
//functions to complete the cycles.
void jobOne() {
  alarmColorOFF();
  jobOne_flag = true;
  jobCycleCounter = 1;
  
  bool pickUpPerformed = false; // Flag to track whether pick-up action has been performed
  bool dropOffPerformed = false;
  bool isLineTracking = true;
  bool colorMatchStarted = true;
  
  unsigned long currentAvoidTimer = 0;
  unsigned long currentTime = 0;
  unsigned long continueTimer = 0;
  unsigned long pickUpTimer = 0; // Timer for pick-up delay
  unsigned int stopDistance = 15;
  
  unsigned int  stepCnt= 1;
  while(jobOne_flag && jobCycleCounter == 1) {
    currentCycle = 11;
    if(carState == RUNNING){
      HC05.println("CYCLE 1: ORANGE - ORANGE STATIONS!!");
      Left_Tra_Value = digitalRead(7);
      Center_Tra_Value = digitalRead(8);
      Right_Tra_Value = digitalRead(A1);
      unsigned int distance_cm = sonarItem.ping_cm();
      unsigned int distanceAvoid_cm = sonarAvoid.ping_cm();
    
      if(distanceAvoid_cm <=40 && distanceAvoid_cm > 10){
        if(Left_Tra_Value == Black_Line && Center_Tra_Value == Black_Line && Right_Tra_Value == Black_Line){
          isLineTracking = false;
          Rotate_Left(75);
          delay(200);
          isLineTracking = true;
          Serial.println("if statement <= 40cm > 10cm");
        }
      }  
      else if(distanceAvoid_cm > 40|| distanceAvoid_cm == 0){
        if(Left_Tra_Value == Black_Line && Center_Tra_Value == Black_Line && Right_Tra_Value == Black_Line){
          isLineTracking = false;
          Move_Forward(60);
          delay(100);
          isLineTracking = true;
        } 
      }  

      switch (stepCnt){
        case 1:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            colorMatchStarted = true;
            isLineTracking = true;
            if(isColorOrange && (distance_cm <= stopDistance && distance_cm > 0) && !pickUpPerformed) {
              isLineTracking = false;
              colorMatchStarted = false; // Reset the flag
              if(isItemDropped()){
                Stop();
                delay(200);
                pick_itemOrange();
                stepCnt = 2;
              }
            }else if(current_claw_degrees <= 20){
              stepCnt= 2;
            }
          }
          break;
        case 2:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            isLineTracking = true;
            colorMatchStarted = true;
            isLineTracking = true;
            if (isItemPicked()) {
              if(pickUpTimer == 0){
                pickUpTimer = millis();
              }
              if (millis() - pickUpTimer >= 3000) {
                pickUpPerformed = true;
                colorMatchStarted = true;
                pickUpTimer = 0;
                stepCnt = 3;
              }
            }
          }
          break;
        case 3:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            colorMatchStarted = true;
            isLineTracking = true;
            if(pickUpPerformed && isColorOrange && current_claw_degrees <= 20 ){
              isLineTracking = false;
              colorMatchStarted = false; // Reset the flag
              if(isItemPicked()){
                Stop();
                delay(200);
                drop_itemOrange();
                dropOffPerformed = true;
                isLineTracking = true;
                colorMatchStarted = true;
                stepCnt = 4;
              }  
            }
          }
          break;
        case 4:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            colorMatchStarted = true;
            isLineTracking = true;
          }
          break;  
        } 
    
      if(isColorRed){
        delay(150);
        Stop();
        delay(800);
        isLineTracking = true;
        colorMatchStarted = true;
        jobCycleCounter = 2;
        jobOne_flag = false;
        currentCycle = 12;
      }
      if(isLineTracking){
        black_line();
      } 
    
      if(colorMatchStarted){
        colorMatch();
      }
      if( HC05.available()>0){
        command = HC05.read();
        if(command == 's'){
          carState = STOPPED;
          Stop();
          HC05.println("Vehicle has been STOPPED!!");
        }
      }
    } 
    else if(carState == STOPPED){
      if(HC05.available()>0){
        command = HC05.read();
        if(command == 'n'){
          currentCycle = 11;
          HC05.println("CYCLE 1: ORANGE - ORANGE STATIONS!!");
          carState = RUNNING;
        }
      }
    }
  }
}
 


void jobTwo() {
  jobTwo_flag = true;
  alarmColorOFF();
  bool pickUpPerformed = false; // Flag to track whether pick-up action has been performed
  bool dropOffPerformed = false;
  bool isLineTracking = true;
  bool colorMatchStarted = true;
  
  unsigned long currentAvoidTimer = 0;
  unsigned long currentTime = 0;
  unsigned long continueTimer = 0;
  unsigned long pickUpTimer = 0; // Timer for pick-up delay
  unsigned int stopDistance = 15;
  unsigned int  stepCnt= 1;
  
  while(jobTwo_flag && jobCycleCounter == 2) {
    currentCycle = 12;
    if(carState == RUNNING){
      HC05.println("CYCLE 2: BLUE - BLUE STATIONS!!");
      Left_Tra_Value = digitalRead(7);
      Center_Tra_Value = digitalRead(8);
      Right_Tra_Value = digitalRead(A1);
      unsigned int distance_cm = sonarItem.ping_cm();
      unsigned int distanceAvoid_cm = sonarAvoid.ping_cm();
      if(distanceAvoid_cm <=40 && distanceAvoid_cm > 10){
        if(Left_Tra_Value == Black_Line && Center_Tra_Value == Black_Line && Right_Tra_Value == Black_Line){
          isLineTracking = false;
          Rotate_Left(75);
          delay(200);
          isLineTracking = true;
          Serial.println("if statement <= 40cm > 10cm");
        }
      }  
      else if(distanceAvoid_cm > 40|| distanceAvoid_cm == 0){
        if(Left_Tra_Value == Black_Line && Center_Tra_Value == Black_Line && Right_Tra_Value == Black_Line){
          isLineTracking = false;
          Move_Forward(60);
          delay(100);
          isLineTracking = true;
          Serial.println("if statement >40cm");
        } 
      }  
      switch (stepCnt){
        case 1:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            colorMatchStarted = true;
            isLineTracking = true;
            if(isColorBlue && (distance_cm <= stopDistance && distance_cm > 0) && !pickUpPerformed) {
              isLineTracking = false;
              colorMatchStarted = false; // Reset the flag
              if(isItemDropped()){
                Stop();
                delay(200);
                pick_itemBlue();
                stepCnt = 2;
              }
            }else if(current_claw_degrees <= 20){
              stepCnt= 2;
            }
          }
          break;
        case 2:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            isLineTracking = true;
            colorMatchStarted = true;
            isLineTracking = true;
            if (isItemPicked()) {
              if(pickUpTimer == 0){
                pickUpTimer = millis();
              }
              if (millis() - pickUpTimer >= 3000) {
                pickUpPerformed = true;
                colorMatchStarted = true;
                pickUpTimer = 0;
                stepCnt = 3;
              }
            }
          }
          break;
        case 3:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            colorMatchStarted = true;
            isLineTracking = true;
            if(pickUpPerformed && isColorBlue && current_claw_degrees <= 20 ){
              isLineTracking = false;
              colorMatchStarted = false; // Reset the flag
              if(isItemPicked()){
                Stop();
                delay(200);
                drop_itemBlue();
                dropOffPerformed = true;
                isLineTracking = true;
                colorMatchStarted = true;
                stepCnt = 4;
              }  
            }
          }
          break;
        case 4:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            colorMatchStarted = true;
            isLineTracking = true;
          }
          break;  
      } 
    
      if(isColorRed){
        delay(150);
        Stop();
        delay(800);
        currentCycle = 13;
        jobCycleCounter = 3;
        
      }
      if(isLineTracking){
        black_line();
      } 
    
      if(colorMatchStarted){
        colorMatch();
      }
      if( HC05.available()>0){
        command = HC05.read();
        if(command == 's'){
          carState = STOPPED;
          Stop();
          HC05.println("Vehicle has been STOPPED!!");
        }
      }
    } 
    else if(carState == STOPPED){
      if(HC05.available()>0){
        command = HC05.read();
        if(command == 'n'){
          currentCycle = 12;
          HC05.println("CYCLE 2: BLUE - BLUE STATIONS!!");
          carState = RUNNING;
        }
      }
    }
  }
}

void jobThree() {
  jobThree_flag = true;
  alarmColorOFF();
  bool pickUpPerformed = false; // Flag to track whether pick-up action has been performed
  bool dropOffPerformed = false;
  bool isLineTracking = true;
  bool colorMatchStarted = true;
  
  unsigned long currentAvoidTimer = 0;
  unsigned long currentTime = 0;
  unsigned long continueTimer = 0;
  unsigned long pickUpTimer = 0; // Timer for pick-up delay
  unsigned int stopDistance = 15;
  unsigned int  stepCnt= 1;
 
  while(jobThree_flag && jobCycleCounter == 3) {
    currentCycle = 13;
    if(carState == RUNNING){
      HC05.println("CYCLE 3: PURPLE - PURPLE STATIONS!!");
      Left_Tra_Value = digitalRead(7);
      Center_Tra_Value = digitalRead(8);
      Right_Tra_Value = digitalRead(A1);
      unsigned int distance_cm = sonarItem.ping_cm();
      unsigned int distanceAvoid_cm = sonarAvoid.ping_cm();
      
      if(distanceAvoid_cm <=40 && distanceAvoid_cm > 10){
        if(Left_Tra_Value == Black_Line && Center_Tra_Value == Black_Line && Right_Tra_Value == Black_Line){
          isLineTracking = false;
          Rotate_Left(75);
          delay(200);
          isLineTracking = true;
          Serial.println("if statement <= 40cm > 10cm");
        }
      }  
      else if(distanceAvoid_cm > 40|| distanceAvoid_cm == 0){
        if(Left_Tra_Value == Black_Line && Center_Tra_Value == Black_Line && Right_Tra_Value == Black_Line){
          isLineTracking = false;
          Move_Forward(60);
          delay(100);
          isLineTracking = true;
          Serial.println("if statement >40cm");
        } 
      }  
    
      switch (stepCnt){
        case 1:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            colorMatchStarted = true;
            isLineTracking = true;
            if(isColorPurple && (distance_cm <= stopDistance && distance_cm > 0) && !pickUpPerformed) {
              isLineTracking = false;
              colorMatchStarted = false; // Reset the flag
              if(isItemDropped()){
                Stop();
                delay(200);
                pick_itemPurple();
                stepCnt = 2;
              }
            }else if(current_claw_degrees <= 20){
              stepCnt= 2;
            }
          }
          break;
        case 2:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            isLineTracking = true;
            colorMatchStarted = true;
            isLineTracking = true;
            if (isItemPicked()) {
              if(pickUpTimer == 0){
                pickUpTimer = millis();
              }
              if (millis() - pickUpTimer >= 3000) {
                pickUpPerformed = true;
                colorMatchStarted = true;
                pickUpTimer = 0;
                stepCnt = 3;
              }
            }
          }
          break;
        case 3:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            colorMatchStarted = true;
            isLineTracking = true;
            if(pickUpPerformed && isColorPurple && current_claw_degrees <= 20 ){
              isLineTracking = false;
              colorMatchStarted = false; // Reset the flag
              if(isItemPicked()){
                Stop();
                delay(200);
                drop_itemPurple();
                dropOffPerformed = true;
                isLineTracking = true;
                colorMatchStarted = true;
                stepCnt = 4;
              }  
            }
          }
          break;
        case 4:
          if (distanceAvoid_cm <= 10 && distanceAvoid_cm > 0) {
            Stop();
            alarmColorON();
            isLineTracking = false;
            colorMatchStarted = false;
          } 
          else{
            alarmColorOFF();
            colorMatchStarted = true;
            isLineTracking = true;
          }
          break;  
      } 
    
      if(isColorRed){
        delay(150);
        Stop();
        delay(800);
        currentCycle = 10;
        jobCycleCounter = 4;
        jobThree_flag = false;
        break;
      }
      if(isLineTracking){
        black_line();
      } 
    
      if(colorMatchStarted){
        colorMatch();
      }
      if( HC05.available()>0){
        command = HC05.read();
        if(command == 's'){
          carState = STOPPED;
          Stop();
          HC05.println("Vehicle has been STOPPED!!");
        }
      }
    } 
    else if(carState == STOPPED){
      if(HC05.available()>0){
        command = HC05.read();
        if(command == 'n'){
          currentCycle = 13;
          HC05.println("CYCLE 3: PURPLE - PURPLE STATIONS!!");
          carState = RUNNING;
        }
      }
    }
  }
}


//the black line function to track the vehicle and keep it steady and run on the black route only. 
void black_line(){
  Left_Tra_Value = digitalRead(7);
  Center_Tra_Value = digitalRead(8);
  Right_Tra_Value = digitalRead(A1);
    if (Left_Tra_Value != Black_Line && Center_Tra_Value == Black_Line && Right_Tra_Value != Black_Line) {
      Move_Forward(60);
    } else if (Left_Tra_Value == Black_Line && Center_Tra_Value == Black_Line && Right_Tra_Value != Black_Line) {
      Rotate_Left(50);
    } else if (Left_Tra_Value == Black_Line && Center_Tra_Value != Black_Line && Right_Tra_Value != Black_Line) {
      Rotate_Left(50);
    } else if (Left_Tra_Value != Black_Line && Center_Tra_Value != Black_Line && Right_Tra_Value == Black_Line) {
      Rotate_Right(50);
    } else if (Left_Tra_Value != Black_Line && Center_Tra_Value == Black_Line && Right_Tra_Value == Black_Line) {
      Rotate_Right(50);
    } else if(Center_Tra_Value != Black_Line){
      Rotate_Right(40);
      Rotate_Left(40);
    }

}

//these functions are used to move the vehicle on the direction required, some are retrieved from LAFVIN coding.
void Move_Forward(int speed) {
  digitalWrite(2, LOW);   // 2 DIR pin rear motors
  analogWrite(5, speed); // 5 PWM pin rear motors
  digitalWrite(4, HIGH); // 4 DIR pin front motors
  analogWrite(6, speed); // 6 PWM pin front motors
}

void Move_Backward(int speed) {
  digitalWrite(2, HIGH);
  analogWrite(5, speed);
  digitalWrite(4, LOW);
  analogWrite(6, speed);
}

void Rotate_Left(int speed) {
  digitalWrite(2, LOW);
  analogWrite(5, speed);
  digitalWrite(4, LOW);
  analogWrite(6, speed);
}

void Rotate_Right(int speed) {
  digitalWrite(2, HIGH);
  analogWrite(5, speed);
  digitalWrite(4, HIGH);
  analogWrite(6, speed);
}

void Stop() {
  digitalWrite(2, HIGH);
  analogWrite(5, 0);
  digitalWrite(4, LOW);
  analogWrite(6, 0);
}

//these functions are for robot arm movements and functionality using servo motors
void move_claw(int degrees) {
  //this will map the degree so it is only between 0-180 degrees
  int claw_degrees = map(degrees, 0, 180, SERVO1MIN, SERVO1MAX);
  int current_degrees = claw_degrees;
  //getting the pin anc value set to channel 0 that is used for claw
  pwm.setPWM(0,0,claw_degrees);
  int increment = 2;
  if (claw_degrees > current_degrees) {
    for (int i = current_degrees; i <= claw_degrees; i += increment) {
      pwm.setPWM(0,0,i);
      delay(10); 
    }
  } else if (claw_degrees < current_degrees) {
    for (int i = current_degrees; i >= claw_degrees; i -= increment) {
      pwm.setPWM(0,0,i);
      delay(10); 
    }
  }
  current_claw_degrees = degrees;

}

//move the arm up and down
void move_arm(int degrees) {
  int arm_degrees = map(degrees, 0, 180, SERVO2MIN, SERVO2MAX);
  int current_degrees = arm_degrees;
  pwm.setPWM(1,0,arm_degrees);

  int increment = 2;
  if (arm_degrees > current_degrees) {
    for (int i = current_degrees; i <= arm_degrees; i += increment) {
      pwm.setPWM(1,0,i);
      delay(10); 
    }
  } else if (arm_degrees < current_degrees) {
    for (int i = current_degrees; i >= arm_degrees; i -= increment) {
      pwm.setPWM(1,0,i);
      delay(10); 
    }
  }

}

//rotate the base of the robot arm right and left
void base_movement(int degrees) {
  int base_degrees = map(degrees, 0, 180, SERVO3MIN, SERVO3MAX);
  int current_degrees = base_degrees;
  pwm.setPWM(2,0,base_degrees);

  int increment = 2;
  if (base_degrees > current_degrees) {
    for (int i = current_degrees; i <= base_degrees; i += increment) {
      pwm.setPWM(2,0,i);
      delay(10); 
    }
  } else if (base_degrees < current_degrees) {
    for (int i = current_degrees; i >= base_degrees; i -= increment) {
      pwm.setPWM(2,0,i);
      delay(10); 
    }
  }
 
}

//this will reset Arduino values to default from the command taken from Python with 'g'
void resetProgram(){
  HC05.println("ALL OPERATIONS RESETTED!!");
  jobOne_flag = false;
  jobTwo_flag = false;
  jobThree_flag = false;
  stop_flag = false;
  alarm_flag = false;
  isColorOrange = false;
  isColorGreen = false;
  isColorYellow = false;
  isColorLightBlue = false;
  isColorDarkBlue = false;
  isColorBlue = false;
  isColorDarkOrange = false;
  isColorPurple =false;
  isColorPink = false;
  isColorRed = false;
  jobCycleCounter = 1;
  base_degrees = 86;
  arm_degrees = 30;
  claw_degrees = 60;
  forward_flag = false;
  backward_flag = false;
  left_flag = false;
  right_flag = false;
  isLineTracking = true;
  Left_Tra_Value = 1;
  Center_Tra_Value = 1;
  Right_Tra_Value = 1;
  Black_Line = 1;
  colorPassCnt = 0;
  command = 's';
   
  move_claw(claw_degrees);
  delay(500);
  move_arm(arm_degrees);
  delay(500);
  base_movement(base_degrees); 
  delay(500);
  Stop();

}

//the main function of Arduino program.
void setup() {
  //initialize and set all values and pins in this function
  Wire.begin();
   
  // Initialize the TCS34725 RGB color sensor
  if (tcs.begin()) {
  //Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
     while (1); // halt!
  }
  
  jobOne_flag = false;
  jobTwo_flag = false;
  jobThree_flag = false;
  stop_flag = false;
  alarm_flag = false;
  isColorOrange = false;
  isColorGreen = false;
  isColorYellow = false;
  isColorLightBlue = false;
  isColorDarkBlue = false;
  isColorBlue = false;
  isColorDarkOrange = false;
  isColorPurple =false;
  isColorPink = false;
  isColorRed = false;
  jobCycleCounter = 1;
  base_degrees = 83;
  arm_degrees = 30;
  claw_degrees = 60;
  forward_flag = false;
  backward_flag = false;
  left_flag = false;
  right_flag = false;
  isLineTracking = true;
  Left_Tra_Value = 1;
  Center_Tra_Value = 1;
  Right_Tra_Value = 1;
  Black_Line = 1;
  colorPassCnt = 0;
  command = 's';
  //initialize the servo motor driver board
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
  delay(10);

  //initialize the servo motors angles 
  move_claw(claw_degrees);
  delay(500);
  move_arm(arm_degrees);
  delay(500);
  base_movement(base_degrees); 
  delay(500);
  Stop();

  //assign the pins as either input or output
  pinMode(DIRA_PIN, OUTPUT);
  pinMode(PWMA_PIN, OUTPUT);
  pinMode(DIRB_PIN, OUTPUT);
  pinMode(PWMB_PIN, OUTPUT);
  pinMode(7, INPUT); //tracking sensor center
  pinMode(8, INPUT);  //tracking sensor left
  pinMode(A1, INPUT); //tracking sensor right
   
  alarmColorOFF(); //set the LED alarm to off
   
  pinMode(TRIGGER_PIN, OUTPUT);//A4 pin for this ultrasonic sensor
  pinMode(ECHO_PIN, INPUT); // A5 pin for this ultrasonic sensor
  pinMode(TRIGGER_AVOID_PIN, OUTPUT);
  pinMode(ECHO_AVOID_PIN, INPUT);
  
  HC05.begin(9600);//Bluetooth serial communication Baud rate
  Serial.begin(9600); //USB serial communication Baud rate

}

//Main loop function used in Arduino
void loop() {
 //colorMatch();
 
  //reading the command as available from Python communication serial port HC05
  if (HC05.available() > 0) {
    command = HC05.read();
    processCommand(command);
  
  }
  //this is to reset the cycle if needed
  if (shouldReset) {
    resetProgram();
    shouldReset = false; // Reset the flag to prevent continuous resets
  }

}  
//this function is to determine what command to go to where as it receives it,
void processCommand(char command) {
  // Handle the received command
  Serial.print("Data Received: ");
  Serial.println(command);
  switch (command) {
    case 'o':
      orange_orange_pick_drop();
      break;
    case 'b':
      orange_blue_pick_drop();
      break;
    case 'p':
      orange_purple_pick_drop();
      break;
    
    case 'e':
      blue_blue_pick_drop();
      break;
    case 'l':
      blue_orange_pick_drop();
      break;
    case 'm':
      blue_purple_pick_drop();
      break;
    case 'a':
      purple_purple_pick_drop();
      break;
    case 'c':
      purple_orange_pick_drop();
      break;
    case 'd':
      purple_blue_pick_drop();
      break;
    
    case 'i':
      //used for three cycle loop of jobs 1-3
      while(jobCycleCounter != 4){
        if(jobCycleCounter ==1){
          HC05.println("Starting Simulation 1, ORANGE - ORANGE!!");
          jobOne();
        }else if(jobCycleCounter ==2){
          HC05.println("Starting Simulation 2, BLUE - BLUE!!");
          jobTwo();
        } else if (jobCycleCounter ==3){
          HC05.println("Starting Simulation 3, PURPLE - PURPLE!!");
          jobThree();
        }else{
          break;
        }
      }  
      if(jobCycleCounter == 4){
        Stop();
        shouldReset = true;
        jobCycleCounter = 1;
        break;
      }
      break;
    case 's':
      HC05.println("Vehicle has been STOPPED!!");
      Stop();
      delay(50);
      break;
    case 'g':
      HC05.println("ALL OPERATIONS COMPLETED AND RESETTED!!");
      shouldReset = true;
      break;
    case 'x':
      HC05.write('y');
      break;
    case 'n':
      //this will send n to only function that is running at that moment
      //HC05.println("CONTINUE - CYCLE!!");
      while(currentCycle != 10){
        switch (currentCycle){
          case 1:
            orange_orange_pick_drop();
            
            break;
          case 2:
            orange_blue_pick_drop();
            break;
          case 3:
            orange_purple_pick_drop();
            break;
          case 4:
            blue_blue_pick_drop();
            break;
          case 5:
            blue_orange_pick_drop();
            break;
          case 6:
            blue_purple_pick_drop();
            break;
          case 7:
            purple_purple_pick_drop();
            break;
          case 8:
            purple_orange_pick_drop();
            break;
          case 9:
            purple_blue_pick_drop();
            break;
          case 11:
            jobOne();
            break;
          case 12:
            jobTwo();
            break;
          case 13:
            jobThree();
            break;
          case 10:
            break;
        }
      }
      break;
    
  }
  
}
