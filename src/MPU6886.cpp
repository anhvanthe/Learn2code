#include "MPU6886.h"
//#include <Arduino.h>

MPU6886::MPU6886(uint8_t deviceAddress, TwoWire &i2cPort) {
  _deviceAddress = deviceAddress;
  _i2cPort = &i2cPort;
}

uint8_t MPU6886::readByte(uint8_t address) {
  _i2cPort->beginTransmission(_deviceAddress);
  _i2cPort->write(address);
  _i2cPort->endTransmission();
  _i2cPort->requestFrom(_deviceAddress, 1);
  uint8_t val = _i2cPort->read();

  //ESP_LOGD("MPU6886", "readByte(%02X) = %02X", address, val);
  return val;
}

void MPU6886::I2C_Read_NBytes(uint8_t driver_Addr, uint8_t start_Addr, uint8_t number_Bytes, uint8_t *read_Buffer){
    
  _i2cPort->beginTransmission(driver_Addr);
  _i2cPort->write(start_Addr);  
  _i2cPort->endTransmission(false);
  uint8_t i = 0;
  _i2cPort->requestFrom(driver_Addr,number_Bytes);
  
  //! Put read results in the Rx buffer
  while (_i2cPort->available()) {
    read_Buffer[i++] = _i2cPort->read();
  }        
}

void MPU6886::writeByte(uint8_t address, uint8_t data) {
  _i2cPort->beginTransmission(_deviceAddress);
  _i2cPort->write(address);
  _i2cPort->write(data);
  _i2cPort->endTransmission();
  //ESP_LOGD("MPU6886", "writeByte(%02X) = %02X", address, data);
}

void MPU6886::I2C_Write_NBytes(uint8_t driver_Addr, uint8_t start_Addr, uint8_t number_Bytes, uint8_t *write_Buffer){

  _i2cPort->beginTransmission(driver_Addr);
  _i2cPort->write(start_Addr);
  _i2cPort->write(*write_Buffer);
  _i2cPort->endTransmission();
}


void MPU6886::bitOn(uint8_t address, uint8_t bit) {
  uint8_t add = address;
  uint8_t val = readByte(add) | bit;
  writeByte(add, val);
}

void MPU6886::bitOff(uint8_t address, uint8_t bit) {
  uint8_t add = address;
  uint8_t val = readByte(add) & ~bit;
  writeByte(add, val);
}

uint16_t MPU6886::Init(void) {
  // WHO_AM_I : IMU Check
  if (whoAmI() != 0x19) {
    return -1;
  }
  delay(10);

  // PWR_MGMT_1(0x6b)
  writeByte(MPU6886_PWR_MGMT_1, 0x00);
  //delay(10);

  // PWR_MGMT_1(0x6b)
  writeByte(MPU6886_PWR_MGMT_1, 1 << 7);
  //delay(10);

  // PWR_MGMT_1(0x6b)
  writeByte(MPU6886_PWR_MGMT_1, 1 << 0);
  //delay(10);

  // ACCEL_CONFIG(0x1c) : +-8G
  writeByte(MPU6886_ACCEL_CONFIG, 0x10);
  //delay(1);

  // GYRO_CONFIG(0x1b) : +-2000dps
  writeByte(MPU6886_GYRO_CONFIG, 0x18);
  //delay(1);

  // CONFIG(0x1a)
  writeByte(MPU6886_CONFIG, 0x01);
  //delay(1);

  // SMPLRT_DIV(0x19)
  writeByte(MPU6886_SMPLRT_DIV, 0x05);
  //delay(1);

  // INT_ENABLE(0x38)
  writeByte(MPU6886_INT_ENABLE, 0x00);
  //delay(1);

  // ACCEL_CONFIG 2(0x1d)
  writeByte(MPU6886_ACCEL_CONFIG2, 0x00);
  //delay(1);

  // USER_CTRL(0x6a)
  writeByte(MPU6886_USER_CTRL, 0x00);
  //delay(1);

  // FIFO_EN(0x23)
  writeByte(MPU6886_FIFO_EN, 0x00);
  //delay(1);

  // INT_PIN_CFG(0x37)
  writeByte(MPU6886_INT_PIN_CFG, 0x22);
  //delay(1);

  // INT_ENABLE(0x38)
  writeByte(MPU6886_INT_ENABLE, 0x01);
  delay(100);

  return 0;
}

