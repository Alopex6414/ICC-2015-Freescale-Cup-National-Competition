/**********************************************************
Copyright (B), 2014-2015, TYUT JBD TEAM B.
File name: Func.c 
Author: TEAM B
Version: 1.0              Date: 2015.4.2
Description: 
Others:
Function List:
History: Helium				2015.4.2
***********************************************************/
#include "Declare.h"
/***********************�ٶȼ���******************************/
unsigned int SpeedCounter1=0;
unsigned int SpeedCounter2=0;
unsigned int SpeedCounter3=0;
unsigned int SpeedCounter4=0;

float gfLast_Speed_Left=0;
float gfLast_Speed_Right=0;
float gfLast_Speed=0;//�ϴ��ٶ�
float gfLast_Last_Speed=0;//ǰ���ٶ�

int DTCount=0;
int DTdegree=0;
int DTback=0;
int DTflag=2;

int DTCount_2=0;
int DTdegree_2=0;
int DTback_2=0;
int DTflag_2=2;

int monkey;
int pig;
int butterfly=1;

int tyut=0;
int giHindFlagCount=0;
/***********************���ڷ��ͺ�����ʱ************
*  �������ƣ�DelaySci
*  ����˵�������ڷ��ͺ�����ʱ
*  ����˵����
*  �������أ���
*  �޸�ʱ�䣺2015-1-8
*  ��    ע:
*******************************************************/
void DelaySci(int ms)
{   
   int ii,jj;
   if (ms<1) ms=1;
   for(ii=0;ii<ms;ii++)
     for(jj=0;jj<1400;jj++){}        
}

/***********************���ڷ��ͺ���*****************
*  �������ƣ�SciTX
*  ����˵�������ڷ��ͺ���
*  ����˵����
*  �������أ���
*  �޸�ʱ�䣺2015-1-8
*  ��    ע:
*******************************************************/
void SciTX(unsigned char ch)
{        
     DelaySci(1);
     	
     while(LINFLEX_0.UARTSR.B.DTF==1)
     {
     	LINFLEX_0.UARTSR.B.DTF =1;
     }
     
     	LINFLEX_0.BDRL.B.DATA0 =ch;
}

/***********************���ڷ����������ߺ���*****************
*  �������ƣ�Parameter_sent
*  ����˵�������ڷ����������ߺ���
*  ����˵����
*  �������أ���
*  �޸�ʱ�䣺2015-1-8
*  ��    ע:
*******************************************************/
void Parameter_sent(void)
{
  signed int temp;
  unsigned char temp1, temp2;
  SciTX(0xff);
  SciTX(0xff);

  temp = (signed int) gfDirection_AngularVelocity;//gfAngleAD;//gfAngleAD;//gfSendRed1;//White//
  temp1=(temp)>>8;
  temp2=(temp)&0x00ff;
  SciTX (temp1);
  SciTX(temp2);

  temp = (signed int) gfAngleSpeedAD;//giGyro_Y;//gfSendGreen1;//Red
  temp1=(temp)>>8;
  temp2=(temp)&0x00ff;
  SciTX(temp1);
  SciTX(temp2);
  
  temp = (signed int) gfSendGreen1;//giGyro_Z;//gfAngleAD;//Green
  temp1=(temp)>>8;
  temp2=(temp)&0x00ff;
  SciTX(temp1);
  SciTX(temp2);


  /*temp = (signed int)gfAngleSpeedAD;//gfDirection_AngularVelocity//gfAngleSpeedAD
  temp1=(temp)>>8;
  temp2=(temp)&0x00ff;
  SciTX(temp1);
  SciTX(temp2);*/

  SciTX(6);
}

/***********************���ڷ��ͺ�����ʱ************
*  �������ƣ�Delay
*  ����˵�������ڷ��ͺ�����ʱ
*  ����˵����
*  �������أ���
*  �޸�ʱ�䣺2015-1-8
*  ��    ע:	��ʱ1ms
*******************************************************/
void Delay(int ms) 
{
  int ii,jj;
  if (ms<1) ms=1;
  for(ii=0;ii<ms;ii++)
  for(jj=0;jj<2000;jj++);    //40MHz--1ms      
} 

