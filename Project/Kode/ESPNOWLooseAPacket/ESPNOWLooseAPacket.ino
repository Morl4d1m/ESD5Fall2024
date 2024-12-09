// Code is identical to clock synchro, except for the change in packagenumber after 30 seconds.

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
  //Serial.print("Local ESPCAM time: ");
  //Serial.println(micros());
  // Adjust local clock if necessary (for tasks that require synchronization)
  uint32_t correctedTime = esp_timer_get_time() + slaveTimeOffset;
  //Serial.print("Corrected time: ");
  //Serial.println(correctedTime);

  // Placeholder for periodic tasks
  delay(1000);  // Example periodic task
  if (startTime > 30000000) {                                                                                     // Send faulty message to slave after 30 seconds
    packageNumber = 5000;
    esp_err_t result = esp_now_send(slaveMAC, (uint8_t *)&outgoingFaultyMessage, sizeof(outgoingFaultyMessage));  // The esp_err_t result is made to constantly check the integrity, and ensure data is sent

    if (result == ESP_OK) {
      Serial.println("Sent with success");
    } else {
      Serial.println("Error sending the data");
    }
  }
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