uint8_t MPU6886::whoAmI(void) {
  return readByte(MPU6886_WHOAMI);
}

void MPU6886::enableWakeOnMotion(Ascale ascale, uint8_t thresh_num_lsb) {
    uint8_t regdata;
    /* 5.1 WAKE-ON-MOTION INTERRUPT
        The MPU-6886 provides motion detection capability. A qualifying motion sample is one where the high passed sample
        from any axis has an absolute value exceeding a user-programmable threshold. The following steps explain how to
        configure the Wake-on-Motion Interrupt.
    */

    /* Step 0: this isn't explicitly listed in the steps, but configuring the 
       FSR or full-scale-range of the accelerometer is important to setting up
       the accel/motion threshold in Step 4
    */
    regdata = (ascale << 3);
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_ACCEL_CONFIG, 1, &regdata);
    delay(10);

    /* Step 1: Ensure that Accelerometer is running
        • In PWR_MGMT_1 register (0x6B) set CYCLE = 0, SLEEP = 0, and GYRO_STANDBY = 0
        • In PWR_MGMT_2 register (0x6C) set STBY_XA = STBY_YA = STBY_ZA = 0, and STBY_XG = STBY_YG = STBY_ZG = 1
    */
    I2C_Read_NBytes(MPU6886_ADDRESS, MPU6886_PWR_MGMT_1, 1, &regdata);
    regdata = regdata & 0b10001111; // set cyle, sleep, and gyro to standby, i.e. 0
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_PWR_MGMT_1, 1, &regdata);

    regdata = 0b00000111; // set accel x, y, and z to standby 
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_PWR_MGMT_2, 1, &regdata);

    /* Step 2: Set Accelerometer LPF bandwidth to 218.1 Hz
        • In ACCEL_CONFIG2 register (0x1D) set ACCEL_FCHOICE_B = 0 and A_DLPF_CFG[2:0] = 1 (b001)
    */
    I2C_Read_NBytes(MPU6886_ADDRESS, MPU6886_ACCEL_CONFIG2, 1, &regdata);
    regdata = 0b00100001; // average 32 samples, use 218 Hz DLPF
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_ACCEL_CONFIG2, 1, &regdata);

    /* Step 2.5 - active low? */
    I2C_Read_NBytes(MPU6886_ADDRESS, MPU6886_INT_PIN_CFG, 1, &regdata);
    regdata =  ((regdata | 0b10000000) & 0b11011111); // configure pin active-low, no latch
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_INT_PIN_CFG, 1, &regdata);

    /* Step 3: Enable Motion Interrupt
        • In INT_ENABLE register (0x38) set WOM_INT_EN = 111 to enable motion interrupt
    */
    regdata = 0b11100000; // enable wake-on-motion interrupt for X, Y, and Z axes
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_INT_ENABLE, 1, &regdata);
    
    /* Step 4: Set Motion Threshold
        • Set the motion threshold in ACCEL_WOM_THR register (0x1F)
        NOTE: the data sheet mentions 0x1F, but is probably referring to
              registers 0x20, 0x21, and 0x22 based on empirical tests
    */
    regdata = thresh_num_lsb; // set accel motion threshold for X, Y, and Z axes
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_ACCEL_WOM_X_THR, 1, &regdata);
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_ACCEL_WOM_Y_THR, 1, &regdata);
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_ACCEL_WOM_Z_THR, 1, &regdata);

    /* Step 5: Enable Accelerometer Hardware Intelligence
        • In ACCEL_INTEL_CTRL register (0x69) set ACCEL_INTEL_EN = ACCEL_INTEL_MODE = 1;
          Ensure that bit 0 is set to 0
    */
    regdata = 0b11000010; // enable wake-on-motion if any of X, Y, or Z axes is above threshold
    // WOM_STEP5_ACCEL_INTEL_CTRL_INTEL_EN_1_MODE_1_WOM_TH_MODE_0;
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_ACCEL_INTEL_CTRL, 1, &regdata);

    /* Step 7: Set Frequency of Wake-Up
        • In SMPLRT_DIV register (0x19) set SMPLRT_DIV[7:0] = 3.9 Hz – 500 Hz
    */
    // sample_rate = 1e3 / (1 + regdata)
    //   4.0 Hz = 1e3 / (1 + 249)
    //  10.0 Hz = 1e3 / (1 +  99)
    //  20.0 Hz = 1e3 / (1 +  49)
    //  25.0 Hz = 1e3 / (1 +  39)
    //  50.0 Hz = 1e3 / (1 +  19) <----
    // 500.0 Hz = 1e3 / (1 +   1)
    regdata = 19;
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_SMPLRT_DIV, 1, &regdata);

    /* Step 8: Enable Cycle Mode (Accelerometer Low-Power Mode)
        • In PWR_MGMT_1 register (0x6B) set CYCLE = 1
    */
    I2C_Read_NBytes(MPU6886_ADDRESS, MPU6886_PWR_MGMT_1, 1, &regdata);
    regdata = regdata | 0b00100000; // enable accelerometer low-power mode
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_PWR_MGMT_1, 1, &regdata);
}

