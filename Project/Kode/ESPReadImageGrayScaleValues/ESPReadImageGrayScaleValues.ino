#include "esp_camera.h"
#include "FS.h"                // FileSystem support for ESP32
#include "SD_MMC.h"            // SD card interface
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems
#include "driver/rtc_io.h"
#include <EEPROM.h>        // Read and write from flash memory
#include "TJpg_Decoder.h"  // Tiny JPEG decoder library

const char *imageFileName = "/testImage.jpg";  // Path to the JPEG image on the SD card

// Buffer to store grayscale pixel values
uint8_t pixelBuffer[160 * 120];  // For QQVGA resolution

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);  // Disable brownout detector

  Serial.begin(115200);
  while (!Serial)
    ;

  // Initialize SD card using SD_MMC
  if (!SD_MMC.begin()) {
    Serial.println("Error: SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized.");



  // Initialize the JPEG decoder
  TJpgDec.setJpgScale(1);  // Full resolution
  TJpgDec.setCallback(jpegCallback);

  // Read and display the grayscale image
  readJPEGImageFromSD(imageFileName);
}

void loop() {
  // Do nothing
}

// Callback function for TJpgDec
bool jpegCallback(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      uint16_t color = bitmap[j * w + i];
      uint8_t grayscale = color & 0xFF;  // Extract grayscale intensity
      int pixelIndex = (y + j) * 160 + (x + i);
      if (pixelIndex < 160 * 120) {
        pixelBuffer[pixelIndex] = grayscale;  // Store grayscale value
      }
    }
  }
  return true;  // Continue decoding
}

void readJPEGImageFromSD(const char *fileName) {
  File file = SD_MMC.open(fileName, FILE_READ);
  if (!file) {
    Serial.println("Error: Could not open image file.");
    return;
  }

  size_t fileSize = file.size();
  if (fileSize == 0) {
    Serial.println("Error: File is empty.");
    file.close();
    return;
  }
  Serial.printf("File size: %d bytes\n", fileSize);

  uint8_t *jpegData = (uint8_t *)malloc(fileSize);
  if (!jpegData) {
    Serial.println("Error: Memory allocation failed.");
    file.close();
    return;
  }

  file.read(jpegData, fileSize);
  file.close();

  // Debug: Print the first few bytes of the file
  for (int i = 0; i < 16 && i < fileSize; i++) {
    Serial.printf("%02X ", jpegData[i]);
  }
  Serial.println();

  if (jpegData[0] != 0xFF || jpegData[1] != 0xD8) {
    Serial.println("Error: Not a valid JPEG file.");
    free(jpegData);
    return;
  }

  if (!TJpgDec.drawJpg(0, 0, jpegData, fileSize)) {
    Serial.println("Error: JPEG decoding failed.");
  } else {
    Serial.println("JPEG decoding completed.");
    printGrayscaleValues();
  }

  free(jpegData);
}


void printGrayscaleValues() {
  Serial.println("Grayscale Image Pixel Values:");
  for (int y = 0; y < 120; y++) {    // QQVGA height
    for (int x = 0; x < 160; x++) {  // QQVGA width
      Serial.print(pixelBuffer[y * 160 + x]);
      Serial.print("\t");  // Tab for spacing
    }
    Serial.println();  // New line after each row
  }
}
