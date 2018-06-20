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

struct znFAT_Init_Args  Init_Args; //初始化参数集合51定义为idata
struct FileInfo  fileinfo; //文件信息集合51定义为idata
struct DateTime  dt; //日期与时间

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

  	Init_All();//端口模块初始化
  	keyboard();//键盘初始化
  	Init_interrupt();//中断初始化
  	/*if(SDTIME!=0)
  	{
  		init_fat();//文件系统初始化
  	}*/
	Delay(10);
 	SIU.GPDO[11].R=1;//初始化完成亮灯
 	if(SwitchStop==1)
 	{
 		timer=650;
 		dan=1;
 	}
  	for (;;) 
  	{
  		/*if(SDTIME!=0)//SD卡帧数不为0时候记录
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
		//Parameter_sent();//串口发送曲线
		//lanzhou();
		//lanzhou_H();
		//SIU.GPDO[18].R=1;
		Display_Keyboard();//键盘显示
  	}
}

/*********************************************
*中断服务函数周期8ms
*1，3，5，7ms    直立控制(速度控制)
*2ms             CCD1 CCD2采集
*4ms			 SD卡记录当前数据
*6ms			 读取转向陀螺仪
*8ms			 方向控制
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
*键盘显示变量值
*Show UP 上边数码管显示
*Show DOWN 下边数码管显示
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
文件系统初始化
*****************************************************************************/
void init_fat()
{
	SIU_init();
   	znFAT_Device_Init(); //存储设备初始化
   	znFAT_Select_Device(0,&Init_Args); //选择设备
   	znFAT_Init(); //文件系统初始化	
   	znFAT_Delete_File("/helium.txt"); 
	//znFAT_Create_Dir("/SD/",&dt);
	znFAT_Create_File(&fileinfo,"/helium.txt",&dt); //创建文件
}

/****************************************************************************/
/*函数：文件系统作业														*/
/*参数：待发送数组																*/
/*说明：该函数是将单片机中的数据输出										*/
/****************************************************************************/                                   
void SD_input(char ary[128],char ary2[128],char ary3[128],char ary4[128])
{  
	znFAT_WriteData(&fileinfo,4,signal);
    for(i=0;i<128;i++) buf[i]=ary[i];
    for(i=0;i<128;i++) buf[i+128]=ary2[i];
    for(i=0;i<128;i++) buf[i+256]=ary3[i];//高镜头
    for(i=0;i<128;i++) buf[i+384]=ary4[i];//高镜头
    
    buf[512]=giRoadEdgeL[0];
    buf[513]=giRoadEdgeR[0];
    buf[514]=giRoadEdgeFarL[0];
    buf[515]=giRoadEdgeFarR[0];
    buf[516]=giRoadSit[0];
    buf[517]=giRoadSitFar[0];
    
    buf[518]=((int)(giPreHindWidth))>>8;//左轮速度
    buf[519]=((int)(giSpeedCount))>>8;//右轮速度
    buf[520]=((int)(gfAngle))>>8;//速度和
    buf[521]=((int)(giHINDFLAG))>>8;//期望速度
    buf[522]=((int)(hind))>>8;//角度
    buf[523]=((int)(hind1))>>8;//角度PWM//gfAnglePWM
    buf[524]=((int)(giPreObstacleL))>>8;//速度PWM//gfSpeedPWM
    buf[525]=((int)(giPreObstacleR))>>8;//方向PWM//gfDirection_PWM_L
    
    buf[526]=((int)(giPreHindWidth))&0xff;
    buf[527]=((int)(giSpeedCount))&0xff;
    buf[528]=((int)(gfAngle))&0xff;
    buf[529]=((int)(giHINDFLAG))&0xff;
    buf[530]=((int)(hind))&0xff;
    buf[531]=((int)(hind1))&0xff;
    buf[532]=((int)(giPreObstacleL))&0xff;
    buf[533]=((int)(giPreObstacleR))&0xff;
     
    znFAT_WriteData(&fileinfo,540,buf); //向文件写入数据
}
/***************************************************************************
name:		fat_over
parameter:	time
explain:	none
关闭文件系统
***************************************************************************/
void fat_over()
{
		znFAT_Close_File(&fileinfo); //关闭文件
		znFAT_Flush_FS(); //刷新文件系统	
}