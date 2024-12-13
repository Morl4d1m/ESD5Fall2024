#include <Arduino.h>            // Necessary for accessing FreeRTOS via Arduino IDE
#include "Wire.h"               // Necessary for I2C communication
#include <freertos/FreeRTOS.h>  // Necessary for accessing FreeRTOS
#include <freertos/task.h>      // Necessary for using tasks in FreeRTOS
#include <freertos/semphr.h>    // Enables semaphores
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_mac.h>           // For the MAC2STR and MACSTR macros
#include "Adafruit_VL53L0X.h"  // Library for the range sensor

#define INCLUDE_vTaskSuspend 1  // Definition of suspend function being active
#define builtInLED 2            // Builtin LED pin for debugging
#define somethingIsWrongPin 12  // Pin to light up red LED
#define allGoodPin 14           // Pin to light op green LED
#define emergencyButtonPin 34   // Emergency button connection pin
#define emergencyLightPin 2     // LED that signifies an emergency has happened
#define joystickInputXPin 36
#define joystickInputYPin 39
#define joystickSwitchPin 34
#define hBridgeInput1 32
#define hBridgeInput2 33
#define hBridgeInput3 25
#define hBridgeInput4 26
#define configCHECK_FOR_STACK_OVERFLOW 1  // Checks whether or not stack overflow occurs


// Variables to receive
uint8_t ch1Output = 0;
uint8_t ch2Output = 0;
uint8_t ch3Output = 0;
uint8_t ch4Output = 0;
uint32_t packagenumber = 0;
uint32_t lastPackageNumber = 0;

// Time synchronization variables
uint32_t masterTime = 0;
uint32_t slaveTimeOffset = 0;

// Master MAC Address
uint8_t masterMAC[] = { 0x08, 0xa6, 0xf7, 0x10, 0x64, 0xa4 };

// Struct for ESPNOW message
typedef struct {
  uint32_t timestamp;          // Master time
  uint32_t packageNumber;      // Sequence number
  uint8_t ch1, ch2, ch3, ch4;  // Outputs
} esp32MasterMessage_t;

esp32MasterMessage_t incomingMessage;  //Storage for struct

TaskHandle_t hdlDistanceRead;  // Must be made in order for the handles to work in xTaskCreate

SemaphoreHandle_t avgSem, Mutex;  // Creation of semaphore handles

Adafruit_VL53L0X lox = Adafruit_VL53L0X();  // Instantiates the range sensor
VL53L0X_RangingMeasurementData_t measure;   // Struct to keep measurements from range sensor

uint8_t minimumDistance = 50;               // Desired distance in mm
uint16_t currentDistance = 100;               // Variable to store current distance measured by the vl52l0x

void setup() {
  Serial.begin(115200);
  Wire.begin();           // Join I2C bus for VL53L0X communication
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

  // VL53L0X setup
  Serial.println("Adafruit VL53L0X test");        // Tests if a connection to the sensor has been established
  if (!lox.begin()) {                             // Checks all boolean states for buildin begin function from sensor library
    Serial.println(F("Failed to boot VL53L0X"));  // If no connection is present, this flag will be set high
    digitalWrite(somethingIsWrongPin, HIGH);
    /*while (1) {
      delay(10);
    }*/
  }
  lox.configSensor(Adafruit_VL53L0X::VL53L0X_SENSE_LONG_RANGE);  // Changes sensor config from default (1.2metres) to long range (2.2 metres). Other options could be: VL53L0X_SENSE_DEFAULT, VL53L0X_SENSE_LONG_RANGE, VL53L0X_SENSE_HIGH_SPEED, VL53L0X_SENSE_HIGH_ACCURACY

  //FreeRTOS setup
  avgSem = xSemaphoreCreateBinary();  // Assigns avgSem to be a binary semaphore
  xSemaphoreGive(avgSem);
  Mutex = xSemaphoreCreateMutex();  // Assigns Mutex to be a mutex semaphore
  Serial.println("Calibration done");
  Serial.println("Task creation");

  /* 
  No tasks except for the distance measuring are made, as the callback function retrieving data from the ESPCAM functions by interrupt, 
  and since the entire purpose of this program is to analogWrite PWM outputs based on readings, they might as well be updated as fast as possible.

  Furthermore, this program handles the distance measurements and emergency stop as well, which benefits from being tasks, but in actuality,
  they could just as well have been simple functions running in loop.
  */
  xTaskCreate(MyIdleTask, "IdleTask", 1000, NULL, 0, NULL);                              // Idle task to check if there is any time left to execute tasks in
  xTaskCreate(distanceRead, "Reads current distance", 8000, NULL, 1, &hdlDistanceRead);  // Read current distance from range sensor
  attachInterrupt(emergencyButtonPin, stop, RISING);                                     // Emergency stop

  WiFi.mode(WIFI_STA);
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, get receive packet info
  esp_now_register_recv_cb(dataReceive);

  // Ensure the master is a registered peer only once
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, masterMAC, 6);  // Master MAC address
  peerInfo.channel = 0;                      // Match the channel with the master
  peerInfo.encrypt = false;                  // No encryption

  // Add the master as a peer if not already done
  if (!esp_now_is_peer_exist(masterMAC)) {
    esp_err_t result = esp_now_add_peer(&peerInfo);
    if (result != ESP_OK) {
      Serial.println("Failed to add master as a peer");
      return;
    }
    Serial.println("Master peer added successfully");
  }
  Serial.println("Init done");
}

