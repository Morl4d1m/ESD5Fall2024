#include <Arduino.h>            // Necessary for accessing FreeRTOS via Arduino IDE
#include "Wire.h"               // Necessary for I2C communication
#include <freertos/FreeRTOS.h>  // Necessary for accessing FreeRTOS
#include <freertos/task.h>      // Necessary for using tasks in FreeRTOS
#include <freertos/semphr.h>    // Enables semaphores

#define INCLUDE_vTaskSuspend 1  // Definition of suspend function being active
#define builtInLED 2            // Builtin LED pin for debugging
#define somethingIsWrongPin 12  // Pin to light up red LED
#define allGoodPin 14           // Pin to light op green LED
#define emergencyButtonPin 33   // Emergency button connection pin
#define emergencyLightPin 26    // LED that signifies an emergency has happened
#define joystickInputXPin 36
#define joystickInputYPin 39
#define joystickSwitchPin 34
#define hBridgeInput1 32
#define hBridgeInput2 33
#define hBridgeInput3 25
#define hBridgeInput4 26

//Global variables
signed int desiredMove = 0;
signed int desiredTurn = 0;
signed int joystickInputX = 0;
signed int joystickInputY = 0;
signed int buttonPresses = 1;

void setup() {
  Serial.begin(115200);   // Initialisation of serial monitor
  Wire.begin();           // Join I2C bus
  Wire.setClock(400000);  // 400khz

  pinMode(builtInLED, OUTPUT);           // Enables the LED for debugging
  pinMode(somethingIsWrongPin, OUTPUT);  // Enables the pin to be used as a power-source
  pinMode(allGoodPin, OUTPUT);           // Enables the pin to be used as a power-source
  pinMode(emergencyButtonPin, INPUT);    // Enables the pin to be used as a input
  pinMode(emergencyLightPin, OUTPUT);    // Enables the pin to be used as a power-source
  pinMode(joystickInputXPin, INPUT);
  pinMode(joystickInputYPin, INPUT);
  pinMode(joystickSwitchPin, INPUT_PULLDOWN);
  pinMode(hBridgeInput1, OUTPUT);
  pinMode(hBridgeInput2, OUTPUT);
  pinMode(hBridgeInput3, OUTPUT);
  pinMode(hBridgeInput4, OUTPUT);
}

void loop() {
  //Serial.println(micros());
  //movement();
  if (digitalRead(joystickSwitchPin) == LOW) {
    delay(5000);
    buttonPresses++;
  }
  difference07Adjusted();
  //difference12();
  delayMicroseconds(345);
  //spinMeRightRound();
  //spinMeLeftRound();
  //Serial.println(buttonPresses);
  //Serial.println(micros());
  //delay(6000);
  //turnDesired();
  //Serial.println(joystickInputX);
  //Serial.println(joystickInputY);
}

