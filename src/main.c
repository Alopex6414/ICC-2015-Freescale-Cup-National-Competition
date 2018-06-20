/**********************************************************
Copyright (B), 2014-2015, TYUT JBD TEAM B.
File name: main.c 
Author: TEAM B
Version: 1.0              Date: 2015.4.2
Description: 
Others:
Function List:
History: Helium				2015.4.2
***********************************************************/
#include "MPC5604C_M07N.h"
#include "Declare.h"

struct znFAT_Init_Args  Init_Args; //��ʼ����������51����Ϊidata
struct FileInfo  fileinfo; //�ļ���Ϣ����51����Ϊidata
struct DateTime  dt; //������ʱ��

uint32_t Pit1Ctr=0;   /* Counter for PIT 1 interrupts */
float gfCurrent_Speed=0;
float gfCurrent_Speed_Right=0;
float gfCurrent_Speed_Left=0;

int i=0;
int n=0;
int ShowUp=0;
int ShowDown=0;
int giStandFlag=0;
int TEXT1=0,TEXT2=0;
int BeTatest=0;
int fat_timer=0;
//********CCD1***************
char gendata_h[128];
char gendata_l[128];
//********CCD2***************
char gendataFar_h[128];
char gendataFar_l[128];

unsigned char buf[540];
unsigned char empty[256];
unsigned char parameter[6];
unsigned char signal[4]={"ABCD"};

void Display_Keyboard();
void SD_input();
void fat_over();
void init_fat();
char a[128]={};
int SDTIME=0;
int timer=0;

int main(void) 
{

  	Init_All();//�˿�ģ���ʼ��
  	keyboard();//���̳�ʼ��
  	Init_interrupt();//�жϳ�ʼ��
  	/*if(SDTIME!=0)
  	{
  		init_fat();//�ļ�ϵͳ��ʼ��
  	}*/
	Delay(10);
 	SIU.GPDO[11].R=1;//��ʼ���������
 	if(SwitchStop==1)
 	{
 		timer=650;
 		dan=1;
 	}
  	for (;;) 
  	{
  		/*if(SDTIME!=0)//SD��֡����Ϊ0ʱ���¼
  		{
  			
		  	if(fat_timer<SDTIME)
		  	{
		  		fat_timer++;
		  		SD_input(gendata_h,gendata_l,gendataFar_h,gendataFar_l);
			}		
  	
  		
	  		if(fat_timer==SDTIME)
			{
				fat_timer=SDTIME;
			  	fat_over();
			  	SIU.GPDO[11].R=0;
			}
			
			if(fat_timer>SDTIME) 
			{
				fat_timer=SDTIME+1;
				SIU.GPDO[11].R=0;
			}
		}*/
		//Parameter_sent();//���ڷ�������
		//lanzhou();
		//lanzhou_H();
		//SIU.GPDO[18].R=1;
		Display_Keyboard();//������ʾ
  	}
}

/*********************************************
*�жϷ���������8ms
*1��3��5��7ms    ֱ������(�ٶȿ���)
*2ms             CCD1 CCD2�ɼ�
*4ms			 SD����¼��ǰ����
*6ms			 ��ȡת��������
*8ms			 �������
*********************************************/
void Pit1ISR(void)
{
	Pit1Ctr++;
	//SIU.GPDO[69].R=0;
	switch(Pit1Ctr)
	{
		case 1:
				//SIU.GPDO[69].R=1;
	  			giStandFlag++;
	  			if(giStandFlag==5)
	  			{
	  				//SIU.GPDO[69].R=1;
	  				SpeedCalculate();
	  				giStandFlag=0;
	  				//SIU.GPDO[69].R=0;
	  			}
	  	                GetAD();
	                        AngleCalculate(); 
	                        AngleControl();
			 	SetMotorVoltage();
			 	//SIU.GPDO[69].R=0;
				break;
		case 2:						//0.8ms
				//SIU.GPDO[69].R=1;
				Get_Image();
				//SIU.GPDO[69].R=0;
				break;
		case 3:	
				//SIU.GPDO[69].R=1;
	  			giStandFlag++;
	  			if(giStandFlag==5)
	  			{
	  				//SIU.GPDO[69].R=1;
	  				SpeedCalculate();
	  				giStandFlag=0;
	  				//SIU.GPDO[69].R=0;
	  			}
	  	                GetAD();
	                        AngleCalculate(); 
	                        AngleControl();
			 	SetMotorVoltage();
			 	//SIU.GPDO[69].R=0;
				//finally_DT();
				break;
		case 4:						//0.1ms
				//SIU.GPDO[69].R=1;
				gun=Startlinetest_1();
				for(n=0;n<128;n++)//CCD1
				{
					gendata_h[n]=(CCD_M[n])>>8; 
				}
				for(n=0;n<128;n++)
				{
					gendata_l[n]=(CCD_M[n]&0xFF);
				}
					
				for(n=0;n<128;n++)//CCD2
				{
					gendataFar_h[n]=(CCD_H[n])>>8;
				}
				for(n=0;n<128;n++)
				{
					gendataFar_l[n]=(CCD_H[n]&0xFF);
				}
	  			GetDirectionAngularVelocity();
				//SIU.GPDO[69].R=0;
				break;
		case 5:			
				//SIU.GPDO[69].R=1;
	  			giStandFlag++;
	  			if(giStandFlag==5)
	  			{
	  				//SIU.GPDO[69].R=1;
	  				SpeedCalculate();
	  				giStandFlag=0;
	  				//SIU.GPDO[69].R=0;
	  			}
		  	         GetAD();
		                AngleCalculate(); 
		                AngleControl();
			 	SetMotorVoltage();
			 	//SIU.GPDO[69].R=0;
				break;
		case 6:
				//SIU.GPDO[69].R=1;
				dan=Startline_2();
	  			Get_Image_H();
	  			//SIU.GPDO[69].R=0;
				break;
		case 7:
				//SIU.GPDO[69].R=1;
	  			giStandFlag++;
	  			if(giStandFlag==5)
	  			{
	  				//SIU.GPDO[69].R=1;
	  				SpeedCalculate();
	  				giStandFlag=0;
	  				//SIU.GPDO[69].R=0;
	  			}
	  	                GetAD();
	                        AngleCalculate(); 
	            		AngleControl();
			 	SetMotorVoltage();
			 	//SIU.GPDO[69].R=0;
				break;
		case 8:
				//SIU.GPDO[69].R=1;
				CalculateCentre();
  				DirectionControl();
				Pit1Ctr=0;
				//SIU.GPDO[69].R=0;
				break;
		default:  
	  			break;
	}
	PIT.CH[1].TFLG.B.TIF = 1;    // MPC56xxP/B/S: CLear PIT 1 flag by writing 1 
}

