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
const int pwmFreq = 2000;   // Set PWM frequency to 5kHz
const int pwmResolution = 8; // Set resolution to 8-bit
// Define PWM channels and pins
#define pwmChannel1  1
#define pwmChannel2  2
#define pwmChannel3  3
#define pwmChannel4  4

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
  
  // Attach PWM pins to channels and configure
  ledcAttach(hBridgeInput1, pwmChannel1, pwmResolution);
  ledcAttach(hBridgeInput2, pwmChannel2, pwmResolution);
  ledcAttach(hBridgeInput3, pwmChannel3, pwmResolution);
  ledcAttach(hBridgeInput4, pwmChannel4, pwmResolution);

  // Configure each PWM channel
  //ledcSetup(pwmChannel1, pwmFreq, pwmResolution);
  //ledcSetup(pwmChannel2, pwmFreq, pwmResolution);
  //ledcSetup(pwmChannel3, pwmFreq, pwmResolution);
  //ledcSetup(pwmChannel4, pwmFreq, pwmResolution);
}

void loop() {
  //Serial.println(micros());
  //movement();
  if (digitalRead(joystickSwitchPin) == LOW) {
    delay(50);
    buttonPresses++;
  }
  //difference12();
  //delayMicroseconds(345);
  //spinMeRightRound();
  spinMeLeftRound();
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

  // Calculate desiredMove as a value between 56 and 128, based on joystick input range
  if (joystickInputX >= 2400 && joystickInputY >= 2400) {
    desiredMove = map(joystickInputX, 2400, 4095, 56, 127);  // Scale to range 56-128 for forward
    desiredMove = constrain(desiredMove, 56, 127);           // Ensure values stay between 56 and 128
    desiredTurn = map(joystickInputY, 2400, 4095, 56, 127);  // Scale to range 56-128 for turning right
    desiredTurn = constrain(desiredTurn, 56, 127);           // Ensure values stay between 56 and 128
    analogWrite(hBridgeInput1, desiredMove + desiredTurn);
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, (desiredMove / 2) + (desiredTurn / 2));
    analogWrite(hBridgeInput4, 0);
  } else if (joystickInputX >= 2400 && joystickInputY <= 1600) {
    desiredMove = map(joystickInputX, 2400, 4095, 56, 127);  // Scale to range 56-128 for forward
    desiredMove = constrain(desiredMove, 56, 127);           // Ensure values stay between 56 and 128
    desiredTurn = map(joystickInputY, 1600, 0, 56, 127);     // Scale to range 56-128 for turning right
    desiredTurn = constrain(desiredTurn, 25, 127);           // Ensure values stay between 56 and 128
    analogWrite(hBridgeInput1, (desiredMove / 2) + (desiredTurn / 2));
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, desiredMove + desiredTurn);
    analogWrite(hBridgeInput4, 0);
  } else if (joystickInputX <= 1600 && joystickInputY >= 2400) {
    desiredMove = map(joystickInputX, 1600, 0, 56, 127);     // Scale to range 56-128 for forward
    desiredMove = constrain(desiredMove, 56, 127);           // Ensure values stay between 56 and 128
    desiredTurn = map(joystickInputY, 2400, 4095, 56, 127);  // Scale to range 56-128 for turning right
    desiredTurn = constrain(desiredTurn, 56, 127);           // Ensure values stay between 56 and 128
    analogWrite(hBridgeInput1, 0);
    analogWrite(hBridgeInput2, desiredMove + desiredTurn);
    analogWrite(hBridgeInput3, 0);
    analogWrite(hBridgeInput4, (desiredMove / 2) + (desiredTurn / 2));
  } else if (joystickInputX <= 1600 && joystickInputY <= 1600) {
    desiredMove = map(joystickInputX, 1600, 0, 56, 127);  // Scale to range 56-128 for forward
    desiredMove = constrain(desiredMove, 56, 127);        // Ensure values stay between 56 and 128
    desiredTurn = map(joystickInputY, 1600, 0, 56, 127);  // Scale to range 56-128 for turning right
    desiredTurn = constrain(desiredTurn, 56, 127);        // Ensure values stay between 56 and 128
    analogWrite(hBridgeInput1, 0);
    analogWrite(hBridgeInput2, (desiredMove / 2) + (desiredTurn / 2));
    analogWrite(hBridgeInput3, 0);
    analogWrite(hBridgeInput4, desiredMove + desiredTurn);
  } else if (joystickInputX >= 2400) {                       // Forward movement (joystick top)
    desiredMove = map(joystickInputX, 2400, 4095, 56, 128);  // Scale to range 56-128 for forward
    desiredMove = constrain(desiredMove, 56, 128);           // Ensure values stay between 56 and 128
    analogWrite(hBridgeInput1, desiredMove);                 // Sets the PWM output to current desiredMove value, making the left belt move forward
    analogWrite(hBridgeInput3, desiredMove);                 // Sets the PWM output to current desiredMove value, making the right belt move forward
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput4, 0);
  } else if (joystickInputX <= 1600) {                    // Backward movement (joystick bottom)
    desiredMove = map(joystickInputX, 0, 1600, 128, 56);  // Scale to range 128-56 for backward
    desiredMove = constrain(desiredMove, 56, 128);        // Ensure values stay between 56 and 128
    analogWrite(hBridgeInput2, desiredMove);              // Sets the PWM output to current desiredMove value, making the left belt move backward
    analogWrite(hBridgeInput4, desiredMove);              // Sets the PWM output to current desiredMove value, making the right belt move backward
    analogWrite(hBridgeInput1, 0);
    analogWrite(hBridgeInput3, 0);
  } else if (joystickInputY >= 2400) {                       // Turning right movement (joystick right)
    desiredTurn = map(joystickInputY, 2400, 4095, 0, 255);  // Scale to range 56-128 for turning right
    desiredTurn = constrain(desiredTurn, 0, 255);           // Ensure values stay between 56 and 128
    analogWrite(hBridgeInput1, desiredTurn);                 // Sets the PWM output to current desiredMove value, making the right belt move forward
    analogWrite(hBridgeInput4, desiredTurn);                 // Sets the PWM output to current desiredMove value, making the left belt move backward
    analogWrite(hBridgeInput2, 0);
    analogWrite(hBridgeInput3, 0);
  } else if (joystickInputY <= 1600) {                    // Turn left movement (joystick left)
    desiredTurn = map(joystickInputY, 1600, 0, 0, 255);  // Scale to range 56-128 for turning left
    desiredTurn = constrain(desiredTurn, 0, 255);        // Ensure values stay between 56 and 128
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
  if (buttonPresses > 1) {
    ledcAttach(hBridgeInput1, pwmChannel1, pwmResolution);
    ledcWrite(pwmChannel1, 0.53 * 255);
    ledcDetach(hBridgeInput2);
    ledcDetach(hBridgeInput3);
    ledcWrite(pwmChannel4, 0.53 * 255);

  //  Serial.println("Turning left");
  }
  else {  // Drive straight ahead
    ledcDetach(hBridgeInput1);
    ledcWrite(pwmChannel2, 0.11 * 255);
    ledcDetach(hBridgeInput3);
    ledcWrite(pwmChannel4, 0.11 * 255);
  //  Serial.println("Straight");
  }
}
void spinMeLeftRound() {
  if (buttonPresses > 1) {
    ledcAttach(hBridgeInput3, pwmChannel3, pwmResolution);
    ledcDetach(hBridgeInput1);
    ledcWrite(pwmChannel2, 0.11 * 255);
    ledcWrite(pwmChannel3, 0.11 * 255);
    ledcDetach(hBridgeInput4);
  } else {  // Drive straight ahead
    ledcDetach(hBridgeInput1);
    ledcWrite(pwmChannel2, 0.11 * 255);
    ledcDetach(hBridgeInput3);
    ledcWrite(pwmChannel4, 0.11 * 255);
    Serial.println("Straight");
  }
}

