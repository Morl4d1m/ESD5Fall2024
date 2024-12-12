//Stuff for ESP-NOW communication
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_mac.h>  // For the MAC2STR and MACSTR macros

// Master ESPCAM MAC: 08:a6:f7:10:64:a4
// Slave ESP32 MAC: a0:b7:65:4c:0e:f0
bool ackReceived = false;  // Flag for acknowledgment

// Variables to send
uint8_t ch1Output = 127;
uint8_t ch2Output = 0;
uint8_t ch3Output = 127;
uint8_t ch4Output = 0;
uint32_t packageNumber = 0;

// Slave MAC Address
uint8_t slaveMAC[] = { 0xa0, 0xb7, 0x65, 0x4c, 0x0e, 0xf0 };

// Struct for ESPNOW message
typedef struct {
  uint32_t timestamp;          // Master time
  uint32_t packageNumber;      // Sequence number
  uint8_t ch1, ch2, ch3, ch4;  // Outputs
} esp32MasterMessage_t;

esp32MasterMessage_t outgoingMessage;  // Storage for struct

esp_now_peer_info_t peerInfo;  // Store information about peer

//Stuff used for canny edge detection
#include "esp_camera.h"
#include "FS.h"                // FileSystem support for ESP32
#include "SD_MMC.h"            // SD card interface
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems
#include "driver/rtc_io.h"
#include <EEPROM.h>  // Read and write from flash memory

String testImageFileName = "/picture76.jpg";  // Path to image on the SD card

// Image dimensions
const int imgWidth = 160;
const int imgHeight = 120;

// Add mirrored padding to remove noisy border
const int paddedHeight = imgHeight + 2;
const int paddedWidth = imgWidth + 2;
//Stuff used for taking pictures and using SD card
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

String imageFileName = "/picture" + String(pictureNumber) + ".jpg";  // Path to image on the SD card
// Convert the file name to a C-style string for compatibility
//const char *imageFileName = fileName.c_str();

//Stores the camera configuration parameters
camera_config_t config;

//Stuff used for control signal computing
#include <math.h>

// Define matrix dimensions
//#define ROWS 15
//#define COLS 19
// Variables for centroid calculation
float centroidX = 0;
float centroidY = 0;

int i = 0;
const int scale = 5;  // Downsampling factor
const int downImgHeight = imgHeight / scale;
const int downImgWidth = imgWidth / scale;

// Variables for timing functions:
uint32_t startTime = 0;
uint32_t currentTime = 0;
uint32_t finishTime = 0;
uint32_t timeSpent = 0;
uint32_t totalTime = 0;
uint32_t averageTime = 0;
int testIteration = 0;

