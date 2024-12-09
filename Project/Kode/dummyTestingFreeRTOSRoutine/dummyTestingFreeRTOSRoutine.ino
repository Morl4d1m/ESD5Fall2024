#include <Arduino.h>            // Necessary for accessing FreeRTOS via Arduino IDE
#include "Wire.h"               // Necessary for I2C communication
#include <freertos/FreeRTOS.h>  // Necessary for accessing FreeRTOS
#include <freertos/task.h>      // Necessary for using tasks in FreeRTOS
#include <freertos/semphr.h>    // Enables semaphores

#define INCLUDE_vTaskSuspend 1  // Definition of suspend function being active
#define builtInLED 2            // Builtin LED pin for debugging
#define somethingIsWrongPin 12  // Pin to light up red LED
#define allGoodPin 14           // Pin to light op green LED
#define emergencyButtonPin 23   // Emergency button connection pin
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

TaskHandle_t hdlMovement, hdlWirelessCommunication, hdlTakeAndProcessPicture, hdlComputeControlSignals;  // Must be made in order for the handles to work in xTaskCreate //hdlPowerMonitor, hdlSpatialAwareness, hdlWaterAvoidance, hdlMapping, hdlGoHome, hdlUpdateUser, hdlProcessPicture,

SemaphoreHandle_t avgSem, Mutex;  // Creation of semaphore handles

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
  pinMode(hBridgeInput1, OUTPUT);
  pinMode(hBridgeInput2, OUTPUT);
  pinMode(hBridgeInput3, OUTPUT);
  pinMode(hBridgeInput4, OUTPUT);

  while (!Serial) {  // Wait until serial port opens for native USB devices
    delay(10);
  }

  avgSem = xSemaphoreCreateBinary();  // Assigns avgSem to be a binary semaphore
  xSemaphoreGive(avgSem);
  Mutex = xSemaphoreCreateMutex();  // Assigns Mutex to be a mutex semaphore
  Serial.println("Calibration done");
  Serial.println("Task creation");                                                                        // Status update to figure out which function is running
  xTaskCreate(MyIdleTask, "IdleTask", 1000, NULL, 0, NULL);                                               // Idle task to check if there is any time left to execute tasks in
  xTaskCreate(movement, "movement", 1000, NULL, 1, &hdlMovement);                                         // Desired movement of the de-weeder
  xTaskCreate(wirelessCommunication, "wirelessCommunication", 1000, NULL, 2, &hdlWirelessCommunication);  // Task for wireless communication
  xTaskCreate(takeAndProcessPicture, "takePicture", 1000, NULL, 3, &hdlTakeAndProcessPicture);
  xTaskCreate(computeControlSignals, "computeControlSignals", 1000, NULL, 3, &hdlComputeControlSignals);
  // xTaskCreate(vTaskFunction, "Opgave1", 1000, &myData, 1, NULL);      // Funktionerne SKAL vaere defineret som en funktion laengere ned i koden (linje 44) inden man kan compile ellers vil syntax ikke godtage det. Altsaa skal der vaere defineret en funktion, foer man kan kalde xTaskCreate
  // vTaskFunction er funktionen tasket skal udfoere, ligesom i samletSketchPico, hvor loop funktionen kalder en funktion som lysTimer. Man kan lave flere nested funktioner inde i funktionen ligesom normalt, men saa skal man laegge stoerrelsen paa alle funktionerne sammen til ens stack size
  // "Opgave1" benyttes at identificere hvilket task der koerer hvornaar, se dem som navne. Disse bliver IKKE printet nogen steder i arduino IDE. Man kan dog bruge nogen freeRTOS debugging tools til at vise state af de forskellige funktioner, og saa kan man se navnene paa dem
  // 1000 er stack size i byte. Stack size bruger vi til at definerer hvor meget et task maa fylde. En for lille stack kan resultere i at koden vil fejle eller der er nogen dele af den paagaeldende funktion som ikke vil blive afviklet. En for stor stack vil optage unoedvendig meget plads, og resultere i en langsommere kode. ChatGPT anslaar at 10-20% stoerre stack en anslaaet funktionsstoerrelse er passende i de fleste tilfaelde
  // Den foerste NULL er data funktionen skal bruge, det kan f.eks. vaere en referencevaerdi som temp eller fugtighed.
  // 0-n er opgavernes prioritet, hvor 0 har laveste prioritet og n har hoejeste. Hvis der er flere opgaver som har samme prioritet koerer de i round robin med 1 tick som kvante laengde. Dette kan aendres med kodesnippet paa linje 51
  // Det sidste NULL er et task handle. Et task handle kan bruges til at fjerne, pause eller genoptage det paagaeldende task. Man kan eksempelvis kalde vTaskSuspend(TaskHandle) i en anden opgave, og saa paa den maade fjerne et task fra ens schedule, hvis det ikke skal koere naar et givent parameter er opfyldt, ligesom et if/else loop.

  /*
  xTaskCreate(processPicture, "processPicture", 1000, NULL, 3, &hdlProcessPicture);
  xTaskCreate(powerMonitoring, "powerMonitoring", 1000, NULL, 0, &hdlPowerMonitor);                       // Power monitoring task
  xTaskCreate(spatialAwareness, "spatialAwareness", 1000, NULL, 0, &hdlSpatialAwareness);                 //
  xTaskCreate(waterAvoidance, "waterAvoidance", 1000, NULL, 0, &hdlWaterAvoidance);
  xTaskCreate(mapping, "mapping", 1000, NULL, 0, &hdlMapping);
  xTaskCreate(goHome, "goHome", 1000, NULL, 0, &hdlGoHome);
  xTaskCreate(updateUser, "updateUser", 1000, NULL, 0, &hdlUpdateUser);*/
  attachInterrupt(emergencyButtonPin, stop, RISING);  // Emergency stop
  Serial.println("Init done");
}

