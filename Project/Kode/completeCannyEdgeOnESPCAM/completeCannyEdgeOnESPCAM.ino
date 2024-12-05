#include "esp_camera.h"
#include "FS.h"                // FileSystem support for ESP32
#include "SD_MMC.h"            // SD card interface
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems
#include "driver/rtc_io.h"
#include <EEPROM.h>  // Read and write from flash memory

const char *imageFileName = "/picture88.jpg";  // Path to your grayscale image on the SD card

// Example image dimensions
const int imgWidth = 160;
const int imgHeight = 120;

// Add mirrored padding to remove noisy border
const int paddedHeight = imgHeight + 2;
const int paddedWidth = imgWidth + 2;



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

  //Repeated for sumSobelMatrix
  uint8_t **sumSobelMatrix;
  sumSobelMatrix = (uint8_t **)malloc(imgHeight * sizeof(uint8_t *));  // Allocate memory for 120 rows

  if (sumSobelMatrix == NULL) {  // Check if the memory has been allocated
    Serial.println("Not enough memory!");
    return;
  } else {
    Serial.println("Memory allocated for row pointers.");
  }

  // Allocate memory for each row (160 elements per row)
  for (int i = 0; i < imgHeight; i++) {
    sumSobelMatrix[i] = (uint8_t *)malloc(imgWidth * sizeof(uint8_t));
    if (sumSobelMatrix[i] == NULL) {
      Serial.println("Not enough memory for row!");
      return;
    }
  }
  Serial.println("Memory allocated for column pointers.");

  //Repeated for edgeMatrix
  uint8_t **edgeMatrix;
  edgeMatrix = (uint8_t **)malloc(imgHeight * sizeof(uint8_t *));  // Allocate memory for 120 rows

  if (edgeMatrix == NULL) {  // Check if the memory has been allocated
    Serial.println("Not enough memory!");
    return;
  } else {
    Serial.println("Memory allocated for row pointers.");
  }

  // Allocate memory for each row (160 elements per row)
  for (int i = 0; i < imgHeight; i++) {
    edgeMatrix[i] = (uint8_t *)malloc(imgWidth * sizeof(uint8_t));
    if (edgeMatrix[i] == NULL) {
      Serial.println("Not enough memory for row!");
      return;
    }
  }
  Serial.println("Memory allocated for column pointers.");

  Serial.println("Begin");

  // Read and display the grayscale image
  readGrayscaleImageFromSD(imageFileName, grayscaleMatrix);  // Read image and convert to 8bit grayscale
  //displayMatrix(grayscaleMatrix);
  //Serial.println("Grayscale");
  gaussBlurOperator(grayscaleMatrix, gaussBlurMatrix);                                // Gaussian blurring
  //displayMatrix(gaussBlurMatrix);
  //Serial.println("Gauss");
  verticalSobelOperator(gaussBlurMatrix, vSobelMatrix);                               // Vertital sobel operation
  //displayMatrix(vSobelMatrix);
  //Serial.println("vSobel");
  horizontalSobelOperator(gaussBlurMatrix, hSobelMatrix);                             // Horizontal sobel operation
  //displayMatrix(hSobelMatrix);
  //Serial.println("hSobel");
  sumSobel(grayscaleMatrix, vSobelMatrix, hSobelMatrix, sumSobelMatrix);              // Sum sobel operator into 1 image
  applyDoubleThresholding(sumSobelMatrix, edgeMatrix, imgWidth, imgHeight, 100, 190);  // Perform edge detection
  //Serial.println("sumSobel");

  Serial.println("End");

  //displayMatrix(sumSobelMatrix);
  Serial.println();
  // Display the edge-detected matrix
  displayMatrix(edgeMatrix);
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

  /*Serial.println("Reading grayscale image...");

  //Inspect header size - only used while debugging
  for (int i = 0; i < 100 && file.available(); i++) {
    Serial.print(file.read(), HEX);
    Serial.print(" ");
  }
  Serial.println();
  Serial.println("The above is header information.");
  Serial.println();
  Serial.println(file.size());  // Prints the size in bytes, 38400 matches an RGB565 of 160x120 pixels
  Serial.println();*/

  // Header size in bytes (adjust if needed)
  const int pixelDataOffset = 0;  // Offset to pixel data //1078 is used for bitmaps created with paint //0 is used for RGB565
  file.seek(pixelDataOffset);

  int errorLine = imgHeight;  // Used for debugging serial output of image data

  // Buffer for bulk reading (adjust size as needed)
  const int bufferSize = 1024;  // Tried with 2048 as well, with no sign of improved runtime for QQVGA images
  uint8_t buffer[bufferSize];   // Allocate a uint8_t buffer
  int bufferIndex = 0;          // Index for accessing the buffer
  size_t bytesRead = 0;

  //Serial.println("Begin");
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
  //Serial.println("End");
  //delay(500);
  //Serial.println(grayscaleMatrix[118][159]); // Remember that it is Y,X and not X,Y

  // Close the file
  file.close();
  //Serial.println("Finished reading image.");
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

