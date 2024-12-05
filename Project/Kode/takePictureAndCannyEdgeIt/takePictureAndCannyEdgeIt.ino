#include "esp_camera.h"
#include "FS.h"                // FileSystem support for ESP32
#include "SD_MMC.h"            // SD card interface
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems
#include "driver/rtc_io.h"
#include <EEPROM.h>  // Read and write from flash memory

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

String imageFileName = "/picture" + String(pictureNumber) + ".jpg";  // Path to your grayscale image on the SD card
// Convert the file name to a C-style string for compatibility
//const char *imageFileName = fileName.c_str();

//Stores the camera configuration parameters
camera_config_t config;

// Example image dimensions
const uint8_t imgWidth = 160;
const uint8_t imgHeight = 120;

// Add mirrored padding to remove noisy border
const int paddedHeight = imgHeight + 2;
const int paddedWidth = imgWidth + 2;

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
  Serial.println("SD card initialized.");
  //Take and Save Photo
  takeSavePhoto();

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

  executionChecker(imageFileName, grayscaleMatrix, gaussBlurMatrix, vSobelMatrix, hSobelMatrix, sumSobelMatrix, edgeMatrix, imgWidth, imgHeight, 100, 190);

  Serial.println("Begin");

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

  Serial.println("End");

  //displayMatrix(sumSobelMatrix);
  Serial.println();
  // Display the edge-detected matrix
  //displayMatrix(sumSobelMatrix);
}

void executionChecker(String fileName, uint8_t **grayscaleMatrix, uint8_t **gaussBlurMatrix, uint8_t **vSobelMatrix, uint8_t **hSobelMatrix, uint8_t **sumSobelMatrix, uint8_t **edgeMatrix, uint8_t width, uint8_t height, uint8_t lowThreshold, uint8_t highThreshold) {
  uint32_t timeSpent = 0;
  uint32_t currentTime = 0;
  uint32_t finishTime = 0;
  uint32_t averageTime = 0;
  Serial.println("Begin execution time checker");
  for (int i = 0; i < 50; i++) {
    currentTime = micros();
    takeSavePhoto();
    readGrayscaleImageFromSD(imageFileName, grayscaleMatrix);                            // Read image and convert to 8bit grayscale
    gaussBlurOperator(grayscaleMatrix, gaussBlurMatrix);                                 // Gaussian blurring
    verticalSobelOperator(gaussBlurMatrix, vSobelMatrix);                                // Vertital sobel operation
    horizontalSobelOperator(gaussBlurMatrix, hSobelMatrix);                              // Horizontal sobel operation
    sumSobel(grayscaleMatrix, vSobelMatrix, hSobelMatrix, sumSobelMatrix);               // Sum sobel operator into 1 image
    applyDoubleThresholding(sumSobelMatrix, edgeMatrix, imgWidth, imgHeight, 100, 190);  // Perform edge detection
    finishTime = micros();
    timeSpent += finishTime - currentTime;
  }
  averageTime = timeSpent/50;
  Serial.print("The average execution time across 10 iterations to take an image and canny edge process it is: ");
  Serial.println(averageTime);
}

void loop() {
}

void initMicroSDCard() {
  // Start Micro SD card
  Serial.println("Starting SD Card");
  SD_MMC.begin("/sdcard", true);  // Necessary to disable flash
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
  imageFileName = "/picture" + String(pictureNumber) + ".jpg"; //update imageFileName so other functions which number it is at

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

void applyDoubleThresholding(uint8_t **sumSobelMatrix, uint8_t **edgeMatrix, uint8_t width, uint8_t height, uint8_t lowThreshold, uint8_t highThreshold) {
  // Initialize the output matrix
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      edgeMatrix[y][x] = 0;  // Initialize as non-edge
    }
  }

  // Perform double thresholding
  for (int y = 1; y < height - 1; y++) {
    for (int x = 1; x < width - 1; x++) {
      if (sumSobelMatrix[y][x] >= highThreshold) {
        edgeMatrix[y][x] = 255;  // Strong edge
      } else if (sumSobelMatrix[y][x] >= lowThreshold) {
        // Check if connected to a strong edge
        bool connectedToStrongEdge = false;
        for (int ky = -1; ky <= 1; ky++) {
          for (int kx = -1; kx <= 1; kx++) {
            if (sumSobelMatrix[y + ky][x + kx] >= highThreshold) {
              connectedToStrongEdge = true;
              break;
            }
          }
          if (connectedToStrongEdge) break;
        }
        if (connectedToStrongEdge) {
          edgeMatrix[y][x] = 255;  // Weak edge connected to strong edge
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