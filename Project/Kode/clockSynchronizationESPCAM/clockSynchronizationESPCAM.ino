#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_mac.h>  // For the MAC2STR and MACSTR macros

// Variables to receive
float ch1Output = 0, ch2Output = 0, ch3Output = 0, ch4Output = 0;

// Time synchronization variables
uint32_t masterTime = 0;
uint32_t slaveTimeOffset = 0;

// Struct for ESPNOW message
typedef struct {
  uint32_t timestamp;          // Master time
  uint8_t ch1, ch2, ch3, ch4;  // Outputs
} esp32MasterMessage_t;

// Callback to handle received messages
void dataReceive(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  if (len != sizeof(esp32MasterMessage_t)) {  //Checksum to see if the packet has expected size
    Serial.println("Invalid message size");
    return;
  }

  esp32MasterMessage_t incomingMessage;
  memcpy(&incomingMessage, incomingData, sizeof(len));

  // Calculate clock offset
  uint32_t now = esp_timer_get_time();
  uint32_t estimatedMasterTime = incomingMessage.timestamp + (now - incomingMessage.timestamp) / 2;  // Account for transmission delay
  slaveTimeOffset = estimatedMasterTime - now;

  // Update variables
  ch1Output = incomingMessage.ch1;
  ch2Output = incomingMessage.ch2;
  ch3Output = incomingMessage.ch3;
  ch4Output = incomingMessage.ch4;

  Serial.printf("Sync: Offset=%d µs, ch1=%.2f, ch2=%.2f, ch3=%.2f, ch4=%.2f\n", slaveTimeOffset, ch1Output, ch2Output, ch3Output, ch4Output);
}

void setup() {
  Serial.begin(115200);

  Serial.print("ESP32 Board MAC Address: ");
  WiFi.mode(WIFI_STA);
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(dataReceive));
}

void loop() {
  // Adjust local clock if necessary (for tasks that require synchronization)
  uint32_t correctedTime = esp_timer_get_time() + slaveTimeOffset;

  // Placeholder for periodic tasks
  delay(500);  // Example periodic task
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