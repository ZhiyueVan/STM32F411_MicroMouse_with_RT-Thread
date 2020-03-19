#include <icm_20948.h>
#include "stm32f4xx.h"                  // Device header
#include <SPI_init.h>
#include <rtthread.h>

#define delay_ms(time) rt_thread_mdelay(time)

#define IMU_CS_L() GPIO_ResetBits(GPIOB, GPIO_Pin_12)
#define IMU_CS_H() GPIO_SetBits(GPIOB, GPIO_Pin_12)

#define AUX_READ_TIMEOUT 256

void IMU_WriteReg(uint8_t writeAddr, uint8_t writeData)
{
	IMU_CS_L(); //����Ƭѡ
	SPI2_ReadWriteByte(writeAddr);
	SPI2_ReadWriteByte(writeData);
	IMU_CS_H();
}

void IMU_WriteRegs( uint8_t writeAddr, uint8_t *writeData, uint8_t lens )
{
	IMU_CS_L();
	SPI2_ReadWriteByte(writeAddr);
	for (uint8_t i = 0; i < lens; i++) 
	{
		SPI2_ReadWriteByte(writeData[i]);
	}
	IMU_CS_H();
}

uint8_t IMU_ReadReg( uint8_t readAddr )
{
	uint8_t readData;

	IMU_CS_L();
	SPI2_ReadWriteByte((0x80|readAddr));
	readData = SPI2_ReadWriteByte(0x00);
	IMU_CS_H();

	return readData;
}

void IMU_ReadRegs( uint8_t readAddr, uint8_t *readData, uint8_t lens )
{
  IMU_CS_L();
  SPI2_ReadWriteByte((0x80|readAddr));
  for (uint8_t i = 0; i < lens; i++) {
    readData[i] = SPI2_ReadWriteByte(0x00);
  }
  IMU_CS_H();
}

void IMU_AUX_WriteReg( uint8_t slaveAddr, uint8_t writeAddr, uint8_t writeData )
{
  uint8_t  status;
  uint32_t timeout = AUX_READ_TIMEOUT;

  IMU_SwitchUserBank(3);
  IMU_WriteReg(ICM20948_I2C_SLV4_ADDR, slaveAddr >> 1);
  delay_ms(1);
  IMU_WriteReg(ICM20948_I2C_SLV4_REG, writeAddr);
  delay_ms(1);
  IMU_WriteReg(ICM20948_I2C_SLV4_DO, writeData);
  delay_ms(1);
  IMU_WriteReg(ICM20948_I2C_SLV4_CTRL, ICM20948_I2C_SLVx_EN);
  delay_ms(1);
  IMU_SwitchUserBank(0);

  do {
    status = IMU_ReadReg(ICM20948_I2C_MST_STATUS);
    delay_ms(1);
  } while (((status & ICM20948_I2C_SLV4_DONE) == 0) && (timeout--));
}

void IMU_AUX_WriteRegs( uint8_t slaveAddr, uint8_t writeAddr, uint8_t *writeData, uint8_t lens )
{
  uint8_t  status;
  uint32_t timeout = AUX_READ_TIMEOUT;

  IMU_SwitchUserBank(3);
  IMU_WriteReg(ICM20948_I2C_SLV4_ADDR, slaveAddr >> 1);
  delay_ms(1);
  for (uint8_t i = 0; i < lens; i++) {
    IMU_SwitchUserBank(3);
    IMU_WriteReg(ICM20948_I2C_SLV4_REG, writeAddr + i);
    delay_ms(1);
    IMU_WriteReg(ICM20948_I2C_SLV4_DO, writeData[i]);
    delay_ms(1);
    IMU_WriteReg(ICM20948_I2C_SLV4_CTRL, ICM20948_I2C_SLVx_EN);
    delay_ms(1);
    IMU_SwitchUserBank(0);
    do {
      status = IMU_ReadReg(ICM20948_I2C_MST_STATUS);
      delay_ms(1);
    } while (((status & ICM20948_I2C_SLV4_DONE) == 0) && (timeout--));
  }
}