void difference01() {
  if (buttonPresses > 1) {
    ledcWrite(pwmChannel1, 0.53 * 255);
    ledcDetach(hBridgeInput2);
    ledcWrite(pwmChannel3, 0.50 * 255);
    ledcDetach(hBridgeInput4);
  } else {  // Drive straight ahead
    ledcWrite(pwmChannel1, 0.53 * 255);
    ledcDetach(hBridgeInput2);
    ledcWrite(pwmChannel3, 0.53 * 255);
    ledcDetach(hBridgeInput4);
  }
}

void difference02() {
  if (buttonPresses > 1) {
    ledcWrite(pwmChannel1, 0.56 * 255);
    ledcDetach(hBridgeInput2);
    ledcWrite(pwmChannel3, 0.50 * 255);
    ledcDetach(hBridgeInput4);
  } else {
    ledcWrite(pwmChannel1, 0.53 * 255);
    ledcDetach(hBridgeInput2);
    ledcWrite(pwmChannel3, 0.53 * 255);
    ledcDetach(hBridgeInput4);
  }
}

void difference03() {
  if (buttonPresses > 1) {
    ledcWrite(pwmChannel1, 0.56 * 255);
    ledcDetach(hBridgeInput2);
    ledcWrite(pwmChannel3, 0.44 * 255);
    ledcDetach(hBridgeInput4);
  } else {
    ledcWrite(pwmChannel1, 0.53 * 255);
    ledcDetach(hBridgeInput2);
    ledcWrite(pwmChannel3, 0.53 * 255);
    ledcDetach(hBridgeInput4);
  }
}

