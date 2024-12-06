#include <esp_now.h>
#include <WiFi.h>

// Variables to send
float ch1Output = 1.23, ch2Output = 4.56, ch3Output = 7.89, ch4Output = 0.12;

// Slave MAC Address (update with actual MAC)
uint8_t slaveMac[] = {0x24, 0x6F, 0x28, 0xAA, 0xBB, 0xCC};

// Struct for ESPNOW message
typedef struct {
    uint32_t timestamp; // Master time
    float ch1, ch2, ch3, ch4; // Outputs
} espNowMessage_t;

espNowMessage_t outgoingMessage;

// Callback to confirm message sent
void onSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Message sent successfully" : "Message send failed");
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    // Initialize ESPNOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESPNOW");
        return;
    }

    // Register peer
    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(peerInfo));
    memcpy(peerInfo.peer_addr, slaveMac, 6);
    peerInfo.channel = 0; // Default channel
    peerInfo.encrypt = false;
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }

    esp_now_register_send_cb(onSent);
}

void loop() {
    // Prepare message
    outgoingMessage.timestamp = esp_timer_get_time(); // Microsecond precision
    outgoingMessage.ch1 = ch1Output;
    outgoingMessage.ch2 = ch2Output;
    outgoingMessage.ch3 = ch3Output;
    outgoingMessage.ch4 = ch4Output;

    // Send message to slave
    esp_now_send(slaveMac, (uint8_t *)&outgoingMessage, sizeof(outgoingMessage));

    // Update outputs for testing (replace with actual logic)
    ch1Output += 0.1;
    ch2Output += 0.2;
    ch3Output += 0.3;
    ch4Output += 0.4;

    delay(1000); // Periodic synchronization (1 second)
}

void findMACAddress(){
    WiFi.mode(WIFI_STA); // Set WiFi to station mode
    Serial.print("MAC Address: ");
    Serial.println(WiFi.macAddress());
}