//HEAP/STACK INFO
#include "esp_heap_caps.h"
UBaseType_t stackHighWaterMark;

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);  // Disable brownout detector

  Serial.begin(115200);

  //Initialize the camera
  Serial.print("Initializing the camera module...");
  configInitCamera();
  Serial.println("Ok!");

  while (!Serial)
    ;
  SD_MMC.begin("/sdcard", true);  // Necessary to disable flash

  // Initialize SD card using SD_MMC
  if (!SD_MMC.begin()) {
    Serial.println("SD Card Mount Failed");
    return;
  }
  uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD Card attached");
    return;
  }
  Serial.println("SD card initialized.");

  ESPNOWSetup();

  // Stack size monitoring
  printHeapInfo();

  // Initialize all matrices
  uint8_t **grayscaleMatrix = initializeMatrix(imgWidth, imgHeight, "grayscaleMatrix");
  uint8_t **gaussBlurMatrix = initializeMatrix(imgWidth, imgHeight, "gaussBlurMatrix");
  uint8_t **vSobelMatrix = initializeMatrix(imgWidth, imgHeight, "vSobelMatrix");
  uint8_t **hSobelMatrix = initializeMatrix(imgWidth, imgHeight, "hSobelMatrix");
  uint8_t **sumSobelMatrix = initializeMatrix(imgWidth, imgHeight, "sumSobelMatrix");
  uint8_t **edgeMatrix = initializeMatrix(imgWidth, imgHeight, "edgeMatrix");
  uint8_t **downsampledMatrix = initializeMatrix(downImgWidth, downImgHeight, "downsampledMatrix");

  if (!grayscaleMatrix || !gaussBlurMatrix || !vSobelMatrix || !hSobelMatrix || !sumSobelMatrix || !edgeMatrix) {
    Serial.println("Matrix initialization failed!");
    return;
  }

  Serial.println("All matrices initialized successfully.");
  for (int w; w < 10000; w++) {
    testIteration++;
    // Stack size monitoring
    printHeapInfo();
    startTime = millis();

    //Take and Save Photo
    takeSavePhoto();
    //Serial.println("Photo taken?");

    // Read and display the grayscale image
    readGrayscaleImageFromSD(imageFileName, grayscaleMatrix);  // Read image and convert to 8bit grayscale
    //displayMatrix(grayscaleMatrix);
    //Serial.println("Grayscale");
    gaussBlurOperator(grayscaleMatrix, gaussBlurMatrix);  // Gaussian blurring
    //displayMatrix(gaussBlurMatrix);
    //Serial.println("Gauss");
    verticalSobelOperator(gaussBlurMatrix, vSobelMatrix);  // Vertital sobel operation
    //displayMatrix(vSobelMatrix);
    //Serial.println("vSobel");
    horizontalSobelOperator(gaussBlurMatrix, hSobelMatrix);  // Horizontal sobel operation
    //displayMatrix(hSobelMatrix);
    //Serial.println("hSobel");
    sumSobel(grayscaleMatrix, vSobelMatrix, hSobelMatrix, sumSobelMatrix);               // Sum sobel operator into 1 image
    applyDoubleThresholding(sumSobelMatrix, edgeMatrix, imgWidth, imgHeight, 100, 190);  // Perform edge detection
    //Serial.println("sumSobel");
    //displayMatrix(sumSobelMatrix);
    Serial.println();
    // Display the edge-detected matrix
    //displayMatrix(edgeMatrix);
    downsampleFromCenterAndTop(edgeMatrix, downsampledMatrix);
    //Serial.println("Matrix downsampled");
    //downDisplayMatrix(downsampledMatrix);
    analyzeMatrix(downsampledMatrix);
    //Serial.println("Matrix analyzed");


    finishTime = millis();
    // Stack size monitoring
    //printHeapInfo();
    timeSpent = finishTime - startTime;
    totalTime += timeSpent;
    averageTime = totalTime / testIteration;
    Serial.print("Iteration number: ");
    Serial.println(testIteration);
    Serial.print("Time spent this iteration: ");
    Serial.println(timeSpent);
    Serial.print("Average time spent: ");
    Serial.println(averageTime);
    sendMessageReceiveACK();
    //delay(500);
  }
  Serial.print("Done testing! The average time spent to perform was: ");
  Serial.println(averageTime);
}
void loop() {
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
  config.fb_count = 2;                     //Must be 2 to always get the newest image
  config.grab_mode = CAMERA_GRAB_LATEST;   // Must be set to grab latest image and not a previous one - solved using https://github.com/espressif/arduino-esp32/issues/6047

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
  imageFileName = "/picture" + String(pictureNumber) + ".jpg";  //update imageFileName so other functions which number it is at

  fs::FS &fs = SD_MMC;
  //Serial.printf("Picture file name: %s\n", path.c_str());

  File file = fs.open(path.c_str(), FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file in writing mode");
  } else {
    file.write(fb->buf, fb->len);  // payload (image), payload length
    //Serial.printf("Saved file to path: %s\n", path.c_str());
    EEPROM.write(0, pictureNumber);
    EEPROM.commit();
  }
  file.close();

  //return the frame buffer back to the driver for reuse
  esp_camera_fb_return(fb);
}

uint8_t **initializeMatrix(int width, int height, const char *matrixName) {
  Serial.print("Allocating memory for ");
  Serial.println(matrixName);

  uint8_t **matrix = (uint8_t **)ps_malloc(height * sizeof(uint8_t *));
  if (!matrix) {
    Serial.print("Error: Not enough memory for ");
    Serial.println(matrixName);
    return NULL;
  }

  for (int i = 0; i < height; i++) {
    matrix[i] = (uint8_t *)ps_malloc(width * sizeof(uint8_t));
    if (!matrix[i]) {
      Serial.print("Error: Not enough memory for row in ");
      Serial.println(matrixName);
      // Free already allocated rows before returning
      for (int j = 0; j < i; j++) {
        free(matrix[j]);
      }
      free(matrix);
      return NULL;
    }
  }

  Serial.print(matrixName);
  Serial.println(" initialized successfully.");
  return matrix;
}

