#include "esp_camera.h"
#include "FS.h"                // SD Card ESP32
#include "SD_MMC.h"            // SD Card ESP32
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems
#include "driver/rtc_io.h"
#include <EEPROM.h>  // read and write from flash memory

#define EEPROM_SIZE 1

// Pin definition for CAMERA_MODEL_AI_THINKER
// Change pin definition if you're using another ESP32 with camera module
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

// Keep track of number of pictures
uint8_t pictureNumber = 0;

//Stores the camera configuration parameters
camera_config_t config;

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);  //disable brownout detector

  Serial.begin(115200);

  //Initialize the camera
  Serial.print("Initializing the camera module...");
  configInitCamera();
  Serial.println("Ok!");

  //Initialize MicroSD
  Serial.print("Initializing the MicroSD card module... ");
  initMicroSDCard();
}

void loop() {
  //Take and Save Photo
  takeSavePhoto();
  //pictureNumber++;
  delay(5000);  //100 ms is tested and works in current iteration
}

void configInitCamera() {
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_RGB565;  //YUV422,GRAYSCALE,RGB565,JPEG // MUST BE RGB565 TO NOT COMPRESS IMAGE
  config.fb_count = 2;       //Must be 2 to always get the newest image
  config.grab_mode = CAMERA_GRAB_LATEST; // Must be set to grab latest image and not a previous one - solved using https://github.com/espressif/arduino-esp32/issues/6047

  // Select lower framesize if the camera doesn't support PSRAM
  if (psramFound()) {
    config.frame_size = FRAMESIZE_QQVGA;  // FRAMESIZE_ + QQVGA|QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 10;             //10-63 lower number means higher quality
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // Initialize the Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  //s->set_pixformat(s, PIXFORMAT_GRAYSCALE); // Grayscale doesnt seem to work by initializing this either
  s->set_brightness(s, 0);                  // -2 to 2
  s->set_contrast(s, 0);                    // -2 to 2
  s->set_saturation(s, 0);                  // -2 to 2
  s->set_special_effect(s, 0);              // 0 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
  s->set_whitebal(s, 1);                    // 0 = disable , 1 = enable
  s->set_awb_gain(s, 1);                    // 0 = disable , 1 = enable
  s->set_wb_mode(s, 0);                     // 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
  s->set_exposure_ctrl(s, 1);               // 0 = disable , 1 = enable
  s->set_aec2(s, 1);                        // 0 = disable , 1 = enable
  s->set_ae_level(s, 0);                    // -2 to 2
  s->set_aec_value(s, 0);                   // 0 to 1200 Automatic exposure control
  s->set_gain_ctrl(s, 1);                   // 0 = disable , 1 = enable
  s->set_agc_gain(s, 0);                    // 0 to 30
  s->set_gainceiling(s, (gainceiling_t)0);  // 0 to 6
  s->set_bpc(s, 0);                         // 0 = disable , 1 = enable Black pixel correction
  s->set_wpc(s, 1);                         // 0 = disable , 1 = enable White pixel correction
  s->set_raw_gma(s, 1);                     // 0 = disable , 1 = enable
  s->set_lenc(s, 1);                        // 0 = disable , 1 = enable
  s->set_hmirror(s, 0);                     // 0 = disable , 1 = enable
  s->set_vflip(s, 0);                       // 0 = disable , 1 = enable
  s->set_dcw(s, 1);                         // 0 = disable , 1 = enable
  s->set_colorbar(s, 0);                    // 0 = disable , 1 = enable
}

void initMicroSDCard() {
  // Start Micro SD card
  Serial.println("Starting SD Card");
  SD_MMC.begin("/sdcard", true); // Necessary to disable flash
  if (!SD_MMC.begin()) {
    Serial.println("SD Card Mount Failed");
    return;
  }
  uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD Card attached");
    return;
  }
}

void takeSavePhoto() {
  // Take Picture with Camera
  camera_fb_t *fb = esp_camera_fb_get();

  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }
  // initialize EEPROM with predefined size
  EEPROM.begin(EEPROM_SIZE);
  pictureNumber = EEPROM.read(0) + 1;

  // Path where new picture will be saved in SD Card
  String path = "/picture" + String(pictureNumber) + ".jpg";

  fs::FS &fs = SD_MMC;
  Serial.printf("Picture file name: %s\n", path.c_str());

  File file = fs.open(path.c_str(), FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file in writing mode");
  } else {
    file.write(fb->buf, fb->len);  // payload (image), payload length
    Serial.printf("Saved file to path: %s\n", path.c_str());
    EEPROM.write(0, pictureNumber);
    EEPROM.commit();
  }
  file.close();

  //return the frame buffer back to the driver for reuse
  esp_camera_fb_return(fb);
}