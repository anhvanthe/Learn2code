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



/*********** COMMUNICATION SELECTION ***********/
//ADXL345 adxl = ADXL345();             // USE FOR I2C COMMUNICATION
I2C_MPU6886 imu(I2C_MPU6886_DEFAULT_ADDRESS, Wire);


//Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);


void display_drawtest(void);

/******************** SETUP ********************/
/*          Configure ADXL345 Settings         */
void setup(){
  uint8_t i_am = 0;
  Serial.begin(115200);                 // Start the serial terminal
  Serial.println("Let's start learning....");
  Serial.println();
  
  // Wire.begin(21, 22);
  Wire.begin();
  I2C_MPU6886 imu(I2C_MPU6886_DEFAULT_ADDRESS, Wire);



  imu.begin();

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
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setRotation(0);

  // Clear the buffer
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);

  display.println(F("MPU6886...."));
  display.display();
  delay(1000);


}

/****************** MAIN CODE ******************/
/*     Accelerometer Readings and Interrupt    */
void loop(){

  // // Clear the buffer
  // display.clearDisplay();
  // display.setCursor(0, 0);
  // display.setTextSize(2);

  // display.println(F("Hello, world!"));
  // display.display();
  // delay(1000);


}


void display_drawtest(void)
{
  int w = 0, h = 0;
    //Draw a single pixel in white
  display.drawPixel(64, 32, SSD1306_WHITE);

  display.clearDisplay();
  display.display();
  delay(1000);


    // DISPLAY AN INCREASING CIRCLE
  for (h = 0; h < SCREEN_HEIGHT; h++)
  {
    display.drawCircle(64, 32, h, SSD1306_WHITE);
    display.display();
    delay(100);
  }
  delay(1000);
  display.clearDisplay();
    // DISPLAY A DECREASING CIRCLE
  for (h = SCREEN_HEIGHT; h > 0; h--)
  {
    display.drawCircle(64, 32, h, SSD1306_WHITE);
    display.display();
    delay(100);
  }
  delay(1000);
  
  // DISPLAY A FILLING SCREEEN
  for (w = 0; w < SCREEN_WIDTH; w++)
  {
    for (h = 0; h < SCREEN_HEIGHT; h++)
    {
      display.drawPixel(w,h, SSD1306_WHITE);
      
    } 
  }
}