void MPU6886::getAccel(float* ax, float* ay, float* az) {
  float aRes = 8.0 / 32768.0;
  *ax = (int16_t)((readByte(0x3b) << 8) | readByte(0x3c)) * aRes;
  *ay = (int16_t)((readByte(0x3d) << 8) | readByte(0x3e)) * aRes;
  *az = (int16_t)((readByte(0x3f) << 8) | readByte(0x40)) * aRes;
}

void MPU6886::getGyro(float* gx, float* gy, float* gz) {
  float gRes = 2000.0 / 32768.0;
  *gx = (int16_t)((readByte(0x43) << 8) | readByte(0x44)) * gRes;
  *gy = (int16_t)((readByte(0x45) << 8) | readByte(0x46)) * gRes;
  *gz = (int16_t)((readByte(0x47) << 8) | readByte(0x48)) * gRes;
}

void MPU6886::getTemp(float *t) {
  *t = 25.0 + ((readByte(0x41) << 8) | readByte(0x42)) / 326.8;
}

void MPU6886::getAccelAdc(int16_t* ax, int16_t* ay, int16_t* az){

   uint8_t buf[6];  
   I2C_Read_NBytes(MPU6886_ADDRESS,MPU6886_ACCEL_XOUT_H,6,buf);
   
   *ax=((int16_t)buf[0]<<8)|buf[1];
   *ay=((int16_t)buf[2]<<8)|buf[3];
   *az=((int16_t)buf[4]<<8)|buf[5];

}

void MPU6886::getGyroAdc(int16_t* gx, int16_t* gy, int16_t* gz){

  uint8_t buf[6];
  I2C_Read_NBytes(MPU6886_ADDRESS,MPU6886_GYRO_XOUT_H,6,buf);
  
  *gx=((uint16_t)buf[0]<<8)|buf[1];  
  *gy=((uint16_t)buf[2]<<8)|buf[3];  
  *gz=((uint16_t)buf[4]<<8)|buf[5];
  
}

void MPU6886::getTempAdc(int16_t *t){
  
  uint8_t buf[2];  
  I2C_Read_NBytes(MPU6886_ADDRESS,MPU6886_TEMP_OUT_H,2,buf);
  
  *t=((uint16_t)buf[0]<<8)|buf[1];  
}

void MPU6886::getAhrsData(float *pitch,float *roll,float *yaw){

  float accX = 0; 
  float accY = 0;
  float accZ = 0;

  float gyroX = 0;
  float gyroY = 0;
  float gyroZ = 0;


  getGyroData(&gyroX,&gyroY,&gyroZ);
  getAccelData(&accX,&accY,&accZ);
  
  MahonyAHRSupdateIMU(gyroX * DEG_TO_RAD, gyroY * DEG_TO_RAD, gyroZ * DEG_TO_RAD, accX, accY, accZ,pitch,roll,yaw);

}