void movement() {
  joystickInputX = analogRead(joystickInputXPin);  // Reading from the joystick saved as input value
  joystickInputY = analogRead(joystickInputYPin);  // Reading from the joystick saved as input value

  // Calculate desiredMove as a value between 0 and 255, based on joystick input range
  if (joystickInputX >= 2400 && joystickInputY >= 2400) {
    desiredMove = map(joystickInputX, 2400, 4095, 0, 255);  // Scale to range 0-255 for forward
    desiredMove = constrain(desiredMove, 0, 255);           // Ensure values stay between 0 and 255
    desiredTurn = map(joystickInputY, 2400, 4095, 0, 255);  // Scale to range 0-255 for turning right
    desiredTurn = constrain(desiredTurn, 0, 255);           // Ensure values stay between 0 and 255
    analogWrite(hBridgeInput1, desiredMove + desiredTurn);
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, (desiredMove / 2) + (desiredTurn / 2));
    analogWrite(hBridgeInput4, 0);
  } else if (joystickInputX >= 2400 && joystickInputY <= 1600) {
    desiredMove = map(joystickInputX, 2400, 4095, 0, 255);  // Scale to range 0-255 for forward
    desiredMove = constrain(desiredMove, 0, 255);           // Ensure values stay between 0 and 255
    desiredTurn = map(joystickInputY, 1600, 0, 0, 255);     // Scale to range 0-255 for turning right
    desiredTurn = constrain(desiredTurn, 25, 255);           // Ensure values stay between 0 and 255
    analogWrite(hBridgeInput1, (desiredMove / 2) + (desiredTurn / 2));
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, desiredMove + desiredTurn);
    analogWrite(hBridgeInput4, 0);
  } else if (joystickInputX <= 1600 && joystickInputY >= 2400) {
    desiredMove = map(joystickInputX, 1600, 0, 0, 255);     // Scale to range 0-255 for forward
    desiredMove = constrain(desiredMove, 0, 255);           // Ensure values stay between 0 and 255
    desiredTurn = map(joystickInputY, 2400, 4095, 0, 255);  // Scale to range 0-255 for turning right
    desiredTurn = constrain(desiredTurn, 0, 255);           // Ensure values stay between 0 and 255
    analogWrite(hBridgeInput1, 0);
    analogWrite(hBridgeInput2, desiredMove + desiredTurn);
    analogWrite(hBridgeInput3, 0);
    analogWrite(hBridgeInput4, (desiredMove / 2) + (desiredTurn / 2));
  } else if (joystickInputX <= 1600 && joystickInputY <= 1600) {
    desiredMove = map(joystickInputX, 1600, 0, 0, 255);  // Scale to range 0-255 for forward
    desiredMove = constrain(desiredMove, 0, 255);        // Ensure values stay between 0 and 255
    desiredTurn = map(joystickInputY, 1600, 0, 0, 255);  // Scale to range 0-255 for turning right
    desiredTurn = constrain(desiredTurn, 0, 255);        // Ensure values stay between 0 and 255
    analogWrite(hBridgeInput1, 0);
    analogWrite(hBridgeInput2, (desiredMove / 2) + (desiredTurn / 2));
    analogWrite(hBridgeInput3, 0);
    analogWrite(hBridgeInput4, desiredMove + desiredTurn);
  } else if (joystickInputX >= 2400) {                       // Forward movement (joystick top)
    desiredMove = map(joystickInputX, 2400, 4095, 0, 255);  // Scale to range 0-255 for forward
    desiredMove = constrain(desiredMove, 0, 255);           // Ensure values stay between 0 and 255
    analogWrite(hBridgeInput1, desiredMove);                 // Sets the PWM output to current desiredMove value, making the left belt move forward
    analogWrite(hBridgeInput3, desiredMove);                 // Sets the PWM output to current desiredMove value, making the right belt move forward
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput4, 0);
  } else if (joystickInputX <= 1600) {                    // Backward movement (joystick bottom)
    desiredMove = map(joystickInputX, 0, 1600, 255, 0);  // Scale to range 255-0 for backward
    desiredMove = constrain(desiredMove, 0, 255);        // Ensure values stay between 0 and 255
    analogWrite(hBridgeInput2, desiredMove);              // Sets the PWM output to current desiredMove value, making the left belt move backward
    analogWrite(hBridgeInput4, desiredMove);              // Sets the PWM output to current desiredMove value, making the right belt move backward
    analogWrite(hBridgeInput1, 0);
    analogWrite(hBridgeInput3, 0);
  } else if (joystickInputY >= 2400) {                       // Turning right movement (joystick right)
    desiredTurn = map(joystickInputY, 2400, 4095, 0, 255);  // Scale to range 0-255 for turning right
    desiredTurn = constrain(desiredTurn, 0, 255);           // Ensure values stay between 0 and 255
    analogWrite(hBridgeInput1, desiredTurn);                 // Sets the PWM output to current desiredMove value, making the right belt move forward
    analogWrite(hBridgeInput4, desiredTurn);                 // Sets the PWM output to current desiredMove value, making the left belt move backward
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0);
  } else if (joystickInputY <= 1600) {                    // Turn left movement (joystick left)
    desiredTurn = map(joystickInputY, 1600, 0, 0, 255);  // Scale to range 0-255 for turning left
    desiredTurn = constrain(desiredTurn, 0, 255);        // Ensure values stay between 0 and 255
    analogWrite(hBridgeInput2, desiredTurn);              // Sets the PWM output to current desiredMove value, making the left belt move forward
    analogWrite(hBridgeInput3, desiredTurn);              // Sets the PWM output to current desiredMove value, making the right belt move backward
    analogWrite(hBridgeInput1, 0);
    analogWrite(hBridgeInput4, 0);
  } else {                                    // Neutral position, stop movement
    desiredTurn = 0;                          // Reset desiredTurn to 0, so the de-weeder parks flat again
    analogWrite(hBridgeInput1, desiredTurn);  // Stop left belt forward
    analogWrite(hBridgeInput3, desiredTurn);  // Stop right belt forward
    analogWrite(hBridgeInput2, desiredTurn);  // Stop left belt backward
    analogWrite(hBridgeInput4, desiredTurn);  // Stop right belt backward
    desiredMove = 0;                          // Reset desiredMove to 0, so the de-weeder parks flat again
    analogWrite(hBridgeInput1, desiredMove);  // Stop left belt forward
    analogWrite(hBridgeInput3, desiredMove);  // Stop right belt forward
    analogWrite(hBridgeInput2, desiredMove);  // Stop left belt backward
    analogWrite(hBridgeInput4, desiredMove);  // Stop right belt backward
  }
  //Serial.println("movement");  // Status update to figure out which function is running
}