/***************************************
*������ʾ����ֵ
*Show UP �ϱ��������ʾ
*Show DOWN �±��������ʾ
*****************************************/
void Display_Keyboard(void)
{
	ShowUp=giRoadEdgeL[0];//gfCurrent_Speed_Left//giRoadEdgeL[0]
	ShowDown=giRoadEdgeR[0];//gfCurrent_Speed_Right//giRoadEdgeR[0]
			
	DisplayNum(0xc0,ShowUp/1000);
	DisplayNum(0xc2,ShowUp%1000/100);
	DisplayNum(0xc4,ShowUp%100/10);
	DisplayNum(0xc6,ShowUp%10);
		  
	DisplayNum(0xc8,ShowDown/1000);
        DisplayNum(0xca,ShowDown%1000/100);
	DisplayNum(0xcc,ShowDown%100/10);
	DisplayNum(0xce,ShowDown%10);
}
/*****************************************************************************
name:		init_fat
parameter:	none
explain:	none
�ļ�ϵͳ��ʼ��
*****************************************************************************/
void init_fat()
{
	SIU_init();
   	znFAT_Device_Init(); //�洢�豸��ʼ��
   	znFAT_Select_Device(0,&Init_Args); //ѡ���豸
   	znFAT_Init(); //�ļ�ϵͳ��ʼ��	
   	znFAT_Delete_File("/helium.txt"); 
	//znFAT_Create_Dir("/SD/",&dt);
	znFAT_Create_File(&fileinfo,"/helium.txt",&dt); //�����ļ�
}

/****************************************************************************/
/*�������ļ�ϵͳ��ҵ														*/
/*����������������																*/
/*˵�����ú����ǽ���Ƭ���е��������										*/
/****************************************************************************/                                   
void SD_input(char ary[128],char ary2[128],char ary3[128],char ary4[128])
{  
	znFAT_WriteData(&fileinfo,4,signal);
    for(i=0;i<128;i++) buf[i]=ary[i];
    for(i=0;i<128;i++) buf[i+128]=ary2[i];
    for(i=0;i<128;i++) buf[i+256]=ary3[i];//�߾�ͷ
    for(i=0;i<128;i++) buf[i+384]=ary4[i];//�߾�ͷ
    
    buf[512]=giRoadEdgeL[0];
    buf[513]=giRoadEdgeR[0];
    buf[514]=giRoadEdgeFarL[0];
    buf[515]=giRoadEdgeFarR[0];
    buf[516]=giRoadSit[0];
    buf[517]=giRoadSitFar[0];
    
    buf[518]=((int)(giPreHindWidth))>>8;//�����ٶ�
    buf[519]=((int)(giSpeedCount))>>8;//�����ٶ�
    buf[520]=((int)(gfAngle))>>8;//�ٶȺ�
    buf[521]=((int)(giHINDFLAG))>>8;//�����ٶ�
    buf[522]=((int)(hind))>>8;//�Ƕ�
    buf[523]=((int)(hind1))>>8;//�Ƕ�PWM//gfAnglePWM
    buf[524]=((int)(giPreObstacleL))>>8;//�ٶ�PWM//gfSpeedPWM
    buf[525]=((int)(giPreObstacleR))>>8;//����PWM//gfDirection_PWM_L
    
    buf[526]=((int)(giPreHindWidth))&0xff;
    buf[527]=((int)(giSpeedCount))&0xff;
    buf[528]=((int)(gfAngle))&0xff;
    buf[529]=((int)(giHINDFLAG))&0xff;
    buf[530]=((int)(hind))&0xff;
    buf[531]=((int)(hind1))&0xff;
    buf[532]=((int)(giPreObstacleL))&0xff;
    buf[533]=((int)(giPreObstacleR))&0xff;
     
    znFAT_WriteData(&fileinfo,540,buf); //���ļ�д������
}
/***************************************************************************
name:		fat_over
parameter:	time
explain:	none
�ر��ļ�ϵͳ
***************************************************************************/
void fat_over()
{
		znFAT_Close_File(&fileinfo); //�ر��ļ�
		znFAT_Flush_FS(); //ˢ���ļ�ϵͳ	
}