void readGrayscaleImageFromSD(String fileName, uint8_t **grayscaleMatrix) {
  // Open the file from the SD card
  File file = SD_MMC.open(fileName, FILE_READ);
  if (!file) {
    Serial.print("Error: Could not open image file: ");
    Serial.println(fileName);
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

DRAM_ATTR void displayMatrix(uint8_t **matrixToDisplay) {
  for (int y = 0; y < imgHeight; y++) {
    for (int x = 0; x < imgWidth; x++) {
      Serial.print(matrixToDisplay[y][x]);  // Print each value
      Serial.print("\t");                   // Tab-separated
    }
    Serial.println();  // New line after each row
  }
  Serial.println("Matrix printed.");
}

DRAM_ATTR void downDisplayMatrix(uint8_t **matrixToDisplay) {
  for (int y = 0; y < downImgHeight; y++) {
    for (int x = 0; x < downImgWidth; x++) {
      Serial.print(matrixToDisplay[y][x]);  // Print each value
      Serial.print("\t");                   // Tab-separated
    }
    Serial.println();  // New line after each row
  }
  Serial.println("Matrix printed.");
}

void gaussBlurOperator(uint8_t **grayscaleMatrix, uint8_t **gaussBlurMatrix) {
  int gaussBlur[3][3] = { { 1, 2, 1 }, { 2, 4, 2 }, { 1, 2, 1 } };

  // Allocate padded matrix
  uint8_t **paddedMatrix = (uint8_t **)ps_malloc(paddedHeight * sizeof(uint8_t *));
  for (int i = 0; i < paddedHeight; i++) {
    paddedMatrix[i] = (uint8_t *)ps_malloc(paddedWidth * sizeof(uint8_t));
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
  uint8_t **paddedMatrix = (uint8_t **)ps_malloc(paddedHeight * sizeof(uint8_t *));
  for (int i = 0; i < paddedHeight; i++) {
    paddedMatrix[i] = (uint8_t *)ps_malloc(paddedWidth * sizeof(uint8_t));
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
  uint8_t **paddedMatrix = (uint8_t **)ps_malloc(paddedHeight * sizeof(uint8_t *));
  for (int i = 0; i < paddedHeight; i++) {
    paddedMatrix[i] = (uint8_t *)ps_malloc(paddedWidth * sizeof(uint8_t));
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

void analyzeMatrix(uint8_t **matrix) {
  int labelMatrix[downImgHeight][downImgWidth] = { 0 };  // Label matrix
  int label = 1;                                         // Start labeling components from 1

  // Direction arrays for 8-connectivity
  int dr[] = { -1, -1, -1, 0, 1, 1, 1, 0 };
  int dc[] = { -1, 0, 1, 1, 1, 0, -1, -1 };

  // Label connected components
  for (int r = 0; r < downImgHeight; r++) {
    for (int c = 0; c < downImgWidth; c++) {
      if (matrix[r][c] == 255 && labelMatrix[r][c] == 0) {
        // Perform flood fill
        floodFill(matrix, labelMatrix, r, c, label, dr, dc);
        label++;
      }
    }
  }

  // Calculate angle for each component
  for (int l = 1; l < label; l++) {
    float angle = calculateComponentAngle(labelMatrix, l);
    //Serial.print("Component ");
    //Serial.print(l);
    //Serial.print(" Angle relative to centerline: ");
    //Serial.println(angle);
  }
}

// Recursive flood-fill to label connected components
void floodFill(uint8_t **matrix, int labelMatrix[downImgHeight][downImgWidth], int r, int c, int label, int dr[], int dc[]) {
  if (r < 0 || r >= downImgHeight || c < 0 || c >= downImgWidth) return;  // Out of bounds
  if (matrix[r][c] == 0 || labelMatrix[r][c] != 0) return;                // Not part of the component or already labeled

  labelMatrix[r][c] = label;

  // Visit all 8 neighbors
  for (int d = 0; d < 8; d++) {
    floodFill(matrix, labelMatrix, r + dr[d], c + dc[d], label, dr, dc);
  }
}

// Function to calculate the angle of a component relative to the centerline
float calculateComponentAngle(int labelMatrix[downImgHeight][downImgWidth], int label) {
  float sumX = 0, sumY = 0;
  float sumX2 = 0, sumY2 = 0, sumXY = 0;
  int count = 0;

  // Compute moments
  for (int r = 0; r < downImgHeight; r++) {
    for (int c = 0; c < downImgWidth; c++) {
      if (labelMatrix[r][c] == label) {
        // Shift coordinates relative to center column
        int x = c - 8;  // x relative to center column
        int y = -r;     // y relative to top row, inverted for standard coordinates

        sumX += x;
        sumY += y;
        sumX2 += x * x;
        sumY2 += y * y;
        sumXY += x * y;
        centroidX += c;
        centroidY += r;
        count++;
      }
    }
  }

  if (count == 0) return 0.0;  // No points in component

  // Normalize moments
  float meanX = sumX / count;
  float meanY = sumY / count;
  float sigmaXX = sumX2 / count - meanX * meanX;
  float sigmaYY = sumY2 / count - meanY * meanY;
  float sigmaXY = sumXY / count - meanX * meanY;

  // Calculate angle relative to the vertical (y-axis)
  float angleRadians = atan2(sigmaXY, sigmaYY);

  // Convert to degrees
  float angleDegrees = angleRadians * 180.0 / PI;

  // Calculate centroid
  centroidX /= count;
  centroidY /= count;

  // Adjust angle based on centroid location
  /*if (centroidX < (downImgWidth / 2) && angleDegrees > 0) {
    angleDegrees = 0;  // Left half and positive angle
  } else if (centroidX > (downImgWidth / 2) && angleDegrees < 0) {
    angleDegrees = 0;  // Right half and negative angle
  } else if (centroidX < (downImgWidth / 2) && angleDegrees == 0) {
    angleDegrees = -5;  // Left half and negative angle
  } else if (centroidX > (downImgWidth / 2) && angleDegrees == 0) {
    angleDegrees = 5;  // Right half and negative angle
  }*/

  return angleDegrees;
}

void downsampleFromCenterAndTop(uint8_t **inputMatrix, uint8_t **outputMatrix) {
  const int outputRows = downImgHeight;
  const int outputCols = downImgWidth;

  for (int outRow = 0; outRow < outputRows; outRow++) {
    for (int outCol = 0; outCol < outputCols; outCol++) {
      // Pick the top-left pixel of each 10x10 block
      int inRow = outRow * scale + scale / 2;
      int inCol = outCol * scale + scale / 2;

      // Assign the sampled pixel to the output matrix
      outputMatrix[outRow][outCol] = inputMatrix[inRow][inCol];
    }
  }
}


//ESP-NOW functions:
void ESPNOWSetup() {
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

  // Register callback for receiving data (ACK in this case)
  esp_now_register_recv_cb(receiveAck);  // Called when data is received

  // Register peer
  memcpy(peerInfo.peer_addr, slaveMAC, 6);
  peerInfo.channel = 0;      // Assign a channel to transmit data across
  peerInfo.encrypt = false;  // Set true if encryption is wanted

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  uint32_t setupTime = micros();
}

void sendMessageReceiveACK() {
  // Prepare message
  outgoingMessage.timestamp = esp_timer_get_time();  // Microsecond precision
  outgoingMessage.packageNumber = testIteration;     //packageNumber++;
  outgoingMessage.ch1 = ch1Output;
  outgoingMessage.ch2 = ch2Output;
  outgoingMessage.ch3 = ch3Output;
  outgoingMessage.ch4 = ch4Output;

  ackReceived = false;
  for (int retry = 0; retry < 3; retry++) {
    esp_err_t result = esp_now_send(slaveMAC, (uint8_t *)&outgoingMessage, sizeof(outgoingMessage));
    if (result != ESP_OK) {
      Serial.println("Error sending the data");
    } else {
      Serial.println("Sent with success");
    }

    delay(100);  // Wait for acknowledgment
    if (ackReceived == true) break;
  }

  if (!ackReceived) {
    Serial.printf("Failed to receive ACK for package %d\n", outgoingMessage.packageNumber);
    //In the final program, the system should be halted by now, as data has been lost
  }

  // Update outputs for testing
  if (ch1Output < 256) {
    ch1Output += 1;
    ch3Output += 1;
    if (ch1Output == 256) {
      ch1Output = 0;
    }
  }
  if (ch2Output < 256) {
    ch2Output += 1;
    ch4Output += 1;
    if (ch2Output == 256) {
      ch2Output = 0;
    }
  }
}

void dataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  //Serial.print("\r\nLast Packet Send Status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");  // Remember that the ? is an inline if statement, returning the first value if true and the second if false
}

void receiveAck(const esp_now_recv_info *info, const uint8_t *data, int len) {
  if (len != sizeof(outgoingMessage.packageNumber)) {
    Serial.println("Invalid ACK size");
    return;
  }

  // Check and log sender's MAC address
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           info->src_addr[0], info->src_addr[1], info->src_addr[2],
           info->src_addr[3], info->src_addr[4], info->src_addr[5]);
  Serial.printf("ACK received from: %s\n", macStr);
  //Serial.println(outgoingMessage.packageNumber);
  //Serial.println(packageNumber);
  // Verify the package number in the ACK
  //memcpy(&outgoingMessage, data, sizeof(outgoingMessage));
  if (outgoingMessage.packageNumber == testIteration ) {
    ackReceived = true;
    Serial.printf("ACK received for package %d\n", outgoingMessage.packageNumber);
  }
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

void printHeapInfo() {
  stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);  // NULL gets the current task
  Serial.print("Remaining stack size: ");
  Serial.println(stackHighWaterMark * 4);  // Convert to bytes
  Serial.print("Total PSRAM: ");
  Serial.println(ESP.getPsramSize());
  Serial.print("Free heap: ");
  Serial.println(ESP.getFreeHeap());
  Serial.print("Free PSRAM: ");
  Serial.println(ESP.getFreePsram());
}