void spinMeRightRound(){
  if (buttonPresses > 1) {Serial.println("Turning");
    analogWrite(hBridgeInput1, 0.99 * 255);
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0);
    analogWrite(hBridgeInput4, 0.99 * 255);
  //  Serial.println("Turning left");
  }
  else {  // Drive straight ahead
    analogWrite(hBridgeInput1, 0);
    analogWrite(hBridgeInput2, 0);//.17*255);
    analogWrite(hBridgeInput3, 0);
    analogWrite(hBridgeInput4, 0);//.17*255);
  //  Serial.println("Straight");
  }
}
void spinMeLeftRound(){
  if (buttonPresses > 1) {Serial.println("Turning");
    //delay(2000);
    analogWrite(hBridgeInput1, 0);
    analogWrite(hBridgeInput2, 0.99 * 255);
    analogWrite(hBridgeInput3, 0.99 * 255);
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Turning left");
  }
  else {  // Drive straight ahead
    analogWrite(hBridgeInput1, 0);//.17 * 255);
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0);//.17 * 255);
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Straight");
  }
}

void difference01() {
  if (buttonPresses > 1) {Serial.println("Turning");
    analogWrite(hBridgeInput1, 0.53 * 255);
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.50 * 255);
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Turning left");
  }
  else {  // Drive straight ahead
    analogWrite(hBridgeInput1, 0.53 * 255);
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.53 * 255);
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Straight");
  }
}

void difference02() {
  if (buttonPresses > 1) {Serial.println("Turning");
    analogWrite(hBridgeInput1, 0.0 * 255);
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.50 * 255);
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Turning left");
  }
  else {  // Drive straight ahead
    analogWrite(hBridgeInput1, 0.53 * 255);
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.53 * 255);
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Straight");
  }
}

void difference03() {
  if (buttonPresses > 1) {Serial.println("Turning");
    analogWrite(hBridgeInput1, 0.0 * 255);
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.44 * 255);
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Turning left");
  }
  else {  // Drive straight ahead
    analogWrite(hBridgeInput1, 0.53 * 255);
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.53 * 255);
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Straight");
  }
}

void difference04() {
  if (buttonPresses > 1) {Serial.println("Turning");
    analogWrite(hBridgeInput1, 0.58 * 255);
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.44 * 255);
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Turning left");
  }
  else {  // Drive straight ahead
    analogWrite(hBridgeInput1, 0.53 * 255);
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.53 * 255);
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Straight");
  }
}

void difference05() {
  if (buttonPresses > 1) {Serial.println("Turning");
    analogWrite(hBridgeInput1, 0.58 * 255);
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.37 * 255);
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Turning left");
  }
  else {  // Drive straight ahead
    analogWrite(hBridgeInput1, 0.53 * 255);
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.53 * 255);
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Straight");
  }
}

