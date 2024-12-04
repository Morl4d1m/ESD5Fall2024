#include "esp_camera.h"
#include "FS.h"                // FileSystem support for ESP32
#include "SD_MMC.h"            // SD card interface
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems
#include "driver/rtc_io.h"
#include <EEPROM.h>  // Read and write from flash memory

const char *imageFileName = "/picture81TeaMug.jpg";  // Path to your grayscale image on the SD card

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

   //Repeated for gaussBlurMatrix
  uint8_t **gaussBlurMatrix;
  gaussBlurMatrix = (uint8_t **)malloc(imgHeight * sizeof(uint8_t *));  // Allocate memory for 120 rows

  if (gaussBlurMatrix == NULL) {  // Check if the memory has been allocated
    Serial.println("Not enough memory!");
    return;
  } else {
    Serial.println("Memory allocated for row pointers.");
  }

  // Allocate memory for each row (160 elements per row)
  for (int i = 0; i < imgHeight; i++) {
    gaussBlurMatrix[i] = (uint8_t *)malloc(imgWidth * sizeof(uint8_t));
    if (gaussBlurMatrix[i] == NULL) {
      Serial.println("Not enough memory for row!");
      return;
    }
  }
  Serial.println("Memory allocated for column pointers.");

  //Repeated for vSobelMatrix
  uint8_t **vSobelMatrix;
  vSobelMatrix = (uint8_t **)malloc(imgHeight * sizeof(uint8_t *));  // Allocate memory for 120 rows

  if (vSobelMatrix == NULL) {  // Check if the memory has been allocated
    Serial.println("Not enough memory!");
    return;
  } else {
    Serial.println("Memory allocated for row pointers.");
  }

  // Allocate memory for each row (160 elements per row)
  for (int i = 0; i < imgHeight; i++) {
    vSobelMatrix[i] = (uint8_t *)malloc(imgWidth * sizeof(uint8_t));
    if (vSobelMatrix[i] == NULL) {
      Serial.println("Not enough memory for row!");
      return;
    }
  }
  Serial.println("Memory allocated for column pointers.");

  //Repeated for hSobelMatrix
  uint8_t **hSobelMatrix;
  hSobelMatrix = (uint8_t **)malloc(imgHeight * sizeof(uint8_t *));  // Allocate memory for 120 rows

  if (hSobelMatrix == NULL) {  // Check if the memory has been allocated
    Serial.println("Not enough memory!");
    return;
  } else {
    Serial.println("Memory allocated for row pointers.");
  }

  // Allocate memory for each row (160 elements per row)
  for (int i = 0; i < imgHeight; i++) {
    hSobelMatrix[i] = (uint8_t *)malloc(imgWidth * sizeof(uint8_t));
    if (hSobelMatrix[i] == NULL) {
      Serial.println("Not enough memory for row!");
      return;
    }
  }
  Serial.println("Memory allocated for column pointers.");

  //Repeated for processedMatrix
  uint8_t **processedMatrix;
  processedMatrix = (uint8_t **)malloc(imgHeight * sizeof(uint8_t *));  // Allocate memory for 120 rows

  if (processedMatrix == NULL) {  // Check if the memory has been allocated
    Serial.println("Not enough memory!");
    return;
  } else {
    Serial.println("Memory allocated for row pointers.");
  }

  // Allocate memory for each row (160 elements per row)
  for (int i = 0; i < imgHeight; i++) {
    processedMatrix[i] = (uint8_t *)malloc(imgWidth * sizeof(uint8_t));
    if (processedMatrix[i] == NULL) {
      Serial.println("Not enough memory for row!");
      return;
    }
  }
  Serial.println("Memory allocated for column pointers.");

  // Read and display the grayscale image
  readGrayscaleImageFromSD(imageFileName, grayscaleMatrix);
  //displayMatrix(grayscaleMatrix);
  gaussBlurOperator(grayscaleMatrix, gaussBlurMatrix);
  verticalSobelOperator(gaussBlurMatrix, vSobelMatrix);
  horizontalSobelOperator(gaussBlurMatrix, hSobelMatrix);
  imageProcessingDone(grayscaleMatrix, vSobelMatrix, hSobelMatrix, processedMatrix);
  displayMatrix(processedMatrix);
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
  // Create a buffer to hold the entire image data as a string
  String imageString = "";

  for (int y = 0; y < imgHeight; y++) {
    for (int x = 0; x < imgWidth; x++) {
      imageString += String(grayscaleMatrix[y][x]) + "\t";  // Append each value with a tab
    }
    imageString += "\n";  // New line after each row
  }

  // Print the entire matrix at once
  Serial.print(imageString);
  Serial.println("Matrix printed.");
}