uint8_t IMU_AUX_ReadReg( uint8_t slaveAddr, uint8_t readAddr )
{
  uint8_t status;
  uint8_t readData;
  uint32_t timeout = AUX_READ_TIMEOUT;

  IMU_SwitchUserBank(3);
  IMU_WriteReg(ICM20948_I2C_SLV4_ADDR, (slaveAddr >> 1) | 0x80);
  delay_ms(1);
  IMU_WriteReg(ICM20948_I2C_SLV4_REG, readAddr);
  delay_ms(1);
  IMU_WriteReg(ICM20948_I2C_SLV4_CTRL, ICM20948_I2C_SLVx_EN);
  delay_ms(1);
  IMU_SwitchUserBank(0);

  do {
    status = IMU_ReadReg(ICM20948_I2C_MST_STATUS);
    delay_ms(1);
  } while (((status & ICM20948_I2C_SLV4_DONE) == 0) && (timeout--));

  IMU_SwitchUserBank(3);
  readData = IMU_ReadReg(ICM20948_I2C_SLV4_DI);
  IMU_SwitchUserBank(0);

  return readData;
}

void IMU_AUX_ReadRegs( uint8_t slaveAddr, uint8_t readAddr, uint8_t *readData, uint8_t lens )
{
  uint8_t status;
  uint32_t timeout = AUX_READ_TIMEOUT;

  IMU_SwitchUserBank(3);
  IMU_WriteReg(ICM20948_I2C_SLV4_ADDR, (slaveAddr >> 1) | 0x80);
  delay_ms(1);
  for (uint8_t i = 0; i< lens; i++) {
    IMU_SwitchUserBank(3);
    IMU_WriteReg(ICM20948_I2C_SLV4_REG, readAddr + i);
    delay_ms(1);
    IMU_WriteReg(ICM20948_I2C_SLV4_CTRL, ICM20948_I2C_SLVx_EN);
    delay_ms(1);
    IMU_SwitchUserBank(0);
    do {
      status = IMU_ReadReg(ICM20948_I2C_MST_STATUS);
      delay_ms(1);
    } while (((status & ICM20948_I2C_SLV4_DONE) == 0) && (timeout--));

    IMU_SwitchUserBank(3);
    readData[i] = IMU_ReadReg(ICM20948_I2C_SLV4_DI);
    delay_ms(1);
    IMU_SwitchUserBank(0);
  }
}

void IMU_AUX_SLVx_Config( uint8_t slv, uint8_t slaveAddr, uint8_t readAddr, uint8_t lens )
{
  uint8_t offset = slv << 2;

  IMU_SwitchUserBank(3);
  IMU_WriteReg(ICM20948_I2C_SLV0_ADDR + offset, (slaveAddr >> 1) | 0x80);
  delay_ms(2);
  IMU_WriteReg(ICM20948_I2C_SLV0_REG + offset, readAddr);
  delay_ms(2);
  IMU_WriteReg(ICM20948_I2C_SLV0_CTRL + offset, ICM20948_I2C_SLVx_EN | (lens & 0x0F));
  delay_ms(2);
  IMU_SwitchUserBank(0);
}

#define ICM20948_INIT_REG_LENS 14
#define AK09916_INIT_REG_LENS   3
#define IST8308_INIT_REG_LENS   5

