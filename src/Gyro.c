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

int  BUF[8];                        								//�������ݻ�����      	
long int  dis_data;                       							//����
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
��ʼ�ź�
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
ֹͣ�ź�
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
����Ӧ���ź�
��ڲ���:ack (0:ACK 1:NAK)
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
    SCL_H_;                    //����ʱ����
    delay(1);                 //��ʱ
    SCL_L_;                    //����ʱ����
    //stay();                 //��ʱ
}

/**************************************
����Ӧ���ź�
******************************************/
uchar L3G4200D_RecvACK()
{   
    SDAin;		//����7Ϊ����
    SCLout;
    SCL_H_;
    //stay();
    CY=SDAread;
    SCL_L_;
    delay(1);
}   

/**************************************
��IIC���߷���һ���ֽ�����
****************************************/
void L3G4200D_SendByte(BYTE dat)
{                
    BYTE i;
  	SCLout;
	SDAout;	
    for (i=0; i<8; i++)         //8λ������
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
��IIC���߽���һ���ֽ�����
**************************************/
uchar L3G4200D_RecvByte()
{
    BYTE i;
    BYTE dat=0;
	SCLout;
	SDAout;
	
    SDA_H_;
    SDAin;
    for (i=0; i<8; i++)         //8λ������
    {
        SCL_L_;
        delay(1);
        SCL_H_;
        delay(1);
        dat=dat<<1;
        if(SDAread==1)
        	dat++;     
    }
        delay(1);             //��ʱ
    	return dat;
}
//���ֽ�д��*******************************************

void Single_WriteL3G4200D(uchar REG_Address,uchar REG_data)
{
    L3G4200D_Start();                  //��ʼ�ź�
    L3G4200D_SendByte(SlaveAddress);   //�����豸��ַ+д�ź�    
    L3G4200D_SendByte(REG_Address);    //�ڲ��Ĵ�����ַ����ο�����pdf22ҳ     
    L3G4200D_SendByte(REG_data);       //�ڲ��Ĵ������ݣ���ο�����pdf22ҳ 
    L3G4200D_Stop();                   //����ֹͣ�ź�
}
//���ֽڶ�ȡ*****************************************
uchar Single_ReadL3G4200D(uchar REG_Address)
{  
    uchar REG_data=20;
    L3G4200D_Start();                          //��ʼ�ź�
    L3G4200D_SendByte(SlaveAddress);           //�����豸��ַ+д�ź�
    L3G4200D_SendByte(REG_Address);            //���ʹ洢��Ԫ��ַ����0��ʼ	
    L3G4200D_Stop();  
    L3G4200D_Start();                          //��ʼ�ź�
    L3G4200D_SendByte(SlaveAddress+1);         //�����豸��ַ+���ź�
    REG_data=L3G4200D_RecvByte();              //�����Ĵ�������
    L3G4200D_SendACK(1);   
    L3G4200D_Stop();                           //ֹͣ�ź�
    return REG_data; 
}
//*****��ʼ��L3G4200D��������Ҫ��ο�pdf�����޸�************************
void InitL3G4200D()
{
   Single_WriteL3G4200D(CTRL_REG1, 0x0F);   //
   Single_WriteL3G4200D(CTRL_REG2, 0x00);   //
   Single_WriteL3G4200D(CTRL_REG3, 0x08);   //
   Single_WriteL3G4200D(CTRL_REG4, 0x30);  //+-500dps//0x00+-250dps//0x30+-2000dps
   Single_WriteL3G4200D(CTRL_REG5, 0x00);
}
/************************���ݴ���***************************************/
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
//��ʾx��
int display_x()
{ 
    BUF[0]= Single_ReadL3G4200D(OUT_X_L);
    BUF[1]= Single_ReadL3G4200D(OUT_X_H); //��ȡX������
    dis_data=num_deal(BUF[0],BUF[1]);
    return dis_data;
}
//***********************************************************************
//��ʾy��
int display_y()
{  
    BUF[2]= Single_ReadL3G4200D(OUT_Y_L);
    BUF[3]= Single_ReadL3G4200D(OUT_Y_H);//��ȡY������
    dis_data=num_deal(BUF[2],BUF[3]);
	return dis_data;
}
//***********************************************************************
//��ʾz��
int display_z()
{     
	BUF[4]= Single_ReadL3G4200D(OUT_Z_L);
    BUF[5]= Single_ReadL3G4200D(OUT_Z_H);//��ȡZ������
    dis_data=num_deal(BUF[4],BUF[5]);         
	return dis_data;
}

/**************************************************************************/
void ReadGyro(void)
{
		//giGyro_X=display_x()/10;                   
    	//giGyro_Y=display_y()/10; //ת����ת������ת��                 
    	//giGyro_Z=display_z()/10;                   
}