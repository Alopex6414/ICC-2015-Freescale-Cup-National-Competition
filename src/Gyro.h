/**********************************************************
Copyright (B), 2014-2015, TYUT JBD TEAM B.
File name: Gyro.h 
Author: TEAM B
Version: 1.0              Date: 2015.4.2
Description: 
Others:
Function List:
History: Helium				2015.4.2
***********************************************************/
#ifndef _GYRO_H_
#define _GYRO_H_

#define   uchar unsigned char
#define   uint unsigned int	
/**********L3G4200D内部寄存器地址*********/
#define WHO_AM_I 0x0F
#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23
#define CTRL_REG5 0x24
#define REFERENCE 0x25
#define OUT_TEMP 0x26
#define STATUS_REG 0x27
#define OUT_X_L 0x28
#define OUT_X_H 0x29
#define OUT_Y_L 0x2A
#define OUT_Y_H 0x2B
#define OUT_Z_L 0x2C
#define OUT_Z_H 0x2D
#define FIFO_CTRL_REG 0x2E
#define FIFO_SRC_REG 0x2F
#define INT1_CFG 0x30
#define INT1_SRC 0x31
#define INT1_TSH_XH 0x32
#define INT1_TSH_XL 0x33
#define INT1_TSH_YH 0x34
#define INT1_TSH_YL 0x35
#define INT1_TSH_ZH 0x36
#define INT1_TSH_ZL 0x37
#define INT1_DURATION 0x38
#define	SlaveAddress   0xD2	  						//定义器件在IIC总线中的从地址,根据ALT  ADDRESS地址引脚不同修改                        
typedef unsigned char  BYTE;
typedef unsigned short WORD;

/**********************************SCL声明******************************/
#define   SCLout     SIU.PCR[28].R=0x0200
#define   SCL_L_     SIU.GPDO[28].R=0
#define   SCL_H_     SIU.GPDO[28].R=1  
/**********************************SDA声明******************************/
#define   SDAout     SIU.PCR[27].R=0x0200
#define   SDAin      SIU.PCR[27].R=0x0100
#define   SDA_L_     SIU.GPDO[27].R=0
#define   SDA_H_     SIU.GPDO[27].R=1
#define   SDAread  	 SIU.GPDI[27].R 

extern int giGyro_X;												//数字陀螺仪X轴
extern int giGyro_Y;											//数字陀螺仪Y轴
extern int giGyro_Z;


void delay(unsigned int k);
void InitL3G4200D();                							//初始化L3G4200D
void  Single_WriteL3G4200D(uchar REG_Address,uchar REG_data);   //单个写入数据
uchar Single_ReadL3G4200D(uchar REG_Address);                   //单个读取内部寄存器数据

#endif