int32_t IMU_Init( void )
{
  int32_t status;
  uint8_t ICM20948_InitData[ICM20948_INIT_REG_LENS][2] = {
    {0x20, ICM20948_USER_CTRL},             /* [0]  USR0, Release AUX I2C               */
    {0x80, ICM20948_PWR_MGMT_1},            /* [1]  USR0, Reset Device                  */
    {0x01, ICM20948_PWR_MGMT_1},            /* [2]  USR0, Clock Source                  */
    {0x30, ICM20948_USER_CTRL},             /* [3]  USR0, Set I2C_MST_EN, I2C_IF_DIS    */
    {0x10, ICM20948_INT_PIN_CFG},           /* [4]  USR0, Set INT_ANYRD_2CLEAR          */
    {0x01, ICM20948_INT_ENABLE},            /* [5]  USR0, Set RAW_RDY_EN                */
    {0x00, ICM20948_PWR_MGMT_2},            /* [6]  USR0, Enable Accel & Gyro           */

    {0x00, ICM20948_GYRO_SMPLRT_DIV},       /* [7]  USR2, Sample Rate Divider           */
    {0x00, ICM20948_GYRO_CONFIG_1},         /* [8]  USR2, default : +-250dps            */
    {0x00, ICM20948_ACCEL_CONFIG},          /* [9]  USR2, default : +-2G                */
    {0x00, ICM20948_ACCEL_CONFIG_2},        /* [10] USR2, default : AccLPS_460Hz        */
    {0x00, ICM20948_TEMP_CONFIG},           /* [11] USR2, DLPF                          */

    {0x07, ICM20948_I2C_MST_CTRL},          /* [12] USR3, Set INT_ANYRD_2CLEAR          */
    {0x80, ICM20948_I2C_MST_DELAY_CTRL},    /* [13] USR3, Delays Shadowing              */
  };

#if defined(__USE_MAGNETOMETER) && defined(__USE_AK09916)
  uint8_t AK09916_InitData[AK09916_INIT_REG_LENS][2] = {
    {0x01, AK09916_CNTL3},                  /* [0]  Reset Device                        */
    {0x00, AK09916_CNTL2},                  /* [1]  Power-down mode                     */
    {0x08, AK09916_CNTL2},                  /* [2]  Continuous measurement mode 4       */
  };

#elif defined(__USE_MAGNETOMETER) && defined(__USE_IST8308)
  uint8_t IST83_InitData[IST8308_INIT_REG_LENS][2] = {
    {0x01, IST8308_CNTL3},                  /* [0] Soft resert                                  */
    {0x20, IST8308_CNTL1},                  /* [1] Noise Suppression Filter : Low               */
    {0x00, IST8308_CNTL4},                  /* [2] Dynamic Range ��500uT, Sensitivity 6.6 LSB/uT */
    {0x1B, IST8308_OSRCNTL},                /* [3] OSR = 8, 200Hz                               */
    {0x0A, IST8308_CNTL2},                  /* [4] Continuous Measurement Mode with ODR 200Hz   */
  };

#endif

  ICM20948_InitData[8][0]  |= (0 << 3) | (3 << 1) | (1 << 0);       // GYRO_FCHOICE = 1,  GYRO_DLPFCFG = 0,  ��2000 dps
  ICM20948_InitData[9][0]  |= (0 << 3) | (1 << 1) | (1 << 0);       // ACCEL_FCHOICE = 1, ACCEL_DLPFCFG = 0, ��4 g
  ICM20948_InitData[11][0] |= (1 << 0);                             // TEMP_DLPCFG = 1

  IMU_SwitchUserBank(0);
  IMU_WriteReg(ICM20948_USER_CTRL, IMU_ReadReg(ICM20948_USER_CTRL) & ~ICM20948_InitData[0][0]); // release aux i2c
  delay_ms(50);
  IMU_WriteReg(ICM20948_InitData[1][1], ICM20948_InitData[1][0]);     // reset device
  delay_ms(100);
  IMU_WriteReg(ICM20948_InitData[2][1], ICM20948_InitData[2][0]);     // set clock source
  delay_ms(2);
  IMU_WriteReg(ICM20948_InitData[3][1], ICM20948_InitData[3][0]);     // set I2C_MST_EN, I2C_IF_DIS
  delay_ms(2);
  IMU_WriteReg(ICM20948_InitData[4][1], ICM20948_InitData[4][0]);     // set INT_ANYRD_2CLEAR
  delay_ms(2);
  IMU_WriteReg(ICM20948_InitData[5][1], ICM20948_InitData[5][0]);     // set RAW_RDY_EN
  delay_ms(2);
  IMU_WriteReg(ICM20948_InitData[6][1], ICM20948_InitData[6][0]);     // enable accel & gyro
  delay_ms(2);

  IMU_SwitchUserBank(2);
  IMU_WriteReg(ICM20948_InitData[7][1], ICM20948_InitData[7][0]);     // set gyro sample rate divider
  delay_ms(2);
  IMU_WriteReg(ICM20948_InitData[8][1], ICM20948_InitData[8][0]);     // set gyro full-scale range, filter
  delay_ms(2);
  IMU_WriteReg(ICM20948_InitData[9][1], ICM20948_InitData[9][0]);     // set accel full-scale range, filter
  delay_ms(2);
  IMU_WriteReg(ICM20948_InitData[10][1], ICM20948_InitData[10][0]);   // set samples average
  delay_ms(2);
  IMU_WriteReg(ICM20948_InitData[11][1], ICM20948_InitData[11][0]);   // set INT_ANYRD_2CLEAR
  delay_ms(2);

  IMU_SwitchUserBank(3);
  IMU_WriteReg(ICM20948_InitData[12][1], ICM20948_InitData[12][0]);   // set temp filter
  delay_ms(2);
  IMU_WriteReg(ICM20948_InitData[13][1], ICM20948_InitData[13][0]);   // delays shadowing
  delay_ms(2);

#if defined(__USE_MAGNETOMETER) && defined(__USE_AK09916)
  IMU_AUX_WriteReg(AK09916_I2C_ADDR, AK09916_InitData[0][1], AK09916_InitData[0][0]);
  delay_ms(100);
  IMU_AUX_WriteReg(AK09916_I2C_ADDR, AK09916_InitData[1][1], AK09916_InitData[1][0]);
  delay_ms(2);
  IMU_AUX_WriteReg(AK09916_I2C_ADDR, AK09916_InitData[2][1], AK09916_InitData[2][0]);
  delay_ms(2);

#elif defined(__USE_MAGNETOMETER) && defined(__USE_IST8308)
  IMU_AUX_WriteReg(IST8308_I2C_ADDR, IST83_InitData[0][1], IST83_InitData[0][0]);
  delay_ms(50);
  IMU_AUX_WriteReg(IST8308_I2C_ADDR, IST83_InitData[1][1], IST83_InitData[1][0]);
  delay_ms(2);
  IMU_AUX_WriteReg(IST8308_I2C_ADDR, IST83_InitData[2][1], IST83_InitData[2][0]);
  delay_ms(2);
  IMU_AUX_WriteReg(IST8308_I2C_ADDR, IST83_InitData[3][1], IST83_InitData[3][0]);
  delay_ms(2);
  IMU_AUX_WriteReg(IST8308_I2C_ADDR, IST83_InitData[4][1], IST83_InitData[4][0]);
  delay_ms(10);

#endif

  /* config aux i2c */
#if defined(__USE_MAGNETOMETER) && defined(__USE_AK09916)
  IMU_AUX_SLVx_Config(0, AK09916_I2C_ADDR, AK09916_ST1, 9);

#elif defined(__USE_MAGNETOMETER) && defined(__USE_IST8308)
  IMU_AUX_SLVx_Config(0, IST8308_I2C_ADDR, IST8308_STAT, 7);

#endif

  delay_ms(20);
  IMU_SwitchUserBank(0);

  status = IMU_DeviceCheck();
  if (status == ERROR) {
    return ERROR;
  }

  SPI2_SetSpeed(SPI_BaudRatePrescaler_2);

  return SUCCESS;
}