/******************�ٶȼ��㺯��*********************
*  �������ƣ�SpeedCalculate
*  ����˵�������������ٶ�
*  ����˵����
*  �������أ���
*  �޸�ʱ�䣺2015-1-24
*  ��    ע:
*******************************************************/
void SpeedCalculate (void)
{
	SpeedCounter1=EMIOS_0.CH[0].CCNTR.R;
	
	if(SpeedCounter1<SpeedCounter2)
	{
		gfCurrent_Speed_Left=SpeedCounter1-SpeedCounter2+65536;
	}
	
	else 
	{
	    gfCurrent_Speed_Left=SpeedCounter1-SpeedCounter2;
	}
	
	/*if(SpeL)
	{
		gfCurrent_Speed_Left=-gfCurrent_Speed_Left;
	}
	
	else
	{
		gfCurrent_Speed_Left=gfCurrent_Speed_Left;
	}*/
	
	gfLast_Speed_Left=gfCurrent_Speed_Left;
	
	SpeedCounter2=SpeedCounter1;
	
	SpeedCounter3=EMIOS_0.CH[16].CCNTR.R;
	
	if(SpeedCounter3<SpeedCounter4)
	{
		gfCurrent_Speed_Right=SpeedCounter3-SpeedCounter4+65536;
	}
	
	else 
	{
	    gfCurrent_Speed_Right=SpeedCounter3-SpeedCounter4;
	}
	
	/*if(SpeR)
	{
		gfCurrent_Speed_Right=-gfCurrent_Speed_Right;
	}
	
	else
	{
		gfCurrent_Speed_Right=gfCurrent_Speed_Right;
	}*/
	
	gfLast_Speed_Right=gfCurrent_Speed_Right;
	
	SpeedCounter4=SpeedCounter3;
	
	gfCurrent_Speed=gfCurrent_Speed_Right+gfCurrent_Speed_Left;
	           
	gfCurrent_Speed=gfLast_Last_Speed*0.1+gfLast_Speed*0.3+gfCurrent_Speed*0.6;
		      
	gfLast_Last_Speed=gfLast_Speed;	      
		      
	gfLast_Speed=gfCurrent_Speed;
		       
	AngleErrCalculate(); 
}

/******************����ֵ����*********************
*  �������ƣ�ABSCalculate
*  ����˵��������һ�����ľ���ֵ
*  ����˵����
*  �������أ���
*  �޸�ʱ�䣺2015-1-24
*  ��    ע:
*******************************************************/
float ABSCalculate(float Number)
{
	float Temp;
	if(Number>=0)
	{
		Temp=Number;
	}
	
	else if(Number<0)
	{
		Temp=-Number;
	}
	return Temp;
}

/*************************************************
Function: ValueSqrt
Description: ƽ��������
Calls:   ��
Called By: fun.c
Table Accessed: ��
Table Updated: ��
Input: x
Output: ans
Return: ��
Others:��
*************************************************/
 int ValueSqrt(int x)
{
	unsigned char ans = 0,p = 0x80;
	while (p != 0)
	{
		ans+=p;
		if (ans * ans > x)
		{
			ans -= p;
		}
		p = p / 2;
	}
	return (ans);
}

/*************************************************
Function: Findhind

Description: �ϰ���⺯��/�����ڿ�
Calls: ParameterSent 
Called By: PIT0_IRQHandler
Output: ��
Return: ��
Others: ������
*************************************************/
int hind=0,hind1=0,out0=0,out1=0,outs=0,iFlgLH=0,iFlgRH=0;
int BlackBlocFlagL=0;
int BlackBlocFlagR=0;
int gihindEdgeL=0;//�ϰ������
int gihindEdgeR=0;//�ϰ��ұ���
int gihindFlagL=0;//��׼���
int gihindFlagR=0;//�Ұ׼���
int giPreObstacleL=0;//���ϰ�����ж�ʱ�����߽�
int giPreObstacleR=0;//���ϰ�����ж�ʱ����ұ߽�
int giPreObstacleCenter=0;//�ϰ��ж����ʱ������ĵ�
int giPreHindWidth=0;//�ϰ�ǰ��������ȼ�¼
int giSpeedCount=0;//�����ٶȸ�����ʱ����
int giHindAngle=0;
int giHINDFLAG=0;


