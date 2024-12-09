#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_mac.h>  // For the MAC2STR and MACSTR macros

// Master ESP32 MAC: a0:b7:65:4c:0e:f0
// Slave ESPCAM MAC: 08:a6:f7:10:64:a4

// Variables to send
uint8_t ch1Output = 53;
uint8_t ch2Output = 0;
uint8_t ch3Output = 53;
uint8_t ch4Output = 0;

// Slave MAC Address
uint8_t slaveMAC[] = { 0x08, 0xa6, 0xf7, 0x10, 0x64, 0xa4 };

// Struct for ESPNOW message
typedef struct {
  uint32_t timestamp;          // Master time
  uint8_t ch1, ch2, ch3, ch4;  // Outputs
} esp32MasterMessage_t;

esp32MasterMessage_t outgoingMessage;  // Storage for struct

esp_now_peer_info_t peerInfo;  // Store information about peer

// callback when data is sent
void dataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");  // Remember that the ? is an inline if statement, returning the first value if true and the second if false
}

void setup() {
  // Init Serial Monitor
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

  // Register peer
  memcpy(peerInfo.peer_addr, slaveMAC, 6);
  peerInfo.channel = 0;      // Assign a channel to transmit data across
  peerInfo.encrypt = false;  // Set true if encryption is wanted

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  // Prepare message
  outgoingMessage.timestamp = esp_timer_get_time();  // Microsecond precision
  outgoingMessage.ch1 = ch1Output;
  outgoingMessage.ch2 = ch2Output;
  outgoingMessage.ch3 = ch3Output;
  outgoingMessage.ch4 = ch4Output;

  // Send message to slave
  esp_err_t result = esp_now_send(slaveMAC, (uint8_t *)&outgoingMessage, sizeof(outgoingMessage)); // The esp_err_t result is made to constantly check the integrity, and ensure data is sent

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }

  // Update outputs for testing (replace with actual logic)
  ch1Output += 1;
  ch2Output += 1;
  ch3Output += 1;
  ch4Output += 1;
  Serial.printf("ch1=%d, ch2=%d, ch3=%d, ch4=%d\n", ch1Output, ch2Output, ch3Output, ch4Output);

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