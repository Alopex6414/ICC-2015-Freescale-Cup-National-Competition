/**********************************************************
Copyright (B), 2014-2015, TYUT JBD TEAM B.
File name: Gyro.c 
Author: TEAM B
Version: 1.0              Date: 2015.4.2
Description: 
Others:
Function List:
History: Helium				2015.4.2
***********************************************************/
#include "Declare.h"

int  BUF[8];                        								//接收数据缓存区      	
long int  dis_data;                       							//变量
int giGyro_X=0;
int giGyro_Y=0;
int giGyro_Z=0;
char ack;
int CY;
float temp;

void delay(unsigned int k)	
{						
	unsigned int i,j;				
	for(i=0;i<k;i++)
	{			
		for(j=0;j<30;j++)			
		{
			;
		}
	}						
}
/************************************/
void stay()
{
	int i;
	for(i=0;i<20;i++)
	{
		
	}
}
/****************************************
起始信号
*****************************************/
void L3G4200D_Start()
{
	   SCLout;
	   SDAout;		
       SDA_H_;
       SCL_H_;
       delay(1);
       SDA_L_;
       //stay();
       SCL_L_;
       delay(1);
}

/**************************************
停止信号
**************************************/
void L3G4200D_Stop()
{
	SCLout;
	SDAout;		
    SCL_L_; 
    SDA_L_;
	//stay();
	SCL_H_;
	delay(1);
	SDA_H_;
	//stay();
}

/**************************************
发送应答信号
入口参数:ack (0:ACK 1:NAK)
**************************************/
void L3G4200D_SendACK(int ack)
{
	SCLout;
	SDAout;	
    SCL_L_;
    
    if(ack==0)
    {
    	SDA_L_;
    }
    else
    {
    	SDA_H_;
    }
    
    //stay();
    SCL_H_;                    //拉高时钟线
    delay(1);                 //延时
    SCL_L_;                    //拉低时钟线
    //stay();                 //延时
}

/**************************************
接收应答信号
******************************************/
uchar L3G4200D_RecvACK()
{   
    SDAin;		//定义7为读入
    SCLout;
    SCL_H_;
    //stay();
    CY=SDAread;
    SCL_L_;
    delay(1);
}   

/**************************************
向IIC总线发送一个字节数据
****************************************/
void L3G4200D_SendByte(BYTE dat)
{                
    BYTE i;
  	SCLout;
	SDAout;	
    for (i=0; i<8; i++)         //8位计数器
    {   
        if((dat<<i)&0x80)
       	{
        	SDA_H_;	
       	}
       	
        else
        {
        	SDA_L_;
        }
        
        delay(1);
        SCL_H_;
        delay(1);
        SCL_L_;
        delay(1);
    }
   
    delay(1);
    SDA_H_;
    delay(1);
    SCL_H_;
    delay(1);
    SDAin;
    if(SDAread==1)
    ack=0;
    else ack=1;
    SCL_L_;
    delay(1); 
}
    
/**************************************
从IIC总线接收一个字节数据
**************************************/
uchar L3G4200D_RecvByte()
{
    BYTE i;
    BYTE dat=0;
	SCLout;
	SDAout;
	
    SDA_H_;
    SDAin;
    for (i=0; i<8; i++)         //8位计数器
    {
        SCL_L_;
        delay(1);
        SCL_H_;
        delay(1);
        dat=dat<<1;
        if(SDAread==1)
        	dat++;     
    }
        delay(1);             //延时
    	return dat;
}
//单字节写入*******************************************

void Single_WriteL3G4200D(uchar REG_Address,uchar REG_data)
{
    L3G4200D_Start();                  //起始信号
    L3G4200D_SendByte(SlaveAddress);   //发送设备地址+写信号    
    L3G4200D_SendByte(REG_Address);    //内部寄存器地址，请参考中文pdf22页     
    L3G4200D_SendByte(REG_data);       //内部寄存器数据，请参考中文pdf22页 
    L3G4200D_Stop();                   //发送停止信号
}
//单字节读取*****************************************
uchar Single_ReadL3G4200D(uchar REG_Address)
{  
    uchar REG_data=20;
    L3G4200D_Start();                          //起始信号
    L3G4200D_SendByte(SlaveAddress);           //发送设备地址+写信号
    L3G4200D_SendByte(REG_Address);            //发送存储单元地址，从0开始	
    L3G4200D_Stop();  
    L3G4200D_Start();                          //起始信号
    L3G4200D_SendByte(SlaveAddress+1);         //发送设备地址+读信号
    REG_data=L3G4200D_RecvByte();              //读出寄存器数据
    L3G4200D_SendACK(1);   
    L3G4200D_Stop();                           //停止信号
    return REG_data; 
}
//*****初始化L3G4200D，根据需要请参考pdf进行修改************************
void InitL3G4200D()
{
   Single_WriteL3G4200D(CTRL_REG1, 0x0F);   //
   Single_WriteL3G4200D(CTRL_REG2, 0x00);   //
   Single_WriteL3G4200D(CTRL_REG3, 0x08);   //
   Single_WriteL3G4200D(CTRL_REG4, 0x30);  //+-500dps//0x00+-250dps//0x30+-2000dps
   Single_WriteL3G4200D(CTRL_REG5, 0x00);
}
/************************数据处理***************************************/
int num_deal(int num1,int num2)
{
	int number;
	num2=num2*0x100;
	number=num2+num1;
	if(number>0x7FFF)
		number=number-0x10000;
	return number;
}
//***********************************************************************
//显示x轴
int display_x()
{ 
    BUF[0]= Single_ReadL3G4200D(OUT_X_L);
    BUF[1]= Single_ReadL3G4200D(OUT_X_H); //读取X轴数据
    dis_data=num_deal(BUF[0],BUF[1]);
    return dis_data;
}
//***********************************************************************
//显示y轴
int display_y()
{  
    BUF[2]= Single_ReadL3G4200D(OUT_Y_L);
    BUF[3]= Single_ReadL3G4200D(OUT_Y_H);//读取Y轴数据
    dis_data=num_deal(BUF[2],BUF[3]);
	return dis_data;
}
//***********************************************************************
//显示z轴
int display_z()
{     
	BUF[4]= Single_ReadL3G4200D(OUT_Z_L);
    BUF[5]= Single_ReadL3G4200D(OUT_Z_H);//读取Z轴数据
    dis_data=num_deal(BUF[4],BUF[5]);         
	return dis_data;
}

/**************************************************************************/
void ReadGyro(void)
{
		//giGyro_X=display_x()/10;                   
    	//giGyro_Y=display_y()/10; //转向，左转正，右转负                 
    	//giGyro_Z=display_z()/10;                   
}