void difference06() {
  if (buttonPresses > 1) {Serial.println("Turning");
    analogWrite(hBridgeInput1, 0.58 * 255);
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.29 * 255);
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Turning left");
  }
  else {  // Drive straight ahead
    analogWrite(hBridgeInput1, 0.53 * 255);
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.53 * 255);
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Straight");
  }
}

void difference07() {
  if (buttonPresses > 1) {Serial.println("Turning");
    analogWrite(hBridgeInput1, 0.60 * 255);
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.29 * 255);
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Turning left");
  }
  else {  // Drive straight ahead
    analogWrite(hBridgeInput1, 0.53 * 255);
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.53 * 255);
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Straight");
  }
}

void difference08() {
  if (buttonPresses > 1) {Serial.println("Turning");
    analogWrite(hBridgeInput1, 0.60 * 255);
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.24 * 255);
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Turning left");
  }
  else {  // Drive straight ahead
    analogWrite(hBridgeInput1, 0.53 * 255);
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.53 * 255);
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Straight");
  }
}

void difference09() {
  if (buttonPresses > 1) {Serial.println("Turning");
    analogWrite(hBridgeInput1, 0.64 * 255);
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.24 * 255);
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Turning left");
  }
  else {  // Drive straight ahead
    analogWrite(hBridgeInput1, 0.53 * 255);
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.53 * 255);
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Straight");
  }
}

void difference10() {
  if (buttonPresses > 1) {Serial.println("Turning");
    analogWrite(hBridgeInput1, 0.64 * 255);
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.17 * 255);
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Turning left");
  }
  else {  // Drive straight ahead
    analogWrite(hBridgeInput1, 0.53 * 255);
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.53 * 255);
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Straight");
  }
}

void difference11() {
  if (buttonPresses > 1) {Serial.println("Turning");
    analogWrite(hBridgeInput1, 0.71 * 255);
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.17 * 255);
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Turning left");
  }
  else {  // Drive straight ahead
    analogWrite(hBridgeInput1, 0.53 * 255);
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.53 * 255);
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Straight");
  }
}

void difference12() {
  if (buttonPresses > 1) {Serial.println("Turning");
    analogWrite(hBridgeInput1, 0.71 * 255);
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.11 * 255);
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Turning left");
  }
  else {  // Drive straight ahead
    analogWrite(hBridgeInput1, 0.53 * 255);
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.53 * 255);
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Straight");
  }
}

void difference13() {
  if (buttonPresses > 1) {Serial.println("Turning");
    analogWrite(hBridgeInput1, 0.99 * 255);
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.11* 255);
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Turning left");
  }
  else {  // Drive straight ahead
    analogWrite(hBridgeInput1, 0.53 * 255);
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.53 * 255);
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Straight");
  }
}
void difference01Adjusted() {
  if (buttonPresses > 1) {Serial.println("Turning");
    analogWrite(hBridgeInput1, 0.49 * 255); //svarer til 1,2 
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.52 * 255); //svarer til 1,1
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Turning left");
  }
  else {  // Drive straight ahead
    analogWrite(hBridgeInput1, 0.49 * 255); //svarer til 1,2
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.57 * 255); //svarer til 1,2
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Straight");
  }
}

void difference02Adjusted() {
  if (buttonPresses > 1) {Serial.println("Turning");
    analogWrite(hBridgeInput1, 0.53 * 255); //svarer til 1,3
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.52 * 255); //svarer til 1,1
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Turning left");
  }
  else {  // Drive straight ahead
    analogWrite(hBridgeInput1, 0.49 * 255); //svarer til 1,2
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.57 * 255); //svarer til 1,2
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Straight");
  }
}

void difference03Adjusted() {
  if (buttonPresses > 1) {Serial.println("Turning");
    analogWrite(hBridgeInput1, 0.53 * 255); //svarer til 1,3
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.46 * 255); //svarer til 1,0
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Turning left");
  }
  else {  // Drive straight ahead
    analogWrite(hBridgeInput1, 0.49 * 255); //svarer til 1,2
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.57 * 255); //svarer til 1,2
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Straight");
  }
}