void difference04() {
  if (buttonPresses > 1) {
    ledcWrite(pwmChannel1, 0.58 * 255);
    ledcDetach(hBridgeInput2);
    ledcWrite(pwmChannel3, 0.44 * 255);
    ledcDetach(hBridgeInput4);
  } else {
    ledcWrite(pwmChannel1, 0.53 * 255);
    ledcDetach(hBridgeInput2);
    ledcWrite(pwmChannel3, 0.53 * 255);
    ledcDetach(hBridgeInput4);
  }
}

void difference05() {
  if (buttonPresses > 1) {
    ledcWrite(pwmChannel1, 0.58 * 255);
    ledcDetach(hBridgeInput2);
    ledcWrite(pwmChannel3, 0.37 * 255);
    ledcDetach(hBridgeInput4);
  } else {
    ledcWrite(pwmChannel1, 0.53 * 255);
    ledcDetach(hBridgeInput2);
    ledcWrite(pwmChannel3, 0.53 * 255);
    ledcDetach(hBridgeInput4);
  }
}

void difference06() {
  if (buttonPresses > 1) {
    ledcWrite(pwmChannel1, 0.58 * 255);
    ledcDetach(hBridgeInput2);
    ledcWrite(pwmChannel3, 0.29 * 255);
    ledcDetach(hBridgeInput4);
  } else {
    ledcWrite(pwmChannel1, 0.53 * 255);
    ledcDetach(hBridgeInput2);
    ledcWrite(pwmChannel3, 0.53 * 255);
    ledcDetach(hBridgeInput4);
  }
}

void difference07() {
  if (buttonPresses > 1) {
    ledcWrite(pwmChannel1, 0.60 * 255);
    ledcDetach(hBridgeInput2);
    ledcWrite(pwmChannel3, 0.29 * 255);
    ledcDetach(hBridgeInput4);
  } else {
    ledcWrite(pwmChannel1, 0.53 * 255);
    ledcDetach(hBridgeInput2);
    ledcWrite(pwmChannel3, 0.53 * 255);
    ledcDetach(hBridgeInput4);
  }
}

void difference08() {
  if (buttonPresses > 1) {
    ledcWrite(pwmChannel1, 0.60 * 255);
    ledcDetach(hBridgeInput2);
    ledcWrite(pwmChannel3, 0.24 * 255);
    ledcDetach(hBridgeInput4);
  } else {
    ledcWrite(pwmChannel1, 0.53 * 255);
    ledcDetach(hBridgeInput2);
    ledcWrite(pwmChannel3, 0.53 * 255);
    ledcDetach(hBridgeInput4);
  }
}