void gaussBlurOperator(uint8_t **grayscaleMatrix, uint8_t **gaussBlurMatrix) {
  int gaussBlur[3][3] = { { 1, 2, 1 }, { 2, 4, 2 }, { 1, 2, 1 } };

  // Temporary matrix to store the result
  uint8_t **tempMatrix = (uint8_t **)malloc(imgHeight * sizeof(uint8_t *));
  for (int i = 0; i < imgHeight; i++) {
    tempMatrix[i] = (uint8_t *)malloc(imgWidth * sizeof(uint8_t));
  }

  for (int y = 1; y < imgHeight - 1; y++) {
    for (int x = 1; x < imgWidth - 1; x++) {
      int gaussBlurred = 0;

      // Apply Sobel kernel
      for (int ky = -1; ky <= 1; ky++) {
        for (int kx = -1; kx <= 1; kx++) {
          gaussBlurred += grayscaleMatrix[y + ky][x + kx] * gaussBlur[ky + 1][kx + 1];
        }
      }

      // Normalize and clamp the value to [0, 255]
      gaussBlurred/=16;
      gaussBlurred = max(0, min(255, gaussBlurred));

      // Store in the temporary matrix
      tempMatrix[y][x] = (uint8_t)gaussBlurred;
    }
  }

  // Copy the results to the original matrix
  for (int y = 0; y < imgHeight; y++) {
    for (int x = 0; x < imgWidth; x++) {
      gaussBlurMatrix[y][x] = tempMatrix[y][x];
    }
    free(tempMatrix[y]);
  }
  free(tempMatrix);
}

void verticalSobelOperator(uint8_t **gaussBlurMatrix, uint8_t **vSobelMatrix) {
  int vSobel[3][3] = { { -1, 0, 1 }, { -2, 0, 2 }, { -1, 0, 1 } };

  // Temporary matrix to store the result
  uint8_t **tempMatrix = (uint8_t **)malloc(imgHeight * sizeof(uint8_t *));
  for (int i = 0; i < imgHeight; i++) {
    tempMatrix[i] = (uint8_t *)malloc(imgWidth * sizeof(uint8_t));
  }

  for (int y = 1; y < imgHeight - 1; y++) {
    for (int x = 1; x < imgWidth - 1; x++) {
      int sobelOperated = 0;

      // Apply Sobel kernel
      for (int ky = -1; ky <= 1; ky++) {
        for (int kx = -1; kx <= 1; kx++) {
          sobelOperated += gaussBlurMatrix[y + ky][x + kx] * vSobel[ky + 1][kx + 1];
        }
      }

      // Clamp the value to [0, 255]
      sobelOperated = max(0, min(255, sobelOperated));

      // Store in the temporary matrix
      tempMatrix[y][x] = (uint8_t)sobelOperated;
    }
  }

  // Copy the results to the original matrix
  for (int y = 0; y < imgHeight; y++) {
    for (int x = 0; x < imgWidth; x++) {
      vSobelMatrix[y][x] = tempMatrix[y][x];
    }
    free(tempMatrix[y]);
  }
  free(tempMatrix);
}

void horizontalSobelOperator(uint8_t **gaussBlurMatrix, uint8_t **hSobelMatrix) {
  int hSobel[3][3] = { { -1, -2, -1 }, { 0, 0, 0 }, { 1, 2, 1 } };

  // Temporary matrix to store the result
  uint8_t **tempMatrix = (uint8_t **)malloc(imgHeight * sizeof(uint8_t *));
  for (int i = 0; i < imgHeight; i++) {
    tempMatrix[i] = (uint8_t *)malloc(imgWidth * sizeof(uint8_t));
  }

  for (int y = 1; y < imgHeight - 1; y++) {
    for (int x = 1; x < imgWidth - 1; x++) {
      int sobelOperated = 0;

      // Apply Sobel kernel
      for (int ky = -1; ky <= 1; ky++) {
        for (int kx = -1; kx <= 1; kx++) {
          sobelOperated += gaussBlurMatrix[y + ky][x + kx] * hSobel[ky + 1][kx + 1];
        }
      }

      // Clamp the value to [0, 255]
      sobelOperated = max(0, min(255, sobelOperated));

      // Store in the temporary matrix
      tempMatrix[y][x] = (uint8_t)sobelOperated;
    }
  }

  // Copy the results to the original matrix
  for (int y = 0; y < imgHeight; y++) {
    for (int x = 0; x < imgWidth; x++) {
      hSobelMatrix[y][x] = tempMatrix[y][x];
    }
    free(tempMatrix[y]);
  }
  free(tempMatrix);
}

void imageProcessingDone(uint8_t **grayscaleMatrix, uint8_t **vSobelMatrix, uint8_t **hSobelMatrix, uint8_t **processedMatrix) {
  // Copy the results from previous operations to a final matrix
  for (int y = 0; y < imgHeight; y++) {
    for (int x = 0; x < imgWidth; x++) {
      processedMatrix[y][x] = vSobelMatrix[y][x]+hSobelMatrix[y][x];
    }
  }
}