void FindHind(void)
{
  int Left=0;
  int Right=0;
  int i=0;
  
  
  if(SingleLine==0&&TurnLeft==0&&TurnRight==0)
  {
  	Left=giRoadSit[0]-2*HalfWidth;
  	Right=giRoadSit[0]+2*HalfWidth;
  	
  	if(Left<0)
  	{
  		Left=0;
  	}
  	
  	if(Right>127)
  	{
  		Right=127;
  	}
  	
  	/****************�����߷ֱ����½���********************/
  	for (i = Right-4; i >= Left; i--)
    {
       if ((CCD_M[i + 3] - CCD_M[i]) > EYELight && (CCD_M[i + 4] - CCD_M[i + 1]) > EYELight)
       {
          gihindEdgeR = i;
          break;
       }
               
       else
       {
          gihindEdgeR = 0;
       }
    }
    
    for (i = Left+4; i <= Right; i++)
    {
       if ((CCD_M[i - 3] - CCD_M[i]) > EYELight && (CCD_M[i - 4] - CCD_M[i - 1]) > EYELight)
       {
          gihindEdgeL = i;
          break;
       }
       
       else
       {
          gihindEdgeL = 0;
       }
    }
     
    if(gihindEdgeL<0)
    {
    	gihindEdgeL=0;
    }
    
    if(gihindEdgeR>127)
    {
    	gihindEdgeR=127;
    }
    
    if(gihindEdgeL>gihindEdgeR)
    {
    	gihindEdgeL=0;
    	gihindEdgeR=0;
    }
    
    /*************�ж��Ƿ����ϰ�********************/
    if(gihindEdgeL!=0&&gihindEdgeR!=0&&gihindEdgeL<=gihindEdgeR)
    {
    	//�жϽ�����ϰ�
    	if((gihindEdgeR-gihindEdgeL)>10&&(gihindEdgeR-gihindEdgeL)<=30)
    	{
    		if(gihindEdgeR<=giRoadEdgeL[0]&&gihindEdgeL<=giRoadEdgeL[0]
    			&&gihindEdgeR>=giRoadEdgeL[5]&&gihindEdgeL>=giRoadEdgeL[5]
    				&&giRoadEdgeL[5]!=0&&giRoadEdgeR[5]!=127&&gihindFlagL==0)//���ϰ�
    		{
    			hind=1111;//���ϰ�
    		}
    		
    		if(gihindEdgeR>=giRoadEdgeR[0]&&gihindEdgeL>=giRoadEdgeR[0]
    			&&gihindEdgeR<=giRoadEdgeR[5]&&gihindEdgeL<=giRoadEdgeR[5]
    				&&giRoadEdgeL[5]!=0&&giRoadEdgeR[5]!=127&&gihindFlagR==0)//���ϰ�
    		{
    			hind=2222;//���ϰ�
    		}
    		
    	}
    }
    
    else if(gihindEdgeL==0&&gihindEdgeR!=0&&gihindEdgeR<=giRoadEdgeR[0]-HalfWidth
    &&gihindEdgeR>=giRoadEdgeR[0]-2*HalfWidth)
    {
    	if((gihindEdgeR-giRoadEdgeL[5])>10&&(gihindEdgeR-giRoadEdgeL[5])<30)
    	{
    		if(gihindEdgeR<=giRoadEdgeL[0]&&gihindEdgeR>=giRoadEdgeL[5]
    			&&giRoadEdgeL[5]!=0&&giRoadEdgeR[5]!=127&&gihindFlagL==0)//���ϰ�
    		{
    			hind=1111;
    		}
    	}
	
    }
    
    else if(gihindEdgeL!=0&&gihindEdgeR==0&&gihindEdgeL>=giRoadEdgeL[0]+HalfWidth
    &&gihindEdgeL<=giRoadEdgeL[0]+2*HalfWidth)
    {
    	if((giRoadEdgeR[5]-gihindEdgeL)>10&&(giRoadEdgeR[5]-gihindEdgeL)<30)
    	{
    		if(gihindEdgeL>=giRoadEdgeR[0]&&gihindEdgeL<=giRoadEdgeR[5]
    			&&giRoadEdgeL[5]!=0&&giRoadEdgeR[5]!=127&&gihindFlagR==0)//���ϰ�
    		{
    			hind=2222;
    		}
    	}
	
    }
    
    else 
    {
    	hind=0;
    }
    
    if(hind==1111)
    {
       iFlgLH++;
       if(iFlgLH>=3)
       {
          iFlgLH=3;
          hind1=3333;
          giPreObstacleR=giRoadEdgeR[0];//��¼���߽�
          giPreObstacleCenter=giRoadSit[0];//��¼���ĵ�
          giHindAngle=gfAngle;//��¼��ǰ�ĽǶ�ֵ
          //��ʱ����
          if(gfCurrent_Speed>80)
          {
          	if(gfAngle>=8)
          	{
          		giSpeedCount=(5300/(int)(gfCurrent_Speed));
          	}
          	
          	else if(gfAngle>0&&gfAngle<8)
          	{
          		giSpeedCount=((5300+(8-gfAngle)*2.6*130)/(int)(gfCurrent_Speed));//������
          	}
          	
          }
          
          else
          {
          	giSpeedCount=53;
          }
          
          //��¼������ȸ���ƫ����
          if(giRoadEdgeL[5]!=0&&giRoadEdgeR[5]!=127)//��¼�ϰ�ǰ���������
          {
          	giPreHindWidth=(giRoadEdgeR[5]-giRoadEdgeL[5]);
          }
          else
          {
          	giPreHindWidth=HalfWidth*2;
          }
          
       }
    }
    
   if(hind==2222)
    {
       iFlgRH++;
       if(iFlgRH>=3)
       {
          iFlgRH=3;
          hind1=4444;
       	  giPreObstacleL=giRoadEdgeL[0];//��¼���߽�
       	  giPreObstacleCenter=giRoadSit[0];//��¼���ĵ�
       	  giHindAngle=gfAngle;//��¼��ǰ�ĽǶ�ֵ
       	  //��ʱ����
          if(gfCurrent_Speed>80)
          {
          	if(gfAngle>=8)
          	{
          		giSpeedCount=(5300/(int)(gfCurrent_Speed));
          	}
          	
          	else if(gfAngle>0&&gfAngle<8)
          	{
          		giSpeedCount=((5300+(8-gfAngle)*2.6*130)/(int)(gfCurrent_Speed));//������
          	}
          }
          
          else
          {
          	giSpeedCount=53;
          }
       	  
       	  //�������ƫ��������
       	  if(giRoadEdgeL[5]!=0&&giRoadEdgeR[5]!=127)//��¼�ϰ�ǰ���������
          {
          	giPreHindWidth=(giRoadEdgeR[5]-giRoadEdgeL[5]);
          }
          else
          {
          	giPreHindWidth=HalfWidth*2;
          }
       }
    }
    
    //������������
    if (hind1==3333) ///////////�����·��              
    {
    	giHINDFLAG=1;
    	SIU.GPDO[18].R=1;//������   
       giRoadSit[0]=giRoadSit[1]+2;//giRoadEdgeR[0]-12
        
	if(giRoadSit[0]<=(giPreObstacleR-giPreHindWidth*0.18))//giPreHindWidth*0.18
	{
	       	giRoadSit[0]=giRoadSit[0];
	}
	       
	else
	{
	      	giRoadSit[0]=giPreObstacleR-giPreHindWidth*0.18;//giPreHindWidth*0.18
	}

       gihindFlagL=1; 
       out0++;
       if(out0>giSpeedCount)//3500//(4000/gfCurrent_Speed)
       {
          hind=0;
          hind1=0;
          gihindFlagL=0;
	  gihindFlagR=0;
          out0=0;
          iFlgLH=0;
          iFlgRH=0;
	  giPreObstacleR=0;
	  giPreObstacleCenter=0;
	  giHindFlagCount++;
	  giHindAngle=0;
	  SIU.GPDO[18].R=0;//������
       }                  
    }
    else if (hind1==4444)////////�ұ���·��
    {               
       giRoadSit[0]=giRoadSit[1]-2;
       SIU.GPDO[18].R=1;//������
       
       giHINDFLAG=2;
	if(giRoadSit[0]>=(giPreObstacleL+giPreHindWidth*0.18))//12//0.18����//giPreHindWidth*0.18
	{
	      	giRoadSit[0]=giRoadSit[0];
	}
	       
	else
	{
	       	giRoadSit[0]=giPreObstacleL+giPreHindWidth*0.18;//giPreHindWidth*0.18
	}

       
       gihindFlagR=1;
       out1++;
       if (out1>giSpeedCount)//3500//(4000/gfCurrent_Speed)
       { 
          hind=0;
          hind1=0;
          gihindFlagL=0;
	  gihindFlagR=0;
          out1=0;
          iFlgLH=0;
          iFlgRH=0;
          giPreObstacleL=0;
          giPreObstacleCenter=0;
          giHindFlagCount++;
          giHindAngle=0;
          SIU.GPDO[18].R=0;//������
       }                
    }
    
    else 
    {
          giExpectPlaceUpdate=(giRoadEdgeR[0]+giRoadEdgeL[0])/2;
          giRoadSit[0]=(giRoadEdgeR[0]+giRoadEdgeL[0])/2;
    }
  }
}


