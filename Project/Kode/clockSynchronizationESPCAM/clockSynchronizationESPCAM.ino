#include <esp_now.h>
#include <WiFi.h>

// Variables to receive
float ch1Output = 0, ch2Output = 0, ch3Output = 0, ch4Output = 0;

// Time synchronization variables
uint32_t masterTime = 0;
uint32_t slaveTimeOffset = 0;

// Struct for ESPNOW message
typedef struct {
    uint32_t timestamp; // Master time
    float ch1, ch2, ch3, ch4; // Outputs
} espNowMessage_t;

// Callback to handle received messages
void onReceive(const uint8_t *mac_addr, const uint8_t *data, int len) {
    if (len != sizeof(espNowMessage_t)) {
        Serial.println("Invalid message size");
        return;
    }

    espNowMessage_t incomingMessage;
    memcpy(&incomingMessage, data, len);

    // Calculate clock offset
    uint32_t now = esp_timer_get_time();
    uint32_t estimatedMasterTime = incomingMessage.timestamp + (now - incomingMessage.timestamp) / 2; // Account for transmission delay
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
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    // Initialize ESPNOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESPNOW");
        return;
    }

    esp_now_register_recv_cb(onReceive);
}

void loop() {
    // Adjust local clock if necessary (for tasks that require synchronization)
    uint32_t correctedTime = esp_timer_get_time() + slaveTimeOffset;

    // Placeholder for periodic tasks
    delay(1000); // Example periodic task
}

void findMacAddress(){
    WiFi.mode(WIFI_STA); // Set WiFi to station mode
    Serial.print("MAC Address: ");
    Serial.println(WiFi.macAddress());
}