void loop() {
  //Serial.print("Local ESPCAM time: ");
  //Serial.println(micros());
  // Adjust local clock if necessary (for tasks that require synchronization)
  uint32_t correctedTime = esp_timer_get_time() + slaveTimeOffset;

  analogWrite(hBridgeInput1, ch1Output * 255 / 100);
  analogWrite(hBridgeInput2, ch2Output * 255 / 100);
  analogWrite(hBridgeInput3, ch3Output * 255 / 100);
  analogWrite(hBridgeInput4, ch4Output * 255 / 100);
  //Serial.print("Corrected time: ");
  //Serial.println(correctedTime);
  delayMicroseconds(345);

  // Placeholder for periodic tasks
  //delay(1000);  // Example periodic task
}
// Callback to handle received messages
void dataReceive(const esp_now_recv_info_t *recvInfo, const uint8_t *incomingData, int len) {
  if (len != sizeof(esp32MasterMessage_t)) {
    Serial.println("Invalid message size");
    return;
  }

  memcpy(&incomingMessage, incomingData, sizeof(incomingMessage));

  if (incomingMessage.packageNumber > lastPackageNumber + 1 && lastPackageNumber != 0) {
    Serial.printf("Warning: Missing packet(s). Last received: %d, Current: %d\n",
                  lastPackageNumber, incomingMessage.packageNumber);
  }

  lastPackageNumber = incomingMessage.packageNumber;

  // Ensure the master is a registered peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, masterMAC, 6);
  peerInfo.channel = 0;  // Match channel with master
  peerInfo.encrypt = false;

  if (!esp_now_is_peer_exist(masterMAC)) {
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
      Serial.println("Failed to add master as a peer");
      return;
    }
  }

  // Send acknowledgment
  esp_err_t result = esp_now_send(masterMAC, (uint8_t *)&incomingMessage.packageNumber, sizeof(incomingMessage.packageNumber));
  if (result == ESP_OK) {
    Serial.println("ACK sent successfully");
  } else {
    Serial.printf("ACK send failed, error: %d\n", result);
  }

  // Calculate clock offset
  uint32_t now = esp_timer_get_time();
  uint32_t estimatedMasterTime = incomingMessage.timestamp;  // Account for transmission delay
  slaveTimeOffset = estimatedMasterTime - now;

  // Update variables
  ch1Output = incomingMessage.ch1;
  ch2Output = incomingMessage.ch2;
  ch3Output = incomingMessage.ch3;
  ch4Output = incomingMessage.ch4;

  Serial.printf("Sync: Offset=%d µs, ch1=%d, ch2=%d, ch3=%d, ch4=%d, Iteration number=%d\n", slaveTimeOffset, ch1Output, ch2Output, ch3Output, ch4Output, lastPackageNumber);
}


void readMacAddress() {
  uint8_t baseMac[6];
  esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
  if (ret == ESP_OK) {
    Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
                  baseMac[0], baseMac[1], baseMac[2],
                  baseMac[3], baseMac[4], baseMac[5]);
  } else {
    Serial.println("Failed to read MAC address");
  }
}

void stop() {                         // Emergency stop function
  Serial.println("Stop for satan!");  // Status update to figure out which function is running
  esp_now_deinit();
  digitalWrite(emergencyLightPin, LOW);  // Lights up the emergency lights
  analogWrite(hBridgeInput1, 255);       // Stop left belt forward
  analogWrite(hBridgeInput3, 255);       // Stop right belt forward
  analogWrite(hBridgeInput2, 255);       // Stop left belt backward
  analogWrite(hBridgeInput4, 255);       // Stop right belt backward
  vTaskSuspend(hdlDistanceRead);
}

static void distanceRead(void *pvParameters) {
  while (1) {
    lox.rangingTest(&measure, false);  // Pass in 'true' to get debug data printout!

    if (measure.RangeStatus != 4) {  // Phase failures have incorrect data
                                     // Serial.print("Distance (mm): ");
      //Serial.println(measure.RangeMilliMeter);
      currentDistance = measure.RangeMilliMeter;  // Updates the currentDistance variable with the newest measured value
    } else {
      //Serial.println(" out of range ");
    }
    currentDistance = measure.RangeMilliMeter;  // Updates the currentDistance variable with the newest measured value

    if (currentDistance < minimumDistance) {
      stop();
    }
    Serial.print("Distance: ");
    Serial.println(currentDistance);
    vTaskDelay(250 / portTICK_PERIOD_MS);  // Delay for 10 milliseconds
  }
}

static void MyIdleTask(void *pvParameters) {
  while (1) {
    Serial.println(F("Idle state"));
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}