void difference04Adjusted() {
  if (buttonPresses > 1) {Serial.println("Turning");
    analogWrite(hBridgeInput1, 0.55 * 255); //svarer til 1,4
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.46 * 255); //svarer til 1,0
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Turning left");
  }
  else {  // Drive straight ahead
    analogWrite(hBridgeInput1, 0.49 * 255); //svarer til 1,2
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.57 * 255); //svarer til 1,2
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Straight");
  }
}

void difference05Adjusted() {
  if (buttonPresses > 1) {Serial.println("Turning");
    analogWrite(hBridgeInput1, 0.55 * 255); //svarer til 1,4
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.39 * 255); //svarer til 0,9
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Turning left");
  }
  else {  // Drive straight ahead
    analogWrite(hBridgeInput1, 0.49 * 255); //svarer til 1,2
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.57 * 255); //svarer til 1,2
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Straight");
  }
}

void difference06Adjusted() {
  if (buttonPresses > 1) {Serial.println("Turning");
    analogWrite(hBridgeInput1, 0.55 * 255); //svarer til 1,4
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.32 * 255); //svarer til 0,8
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Turning left");
  }
  else {  // Drive straight ahead
    analogWrite(hBridgeInput1, 0.49 * 255); //svarer til 1,2
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.57 * 255); //svarer til 1,2
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Straight");
  }
}

void difference07Adjusted() {
  if (buttonPresses > 1) {Serial.println("Turning");
    analogWrite(hBridgeInput1, 0.57 * 255); //svarer til 1,5
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.32 * 255); //svarer til 0,8
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Turning left");
  }
  else {  // Drive straight ahead
    analogWrite(hBridgeInput1, 0.49 * 255); //svarer til 1,2
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.57 * 255); //svarer til 1,2
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Straight");
  }
}

void difference08Adjusted() {
  if (buttonPresses > 1) {Serial.println("Turning");
    analogWrite(hBridgeInput1, 0.57 * 255); //svarer til 1,5
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.26 * 255); //svarer til 0,7
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Turning left");
  }
  else {  // Drive straight ahead
    analogWrite(hBridgeInput1, 0.49 * 255); //svarer til 1,2
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.57 * 255); //svarer til 1,2
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Straight");
  }
}

void difference09Adjusted() {
  if (buttonPresses > 1) {Serial.println("Turning");
    analogWrite(hBridgeInput1, 0.62 * 255); //svarer til 1,6
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.26 * 255); //svarer til 0,7
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Turning left");
  }
  else {  // Drive straight ahead
    analogWrite(hBridgeInput1, 0.49 * 255); //svarer til 1,2
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.57 * 255); //svarer til 1,2
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Straight");
  }
}

void difference10Adjusted() {
  if (buttonPresses > 1) {Serial.println("Turning");
    analogWrite(hBridgeInput1, 0.62 * 255); //svarer til 1,6
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.19 * 255); //svarer til 0,6
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Turning left");
  }
  else {  // Drive straight ahead
    analogWrite(hBridgeInput1, 0.49 * 255); //svarer til 1,2
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.57 * 255); //svarer til 1,2
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Straight");
  }
}

void difference11Adjusted() {
  if (buttonPresses > 1) {Serial.println("Turning");
    analogWrite(hBridgeInput1, 0.70 * 255); //svarer til 1,7
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.19 * 255); //svarer til 0,6
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Turning left");
  }
  else {  // Drive straight ahead
    analogWrite(hBridgeInput1, 0.49 * 255); //svarer til 1,2
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.57 * 255); //svarer til 1,2
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Straight");
  }
}

void difference12Adjusted() {
  if (buttonPresses > 1) {Serial.println("Turning");
    analogWrite(hBridgeInput1, 0.70 * 255); //svarer til 1,7
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.15 * 255); //svarer til 0,5
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Turning left");
  }
  else {  // Drive straight ahead
    analogWrite(hBridgeInput1, 0.49 * 255); //svarer til 1,2
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.57 * 255); //svarer til 1,2
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Straight");
  }
}

void difference13Adjusted() {
  if (buttonPresses > 1) {Serial.println("Turning");
    analogWrite(hBridgeInput1, 0.98 * 255); //svarer til 1,8
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.15* 255); //svarer til 0,5
    analogWrite(hBridgeInput4, 0);
    Serial.println("Turning left");
  }
  else {  // Drive straight ahead
    analogWrite(hBridgeInput1, 0.49 * 255); //svarer til 1,2
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0.57 * 255); //svarer til 1,2
    analogWrite(hBridgeInput4, 0);
  //  Serial.println("Straight");
  }
}


