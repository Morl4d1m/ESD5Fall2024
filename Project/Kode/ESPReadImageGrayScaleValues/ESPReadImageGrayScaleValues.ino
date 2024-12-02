#include "esp_camera.h"
#include "FS.h"                // FileSystem support for ESP32
#include "SD_MMC.h"            // SD card interface
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems
#include "driver/rtc_io.h"
#include <EEPROM.h>  // Read and write from flash memory

const char *imageFileName = "/picture81.jpg";  // Path to your grayscale image on the SD card

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

  //Inspect header size - only used while debugging
  for (int i = 0; i < 100 && file.available(); i++) {
    Serial.print(file.read(), HEX);
    Serial.print(" ");
  }
  Serial.println();
  Serial.println("The above is header information.");
  Serial.println();

  Serial.println(file.size());
  Serial.println();
  // Header size in bytes (adjust if needed)
  const int pixelDataOffset = 0;  // Offset to pixel data //1078 is used for bitmaps created with paint
  file.seek(pixelDataOffset);

  int errorLine = imgHeight;

  // Read the pixel values row by row
  for (int y = 0; y < imgHeight; y++) {
    for (int x = 0; x < imgWidth; x++) {
      if (file.available() && file.size() > 20278) {
        uint8_t byte1 = file.read();
        uint8_t byte2 = file.read();
        uint16_t pixel = (byte1 << 8) | byte2;  // RGB565 format

        // Extract RGB components
        uint8_t r = ((pixel >> 11) & 0x1F) * 255 / 31;
        uint8_t g = ((pixel >> 5) & 0x3F) * 255 / 63;
        uint8_t b = (pixel & 0x1F) * 255 / 31;

        // Convert to grayscale
        uint8_t gray = (0.299 * r) + (0.587 * g) + (0.114 * b);
        Serial.print(gray);
        Serial.print("\t");                                   // Tab for spacing
      } else if (file.available() && file.size() <= 20278) {  //The threshold of 20278 bytes matches the discrepancy between 256bit bitmaps and RGB565 formats
        uint8_t pixelVal = file.read();                       // Read one byte (grayscale value)
        Serial.print(pixelVal);
        Serial.print("\t");  // Tab for spacing
      } else {
        Serial.println("Error: Unexpected end of file.");
        errorLine--;
        Serial.println(errorLine);
        break;
      }
    }
    Serial.println();  // New line after each row
  }

  // Close the file
  file.close();
  Serial.println("Finished reading image.");
}