void difference09() {
  if (buttonPresses > 1) {
    ledcWrite(pwmChannel1, 0.64 * 255);
    ledcDetach(hBridgeInput2);
    ledcWrite(pwmChannel3, 0.24 * 255);
    ledcDetach(hBridgeInput4);
  } else {
    ledcWrite(pwmChannel1, 0.53 * 255);
    ledcDetach(hBridgeInput2);
    ledcWrite(pwmChannel3, 0.53 * 255);
    ledcDetach(hBridgeInput4);
  }
}

void difference10() {
  if (buttonPresses > 1) {
    ledcWrite(pwmChannel1, 0.64 * 255);
    ledcDetach(hBridgeInput2);
    ledcWrite(pwmChannel3, 0.17 * 255);
    ledcDetach(hBridgeInput4);
  } else {
    ledcWrite(pwmChannel1, 0.53 * 255);
    ledcDetach(hBridgeInput2);
    ledcWrite(pwmChannel3, 0.53 * 255);
    ledcDetach(hBridgeInput4);
  }
}

void difference11() {
  if (buttonPresses > 1) {
    ledcWrite(pwmChannel1, 0.71 * 255);
    ledcDetach(hBridgeInput2);
    ledcWrite(pwmChannel3, 0.17 * 255);
    ledcDetach(hBridgeInput4);
  } else {
    ledcWrite(pwmChannel1, 0.53 * 255);
    ledcDetach(hBridgeInput2);
    ledcWrite(pwmChannel3, 0.53 * 255);
    ledcDetach(hBridgeInput4);
  }
}

void difference12() {
  if (buttonPresses > 1) {
    ledcWrite(pwmChannel1, 0.71 * 255);
    ledcDetach(hBridgeInput2);
    ledcWrite(pwmChannel3, 0.11 * 255);
    ledcDetach(hBridgeInput4);
  //  Serial.println("Turning left");
  }
  else {  // Drive straight ahead
    ledcWrite(pwmChannel1, 0.53 * 255);
    ledcDetach(hBridgeInput2);
    ledcWrite(pwmChannel3, 0.53 * 255);
    ledcDetach(hBridgeInput4);
  //  Serial.println("Straight");
  }
}

void difference13() {
  if (buttonPresses > 1) {
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


/*
void moveDesired() {
  joystickInputX = analogRead(joystickInputXPin);  // Reading from the joystick saved as input value

  // Calculate desiredMove as a value between 56 and 128, based on joystick input range
  if (joystickInputX >= 2400) {                              // Forward movement (joystick top)
    desiredMove = map(joystickInputX, 2400, 4095, 56, 128);  // Scale to range 56-128 for forward
    desiredMove = constrain(desiredMove, 56, 128);           // Ensure values stay between 56 and 128
    analogWrite(hBridgeInput1, desiredMove);                 // Sets the PWM output to current desiredMove value, making the left belt move forward
    analogWrite(hBridgeInput3, desiredMove);                 // Sets the PWM output to current desiredMove value, making the right belt move forward
  } else if (joystickInputX <= 1600) {                       // Backward movement (joystick bottom)
    desiredMove = map(joystickInputX, 0, 1600, 128, 56);     // Scale to range 128-56 for backward
    desiredMove = constrain(desiredMove, 56, 128);           // Ensure values stay between 56 and 128
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

  // Calculate desiredTurn as a value between 56 and 128, based on joystick input range
  if (joystickInputY >= 2400) {                              // Turning right movement (joystick right)
    desiredTurn = map(joystickInputY, 2400, 4095, 56, 128);  // Scale to range 56-128 for turning right
    desiredTurn = constrain(desiredTurn, 25, 128);           // Ensure values stay between 56 and 128
    analogWrite(hBridgeInput3, desiredTurn);                 // Sets the PWM output to current desiredMove value, making the right belt move forward
    analogWrite(hBridgeInput2, desiredTurn);                 // Sets the PWM output to current desiredMove value, making the left belt move backward
  } else if (joystickInputY <= 1600) {                       // Turn left movement (joystick left)
    desiredTurn = map(joystickInputY, 1600, 0, 56, 128);     // Scale to range 56-128 for turning left
    desiredTurn = constrain(desiredTurn, 56, 128);           // Ensure values stay between 56 and 128
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