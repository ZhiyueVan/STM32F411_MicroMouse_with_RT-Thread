#ifndef __SPI_INIT_H
#define __SPI_INIT_H

#include "stm32f4xx.h" 

void SPI2_Init(void);			 //��ʼ��SPI1��
void SPI2_SetSpeed(u8 SpeedSet); //����SPI1�ٶ�   
u8 SPI2_ReadWriteByte(u8 TxData);//SPI1���߶�дһ���ֽ�

#endif