/*************************************************
Function: SendHex
Description: ��һ��8 λ����ASCII ����ʽͨ������Ӳ���޹�
Calls: isr.c
Called By: 
Output: ��
Return: ��
Others: ��
*************************************************/
 //SendHex ��һ��8 λ����ASCII ����ʽͨ������Ӳ���޹ء�
    void SendHex(unsigned char data)
    {
       unsigned char temp;
       temp = data >> 4;
       if (temp >= 10)
          {
               SciTX(temp-10+'A');//uart_putchar(UART4,temp-10+'A');//gfSendOne=temp-10+'A';//PutChar(temp-10+'A');
           }
           else
             {
                 SciTX(temp+'0');//uart_putchar(UART4,temp+'0');// gfSendOne=temp+'0';//PutChar(temp+'0');
              }
       temp = data & 0x0F;
       if (temp >= 10)
          {  
              SciTX(temp-10+'A');//uart_putchar(UART4,temp-10+'A');//gfSendOne=temp-10+'A'; //PutChar(temp-10+'A');
           }
           else
              {
                   SciTX(temp+'0');//uart_putchar(UART4,temp+'0');//gfSendOne=temp+'0';  //PutChar(temp+'0');
               }
      }

/*************************************************
Function: SCI_SendData
Description: ��PC ����һ֡���ݣ���Ӳ���޹ء�
Calls: isr.c
Called By: 
Output: ��
Return: ��
Others: ��
*************************************************/
//SCI_SendData ��PC ����һ֡���ݣ���Ӳ���޹ء�
unsigned char SCI_SendData(unsigned char *data)
{
  int len;
  unsigned char lrc=0;
 SciTX('*');// uart_putchar(UART4,'*');//gfSendOne='*';//PutChar('*'); // ����֡ͷ��һ���ֽ�
  len = (int)(data[0]<<8) | (int)(data[1]) ;
  data += 2; // ����ָ��
 SciTX('L');// uart_putchar(UART4,'L');//gfSendOne='L';//PutChar('L'); // ����֡���ͣ��������ֽ�
 SciTX('D'); //uart_putchar(UART4,'D');// gfSendOne='D';//PutChar('D');
  while(len--) // �������ݵ�ASCII �룬�������ֽں�CCD ����
  {
     SendHex(*data);
     lrc += *data++;
  }
  lrc = 0-lrc; // ����CRC������Ϊ����ֵ
  SendHex(lrc); // ����CRC У��ASCII
  SciTX('#');//uart_putchar(UART4,'#');//gfSendOne='#';//PutChar('#'); // ����֡β��һ���ֽ�
}

