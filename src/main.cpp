#include <Arduino.h>

#include "config.h"
#include "MPU6886.h"
// Basic demo for accelerometer readings from Adafruit MPU6050
#include <Wire.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/************** DEFINED VARIABLES **************/
/*                                             */
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

float accX = 0;
float accY = 0;
float accZ = 0;

float gyroX = 0;
float gyroY = 0;
float gyroZ = 0;

float temp = 0;

/*********** COMMUNICATION SELECTION ***********/
//ADXL345 adxl = ADXL345();             // USE FOR I2C COMMUNICATION
MPU6886 imu(MPU6886_ADDRESS, Wire);


//Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);


/******************** SETUP ********************/
/*          Configure ADXL345 Settings         */
void setup(){
  uint8_t i_am = 0;
  Serial.begin(115200);                 // Start the serial terminal
  Serial.println("Let's start learning....");
  Serial.println();
  
  // Wire.begin(21, 22);
  Wire.begin();
  MPU6886 imu(MPU6886_ADDRESS, Wire);



  imu.Init();

  i_am = imu.whoAmI();

  Serial.println(F("WhoAmI() = "));
  Serial.println(i_am, HEX);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  display.display();
  delay(500); // Pause for 2 seconds
  //display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setRotation(0);

  // Clear the buffer
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);

  display.println(F("MPU6886..."));
  display.display();
  delay(1000);


}

/****************** MAIN CODE ******************/
/*     Accelerometer Readings and Interrupt    */
void loop(){
  //display.clearDisplay();

  imu.getGyroData(&gyroX,&gyroY,&gyroZ);
  imu.getAccelData(&accX,&accY,&accZ);
  imu.getTempData(&temp);

  Serial.println(accX,DEC);
  Serial.println(accY,DEC);
  Serial.println(accZ,DEC);
  Serial.println(temp,10);
  //printf("\n\r");
  delay(1000);
  //display.setCursor(0, 0);
  //display.printFloat(gyroX, 2);

}

