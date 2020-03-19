#ifndef __SPI_INIT_H
#define __SPI_INIT_H

#include "stm32f4xx.h" 

void SPI2_Init(void);			 //初始化SPI1口
void SPI2_SetSpeed(u8 SpeedSet); //设置SPI1速度   
u8 SPI2_ReadWriteByte(u8 TxData);//SPI1总线读写一个字节

#endif