void lanzhou (void)
{   
	unsigned char SciBuf[200];// ��������
	int i=0;
	SciBuf[0] = 0; // ���ȸ��ֽڣ�������Ϊ0
	SciBuf[1] = 132;// 132
	SciBuf[2] = 0; // �����ֽڣ�����Ϊ0
	SciBuf[3] = 0; // �����ֽڣ�����Ϊ0
	SciBuf[4] = 0; // �����ֽڣ�����Ϊ0
	SciBuf[5] = 0; // �����ֽڣ�����Ϊ0
	for(i=0;i<128;i++)
	{
		SciBuf[i+6] =(int)(3*CCD_M[i])>>2;
	}
	SCI_SendData(&SciBuf[0]); 
}

void lanzhou_H (void)
{   
	unsigned char SciBuf[200];// ��������
	int i=0;
	SciBuf[0] = 0; // ���ȸ��ֽڣ�������Ϊ0
	SciBuf[1] = 132;// 132
	SciBuf[2] = 0; // �����ֽڣ�����Ϊ0
	SciBuf[3] = 0; // �����ֽڣ�����Ϊ0
	SciBuf[4] = 0; // �����ֽڣ�����Ϊ0
	SciBuf[5] = 0; // �����ֽڣ�����Ϊ0
	for(i=0;i<128;i++)
	{
		SciBuf[i+6] =(int)(3*CCD_H[i])>>2;
	}
	SCI_SendData(&SciBuf[0]); 
}

