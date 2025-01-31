#include "esp_camera.h"
#include "FS.h"                // FileSystem support for ESP32
#include "SD_MMC.h"            // SD card interface
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems
#include "driver/rtc_io.h"
#include <EEPROM.h>  // Read and write from flash memory

const char *imageFileName = "/picture67.jpg";  // Path to your grayscale image on the SD card

// Example image dimensions
const int imgWidth = 160;
const int imgHeight = 120;

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);  // Disable brownout detector

  Serial.begin(115200);
  while (!Serial)
    ;
  SD_MMC.begin("/sdcard", true);  // Necessary to disable flash

  // Initialize SD card using SD_MMC
  if (!SD_MMC.begin()) {
    Serial.println("Error: SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized.");
  // Malloc basics for 2D matrix storage
  uint8_t **grayscaleMatrix;
  grayscaleMatrix = (uint8_t **)malloc(imgHeight * sizeof(uint8_t *));  // Allocate memory for 120 rows

  if (grayscaleMatrix == NULL) {  // Check if the memory has been allocated
    Serial.println("Not enough memory!");
    return;
  } else {
    Serial.println("Memory allocated for row pointers.");
  }
  // Allocate memory for each row (160 elements per row)
  for (int i = 0; i < imgHeight; i++) {
    grayscaleMatrix[i] = (uint8_t *)malloc(imgWidth * sizeof(uint8_t));
    if (grayscaleMatrix[i] == NULL) {
      Serial.println("Not enough memory for row!");
      return;
    }
  }
  Serial.println("Memory allocated for column pointers.");
  // Read and display the grayscale image
  readGrayscaleImageFromSD(imageFileName, grayscaleMatrix);
  displayMatrix(grayscaleMatrix);
}

void loop() {
  // Do nothing
}

void readGrayscaleImageFromSD(const char *fileName, uint8_t **grayscaleMatrix) {
  // Open the file from the SD card
  File file = SD_MMC.open(fileName, FILE_READ);
  if (!file) {
    Serial.println("Error: Could not open image file.");
    return;
  }

  Serial.println("Reading grayscale image...");

  //Inspect header size - only used while debugging
  for (int i = 0; i < 100 && file.available(); i++) {
    Serial.print(file.read(), HEX);
    Serial.print(" ");
  }
  Serial.println();
  Serial.println("The above is header information.");
  Serial.println();
  Serial.println(file.size());  // Prints the size in bytes, 38400 matches an RGB565 of 160x120 pixels
  Serial.println();

  // Header size in bytes (adjust if needed)
  const int pixelDataOffset = 0;  // Offset to pixel data //1078 is used for bitmaps created with paint //0 is used for RGB565
  file.seek(pixelDataOffset);

  int errorLine = imgHeight;  // Used for debugging serial output of image data

  // Buffer for bulk reading (adjust size as needed)
  const int bufferSize = 1024;
  uint8_t buffer[bufferSize];  // Allocate a uint16_t buffer
  int bufferIndex = 0;         // Index for accessing the buffer
  size_t bytesRead = 0;

  Serial.println("Begin");
  // Read the pixel values row by row
  for (int y = 0; y < imgHeight; y++) {
    for (int x = 0; x < imgWidth; x++) {
      // Refill the buffer if necessary
      if (bufferIndex >= bytesRead) {
        bytesRead = file.read(buffer, bufferSize);  // Read a chunk of data
        bufferIndex = 0;
        if (bytesRead == 0) {
          Serial.println("Error: Unexpected end of file.");
          return;
        }
      }

      // Read two bytes for RGB565
      uint8_t byte1 = buffer[bufferIndex++];
      uint8_t byte2 = buffer[bufferIndex++];
      uint16_t pixel = (byte1 << 8) | byte2;  // RGB565 format

      // Extract RGB components
      uint8_t r = ((pixel >> 11) & 0x1F) * 255 / 31;
      uint8_t g = ((pixel >> 5) & 0x3F) * 255 / 63;
      uint8_t b = (pixel & 0x1F) * 255 / 31;

      // Convert to grayscale
      uint8_t gray = (0.299 * r) + (0.587 * g) + (0.114 * b);
      //Serial.print(gray);
      //Serial.print("\t");                                   // Tab for spacing
      grayscaleMatrix[y][x] = gray;  // Store grayscale value in the matrix
      /*} else if (file.available() && file.size() <= 20278) {  //The threshold of 20278 bytes matches the discrepancy between 256bit bitmaps and RGB565 formats
        uint8_t pixelVal = file.read();                       // Read one byte (grayscale value)
        Serial.print(pixelVal);
        Serial.print("\t");  // Tab for spacing
      } else {
        Serial.println("Error: Unexpected end of file.");
        errorLine--;
        Serial.println(errorLine);
        break;
      }*/
    }
    //Serial.println();  // New line after each row
  }
  Serial.println("End");
  //delay(500);
  //Serial.println(grayscaleMatrix[118][159]); // Remember that it is Y,X and not X,Y

  //Det her nedenunder virker
  /*int x[2][4] = { { 0, 1, 2, 3 }, { 4, 5, 6, 7 } };
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 4; j++) {
      Serial.println(x[i][j]);
    }
  }*/

  // Close the file
  file.close();
  Serial.println("Finished reading image.");
}

void displayMatrix(uint8_t **grayscaleMatrix) {
  for (int y = 0; y < imgHeight; y++) {
    for (int x = 0; x < imgWidth; x++) {
      Serial.print(grayscaleMatrix[y][x]);  // Print each value
      Serial.print("\t");                   // Tab-separated
    }
    Serial.println();  // New line after each row
  }
  Serial.println("Matrix printed.");
}