void gaussBlurOperator(uint8_t **grayscaleMatrix, uint8_t **gaussBlurMatrix) {
  int gaussBlur[3][3] = { { 1, 2, 1 }, { 2, 4, 2 }, { 1, 2, 1 } };

  // Allocate padded matrix
  uint8_t **paddedMatrix = (uint8_t **)malloc(paddedHeight * sizeof(uint8_t *));
  for (int i = 0; i < paddedHeight; i++) {
    paddedMatrix[i] = (uint8_t *)malloc(paddedWidth * sizeof(uint8_t));
  }

  // Fill padded matrix with mirrored values
  for (int y = 0; y < paddedHeight; y++) {
    for (int x = 0; x < paddedWidth; x++) {
      int srcY = min(max(y - 1, 0), imgHeight - 1);
      int srcX = min(max(x - 1, 0), imgWidth - 1);
      paddedMatrix[y][x] = grayscaleMatrix[srcY][srcX];
    }
  }

  // Apply Gaussian blur to the padded matrix
  for (int y = 1; y < paddedHeight - 1; y++) {
    for (int x = 1; x < paddedWidth - 1; x++) {
      int gaussBlurred = 0;

      for (int ky = -1; ky <= 1; ky++) {
        for (int kx = -1; kx <= 1; kx++) {
          gaussBlurred += paddedMatrix[y + ky][x + kx] * gaussBlur[ky + 1][kx + 1];
        }
      }

      // Normalize and clamp to [0, 255]
      gaussBlurred /= 16;
      gaussBlurred = max(0, min(255, gaussBlurred));

      // Write to the output matrix (adjust for offset)
      gaussBlurMatrix[y - 1][x - 1] = (uint8_t)gaussBlurred;
    }
  }

  // Free padded matrix
  for (int i = 0; i < paddedHeight; i++) {
    free(paddedMatrix[i]);
  }
  free(paddedMatrix);
}

void verticalSobelOperator(uint8_t **gaussBlurMatrix, uint8_t **vSobelMatrix) {
  int vSobel[3][3] = { { -1, 0, 1 }, { -2, 0, 2 }, { -1, 0, 1 } };

  // Allocate padded matrix
  uint8_t **paddedMatrix = (uint8_t **)malloc(paddedHeight * sizeof(uint8_t *));
  for (int i = 0; i < paddedHeight; i++) {
    paddedMatrix[i] = (uint8_t *)malloc(paddedWidth * sizeof(uint8_t));
  }
/*
  // Fill padded matrix with mirrored values
  for (int y = 0; y < paddedHeight; y++) {
    for (int x = 0; x < paddedWidth; x++) {
      int srcY = min(max(y - 1, 0), imgHeight - 1);
      int srcX = min(max(x - 1, 0), imgWidth - 1);
      paddedMatrix[y][x] = gaussBlurMatrix[srcY][srcX];
    }
  }
*/
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
      sobelOperated = abs(sobelOperated);
      sobelOperated = max(0, min(255, sobelOperated));

      // Store in the temporary matrix
      paddedMatrix[y][x] = (uint8_t)sobelOperated;
    }
  }


  // Copy the results to the original matrix
  for (int y = 0; y < imgHeight; y++) {
    for (int x = 0; x < imgWidth; x++) {
      vSobelMatrix[y][x] = paddedMatrix[y][x];
    }
    free(paddedMatrix[y]);
  }
  free(paddedMatrix);
}