void IMU_SwitchUserBank( uint8_t bank )
{
  IMU_WriteReg(ICM20948_REG_BANK_SEL, (bank & 0xCF) << 4);
}

int32_t IMU_DeviceCheck( void )
{
  uint8_t deviceID;

  deviceID = IMU_ReadReg(ICM20948_WHO_AM_I);
//  printf("icm20948 device id = 0x%02X\r\n", deviceID);
  if (deviceID != ICM20948_DeviceID) {
    return ERROR;
  }

#if defined(__USE_MAGNETOMETER) && defined(__USE_AK09916)
  deviceID = IMU_AUX_ReadReg(AK09916_I2C_ADDR, AK09916_WIA);
//  printf("ak09916 device id = 0x%02X\r\n", deviceID);
  if (deviceID != AK09916_DeviceID) {
    return ERROR;
  }

#elif defined(__USE_MAGNETOMETER) && defined(__USE_IST8308)
  deviceID = IMU_AUX_ReadReg(IST8308_I2C_ADDR, IST8308_WAI);
//  printf("ist8308 device id = 0x%02X\r\n", deviceID);
  if (deviceID != IST8308_DEVICD_ID) {
//    return ERROR;
  }

#endif

  return SUCCESS;
}

int32_t IMU_GetRawData( int16_t *raw )
{
#if defined(__USE_MAGNETOMETER) && defined(__USE_AK09916)
  uint8_t buff[23] = {0};
  IMU_ReadRegs(ICM20948_ACCEL_XOUT_H, buff, 23);  /* Acc, Gyr, Mag, Temp */

#elif defined(__USE_MAGNETOMETER) && defined(__USE_IST8308)
  uint8_t buff[21] = {0};
  IMU_ReadRegs(ICM20948_ACCEL_XOUT_H, buff, 21);  /* Acc, Gyr, Mag, Temp */

#else
  uint8_t buff[14] = {0};
  IMU_ReadRegs(ICM20948_ACCEL_XOUT_H, buff, 14);  /* Acc, Gyr, Temp */

#endif

  raw[0] = (int16_t)(buff[12] << 8) | buff[13];   /* ICTemp */
  raw[1] = (int16_t)(buff[6]  << 8) | buff[7];    /* Gyr.X */
  raw[2] = (int16_t)(buff[8]  << 8) | buff[9];    /* Gyr.Y */
  raw[3] = (int16_t)(buff[10] << 8) | buff[11];   /* Gyr.Z */
  raw[4] = (int16_t)(buff[0]  << 8) | buff[1];    /* Acc.X */
  raw[5] = (int16_t)(buff[2]  << 8) | buff[3];    /* Acc.Y */
  raw[6] = (int16_t)(buff[4]  << 8) | buff[5];    /* Acc.Z */

  raw[2] = -raw[2];
  raw[3] = -raw[3];
  raw[4] = -raw[4];
#if defined(__USE_MAGNETOMETER) && defined(__USE_AK09916)
//  if (!(buf[14] & (AK09916_STATUS_DRDY)) || (buf[14] & AK09916_STATUS_DOR) || (buf[21] & AK09916_STATUS_HOFL)) {
//    return ERROR;
//  }
//  if (!(buf[14] & AK09916_STATUS_DRDY)) {
//    return ERROR;
//  }

  raw[7] = (int16_t)(buff[16]  << 8) | buff[15]; /* Mag.X */
  raw[8] = (int16_t)(buff[18]  << 8) | buff[17]; /* Mag.Y */
  raw[9] = (int16_t)(buff[20]  << 8) | buff[19]; /* Mag.Z */

#elif defined(__USE_MAGNETOMETER) && defined(__USE_IST8308)
//  if ((IST83_ReadReg(IST8308_STAT) & 0x01) == 0x01) {
  raw[7] = (int16_t)(buff[16]  << 8) | buff[15]; /* Mag.X */
  raw[8] = (int16_t)(buff[18]  << 8) | buff[17]; /* Mag.Y */
  raw[9] = (int16_t)(buff[20]  << 8) | buff[19]; /* Mag.Z */
//    return SUCCESS;
//  }

#endif

  return SUCCESS;
}

int32_t IMU_GetData( float32_t *data )
{
  int16_t rawData[10] = {0};
  IMU_GetRawData(rawData);

  data[0] = rawData[0] * 0.00299517776f + 21.0f;
  data[1] = rawData[1] * 0.0609756098f;
  data[2] = rawData[2] * 0.0609756098f;
  data[3] = rawData[3] * 0.0609756098f;
  data[4] = rawData[4] * 0.000122070312f;
  data[5] = rawData[5] * 0.000122070312f;
  data[6] = rawData[6] * 0.000122070312f;
  data[7] = rawData[7] * 0.15f;
  data[8] = rawData[8] * 0.15f;
  data[9] = rawData[9] * 0.15f;

  return SUCCESS;
}
