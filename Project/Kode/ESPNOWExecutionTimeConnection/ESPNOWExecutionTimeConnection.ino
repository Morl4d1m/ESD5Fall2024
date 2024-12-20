// Code is identical to that of clock synchro options, except that the code for both master and slave is present here
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_mac.h>  // For the MAC2STR and MACSTR macros

// Master ESP32 MAC: a0:b7:65:4c:0e:f0
// Slave ESPCAM MAC: 08:a6:f7:10:64:a4
bool ackReceived = false;  // Flag for acknowledgment

// Variables to send
uint8_t ch1Output = 53;
uint8_t ch2Output = 0;
uint8_t ch3Output = 53;
uint8_t ch4Output = 0;
uint32_t packageNumber = 0;

// Timing variables for calculating executiontimes
uint32_t timeSpent = 0;
uint32_t startTime = 0;

// Slave MAC Address
uint8_t slaveMAC[] = { 0x08, 0xa6, 0xf7, 0x10, 0x64, 0xa4 };

// Struct for ESPNOW message
typedef struct {
  uint32_t timestamp;          // Master time
  uint32_t packageNumber;      // Sequence number
  uint8_t ch1, ch2, ch3, ch4;  // Outputs
} esp32MasterMessage_t;

esp32MasterMessage_t outgoingMessage;  // Storage for struct

esp_now_peer_info_t peerInfo;  // Store information about peer

// callback when data is sent
void dataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  //Serial.print("\r\nLast Packet Send Status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");  // Remember that the ? is an inline if statement, returning the first value if true and the second if false
}

void receiveAck(const esp_now_recv_info *info, const uint8_t *data, int len) {
  if (len != sizeof(outgoingMessage.packageNumber)) {
    Serial.println("Invalid ACK size");
    return;
  }

  // Check and log sender's MAC address
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           info->src_addr[0], info->src_addr[1], info->src_addr[2],
           info->src_addr[3], info->src_addr[4], info->src_addr[5]);
  Serial.printf("ACK received from: %s\n", macStr);
  Serial.println(outgoingMessage.packageNumber);
  Serial.println(packageNumber);
  // Verify the package number in the ACK
  //memcpy(&outgoingMessage, data, sizeof(outgoingMessage));
  if (outgoingMessage.packageNumber == packageNumber-1) {
    ackReceived = true;
    Serial.printf("ACK received for package %d\n", outgoingMessage.packageNumber);
  }
}

void setup() {
  // Init Serial Monitor
  startTime = micros();
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of trans  mitted packet
  esp_now_register_send_cb(dataSent);  // Personalization instead of the std ESPNOW callback

  // Register callback for receiving data (ACK in this case)
  esp_now_register_recv_cb(receiveAck);  // Called when data is received

  // Register peer
  memcpy(peerInfo.peer_addr, slaveMAC, 6);
  peerInfo.channel = 0;      // Assign a channel to transmit data across
  peerInfo.encrypt = false;  // Set true if encryption is wanted

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  uint32_t setupTime = micros();
  timeSpent = setupTime - startTime;
  //Serial.println("SetupTime");
  //Serial.println(timeSpent);
}

void loop() {
  startTime = micros();
  // Prepare message
  outgoingMessage.timestamp = esp_timer_get_time();  // Microsecond precision
  outgoingMessage.packageNumber = packageNumber++;
  outgoingMessage.ch1 = ch1Output;
  outgoingMessage.ch2 = ch2Output;
  outgoingMessage.ch3 = ch3Output;
  outgoingMessage.ch4 = ch4Output;

  ackReceived = false;
  for (int retry = 0; retry < 5; retry++) {
    esp_err_t result = esp_now_send(slaveMAC, (uint8_t *)&outgoingMessage, sizeof(outgoingMessage));
    if (result != ESP_OK) {
      Serial.println("Error sending the data");
    } else {
      Serial.println("Sent with success");
    }

    delay(100);  // Wait for acknowledgment
    if (ackReceived) break;
  }

  if (!ackReceived) {
    Serial.printf("Failed to receive ACK for package %d\n", outgoingMessage.packageNumber);
    //In the final program, the system should be halted by now, as data has been lost
  }

  // Update outputs for testing 
  ch1Output += 1;
  ch2Output += 1;
  ch3Output += 1;
  ch4Output += 1;
  //Serial.printf("ch1=%d, ch2=%d, ch3=%d, ch4=%d, PackageNumber=%d\n", ch1Output, ch2Output, ch3Output, ch4Output, packageNumber);
  uint32_t loopTime = micros();
  timeSpent = loopTime - startTime;
  //Serial.println("LoopTime");
  //Serial.println(timeSpent);
  //Serial.print("Local ESP32 time: ");
  //Serial.println(micros());
  delay(1000);  // Periodic synchronization
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Below is for the slave device:
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_mac.h>  // For the MAC2STR and MACSTR macros

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
uint8_t masterMAC[] = { 0xa0, 0xb7, 0x65, 0x4c, 0x0e, 0xf0 };

// Struct for ESPNOW message
typedef struct {
  uint32_t timestamp;          // Master time
  uint32_t packageNumber;      // Sequence number
  uint8_t ch1, ch2, ch3, ch4;  // Outputs
} esp32MasterMessage_t;

esp32MasterMessage_t incomingMessage;  //Storage for struct

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

  Serial.printf("Sync: Offset=%d µs, ch1=%d, ch2=%d, ch3=%d, ch4=%d, PackageNumber=%d\n", slaveTimeOffset, ch1Output, ch2Output, ch3Output, ch4Output, lastPackageNumber);
}

void setup() {
  Serial.begin(115200);

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
  peerInfo.channel = 0;  // Match the channel with the master
  peerInfo.encrypt = false;  // No encryption

  // Add the master as a peer if not already done
  if (!esp_now_is_peer_exist(masterMAC)) {
    esp_err_t result = esp_now_add_peer(&peerInfo);
    if (result != ESP_OK) {
      Serial.println("Failed to add master as a peer");
      return;
    }
    Serial.println("Master peer added successfully");
  }
}

void loop() {
  Serial.print("Local ESPCAM time: ");
  Serial.println(micros());
  // Adjust local clock if necessary (for tasks that require synchronization)
  uint32_t correctedTime = esp_timer_get_time() + slaveTimeOffset;
  Serial.print("Corrected time: ");
  Serial.println(correctedTime);

  // Placeholder for periodic tasks
  delay(1000);  // Example periodic task
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