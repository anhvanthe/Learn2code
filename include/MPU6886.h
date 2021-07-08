/*
 Note: The MPU6886 is an I2C sensor and uses the Arduino Wire library.
 Because the sensor is not 5V tolerant, we are using a 3.3 V 8 MHz Pro Mini or
 a 3.3 V Teensy 3.1. We have disabled the internal pull-ups used by the Wire
 library in the Wire.h/twi.c utility file. We are also using the 400 kHz fast
 I2C mode by setting the TWI_FREQ  to 400000L /twi.h utility file.
 */
#ifndef _MPU6886_H_
#define _MPU6886_H_

#include <Arduino.h>
#include <Wire.h>

#define I2C_MPU6886_DEFAULT_ADDRESS 0x68

class I2C_MPU6886 {
  public:
    I2C_MPU6886(uint8_t deviceAddress = I2C_MPU6886_DEFAULT_ADDRESS, TwoWire &i2cPort = Wire);

    int begin(void);

    uint8_t whoAmI();

    void getAccel(float* ax, float* ay, float* az);
    void getGyro(float* gx, float* gy, float* gz);
    void getTemp(float *t);

  private:
    uint8_t readByte(uint8_t address);
    void writeByte(uint8_t address, uint8_t data);
    void bitOn(uint8_t address, uint8_t bit);
    void bitOff(uint8_t address, uint8_t bit);

    TwoWire *_i2cPort;
    int _deviceAddress;
};


#endif