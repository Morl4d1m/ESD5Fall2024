#include "esp_camera.h"
#include "FS.h"                // FileSystem support for ESP32
#include "SD_MMC.h"            // SD card interface
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems
#include "driver/rtc_io.h"
#include <EEPROM.h>            // Read and write from flash memory

const char *imageFileName = "/testImage2.bmp"; // Path to your grayscale image on the SD card

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // Disable brownout detector

  Serial.begin(115200);
  while (!Serial);

  // Initialize SD card using SD_MMC
  if (!SD_MMC.begin()) {
    Serial.println("Error: SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized.");

  // Read and display the grayscale image
  readGrayscaleImageFromSD(imageFileName);
}

void loop() {
  // Do nothing
}

void readGrayscaleImageFromSD(const char *fileName) {
  // Open the file from the SD card
  File file = SD_MMC.open(fileName, FILE_READ);
  if (!file) {
    Serial.println("Error: Could not open image file.");
    return;
  }

  Serial.println("Reading grayscale image...");

  // Example image dimensions (adjust as necessary for your image)
  const int imgWidth = 160;
  const int imgHeight = 120;

  // Read the pixel values row by row
  for (int y = 0; y < imgHeight; y++) {
    for (int x = 0; x < imgWidth; x++) {
      if (file.available()) {
        uint8_t pixelVal = file.read(); // Read one byte (grayscale value)
        Serial.print(pixelVal);
        Serial.print("\t"); // Tab for spacing
      } else {
        Serial.println("Error: Unexpected end of file.");
        break;
      }
    }
    Serial.println(); // New line after each row
  }

  // Close the file
  file.close();
  Serial.println("Finished reading image.");
}