/*********************************
name:		io_able
parameter:	none
explain:	none
**********************************/
int red_get1()
{
	int monkey;
	monkey=SIU.GPDI[17].R;	
	return monkey;
}

void red_write1(unsigned char pig)
{
	SIU.GPDO[74].R=pig;
}

int red_get2()
{
	int monkey;
	monkey=SIU.GPDI[42].R;	
	return monkey;
}

void red_write2(unsigned char pig)
{
	SIU.GPDO[75].R=pig;
}

void red_write3(unsigned char pig)
{
	SIU.GPDO[72].R=pig;	
}

/*************************************************
name:		Startlinetest_1
parameter:	none
explain:	none
*************************************************/
int Startlinetest_1(void)      
{     
     DTdegree=1-DTdegree;
     if(DTdegree%2==0)
       {
         if(red_get1()==1)   //�������IO�ڵĸߵ�
           {                
             DTflag=1;              //����
            }
         else
           {
             DTflag=2;              //����
            }
         red_write1(0);       //���IO���õ�
        }
     else
       {
         red_write1(1);       //���IO���ø�
        }
    return DTflag;   
}

/*************************************************
Name:		Startline_1
explain:	�����߼�� ���˺�������Լ2ms ����һ�Σ�
			���IO�ڳ�ʼ��Ϊ�ߡ�IO�������ѹ5V��
			3.3V���ɡ�
Parameter:	none
Return:   0����ֹ��1��������2�������ӣ��٣�3��ͣ����
*************************************************/
int Startline_1(void)      
{    
	if(DTback==0)                   
   	{
     	if(red_get1()==1)    //�������IO�ڵĸߵ�
        {                
          	DTback=1;
          	DTflag=1;//����
        }
    }
   	if(DTback==1)                   
   	{
     	DTdegree=1-DTdegree;
     	if(DTdegree%2==0)
       	{
         	if(red_get1()==1)   //�������IO�ڵĸߵ�
           	{                
             	DTflag=1;              //����
           	}
        	else
        	{
        		DTflag=2;              //����
        	}
        	red_write1(0);       //���IO���õ�
    	}
    	else
    	{
    		red_write1(1);       //���IO���ø�
    	}
    	if(DTflag==2)
    	{
    		DTCount++;
    	}
    	else
    	{
    		DTCount=0;
    	}
   		if(DTCount==50)
   		{
    		DTback=2;
    	}
	}
   	if(DTback==2)                    
   	{
     	red_write1(1);
     	if(red_get1()==1)       //�������IO�ڵĸߵ�
        {                
          	DTflag=1;                 //����
          	DTback=3;
        }
   	}
   	return DTback;   
}