void horizontalSobelOperator(uint8_t **gaussBlurMatrix, uint8_t **hSobelMatrix) {
  int hSobel[3][3] = { { 1, 2, 1 }, { 0, 0, 0 }, { -1, -2, -1 } };

  // Allocate padded matrix
  uint8_t **paddedMatrix = (uint8_t **)malloc(paddedHeight * sizeof(uint8_t *));
  for (int i = 0; i < paddedHeight; i++) {
    paddedMatrix[i] = (uint8_t *)malloc(paddedWidth * sizeof(uint8_t));
  }
/*
  // Fill padded matrix with mirrored values
  for (int y = 0; y < paddedHeight; y++) {
    for (int x = 0; x < paddedWidth; x++) {
      int srcY = min(max(y - 1, 0), imgHeight - 1);
      int srcX = min(max(x - 1, 0), imgWidth - 1);
      paddedMatrix[y][x] = gaussBlurMatrix[srcY][srcX];
    }
  }
*/
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
      sobelOperated = abs(sobelOperated);
      sobelOperated = max(0, min(255, sobelOperated));

      // Store in the temporary matrix
      paddedMatrix[y][x] = (uint8_t)sobelOperated;
    }
  }

  // Copy the results to the original matrix
  for (int y = 0; y < imgHeight; y++) {
    for (int x = 0; x < imgWidth; x++) {
      hSobelMatrix[y][x] = paddedMatrix[y][x];
    }
    free(paddedMatrix[y]);
  }
  free(paddedMatrix);
}

void applyDoubleThresholding(uint8_t **inputMatrix, uint8_t **outputMatrix, int width, int height, int lowThreshold, int highThreshold) {
  // Initialize the output matrix
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      outputMatrix[y][x] = 0;  // Initialize as non-edge
    }
  }

  // Perform double thresholding
  for (int y = 1; y < height - 1; y++) {
    for (int x = 1; x < width - 1; x++) {
      if (inputMatrix[y][x] >= highThreshold) {
        outputMatrix[y][x] = 255;  // Strong edge
      } else if (inputMatrix[y][x] >= lowThreshold) {
        // Check if connected to a strong edge
        bool connectedToStrongEdge = false;
        for (int ky = -1; ky <= 1; ky++) {
          for (int kx = -1; kx <= 1; kx++) {
            if (inputMatrix[y + ky][x + kx] >= highThreshold) {
              connectedToStrongEdge = true;
              break;
            }
          }
          if (connectedToStrongEdge) break;
        }
        if (connectedToStrongEdge) {
          outputMatrix[y][x] = 255;  // Weak edge connected to strong edge
        }
      }
    }
  }
}

void sumSobel(uint8_t **grayscaleMatrix, uint8_t **vSobelMatrix, uint8_t **hSobelMatrix, uint8_t **sumSobelMatrix) {
  // Copy the results from previous operations to a final matrix
  for (int y = 0; y < imgHeight; y++) {
    for (int x = 0; x < imgWidth; x++) {
      sumSobelMatrix[y][x] = max(0, min(255, vSobelMatrix[y][x] + hSobelMatrix[y][x]));
    }
  }
}