void MPU6886::getGres(){

   switch (Gyscale)
   {
  // Possible gyro scales (and their register bit settings) are:
     case GFS_250DPS:
           gRes = 250.0/32768.0;
           break;
     case GFS_500DPS:
           gRes = 500.0/32768.0;
           break;
     case GFS_1000DPS:
           gRes = 1000.0/32768.0;
           break;
     case GFS_2000DPS:
           gRes = 2000.0/32768.0;
           break;
   }

}

void MPU6886::getAres(){
  switch (Acscale)
  {
  // Possible accelerometer scales (and their register bit settings) are:
  // 2 Gs (00), 4 Gs (01), 8 Gs (10), and 16 Gs  (11). 
  // Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit value:
  case AFS_2G:
        aRes = 2.0/32768.0;
        break;
  case AFS_4G:
        aRes = 4.0/32768.0;
        break;
  case AFS_8G:
        aRes = 8.0/32768.0;
        break;
  case AFS_16G:
        aRes = 16.0/32768.0;
        break;
}

}

void MPU6886::SetGyroFsr(Gscale scale)
{
    //return IIC_Write_Byte(MPU_GYRO_CFG_REG,scale<<3);//设置陀螺仪满量程范围
    unsigned char regdata;	
    regdata = (scale<<3);
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_GYRO_CONFIG, 1, &regdata);
    delay(10);

    Gyscale = scale;
    getGres();
}

void MPU6886::SetAccelFsr(Ascale scale)
{
    unsigned char regdata;	
    regdata = (scale<<3);
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_ACCEL_CONFIG, 1, &regdata);
    delay(10);

    Acscale = scale;
    getAres();
}


void MPU6886::getAccelData(float* ax, float* ay, float* az){


  int16_t accX = 0;
  int16_t accY = 0;
  int16_t accZ = 0;
  getAccelAdc(&accX,&accY,&accZ);


  *ax = (float)accX * aRes;
  *ay = (float)accY * aRes;
  *az = (float)accZ * aRes;

}
      
void MPU6886::getGyroData(float* gx, float* gy, float* gz){
  int16_t gyroX = 0;
  int16_t gyroY = 0;
  int16_t gyroZ = 0;
  getGyroAdc(&gyroX,&gyroY,&gyroZ);

  *gx = (float)gyroX * gRes;
  *gy = (float)gyroY * gRes;
  *gz = (float)gyroZ * gRes;
}

void MPU6886::getTempData(float *t){
  
  int16_t temp = 0;
  getTempAdc(&temp);
  
  *t = (float)temp / 326.8 + 25.0;
}

void MPU6886::SetINTPinActiveLogic(uint8_t level) {
  uint8_t tempdata;
  I2C_Read_NBytes(MPU6886_ADDRESS, MPU6886_INT_PIN_CFG, 1, &tempdata);
  tempdata &= 0x7f;
  tempdata |= level ? 0x00 : (0x01 << 7);
  I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_INT_PIN_CFG, 1, &tempdata);
}

void MPU6886::DisableAllIRQ() {
  uint8_t tempdata = 0x00;
  I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_INT_ENABLE, 1, &tempdata);
  I2C_Read_NBytes(MPU6886_ADDRESS, MPU6886_INT_PIN_CFG, 1, &tempdata);
  tempdata |= 0x01 << 6;
  // int pin is configured as open drain
  I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_INT_PIN_CFG, 1, &tempdata);
}

void MPU6886::ClearAllIRQ() {
  uint8_t tempdata = 0x00;
  I2C_Read_NBytes(MPU6886_ADDRESS, MPU6886_FIFO_WM_INT_STATUS, 1, &tempdata);
  I2C_Read_NBytes(MPU6886_ADDRESS, MPU6886_INT_STATUS, 1, &tempdata);
}