void loop() {
  //Serial.println("Der skal ske noget her for at ESP'en er tilfreds");  // #skodMCU
  //getData();
  delay(1000);
}

static void movement(void *pvParameters) {
  while (1) {
    Serial.println("movement");
    joystickInputX = analogRead(joystickInputXPin);  // Reading from the joystick saved as input value
    joystickInputY = analogRead(joystickInputYPin);  // Reading from the joystick saved as input value

    // Calculate desiredMove as a value between 56 and 128, based on joystick input range
    if (joystickInputX >= 2400 && joystickInputY >= 2400) {
      desiredMove = map(joystickInputX, 2400, 4095, 56, 127);  // Scale to range 56-128 for forward
      desiredMove = constrain(desiredMove, 56, 127);           // Ensure values stay between 56 and 128
      desiredTurn = map(joystickInputY, 2400, 4095, 56, 127);  // Scale to range 56-128 for turning right
      desiredTurn = constrain(desiredTurn, 25, 127);           // Ensure values stay between 56 and 128
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
      desiredTurn = constrain(desiredTurn, 25, 127);           // Ensure values stay between 56 and 128
      analogWrite(hBridgeInput1, 0);
      analogWrite(hBridgeInput2, desiredMove + desiredTurn);
      analogWrite(hBridgeInput3, 0);
      analogWrite(hBridgeInput4, (desiredMove / 2) + (desiredTurn / 2));
    } else if (joystickInputX <= 1600 && joystickInputY <= 1600) {
      desiredMove = map(joystickInputX, 1600, 0, 56, 127);  // Scale to range 56-128 for forward
      desiredMove = constrain(desiredMove, 56, 127);        // Ensure values stay between 56 and 128
      desiredTurn = map(joystickInputY, 1600, 0, 56, 127);  // Scale to range 56-128 for turning right
      desiredTurn = constrain(desiredTurn, 25, 127);        // Ensure values stay between 56 and 128
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
      desiredTurn = map(joystickInputY, 2400, 4095, 56, 128);  // Scale to range 56-128 for turning right
      desiredTurn = constrain(desiredTurn, 25, 128);           // Ensure values stay between 56 and 128
      analogWrite(hBridgeInput1, desiredTurn);                 // Sets the PWM output to current desiredMove value, making the right belt move forward
      analogWrite(hBridgeInput4, desiredTurn);                 // Sets the PWM output to current desiredMove value, making the left belt move backward
      analogWrite(hBridgeInput2, 0);
      analogWrite(hBridgeInput3, 0);
    } else if (joystickInputY <= 1600) {                    // Turn left movement (joystick left)
      desiredTurn = map(joystickInputY, 1600, 0, 56, 128);  // Scale to range 56-128 for turning left
      desiredTurn = constrain(desiredTurn, 56, 128);        // Ensure values stay between 56 and 128
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
  vTaskDelay(100 / portTICK_PERIOD_MS);  // Delay for 50 milliseconds
  }
}


static void wirelessCommunication(void *pvParameters) {
  while (1) {
    Serial.println("wirelessCommunication");
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

static void takeAndProcessPicture(void *pvParameters) {
  while (1) {
    Serial.println("takeAndProcessPicture");
    vTaskDelay(400 / portTICK_PERIOD_MS);
  }
}

static void computeControlSignals(void *pvParameters) {
  while (1) {
    Serial.println("computeControlSignals");
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}


void stop() {                              // Emergency stop function
  Serial.println("Stop for satan!");       // Status update to figure out which function is running
  digitalWrite(emergencyLightPin, LOW);    // Lights up the emergency lights
  vTaskSuspend(hdlMovement);               // Suspends movement task
  vTaskSuspend(hdlWirelessCommunication);  // Suspends wirelessCommunication task
  /*
  vTaskSuspend(hdlPowerMonitor);           // Suspends powerMonitoring task
  vTaskSuspend(hdlSpatialAwareness);       // Suspends spatialAwareness task
  vTaskSuspend(hdlWaterAvoidance);         // Suspends waterAvoidance task
  vTaskSuspend(hdlMapping);                // Suspends mapping task
  vTaskSuspend(hdlGoHome);                 // Suspends goHome task
  vTaskSuspend(hdlUpdateUser);             // Suspends updateUser task
  vTaskSuspend(hdlTakePicture);            // Suspends takePicture task
  */
  vTaskSuspend(hdlTakeAndProcessPicture);         // Suspends processPicture task
  vTaskSuspend(hdlComputeControlSignals);  // Suspends computeControlSignals task
  analogWrite(hBridgeInput1, HIGH);        // Stop left belt forward
  analogWrite(hBridgeInput3, HIGH);        // Stop right belt forward
  analogWrite(hBridgeInput2, HIGH);        // Stop left belt backward
  analogWrite(hBridgeInput4, HIGH);        // Stop right belt backward
}

static void MyIdleTask(void *pvParameters) {
  while (1) {
    Serial.println(F("Idle state"));
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}


/*

static void processPicture(void *pvParameters) {
  while (1) {
    Serial.println("processPicture");
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}
static void powerMonitoring(void *pvParameters) {
  while (1) {
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

static void spatialAwareness(void *pvParameters) {
  while (1) {
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

static void waterAvoidance(void *pvParameters) {
  while (1) {
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

static void mapping(void *pvParameters) {
  while (1) {
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

static void goHome(void *pvParameters) {
  while (1) {
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

static void updateUser(void *pvParameters) {
  while (1) {
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

*/