/*
void moveDesired() {
  joystickInputX = analogRead(joystickInputXPin);  // Reading from the joystick saved as input value

  // Calculate desiredMove as a value between 0 and 255, based on joystick input range
  if (joystickInputX >= 2400) {                              // Forward movement (joystick top)
    desiredMove = map(joystickInputX, 2400, 4095, 0, 255);  // Scale to range 0-255 for forward
    desiredMove = constrain(desiredMove, 0, 255);           // Ensure values stay between 0 and 255
    analogWrite(hBridgeInput1, desiredMove);                 // Sets the PWM output to current desiredMove value, making the left belt move forward
    analogWrite(hBridgeInput3, desiredMove);                 // Sets the PWM output to current desiredMove value, making the right belt move forward
  } else if (joystickInputX <= 1600) {                       // Backward movement (joystick bottom)
    desiredMove = map(joystickInputX, 0, 1600, 255, 0);     // Scale to range 255-0 for backward
    desiredMove = constrain(desiredMove, 0, 255);           // Ensure values stay between 0 and 255
    analogWrite(hBridgeInput2, desiredMove);                 // Sets the PWM output to current desiredMove value, making the left belt move backward
    analogWrite(hBridgeInput4, desiredMove);                 // Sets the PWM output to current desiredMove value, making the right belt move backward
  } else {                                                   // Neutral position, stop movement
    desiredMove = 0;                                         // Reset desiredMove to 0, so the de-weeder parks flat again
    analogWrite(hBridgeInput1, desiredMove);                 // Stop left belt forward
    analogWrite(hBridgeInput3, desiredMove);                 // Stop right belt forward
    analogWrite(hBridgeInput2, desiredMove);                 // Stop left belt backward
    analogWrite(hBridgeInput4, desiredMove);                 // Stop right belt backward
  }

//  Serial.println("moveDesired");  // Status update to figure out which function is running
//  Serial.println(desiredMove);
}
void turnDesired() {
  joystickInputY = analogRead(joystickInputYPin);  // Reading from the joystick saved as input value

  // Calculate desiredTurn as a value between 0 and 255, based on joystick input range
  if (joystickInputY >= 2400) {                              // Turning right movement (joystick right)
    desiredTurn = map(joystickInputY, 2400, 4095, 0, 255);  // Scale to range 0-255 for turning right
    desiredTurn = constrain(desiredTurn, 25, 255);           // Ensure values stay between 0 and 255
    analogWrite(hBridgeInput3, desiredTurn);                 // Sets the PWM output to current desiredMove value, making the right belt move forward
    analogWrite(hBridgeInput2, desiredTurn);                 // Sets the PWM output to current desiredMove value, making the left belt move backward
  } else if (joystickInputY <= 1600) {                       // Turn left movement (joystick left)
    desiredTurn = map(joystickInputY, 1600, 0, 0, 255);     // Scale to range 0-255 for turning left
    desiredTurn = constrain(desiredTurn, 0, 255);           // Ensure values stay between 0 and 255
    analogWrite(hBridgeInput1, desiredTurn);                 // Sets the PWM output to current desiredMove value, making the left belt move forward
    analogWrite(hBridgeInput4, desiredTurn);                 // Sets the PWM output to current desiredMove value, making the right belt move backward
  } else {                                                   // Neutral position, stop movement
    desiredTurn = 0;                                         // Reset desiredTurn to 0, so the de-weeder parks flat again
    analogWrite(hBridgeInput1, desiredTurn);                 // Stop left belt forward
    analogWrite(hBridgeInput3, desiredTurn);                 // Stop right belt forward
    analogWrite(hBridgeInput2, desiredTurn);                 // Stop left belt backward
    analogWrite(hBridgeInput4, desiredTurn);                 // Stop right belt backward
  }
//  Serial.println("turnDesired");  // Status update to figure out which function is running
//  Serial.println(desiredTurn);
}*/