/*************************************************
Name:		Startline_double
explain:	�����߼�� ���˺�������Լ2ms ����һ�Σ�
			���IO�ڳ�ʼ��Ϊ�ߡ�IO�������ѹ5V��
			3.3V���ɡ�
Parameter:	none
Return:   0����ֹ��1��������2�������ӣ��٣�3��ͣ����
*************************************************/
int Startline_double(void)      
{    
	if(DTback==0)                   
   	{
     	if(red_get1()==1 && red_get2()==1)    //�������IO�ڵĸߵ�
        {                
          	DTback=1;
          	DTflag=1;//����
        }
    }
   	if(DTback==1)                   
   	{
     	DTdegree=1-DTdegree;
     	if(DTdegree%2==0)
       	{
         	if(red_get1()==1 && red_get2()==1)   //�������IO�ڵĸߵ�
           	{                
             	DTflag=1;              //����
           	}
        	else
        	{
        		DTflag=2;              //����
        	}
        	red_write1(0);       //���IO���õ�
        	red_write2(0);
    	}
    	else
    	{
    		red_write1(1);       //���IO���ø�
    		red_write2(1);
    	}
    	if(DTflag==2)
    	{
    		DTCount++;
    	}
    	else
    	{
    		DTCount=0;
    	}
   		if(DTCount==50)
   		{
    		DTback=2;
    	}
	}
   	if(DTback==2)                    
   	{
     	red_write1(1);
     	red_write2(1);
     	if(red_get1()==1 && red_get2()==1)       //�������IO�ڵĸߵ�
        {                
          	DTflag=1;                 //����
          	DTback=3;
        }
   	}
   	return DTback;   
}

/*************************************************
Name:		Startline_2
explain:	�����߼�� ���˺�������Լ2ms ����һ�Σ�
			���IO�ڳ�ʼ��Ϊ�ߡ�IO�������ѹ5V��
			3.3V���ɡ�
Parameter:	none
Return:   0����ֹ��1��������2�������ӣ��٣�3��ͣ����
*************************************************/
int Startline_2(void)      
{    
	if(DTback_2==0)                   
   	{
     	if(red_get2()==1)    //�������IO�ڵĸߵ�
        {                
          	DTback_2=1;
          	DTflag_2=1;//����
        }
    }
   	if(DTback_2==1)                   
   	{
     	if(Startlinetest_1()==2)
     	{	
     		DTdegree_2=1-DTdegree_2;
     		if(DTdegree_2%2==0)
       		{
         		if(red_get2()==1 )   //�������IO�ڵĸߵ�
           		{                
             		DTflag_2=1;              //����
           		}
        		else
        		{
        			DTflag_2=2;              //����
        		}
        		red_write2(0);       //���IO���õ�
        		red_write3(0);
    		}
    		else
    		{
    			red_write2(1);       //���IO���ø�
    			red_write3(1);
    		}
    		if(DTflag_2==2)
    		{
    			DTCount_2++;
    		}
    		else
    		{
    			DTCount_2=0;
    		}
   			if(DTCount_2==10)
   			{
    			DTback_2=2;
    		}
    	}	
	}
   	if(DTback_2==2)                    
   	{
     	red_write2(1);
     	red_write3(1);
     	if(red_get2()==1)       //�������IO�ڵĸߵ�
        {                
          	DTflag_2=1;                 //����
          	DTback_2=3;
        }
   	}
   	return DTback_2;   
}

/******************************
name:		final_DT
parameter:	none
explain:	0->��ֹ	1->����		
			2->���	3->ɲ��
*******************************/
int final_DT()
{
	int bus;
	monkey=	Startline_3();
	pig=Startline_double();
	if(monkey==1)
	{
		butterfly=0;
	}
	if(pig==2)
	{
		butterfly=1;
	}
	
	if(butterfly=0)
	{	}
	if(butterfly=1)
	{
		bus=monkey;
	}
	
	return bus;
}