/**********************************************************
Copyright (B), 2014-2015, TYUT JBD TEAM B.
File name: Getinfor.c 
Author: TEAM B
Version: 1.0              Date: 2015.4.2
Description: 
Others:
Function List:
History: Helium				2015.4.2
***********************************************************/
#include "Declare.h"
/**********过零启动*********/
char STARTFLAG=0;//过零启动标志

/************中心值******************/
float gfAngleCenter = 0;//加速度计中心值（直立）
float gfAngleRenew[2] = {0};//过零启动判断用

/*********数据融合*******************/
float gfAngleSpeedIntegral = 0;//陀螺仪积分值
float gfAngleSpeedCurrent = 0;//陀螺仪归一化后值
float gfAngle=0;//角度                            							//融合后输出角度

/*********角度和角速度的AD值*********/
float gfAngleAD=0;       
float gfAngleSpeedAD=0;

/*************传送曲线变量****************************/
float gfSendRed1=0;//加速度值归一化
float gfSendGreen1=0;//陀螺仪积分

/**************记录CCD点*****************************/
int CCD_H[128]={0};//高处CCD
int CCD_M[128]={0};//中间CCD

/**************边界采集*******************************/
int giRoadEdgeL[20] = {0}; /*赛道左边 存20个历史*/
int giRoadEdgeR[20] = {127}; /*赛道右边 存20个历史*/
int giRoadSit[20] = {64};/*赛道中心 存20个历史*/

float gfPosition = 0;
char gcShout=0;

int LostFarL=0;
int LostFarR=0;

int FarJudgeL=0;//预判左边丢失直角
int FarJudgeR=0;//预判右边丢失直角

int giLostFlagFarL=0;
int giLostFlagFarR=0;

char TempFlag=0;//标志位
float gfActualCurvature=0;//实际曲率=w/v
int SwitchStop=0;//起跑线方案 0起跑线检测 1起跑线不检测
/****************************************************************************************************************************************/

/***************获取转向陀螺仪的角速度值************
*  函数名称：GetDirectionAngularVelocity
*  功能说明：转向陀螺仪的角速度
*  参数说明：
*  函数返回：无
*  修改时间：2015-1-8
*  备    注:
****************************************************/
void GetDirectionAngularVelocity(void)
{
	gfActualCurvature=0;
	gfDirection_AngularVelocity=display_x()*0.1; //转向，左转正，右转负
	if(gfCurrent_Speed!=0)
	{
		gfActualCurvature=abs(gfDirection_AngularVelocity)/gfCurrent_Speed;
	}

}

/***********************获取角度与角速度的累加值************
*  函数名称：GetAD
*  功能说明：获取角度与角速度的累加值
*  参数说明：
*  函数返回：无
*  修改时间：2015-1-8
*  备    注:
*  变量: 
		(局部变量)
				liAngleSum:20次加速度计累加AD值
				liAngleDotSum:20次陀螺仪累加AD值
				
		(全局变量)
				gfAngleAD：平均加速度计的AD
				gfAngleSpeedAD：平均陀螺仪的AD
*************************************************************/
void GetAD (void)
{
    int i = 0;
    long int liAngleSum = 0;//加速度计角度值（20次累加）
    long int liAngleDotSum = 0;//陀螺仪角速度值（20次累加）
     
    for (i = 0; i < 20; i++)                                     
    {
		while(!ADC.CDR[36].B.VALID)     							//20次读平均值求加速度计AD值 (D15)(等待AD转换完成，;代表空循环）
		{};
		liAngleSum += ADC.CDR[36].B.CDATA;
     }
     	
        giGyro_Y=display_y();
	gfAngleAD = (float)(liAngleSum * 0.05);            				//取平均值（除以20就是乘以0.05）
	gfAngleSpeedAD=(int)(giGyro_Y*0.1);
}

/***********************角度互补滤波融合************
*  函数名称：AngleCalculate
*  功能说明：陀螺仪积分与加速度计互补滤波
*  参数说明：
*  函数返回：无
*  修改时间：2015-1-8
*  备    注:
*  变量: 
		(局部变量)
				lfDeltaValue:
				lfAngleCurrent:加速度计值归一化
				
		(全局变量)		
				gfAngleAD：平均加速度计的AD
				gfAngleSpeedAD：平均陀螺仪的AD
				gfAngleCenter:加速度计中心值
				gfAngleSpeedCenter:陀螺仪中心值(静止获取)
*************************************************************/
void AngleCalculate(void)
{
    float lfDeltaValue = 0;                   											//角度融合值与就角度AD值偏差
    float lfAngleCurrent = 0;                											//加速度计归一化值
    float lfAngleAD = 0;                     											//加速度计AD值
    float lfAngleSpeedAD = 0;                											//陀螺仪AD值
																		//385
    lfAngleCurrent = (gfAngleCenter- gfAngleAD) * 180 / (224 - 650);           						//gfAngleCenter					//加速度计归一化
    gfAngleSpeedCurrent  = (float)((gfAngleSpeedAD - gfAngleSpeedCenter) * 1.0643);//1.05893//0.86      	//陀螺仪归一化        
   
    gfSendRed1 = lfAngleCurrent+10000;										//加速度计归一化传送到上位机曲线
            
    gfAngleRenew[1] = gfAngleRenew[0];                      							//历史
    gfAngleRenew[0] = lfAngleCurrent;                       								//当前
    
    if(gfAngleRenew[1] > 0 && gfAngleRenew[0] < 0) 
    {
        STARTFLAG = 1;                                                 
    }																		//首次角度在中心值前后变动则过零启动
    
    if(STARTFLAG == 1)                                               
    {
        gfAngle = gfAngleSpeedIntegral;
        gfSendGreen1 = gfAngle + 10000;											//陀螺仪积分与加速度计互补滤波输出最终角度
        lfDeltaValue = (float)((lfAngleCurrent - gfAngle) * 1.2);//1.2
        gfAngleSpeedIntegral += (gfAngleSpeedCurrent + lfDeltaValue) * 0.002;
    }																		//过零启动
}

/***********************信息采集延时**********************
*  函数名称：Image_Delay
*  功能说明：采集信息延时
*  参数说明：
*  函数返回：无
*  修改时间：2015-1-12
*  备    注:
*************************************************************/
void Image_Delay(void)
{
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
}

/***********************CCD信息采集**********************
*  函数名称：Get_Image
*  功能说明：获取CCD采集的信息
*  参数说明：
*  函数返回：无
*  修改时间：2015-1-12
*  备    注:
*  变量: 
*************************************************************/
void Get_Image(void)
{
	int i=0;

	//SIU.GPDO[69].R=0;

	SIU.GPDO[3].R=1;//CLK高电平
	SIU.GPDO[8].R=0;//SI低电平
	Image_Delay();
	
	SIU.GPDO[3].R=0;//CLK低电平
	SIU.GPDO[8].R=1;//SI高电平
	Image_Delay();
	
	SIU.GPDO[3].R=1;//CLK高电平
	SIU.GPDO[8].R=0;//SI低电平
	Image_Delay();
	
	for(i=0;i<128;i++)
	{	
		SIU.GPDO[3].R=0;//CLK低电平
		//Image_Delay();
		
		while(!ADC.CDR[38].B.VALID); //中间镜头  
		CCD_M[i]= ADC.CDR[38].B.CDATA;                
		
		SIU.GPDO[3].R=1;//CLK低电平
		Image_Delay();
	}
	
	//SIU.GPDO[69].R=1;	
}

/***********************CCD信息采集**********************
*  函数名称：Get_Image_H
*  功能说明：获取CCD采集的信息
*  参数说明：
*  函数返回：无
*  修改时间：2015-1-12
*  备    注:
*  变量: 
*************************************************************/
void Get_Image_H(void)
{
	int i=0;

	//SIU.GPDO[69].R=0;

	SIU.GPDO[34].R=1;//CLK高电平
	SIU.GPDO[6].R=0;//SI低电平
	Image_Delay();
	
	SIU.GPDO[34].R=0;//CLK低电平
	SIU.GPDO[6].R=1;//SI高电平
	Image_Delay();
	
	SIU.GPDO[34].R=1;//CLK高电平
	SIU.GPDO[6].R=0;//SI低电平
	Image_Delay();
	
	for(i=0;i<128;i++)
	{	
		SIU.GPDO[34].R=0;//CLK低电平
		//Image_Delay();
		
		while(!ADC.CDR[39].B.VALID);//上面镜头   
		CCD_H[i]= ADC.CDR[39].B.CDATA;
		
		SIU.GPDO[34].R=1;//CLK低电平
		Image_Delay();
	}
	
	//SIU.GPDO[69].R=1;
	
}

/***********************边界采集****************************
*  函数名称：FindEdge
*  功能说明：采集提取赛道边界黑白跳变
*  参数说明：
*  函数返回：无
*  修改时间：2015-1-24
*  备    注:
*  变量: 
*************************************************************/
int giWhiteCountL=0;//白色计数
int giWhiteCountR=0;
int SingleLine=0;
int TurnRight=0;
int TurnLeft=0;
int FlagL=0;
int FlagR=0;

int giLostFlagL=0;
int giLostFlagR=0;

int giAnR=0;
int giAnL=0;
int giAnR_Old=0;
int giAnL_Old=0;
int giAnFarR=0;//远处CCD找到丢失的右边
int giAnFarL=0;//远处CCD找到丢失的左边
//int giAnFarR_Old=0;
//int giAnFarL_Old=0;

int TurnFlagL=0;//左转
int TurnFlagR=0;//右转

int giNoiseL=0;
int giNoiseR=0;

int giPreL=0;//丢失前左边
int giPreR=0;//丢失前右边

int giTurnS=0;//直角开始和结束标志
int giPreLostEdgeL=0;
int giPreLostEdgeR=0;

int giBlockBlocL=0;
int giBlockBlocR=0;
int giBlockFarL=0;//远处左边沿
int giBlockFarR=0;//远处右边沿

int giRampFlag=0;

void FindEdge(void)
{
     int i = 0,j = 0;
     int liexpect_place=0;
     
     liexpect_place = giExpectPlaceUpdate;
     giRoadEdgeL[0] = 0; 
     giRoadEdgeR[0] = 127;
     giWhiteCountL=0;
     giWhiteCountR=0;
     giBlockBlocL=0;
     giBlockBlocR=0;
    /************根据阈值找左右边界*******************/     
     if(liexpect_place>=4&&liexpect_place<=123)
     {
     	if(liexpect_place>=10&&liexpect_place<=117)
     	{
     		for (i = liexpect_place; i >= 0; i--)
          {
               if ((CCD_M[i + 3] - CCD_M[i]) > EYELight && (CCD_M[i + 4] - CCD_M[i + 1]) > EYELight
               &&(CCD_M[i + 9] - CCD_M[i]) > EYELight*0.5 && (CCD_M[i + 10] - CCD_M[i+1]) > EYELight*0.5)
               {
                     giRoadEdgeL[0] = i;
                     break;
               }
               giWhiteCountL++;
          }
          
          for (i = liexpect_place; i <= 127; i++)
          {
               if ((CCD_M[i - 3] - CCD_M[i]) > EYELight && (CCD_M[i - 4] - CCD_M[i - 1]) > EYELight
               && (CCD_M[i - 9] - CCD_M[i]) > EYELight*0.5 && (CCD_M[i - 10] - CCD_M[i-1]) > EYELight*0.5)
               {
                    giRoadEdgeR[0] = i;
                    break;
               }
               giWhiteCountR++;
          }
     	}
     	
     	if((liexpect_place>=4&&liexpect_place<10)
     	||(liexpect_place>117&&liexpect_place<=123))
     	{
     	  for (i = liexpect_place; i >= 0; i--)
          {
               if ((CCD_M[i + 3] - CCD_M[i]) > EYELight && (CCD_M[i + 4] - CCD_M[i + 1]) > EYELight)
               {
                     giRoadEdgeL[0] = i;
                     break;
               }
               giWhiteCountL++;
          }
          
          for (i = liexpect_place; i <= 127; i++)
          {
               if ((CCD_M[i - 3] - CCD_M[i]) > EYELight && (CCD_M[i - 4] - CCD_M[i - 1]) > EYELight)
               {
                    giRoadEdgeR[0] = i;
                    break;
               }
               giWhiteCountR++;
          }
     	}
     }
     
      else if(liexpect_place < 4)
      {
            for(i=4;i<=127;i++)
            {
                if((CCD_M[i - 3]-CCD_M[i])>EYELight && (CCD_M[i - 4]-CCD_M[i - 1])>EYELight)
                {
                   giRoadEdgeR[0] = i;
                   giRoadEdgeL[0] = 0;
                   break;
                }
                giWhiteCountR++;
            }
      }
    
      else if(liexpect_place > 123)
      {
          for(i = 123;i >= 0;i--)
         {
              if((CCD_M[i + 3] - CCD_M[i]) > EYELight && (CCD_M[i + 4] - CCD_M[i + 1]) > EYELight)
              {
                   giRoadEdgeL[0] = i;
                   giRoadEdgeR[0] = 127;
                   break;
              }
              giWhiteCountL++;
         }
     }
     
     else 
     {
          giRoadEdgeL[0]=0;
          giRoadEdgeR[0]=127;
     }
  
    if (giRoadEdgeL[0] < 0 )
    {
         giRoadEdgeL[0] = 0;
    }
    
    else if(giRoadEdgeL[0] > 127&&giRoadEdgeR[0] == 127)
    {
        giRoadEdgeL[0] = giRoadEdgeL[1];
    }
    
    if (giRoadEdgeR[0] > 127)
    {
        giRoadEdgeR[0] = 127;
    }
    
    else if(giRoadEdgeR[0] < 0 && giRoadEdgeL[0] == 0)
    {
        giRoadEdgeR[0] = giRoadEdgeR[1];
    }
    
    if(giRoadEdgeL[0]==127&&giRoadEdgeR[0]==giRoadEdgeL[1])
    {
    	giRoadEdgeL[0]=giRoadEdgeL[1];
    	giRoadEdgeR[0]=giRoadEdgeR[1];
    }
    
    if(giRoadEdgeL[0]==giRoadEdgeR[1]&&giRoadEdgeR[0]==0)
    {
    	giRoadEdgeL[0]=giRoadEdgeL[1];
    	giRoadEdgeR[0]=giRoadEdgeR[1];
    }
    
    if(giRoadEdgeL[0]>=giRoadEdgeR[0])
    {
    	giRoadEdgeL[0]=giRoadEdgeL[1];
    	giRoadEdgeR[0]=giRoadEdgeR[1];
    }
     /****************反光处理************************/
    if(giRoadEdgeL[0]!=0&&giRoadEdgeR[0]!=127&&(giRoadEdgeL[1]!=0||giRoadEdgeR[1]!=127)&&SingleLine==0
       &&((giRoadEdgeL[0]-giRoadEdgeL[1])>=10||(giRoadEdgeR[1]-giRoadEdgeR[0])>=10))
    {
      for (i = giRoadEdgeR[1]-10; i >= giRoadEdgeL[1]; i--)//下降沿为黑
        {
            if ((CCD_M[i+10]-CCD_M[i])>EYELight*0.5)
            {
                giNoiseL = i;
                break;
            }
            
            else
            {
                giNoiseL=0;
            }
        }
        for (i = giRoadEdgeL[1]+10; i <= giRoadEdgeR[1]; i++)
        {
            if ((CCD_M[i-10]-CCD_M[i])>EYELight*0.5)
            {
                giNoiseR = i;
                break;
            }
            
            else
            {
                giNoiseR=0;
            }
        }
      
        if(giNoiseL!=0&&giNoiseR!=0
           &&giNoiseL>=giRoadEdgeL[1]&&giNoiseR<=giRoadEdgeR[1]
             &&giNoiseL<=giNoiseR)
        {
           for (i = giNoiseL-6; i >= 0; i--)//下降沿为黑
          {
            if ((CCD_M[i+3]-CCD_M[i])>EYELight&&(CCD_M[i+4]-CCD_M[i+1])>EYELight)
            {
                giRoadEdgeL[0]=i;
                break;
            }
            
            else
            {
                giRoadEdgeL[0]=giRoadEdgeL[1];
            }
          }
        
          for (i = giNoiseR+6; i <= 127; i++)
          {
            if ((CCD_M[i-3]-CCD_M[i])>EYELight&&(CCD_M[i-4]-CCD_M[i-1])>EYELight)
            {
                giRoadEdgeR[0]=i;
                break;
            }
            
            else
            {
                giRoadEdgeR[0]=giRoadEdgeR[1];
            }
          }
        }
    }
        
	/****************处理控制行的信息****************/	
    if(giRoadEdgeL[0]!=0&&giRoadEdgeR[0]!=127)
    {
    	int i=0;
        SingleLine=0;
        
        giLostFlagL=0;//直角左边丢计数
        giLostFlagR=0;//直角右边丢计数
        giBlockFarL=0;
        giBlockFarR=0;
        
        giPreLostEdgeL=0;
        giPreLostEdgeR=0;
        
        giRoadSit[0]=(giRoadEdgeL[0]+giRoadEdgeR[0])*0.5;
    
        if(giRoadSit[0]>20&&giRoadSit[0]<110)
        {
            TurnRight=0;
            TurnLeft=0;
	    //SIU.GPDO[18].R=0;//蜂鸣器
        }
        
       if(giRoadEdgeL[0]>=5&&giRoadEdgeL[0]<=35&&giRoadEdgeR[0]>=95&&giRoadEdgeR[0]<=125
            &&giRoadEdgeL[1]>=5&&giRoadEdgeL[1]<=35&&giRoadEdgeR[1]>=95&&giRoadEdgeR[1]<=125
             &&giRoadEdgeL[3]>=5&&giRoadEdgeL[3]<=35&&giRoadEdgeR[3]>=95&&giRoadEdgeR[3]<=125
               &&giRoadEdgeL[5]>=5&&giRoadEdgeL[5]<=35&&giRoadEdgeR[5]>=95&&giRoadEdgeR[5]<=125
                 &&giRoadEdgeL[7]>=5&&giRoadEdgeL[7]<=35&&giRoadEdgeR[7]>=95&&giRoadEdgeR[7]<=125
                   &&giRoadEdgeL[9]>=5&&giRoadEdgeL[9]<=35&&giRoadEdgeR[9]>=95&&giRoadEdgeR[9]<=125)
         {
           giTurnS=0;
         }
        
        if(giRoadEdgeL[0]>=5&&giRoadEdgeL[0]<=35&&giRoadEdgeR[0]>=95&&giRoadEdgeR[0]<=125
           &&giRoadEdgeL[1]>=5&&giRoadEdgeL[1]<=35&&giRoadEdgeR[1]>=95&&giRoadEdgeR[1]<=125
             &&giRoadEdgeL[3]>=5&&giRoadEdgeL[3]<=35&&giRoadEdgeR[3]>=95&&giRoadEdgeR[3]<=125
               &&giRoadEdgeL[5]>=5&&giRoadEdgeL[5]<=35&&giRoadEdgeR[5]>=95&&giRoadEdgeR[5]<=125
                 &&giRoadEdgeL[7]>=5&&giRoadEdgeL[7]<=35&&giRoadEdgeR[7]>=95&&giRoadEdgeR[7]<=125
                   &&giRoadEdgeL[9]>=5&&giRoadEdgeL[9]<=35&&giRoadEdgeR[9]>=95&&giRoadEdgeR[9]<=125)
         {
           giTurnS=0;
         }
         
         /**********************坡道检测**************************/
         /*if(giRoadEdgeL[0]<giRoadEdgeL[1]&&giRoadEdgeR[0]>giRoadEdgeR[1]
         	&&giRoadEdgeL[1]<giRoadEdgeL[2]&&giRoadEdgeR[1]>giRoadEdgeR[2]
         		&&giRoadEdgeL[2]<giRoadEdgeL[3]&&giRoadEdgeR[2]>giRoadEdgeR[3]
         			&&giRoadEdgeL[3]<giRoadEdgeL[4]&&giRoadEdgeR[3]>giRoadEdgeR[4]
         				&&giRoadEdgeL[4]<giRoadEdgeL[5]&&giRoadEdgeR[4]>giRoadEdgeR[5])
         {
         	giRampFlag=1;
         }*/
         
         if(hind==1111||hind==2222)
         { 
            outs++;
            if(outs>300)
            {
               hind=0;
               hind1=0;
               outs=0;
               iFlgLH=0;
               iFlgRH=0;
	       giPreObstacleL=0;
	       giPreObstacleR=0;
	       giPreObstacleCenter=0;
	       giHindAngle=0;
	       SIU.GPDO[18].R=0;//蜂鸣器
	     }
          }
          //if((giRoadEdgeR[0]-giRoadEdgeL[0])<2*HalfWidth)
          //{
          	FindHind();
          //}       
        
        liexpect_place=(giRoadEdgeL[0]+giRoadEdgeR[0])*0.5;//(giRoadEdgeL[0]+giRoadEdgeR[0])*0.5
    }
    else if(giRoadEdgeL[0]==0&&giRoadEdgeR[0]!=127)//左边丢，左弯
    {  
    	 int i=0;
         FlagR=0;
         TurnRight=0;
         giFlagCurveR=0;
	TurnFlagR=0;
	giLostFlagR=0;
	giLostFlagFarR=0;
	
	
	if(giRoadEdgeL[1]!=0&&giRoadEdgeR[1]!=127)
	{
		giPreLostEdgeL=giRoadEdgeL[1];//丢左边前的左边
	}
		
         //直角检测
         if(giRoadEdgeL[1]>=10&&giRoadEdgeL[1]<=40&&giRoadEdgeR[1]>=90&&giRoadEdgeR[1]<=120
             &&giRoadEdgeL[3]>=10&&giRoadEdgeL[3]<=40&&giRoadEdgeR[3]>=90&&giRoadEdgeR[3]<=120
               &&giRoadEdgeL[5]>=10&&giRoadEdgeL[5]<=40&&giRoadEdgeR[5]>=90&&giRoadEdgeR[5]<=120
                 &&giRoadEdgeL[7]>=10&&giRoadEdgeL[7]<=40&&giRoadEdgeR[7]>=90&&giRoadEdgeR[7]<=120
                   &&giRoadEdgeL[9]>=10&&giRoadEdgeL[9]<=40&&giRoadEdgeR[9]>=90&&giRoadEdgeR[9]<=120)//&&giPreLostEdgeL>=15&&giPreLostEdgeL<=40
         {
           TurnFlagL=1;
          }
         
         if(giRoadEdgeR[0]>=90&&giRoadEdgeR[0]<=120&&gfActualCurvature<1)//&&giTurnS==0//扩大待测
         {
           giLostFlagL++;
           if(giLostFlagL>12)
           {
             giLostFlagL=12;
           }
         }
         
         else if(giRoadEdgeR[0]<90||giRoadEdgeR[0]>120)
         {
         	giLostFlagL=0;
         }
         
         /*else
         {
         	giLostFlagL=0;
         }*/
         
        /* else if(giRoadEdgeR[0]<100||giRoadEdgeR[0]>120)//边界不在范围内直角标志清零
         {
         	giLostFlagL=0;
         }*/
         
         /*if(giRoadEdgeR[0]>=85&&giRoadEdgeR[0]<=115&&giTurnS==1)
         {
           giLostFlagL++;
           if(giLostFlagL>12)
           {
             giLostFlagL=12;
           }
         }*/
         
         /*if((giLostFlagL>=8||(FarJudgeL==1&&giLostFlagL>=8))&&giTurnS==1)
         {
             TurnLeft=1;
             giPreR=giRoadEdgeR[0];
             giTurnS=1;
             FarJudgeL=0;
         }*/
         
         if((giLostFlagL>=10||(FarJudgeL==1&&giLostFlagL>=5))&&TurnFlagL==1)//&&giTurnS==0//||(FarJudgeL==1&&giLostFlagL>=8)
         {
           if((giRoadEdgeR[0]-giRoadEdgeR[10])>=-10&&(giRoadEdgeR[0]-giRoadEdgeR[10])<=10
              &&(giRoadEdgeR[1]-giRoadEdgeR[11])>=-10&&(giRoadEdgeR[1]-giRoadEdgeR[11])<=10
                &&(giRoadEdgeR[2]-giRoadEdgeR[12])>=-10&&(giRoadEdgeR[2]-giRoadEdgeR[12])<=10
                  &&(giRoadEdgeR[3]-giRoadEdgeR[13])>=-10&&(giRoadEdgeR[3]-giRoadEdgeR[13])<=10
                    &&(giRoadEdgeR[4]-giRoadEdgeR[14])>=-10&&(giRoadEdgeR[4]-giRoadEdgeR[14])<=10
                      &&(giRoadEdgeR[0]-giRoadEdgeR[14])>=-12&&(giRoadEdgeR[0]-giRoadEdgeR[14])<=12)
           {
             TurnLeft=1;
             giPreR=giRoadEdgeR[0];
             //giTurnS=1;
             giFlagCurveL=0;
             FarJudgeL=0;
           }
         }

        if(giRoadEdgeL[1]!=0)
         {
             if(giRoadEdgeL[1]<=5)
             {
                 FlagL=1;    //左边贴边丢，左弯
                 giRoadSit[0]=giRoadEdgeR[0]-HalfWidth-7;
             }
             if(giRoadEdgeL[1]>5&&giRoadEdgeL[1]<15)
             {
                  FlagL=2;
                  giRoadSit[0]=giRoadEdgeR[0]-HalfWidth-5;
             }
         }
       
         
        if(FlagL==1&&giRoadEdgeR[1]!=127)
        {
            giRoadSit[0]=giRoadEdgeR[0]-HalfWidth-7;//(giRoadEdgeR[1]-giRoadSit[1]);
        }
        else if(FlagL==2&&giRoadEdgeR[1]!=127)
        {
            giRoadSit[0]=giRoadEdgeR[0]-HalfWidth-5;//(giRoadEdgeR[1]-giRoadSit[1]);
        }
        else
        {
            giRoadSit[0]=giRoadEdgeR[0]-HalfWidth-3;         
        } 
        
        /******************不正常找边处理**************/
        /*if(((giRoadEdgeR[0]-giRoadEdgeR[1])>HalfWidth
        	||(giRoadEdgeR[0]-giRoadEdgeR[1])<-HalfWidth))
        {
        	giRoadSit[0]=giRoadSit[1];
        }*/
        /******************坡道处理********************/
        if((giRoadEdgeR[1]-giRoadEdgeL[1])>=HalfWidth*3)
        {
	        if(giRoadEdgeFarL[0]!=0&&giRoadEdgeFarR[0]!=127
	        &&(giRoadEdgeFarR[0]-giRoadEdgeFarL[0])>=HalfWidthFar*3)
	        {
	        	giRoadSit[0]=(giRoadEdgeL[0]+giRoadEdgeR[0])*0.5;
	        }
	        
	        else if(giRoadEdgeFarL[0]!=0&&giRoadEdgeFarR[0]==127
	        &&(giRoadEdgeFarR[0]-giRoadEdgeFarL[0])>=HalfWidthFar*3)
	        {
	        	giRoadSit[0]=(giRoadEdgeL[0]+giRoadEdgeR[0])*0.5;
	        }
	        
	        else if(giRoadEdgeFarL[0]==0&&giRoadEdgeFarR[0]!=127
	        &&(giRoadEdgeFarR[0]-giRoadEdgeFarL[0])>=HalfWidthFar*3)
	        {
	        	giRoadSit[0]=(giRoadEdgeL[0]+giRoadEdgeR[0])*0.5;
	        }
        }
                    
        liexpect_place=giRoadSit[0];
    }
    else if(giRoadEdgeL[0]!=0&&giRoadEdgeR[0]==127)
    {
    	int i=0;
        FlagL=0;
        TurnLeft=0;
        giFlagCurveL=0;
	TurnFlagL=0;
	giLostFlagL=0;
	giLostFlagFarL=0;
	
		
	if(giRoadEdgeL[1]!=0&&giRoadEdgeR[1]!=127)
	{
		giPreLostEdgeR=giRoadEdgeR[1];//丢左边前的左边
	}
		
        //直角检测
         if(giRoadEdgeL[1]>=10&&giRoadEdgeL[1]<=40&&giRoadEdgeR[1]>=90&&giRoadEdgeR[1]<=120
             &&giRoadEdgeL[3]>=10&&giRoadEdgeL[3]<=40&&giRoadEdgeR[3]>=90&&giRoadEdgeR[3]<=120
               &&giRoadEdgeL[5]>=10&&giRoadEdgeL[5]<=40&&giRoadEdgeR[5]>=90&&giRoadEdgeR[5]<=120
                 &&giRoadEdgeL[7]>=10&&giRoadEdgeL[7]<=40&&giRoadEdgeR[7]>=90&&giRoadEdgeR[7]<=120
                   &&giRoadEdgeL[9]>=10&&giRoadEdgeL[9]<=40&&giRoadEdgeR[9]>=90&&giRoadEdgeR[9]<=120)//&&(127-giPreLostEdgeR)>=15&&(127-giPreLostEdgeR)<=40
         {
           TurnFlagR=1;
         }
         
         if(giRoadEdgeL[0]>=10&&giRoadEdgeL[0]<=40&&gfActualCurvature<1)//&&giTurnS==0
         {
           giLostFlagR++;
           if(giLostFlagR>12)
           {
             giLostFlagR=12;
           }
         }
         
         else if(giRoadEdgeL[0]<10||giRoadEdgeL[0]>40)
         {
         	giLostFlagR=0;
         }
        /*else
         {
         	giLostFlagR=0;
         }*/
         
         /*else if(giRoadEdgeL[0]<10||giRoadEdgeL[0]>30)//边界不在范围内直角标志清零
         {
         	giLostFlagR=0;
         }*/
         
         /*if(giRoadEdgeL[0]>=10&&giRoadEdgeL[0]<=40&&giTurnS==1)
         {
           giLostFlagR++;
           if(giLostFlagR>12)
           {
             giLostFlagR=12;
           }
         }*/
         
         /*if((giLostFlagR>=8||(FarJudgeR==1&&giLostFlagR>=8))&&giTurnS==1)
         {
             TurnRight=1;
             giPreL=giRoadEdgeL[0];
             giTurnS=1;
             FarJudgeR=0;
         }*/
         
         if((giLostFlagR>=10||(FarJudgeR==1&&giLostFlagR>=5))&&TurnFlagR==1)//&&giTurnS==0//||(FarJudgeR==1&&giLostFlagR>=8)
         {
           if((giRoadEdgeL[0]-giRoadEdgeL[10])>=-10&&(giRoadEdgeL[0]-giRoadEdgeL[10])<=10
              &&(giRoadEdgeL[1]-giRoadEdgeL[11])>=-10&&(giRoadEdgeL[1]-giRoadEdgeL[11])<=10
                &&(giRoadEdgeL[2]-giRoadEdgeL[12])>=-10&&(giRoadEdgeL[2]-giRoadEdgeL[12])<=10
                  &&(giRoadEdgeL[3]-giRoadEdgeL[13])>=-10&&(giRoadEdgeL[3]-giRoadEdgeL[13])<=10
                    &&(giRoadEdgeL[4]-giRoadEdgeL[14])>=-10&&(giRoadEdgeL[4]-giRoadEdgeL[14])<=10
                      &&(giRoadEdgeL[0]-giRoadEdgeL[14])>=-12&&(giRoadEdgeL[0]-giRoadEdgeL[14])<=12)
           {
             	TurnRight=1;
             	giPreL=giRoadEdgeL[0];
             	//giTurnS=1;
             	giFlagCurveR=0;
    		 FarJudgeR=0;
           }
         }
         
        if(giRoadEdgeR[1]!=127)
        {
            if(127-giRoadEdgeR[1]<=5)
            {
                FlagR=1;   //右缓慢丢
                giRoadSit[0]=giRoadEdgeL[0]+HalfWidth+7;
                
            }
            if((127-giRoadEdgeR[1])>5&&(127-giRoadEdgeR[1]<15))
            {
                FlagR=2; 
                giRoadSit[0]=giRoadEdgeL[0]+HalfWidth+5;
            }
        }       
        
        if(FlagR==1&&giRoadEdgeL[1]!=0)
        {
            giRoadSit[0]=giRoadEdgeL[0]+HalfWidth+7;
        }
        else if(FlagR==2&&giRoadEdgeL[1]!=0)
        {
            giRoadSit[0]=giRoadEdgeL[0]+HalfWidth+5;
        }               
        else 
        {        
            giRoadSit[0]=giRoadEdgeL[0]+HalfWidth+3; 
        }
        
        /******************不正常找边处理**************/
        /*if(((giRoadEdgeL[0]-giRoadEdgeL[1])>HalfWidth
        	||(giRoadEdgeL[0]-giRoadEdgeL[1])<-HalfWidth))
        {
        	giRoadSit[0]=giRoadSit[1];
        }*/
		/******************坡道处理********************/
        if((giRoadEdgeR[1]-giRoadEdgeL[1])>=HalfWidth*3)
        {
	        if(giRoadEdgeFarL[0]!=0&&giRoadEdgeFarR[0]!=127
	        &&(giRoadEdgeFarR[0]-giRoadEdgeFarL[0])>=HalfWidthFar*3)
	        {
	        	giRoadSit[0]=(giRoadEdgeL[0]+giRoadEdgeR[0])*0.5;
	        }
	        
	        else if(giRoadEdgeFarL[0]!=0&&giRoadEdgeFarR[0]==127
	        &&(giRoadEdgeFarR[0]-giRoadEdgeFarL[0])>=HalfWidthFar*3)
	        {
	        	giRoadSit[0]=(giRoadEdgeL[0]+giRoadEdgeR[0])*0.5;
	        }
	        
	        else if(giRoadEdgeFarL[0]==0&&giRoadEdgeFarR[0]!=127
	        &&(giRoadEdgeFarR[0]-giRoadEdgeFarL[0])>=HalfWidthFar*3)
	        {
	        	giRoadSit[0]=(giRoadEdgeL[0]+giRoadEdgeR[0])*0.5;
	        }
        }
        
        liexpect_place=giRoadSit[0];
    }
    
    else if(giRoadEdgeL[0]==0&&giRoadEdgeR[0]==127)
    {
        FlagL=0;
        FlagR=0;
        giLostFlagL=0;//直角左边丢计数
        giLostFlagR=0;//直角右边丢计数
        
        	
        if(giRoadEdgeL[1]!=0&&giRoadEdgeR[1]==127)//全丢之前车身向左斜，记左历史
        {
            giRoadSit[0]=giRoadEdgeL[1]+HalfWidth;
        }
        if(giRoadEdgeL[1]==0&&giRoadEdgeR[1]!=127)//车身向右斜
        {
            giRoadSit[0]=giRoadEdgeR[1]-HalfWidth;
        }
               
        if(giRoadSit[1]>20&&giRoadSit[1]<110)
        {
            if(giRoadEdgeFarL[0]!=0&&giRoadEdgeFarR[0]!=127)//跑跑看会不会出现只有单边的情况，然后再补充
            {
                giRoadSit[0]=giRoadSitFar[0]*0.5+giRoadSit[1]*0.3+giRoadSit[2]*0.2;
            }
            
            if(giRoadEdgeFarL[0]!=0&&giRoadEdgeFarR[0]==127)
            {
            	/*if((giRoadSitFar[0]-giRoadSit[1])<-(HalfWidth)
            	||(giRoadSitFar[0]-giRoadSit[1])>(HalfWidth))
            	{
            		giRoadSit[0]=giRoadSit[1];
            	}
            	
            	else if((giRoadSitFar[0]-giRoadSit[1])>=-(HalfWidth)
            	&&(giRoadSitFar[0]-giRoadSit[1])<=(HalfWidth))
            	{*/
            		giRoadSit[0]=giRoadSitFar[0]*0.5+giRoadSit[1]*0.5;
            	//}
            	
            }
            
            if(giRoadEdgeFarL[0]==0&&giRoadEdgeFarR[0]!=127)
            {
            	/*if((giRoadSitFar[0]-giRoadSit[1])<-(HalfWidth)
            	||(giRoadSitFar[0]-giRoadSit[1])>(HalfWidth))
            	{
            		giRoadSit[0]=giRoadSit[1];
            	}
            	
            	else if((giRoadSitFar[0]-giRoadSit[1])>=-(HalfWidth)
            	&&(giRoadSitFar[0]-giRoadSit[1])<=(HalfWidth))
            	{*/
            		giRoadSit[0]=giRoadSitFar[0]*0.5+giRoadSit[1]*0.5;
            	//}
            }
            
            if(giRoadEdgeFarL[0]==0&&giRoadEdgeFarR[0]==127)
            {
              	//giRoadSit[0]=giRoadSit[1];
              	if(giRoadEdgeL[1]!=0&&giRoadEdgeR[1]==127)//向左边偏
				{
				   for(i = 123;i >= giRoadEdgeL[1];i--)
				   {
				      if((CCD_H[i + 3] - CCD_H[i]) > EYELightF && (CCD_H[i + 4] - CCD_H[i + 1]) > EYELightF)
				      {
					        giRoadEdgeFarL[0] = i;
					       	giRoadEdgeFarR[0] = 127;
					       	break;
				      }
				      
				      else
				      {
				      		giRoadEdgeFarL[0] = 0;
					       	giRoadEdgeFarR[0] = 127;
				      }
				   }
				   
				   if(giRoadEdgeFarL[0]!=0&&giRoadEdgeFarR[0]==127)
				   {
				   		giRoadSitFar[0]=(giRoadEdgeFarL[0]+giRoadEdgeFarR[0])*0.5;
				   		giRoadSit[0]=giRoadSitFar[0]*0.8+giRoadSit[1]*0.2;
				   		giExpectPlaceUpdateFar=giRoadSitFar[0];
				   }
				   
				}
				
				if(giRoadEdgeL[1]==0&&giRoadEdgeR[1]!=127)//向左边偏
				{
				  for(i=4;i<=giRoadEdgeR[1];i++)
				  {
				    if((CCD_H[i - 3]-CCD_H[i])>EYELightF && (CCD_H[i - 4]-CCD_H[i - 1])>EYELightF)
				    {
					     giRoadEdgeFarR[0] = i;
					     giRoadEdgeFarL[0] = 0;
					     break;
				    }
				    
				    else
				    {
				    	giRoadEdgeFarR[0] = 127;
					    giRoadEdgeFarL[0] = 0;
				    }
				  }
				   
				   if(giRoadEdgeFarL[0]==0&&giRoadEdgeFarR[0]!=127)
				   {
				   		giRoadSitFar[0]=(giRoadEdgeFarL[0]+giRoadEdgeFarR[0])*0.5;
				   		giRoadSit[0]=giRoadSitFar[0]*0.8+giRoadSit[1]*0.2;
				   		giExpectPlaceUpdateFar=giRoadSitFar[0];
				   }
				   
				}
				else giRoadSit[0]=giRoadSit[1];
            }
        }
        else giRoadSit[0]=giRoadSit[1];
        
        if(giRoadSitY_Old!=0&&giRoadSitY==0)
        {
        	giRoadSit[0]=giRoadSit[1];
        	giRoadSitY=giRoadSitY_Old;
        }
        
        liexpect_place=giRoadSit[0];
    }
       
    if(liexpect_place<=0)
    {
        liexpect_place=0;
    }
    else if(liexpect_place>=127)
    {
        liexpect_place=127;
    }
    
    giExpectPlaceUpdate=liexpect_place;  
}

/*************第二个镜头,带2的都是远处镜头的信息************/
char giRoadEdgeFarL[20] = {0}; /*赛道左边 存10个历史*/
char giRoadEdgeFarR[20] = {127}; /*赛道右边 存10个历史*/
char giRoadSitFar[20]={64};
int giExpectPlaceUpdateFar = 64;//每次扫边的起始位置，在每次找边后更新
int giWhiteCountFarL=0,giWhiteCountFarR=0;

int giFlagCurveR=0,giFlagCurveL=0;//直角判断标志

void FindEdgeFar()
{
    int i=0,j=0;
    int  iExpectPlaceF=0; //程序中用局部变量
    
    giRoadEdgeFarL[0] = 0; //采当前边
    giRoadEdgeFarR[0] = 127;
    iExpectPlaceF=giExpectPlaceUpdateFar;
    giWhiteCountFarL=0;
    giWhiteCountFarR=0;
 /********************读第二个镜头**********************/
 	if(iExpectPlaceF>=4&&iExpectPlaceF<=123)
	{
	 for (i = iExpectPlaceF; i >= 0; i--)//下降沿为黑
	 {
	    if ((CCD_H[i + 3] - CCD_H[i]) > EYELightF && (CCD_H[i + 4] - CCD_H[i + 1]) > EYELightF)
	    {
	        giRoadEdgeFarL[0] = i;
	    	break;
	    }
	   giWhiteCountFarL++;
	 }
	 for (i = iExpectPlaceF; i <= 127; i++)
	 {
	    if ((CCD_H[i - 3] - CCD_H[i]) > EYELightF && (CCD_H[i - 4] - CCD_H[i - 1]) > EYELightF)
	    {
	        giRoadEdgeFarR[0] = i;
	    	break;
	    }
	 	giWhiteCountFarR++;
	 }
	}
	else if(iExpectPlaceF < 4)
	{
	  for(i=4;i<=127;i++)
	  {
	    if((CCD_H[i - 3]-CCD_H[i])>EYELightF && (CCD_H[i - 4]-CCD_H[i - 1])>EYELightF)
	    {
		     giRoadEdgeFarR[0] = i;
		     giRoadEdgeFarL[0] = 0;
		     break;
	    }
	    giWhiteCountFarR++;
	  }
	}
	    
	else if(iExpectPlaceF > 123)
	{
	   for(i = 123;i >= 0;i--)
	   {
	      if((CCD_H[i + 3] - CCD_H[i]) > EYELightF && (CCD_H[i + 4] - CCD_H[i + 1]) > EYELightF)
	      {
		        giRoadEdgeFarL[0] = i;
		       	giRoadEdgeFarR[0] = 127;
		       	break;
	      }
	     giWhiteCountFarL++;
	   }
	}
	else 
	{
	   giRoadEdgeFarL[0]=0;
	       
	   giRoadEdgeFarR[0]=127;
	}
 

/*********限制边沿，用历史************/
    if (giRoadEdgeFarL[0] < 0 )
    {
         giRoadEdgeFarL[0] = 0;
    }
    else if(giRoadEdgeFarL[0] > 127&&giRoadEdgeFarR[0] == 127)
    {
        giRoadEdgeFarL[0] = giRoadEdgeFarL[1];
    }
    if (giRoadEdgeFarR[0] > 127)
    {
        giRoadEdgeFarR[0] = 127;
    }
    else if(giRoadEdgeFarR[0] < 0 && giRoadEdgeFarL[0] == 0)
    {
        giRoadEdgeFarR[0] = giRoadEdgeFarR[1];
    }
    
    if(giRoadEdgeFarL[0]==127&&giRoadEdgeFarR[0]==giRoadEdgeFarL[1])
    {
    	giRoadEdgeFarL[0]=giRoadEdgeFarL[1];
    	giRoadEdgeFarR[0]=giRoadEdgeFarR[1];
    }
    
    if(giRoadEdgeFarL[0]==giRoadEdgeFarR[1]&&giRoadEdgeFarR[0]==0)
    {
    	giRoadEdgeFarL[0]=giRoadEdgeFarL[1];
    	giRoadEdgeFarR[0]=giRoadEdgeFarR[1];
    }
    
    if(giRoadEdgeFarL[0]>=giRoadEdgeFarR[0])
    {
    	giRoadEdgeFarL[0]=giRoadEdgeFarL[1];
    	giRoadEdgeFarR[0]=giRoadEdgeFarR[1];
    }
/**********获取远处镜头的giExpectPlaceUpdate值************/
    if(giRoadEdgeFarL[0]!=0&&giRoadEdgeFarR[0]!=127)
    {
    	giFlagCurveL=0;
    	giFlagCurveR=0;
    	LostFarL=0;
    	LostFarR=0;
    	giLostFlagFarL=0;
    	giLostFlagFarR=0;
    	//giLostFlagL=0;
    	//giLostFlagR=0;
        giRoadSitFar[0]=(giRoadEdgeFarL[0]+giRoadEdgeFarR[0])*0.5;
        iExpectPlaceF=giRoadSitFar[0];
    }
    else if(giRoadEdgeFarL[0]!=0&&giRoadEdgeFarR[0]==127)
    {
    	FarJudgeL=0;
    	giLostFlagFarL=0;
    	LostFarL=0;
        //远处直角检测
        if(giRoadEdgeFarL[1]>=20&&giRoadEdgeFarL[1]<=50&&giRoadEdgeFarR[1]>=80&&giRoadEdgeFarR[1]<=110
             &&giRoadEdgeFarL[3]>=20&&giRoadEdgeFarL[3]<=50&&giRoadEdgeFarR[3]>=80&&giRoadEdgeFarR[3]<=110
               &&giRoadEdgeFarL[5]>=20&&giRoadEdgeFarL[5]<=50&&giRoadEdgeFarR[5]>=80&&giRoadEdgeFarR[5]<=110
                 &&giRoadEdgeFarL[7]>=20&&giRoadEdgeFarL[7]<=50&&giRoadEdgeFarR[7]>=80&&giRoadEdgeFarR[7]<=110
                   &&giRoadEdgeFarL[9]>=20&&giRoadEdgeFarL[9]<=50&&giRoadEdgeFarR[9]>=80&&giRoadEdgeFarR[9]<=110)
         {
           LostFarR=1;
         }
        
        if(giRoadEdgeFarL[0]>=30&&giRoadEdgeFarL[0]<=50
        &&(giRoadEdgeFarL[0]-giRoadEdgeL[0])<=20&&gfActualCurvature<1)
         {
           giLostFlagFarR++;
           if(giLostFlagFarR>10)
           {
             giLostFlagFarR=10;
           }
         }
         
         if(giLostFlagFarR==10&&LostFarR==1)
         {
             FarJudgeR=1;
             //SIU.GPDO[18].R=1;
         }
         
        giRoadSitFar[0]=giRoadEdgeFarL[0]+HalfWidthFar;
        
        iExpectPlaceF=giRoadSitFar[0];
    }
    else if(giRoadEdgeFarL[0]==0&&giRoadEdgeFarR[0]!=127)
    {         
    	FarJudgeR=0;
    	giLostFlagFarR=0;
    	LostFarR=0;
    	//远处直角检测
        if(giRoadEdgeFarL[1]>=20&&giRoadEdgeFarL[1]<=50&&giRoadEdgeFarR[1]>=80&&giRoadEdgeFarR[1]<=110
             &&giRoadEdgeFarL[3]>=20&&giRoadEdgeFarL[3]<=50&&giRoadEdgeFarR[3]>=80&&giRoadEdgeFarR[3]<=110
               &&giRoadEdgeFarL[5]>=20&&giRoadEdgeFarL[5]<=50&&giRoadEdgeFarR[5]>=80&&giRoadEdgeFarR[5]<=110
                 &&giRoadEdgeFarL[7]>=20&&giRoadEdgeFarL[7]<=50&&giRoadEdgeFarR[7]>=80&&giRoadEdgeFarR[7]<=110
                   &&giRoadEdgeFarL[9]>=20&&giRoadEdgeFarL[9]<=50&&giRoadEdgeFarR[9]>=80&&giRoadEdgeFarR[9]<=110)
         {
           LostFarL=1;
         }
        
        if(giRoadEdgeFarR[0]>=80&&giRoadEdgeFarR[0]<=100
        &&(giRoadEdgeR[0]-giRoadEdgeFarR[0])<=20&&gfActualCurvature<1)
         {
           giLostFlagFarL++;
           if(giLostFlagFarL>10)
           {
             giLostFlagFarL=10;
           }
         }
         
         if(giLostFlagFarL==10&&LostFarL==1)
         {
             FarJudgeL=1;
             //SIU.GPDO[18].R=1;
         }
  
    	giRoadSitFar[0]=giRoadEdgeFarR[0]-HalfWidthFar;
        
        iExpectPlaceF=giRoadSitFar[0];
    }
    else if(giRoadEdgeFarL[0]==0&&giRoadEdgeFarR[0]==127)
    {
    	int i=0;
    
    	//当近处CCD丢左边时候
    	/*if(giRoadEdgeL[0]==0&&giRoadEdgeR[0]!=127)
    	{
    		for (i = 4; i <= giRoadEdgeR[0]; i++)
	 		{
			    if ((CCD_H[i - 3] - CCD_H[i]) > EYELightF && (CCD_H[i - 4] - CCD_H[i - 1]) > EYELightF)
			    {
			        giRoadEdgeFarR[0] = i;
			    	break;
			    }
			 	
			 	else
			 	{
			 		giRoadEdgeFarR[0] = 127;
			 	}
	 		}
    	}
    	
    	//当近处CCD丢右边时候
    	if(giRoadEdgeL[0]!=0&&giRoadEdgeR[0]==127)
    	{
			for (i = 123; i >= giRoadEdgeL[0]; i--)//下降沿为黑
			{
				if ((CCD_H[i + 3] - CCD_H[i]) > EYELightF && (CCD_H[i + 4] - CCD_H[i + 1]) > EYELightF)
				{
				    giRoadEdgeFarL[0] = i;
				 	break;
				}
				
				else
				{
					giRoadEdgeFarL[0]=0;
				}
			}
    	}
    	
    	if(giRoadEdgeFarL[0]!=0&&giRoadEdgeFarR[0]==127)
    	{
    		giRoadSitFar[0]=giRoadEdgeFarL[0]+HalfWidthFar;
    	}
    	
    	else if(giRoadEdgeFarL[0]==0&&giRoadEdgeFarR[0]!=127)
    	{
    		giRoadSitFar[0]=giRoadEdgeFarR[0]-HalfWidthFar;
    	}*/
        
        //else
        //{
        		giRoadSitFar[0]=64;
        //}
        
        iExpectPlaceF=giRoadSitFar[0];
    }
    
    if(giRoadSitFar[0]>=127)
    {
        giRoadSitFar[0]=127;
    }
    if(giRoadSitFar[0]<=0)
    {
        giRoadSitFar[0]=0;
    }
    
    for (j = 19; j > 0; j--) //记历史
    {
        giRoadEdgeFarL[j] = giRoadEdgeFarL[j - 1];  
        giRoadEdgeFarR[j] = giRoadEdgeFarR[j - 1];
        giRoadSitFar[j]=giRoadSitFar[j-1];
    }
    if(iExpectPlaceF<=0)
    {
        iExpectPlaceF=0;
    }
    else if(iExpectPlaceF>=127)
    {
        iExpectPlaceF=127;
    }
    giExpectPlaceUpdateFar=iExpectPlaceF;
}

int giExpectPlaceUpdate = 64;//每次扫边的起始位置，在每次找边后更新
int giRoadEdgeRM=0,giRoadEdgeLM=0;
int giRoadSitYL=0,giRoadSitYR=0,giRoadSitY=0;
int giRoadSitYL_Old=0,giRoadSitYR_Old=0,giRoadSitY_Old=0;
char gcSingleLostFlagL=0;
char gcSingleLostFlagR=0;
int giRoadSitSL=0,giRoadSitSR=0;//找错边以后再从两边向中间找边

void CalculateCentre(void)
{
    unsigned char i=0,j=0;
    //int giRoadSitYL=0,giRoadSitYR=0,giRoadSitY=0;
    
    giRoadSitYL=0;
    giRoadSitYR=0;
    giRoadSitY=0;
    giRoadSitSL=0;
    giRoadSitSR=0;
    
    if(giExpectPlaceUpdate<122)//27//100//giExpectPlaceUpdate>6&&
    {
      if(giExpectPlaceUpdate>=40&&giExpectPlaceUpdate<=90)
      {
        for (i =giExpectPlaceUpdate-30; i <=giExpectPlaceUpdate+30; i++)//下降沿为黑
        {
            if ((CCD_M[i -3] - CCD_M[i]) > EYELightSingle*0.8 &&(CCD_M[i - 4] - CCD_M[i-1]) > EYELightSingle*0.8)
            {
                giRoadSitYL = i;
                break;        
             }       
        }                   
    
        for (i = giExpectPlaceUpdate+30; i >=giExpectPlaceUpdate-30; i--)//下降沿为黑
        {
            if ((CCD_M[i + 3] - CCD_M[i]) > EYELightSingle*0.8 &&(CCD_M[i + 4] - CCD_M[i + 1]) > EYELightSingle*0.8)
            {
                giRoadSitYR = i;
                break;
            }
        }
      }
      
      else if(giExpectPlaceUpdate>90&&giExpectPlaceUpdate<=102)
      {
        for (i =giExpectPlaceUpdate-20; i <=giExpectPlaceUpdate+20; i++)//下降沿为黑
        {
            if ((CCD_M[i -3] - CCD_M[i]) > EYELightSingle*0.6)
            {
                giRoadSitYL = i;
                break;        
             }       
             
        }                   
    
        for (i = giExpectPlaceUpdate+18; i >=giExpectPlaceUpdate-20; i--)//下降沿为黑
        {
            if ((CCD_M[i + 6] - CCD_M[i]) > EYELightSingle*0.4)
            {
                giRoadSitYR = i;
                break;
            }
        }
      }
      
      else if(giExpectPlaceUpdate>102&&giExpectPlaceUpdate<=115)
      {
        for (i =giExpectPlaceUpdate-15; i <=giExpectPlaceUpdate+12; i++)//下降沿为黑
        {
            if ((CCD_M[i -6] - CCD_M[i]) > EYELightSingle*0.6)
            {
                giRoadSitYL = i;
                break;        
             }       
             
        }                   
    
        for (i = 121; i >=giExpectPlaceUpdate-15; i--)//下降沿为黑
        {
            if ((CCD_M[i + 6] - CCD_M[i]) > EYELightSingle*0.4)
            {
                giRoadSitYR = i;
                break;
            }
        }
      }
      
      else if(giExpectPlaceUpdate>115&&giExpectPlaceUpdate<122)
      {
        for (i =giExpectPlaceUpdate-15; i <=127; i++)//下降沿为黑
        {
            if ((CCD_M[i -6] - CCD_M[i]) > EYELightSingle*0.4)
            {
                giRoadSitYL = i;
                break;        
             }       
        }                   
    
        for (i = 121; i >=giExpectPlaceUpdate-15; i--)//下降沿为黑
        {
            if ((CCD_M[i + 6] - CCD_M[i]) > EYELightSingle*0.4)
            {
                giRoadSitYR = i;
                break;
            }
        }
      }
      
      /*else if(giExpectPlaceUpdate>=122)
      {
        for (i =giExpectPlaceUpdate-15; i <=127; i++)//下降沿为黑
        {
            if ((CCD_M[i -3] - CCD_M[i]) > EYELightSingle*0.5)
            {
                giRoadSitYL = i;
                break;        
             }       
        }                   
    
        for (i = 124; i >=giExpectPlaceUpdate-15; i--)//下降沿为黑
        {
            if ((CCD_M[i + 3] - CCD_M[i]) > EYELightSingle*0.4)
            {
                giRoadSitYR = i;
                break;
            }
        }
      }*/
      
      /******************************************************/
      else if(giExpectPlaceUpdate>30&&giExpectPlaceUpdate<40)
      {
        for (i =giExpectPlaceUpdate-20; i <=giExpectPlaceUpdate+20; i++)//下降沿为黑
        {
            if ((CCD_M[i - 3] - CCD_M[i]) > EYELightSingle*0.7)
            {
                giRoadSitYL = i;
                break;        
             }       
        }                   
    
        for (i = giExpectPlaceUpdate+20; i >=giExpectPlaceUpdate-20; i--)//下降沿为黑
        {
            if ((CCD_M[i + 3] - CCD_M[i]) > EYELightSingle*0.7)
            {
                giRoadSitYR = i;
                break;
            }
        }
      }
      
      else if(giExpectPlaceUpdate>23&&giExpectPlaceUpdate<=30)
      {
        for (i =giExpectPlaceUpdate-15; i <=giExpectPlaceUpdate+15; i++)//下降沿为黑
        {
            if ((CCD_M[i - 3] - CCD_M[i]) > EYELightSingle*0.5)
            {
                giRoadSitYL = i;
                break;        
             }       
        }                   
    
        for (i = giExpectPlaceUpdate+15; i >=giExpectPlaceUpdate-15; i--)//下降沿为黑
        {
            if ((CCD_M[i + 3] - CCD_M[i]) > EYELightSingle*0.5)
            {
                giRoadSitYR = i;
                break;
            }
        }
      }
      
      else if(giExpectPlaceUpdate>6&&giExpectPlaceUpdate<=23)
      {
        for (i =3; i <=giExpectPlaceUpdate+20; i++)//下降沿为黑
        {
            if ((CCD_M[i -3] - CCD_M[i]) > EYELightSingle*0.5)
            {
                giRoadSitYL = i;
                break;        
             }       
        }                   
    
        for (i = giExpectPlaceUpdate+20; i >=0; i--)//下降沿为黑
        {
            if ((CCD_M[i + 3] - CCD_M[i]) > EYELightSingle*0.5)
            {
                giRoadSitYR = i;
                break;
            }
        }
      }
      
      else if(giExpectPlaceUpdate<=6)
      {
        for (i =3; i <=giExpectPlaceUpdate+20; i++)//下降沿为黑
        {
            if ((CCD_M[i -3] - CCD_M[i]) > EYELightSingle*0.4)
            {
                giRoadSitYL = i;
                break;        
             }       
        }                   
    
        for (i = giExpectPlaceUpdate+20; i >=0; i--)//下降沿为黑
        {
            if ((CCD_M[i + 3] - CCD_M[i]) > EYELightSingle*0.5)
            {
                giRoadSitYR = i;
                break;
            }
        }
      }
        /*********纠错************/
        if (giRoadSitYR < 0 )
        {
             giRoadSitYR = 0;
        }
        else if(giRoadSitYR > 127)
        {
            giRoadSitYR = 0;
        }
        if (giRoadSitYL > 127)
        {
            giRoadSitYL = 0;
        }
        else if(giRoadSitYL < 0 )
        {
            giRoadSitYL = 0;
        }
           
        //这里对于黑线宽度的限制是否太宽松了？实际测量也就5左右？
        if(giRoadSitYL!=0&&giRoadSitYR!=0&&(giRoadSitYR-giRoadSitYL)<=10&&(giRoadSitYR-giRoadSitYL)>=0)
        {
            giRoadSitY=(giRoadSitYL+giRoadSitYR)*0.5;
            gcSingleLostFlagL=0;
            gcSingleLostFlagR=0;
        }
        
        if(giRoadSitYL!=0&&giRoadSitYR!=0&&(giRoadSitYR-giRoadSitYL)>15)
        {
	        /*for (i =giRoadSitYL+3; i <=giRoadSitYR; i++)//下降沿为黑
	        {
	            if ((CCD_M[i -3] - CCD_M[i]) > EYELightSingle)
	            {
	                giRoadSitSL = i;
	                break;        
	             }
	             
	             else
	             {
	             	giRoadSitSL=0;
	             }
	                    
	        }
	        
		for (i =giRoadSitYR-3; i >=giRoadSitYL; i--)//下降沿为黑
	        {
	            if ((CCD_M[i + 3] - CCD_M[i]) > EYELightSingle)
	            {
	                giRoadSitSR = i;
	                break;
	            }
	            
	            else
	            {
	            	giRoadSitSR=0;
	            }
	        }*/
	       if(abs(giRoadSit[1]-giRoadSitYL)>abs(giRoadSit[1]-giRoadSitYR))
	       {
		        for (i =giRoadSitYL+3; i <=giRoadSitYR; i++)//下降沿为黑
		        {
		            if ((CCD_M[i -3] - CCD_M[i]) > EYELightSingle)
		            {
		                giRoadSitSL = i;
		                break;        
		             }
		             
		             else
		             {
		             	giRoadSitSL=0;
		             }
		                    
		        }
		        
		        if(giRoadSitSL!=0&&giRoadSitSL>giRoadSitYL&&giRoadSitSL<giRoadSitYR
		        	&&(giRoadSitYR-giRoadSitSL)<15)
		        {
		        	giRoadSitY=(giRoadSitSL+giRoadSitYR)*0.5;
		        }
	       }
	       
	       else if(abs(giRoadSit[1]-giRoadSitYL)<abs(giRoadSit[1]-giRoadSitYR))
	       {
		       	for (i =giRoadSitYR-3; i >=giRoadSitYL; i--)//下降沿为黑
		        {
		            if ((CCD_M[i + 3] - CCD_M[i]) > EYELightSingle)
		            {
		                giRoadSitSR = i;
		                break;
		            }
		            
		            else
		            {
		            	giRoadSitSR=0;
		            }
		        }
		        
		        if(giRoadSitSR!=0&&giRoadSitSR>giRoadSitYL&&giRoadSitSR<giRoadSitYR
		        	&&(giRoadSitSR-giRoadSitYL)<15)
		        {
		        	giRoadSitY=(giRoadSitSR+giRoadSitYL)*0.5;
		        }
	       }
	       
	       else
	       {
	       		giRoadSitY=giRoadSit[1];
	       }
	       
        }
        
        if(giRoadSitYL!=0&&giRoadSitYR==0&&giRoadSitY_Old!=0
        &&SingleLine==1&&giRoadSitYL<=120&&giRoadSitYL>=3
        &&CCD_M[giRoadSitYL-3]-CCD_M[giRoadSitYL+3]>EYELightSingle*0.5
        &&CCD_M[giRoadSitYL-3]-CCD_M[giRoadSitYL+5]>EYELightSingle*0.5
        &&CCD_M[giRoadSitYL-3]-CCD_M[giRoadSitYL+7]>EYELightSingle*0.5)//单线丢线处理
        {
        	giRoadSitY=giRoadSitYL;//5待测
        	gcSingleLostFlagR=1;
        	gcSingleLostFlagL=0;
        }
        
        if(giRoadSitYL==0&&giRoadSitYR!=0&&giRoadSitY_Old!=0
        &&SingleLine==1&&giRoadSitYR<=124&&giRoadSitYR>=7
        &&CCD_M[giRoadSitYR+3]-CCD_M[giRoadSitYR-3]>EYELightSingle*0.5
        &&CCD_M[giRoadSitYR+3]-CCD_M[giRoadSitYR-5]>EYELightSingle*0.5
        &&CCD_M[giRoadSitYR+3]-CCD_M[giRoadSitYR-7]>EYELightSingle*0.5)//单线丢线处理
        {
        	giRoadSitY=giRoadSitYR;//5待测
        	gcSingleLostFlagL=1;
        	gcSingleLostFlagR=0;
        }
        
        if(giRoadSitYL==0&&giRoadSitYR==0&&SingleLine==1&&giRoadSitY_Old!=0
        &&giRoadEdgeL[0]==0&&giRoadEdgeR[0]==127)
        {
        	giRoadSitY=giRoadSitY_Old;
        	//SIU.GPDO[18].R=1;
        }
        
       /* else if(giRoadSitYL==0&&giRoadSitYR==0&&SingleLine==1&&giRoadSitY_Old!=0
        &&gcSingleLostFlagL==0&&gcSingleLostFlagR==1)
        {
        	giRoadSitY=giRoadSitY_Old;
        	//SIU.GPDO[18].R=1;
        }
        
        else
        {
        	//SIU.GPDO[18].R=0;
        }*/
            
        if(giRoadSitY!=0)//单边giRoadSitY!=0
        {
            SingleLine=1;//单边标志
            giLostFlagL=0;//直角左边丢计数
            giLostFlagR=0;//直角右边丢计数
         	//TEXT2=giRoadSitY;
            TEXT1=11;
            
            //SIU.GPDO[18].R=1;
	        if(giRoadSitY>50&&giRoadSitY<70)
	        {
	           giRoadSit[0]=giRoadSitY;
	        }
	            
	        if(giRoadSitY>45&&giRoadSitY<=50)
	        {
	           giRoadSit[0]=giRoadSitY-1;
	        }
	            
	        if(giRoadSitY>40&&giRoadSitY<=45)
	        {
	           giRoadSit[0]=giRoadSitY-2;
	        }
	            
	        if(giRoadSitY>35&&giRoadSitY<=40)
	        {
	           giRoadSit[0]=giRoadSitY-3;
	        }
	            
	        if(giRoadSitY>25&&giRoadSitY<=35)
	        {
	           giRoadSit[0]=giRoadSitY-4;
	        }
	            
	        if(giRoadSitY>15&&giRoadSitY<=25)
	        {
	           giRoadSit[0]=giRoadSitY-5;
	        }
	            
	        if(giRoadSitY>=0&&giRoadSitY<=15)
	        {
	           giRoadSit[0]=giRoadSitY-6;
	        }
	            
	        if(giRoadSitY>=70&&giRoadSitY<75)
	        {
	           giRoadSit[0]=giRoadSitY+1;
	        }
	            
	        if(giRoadSitY>=75&&giRoadSitY<80)
	        {
	           giRoadSit[0]=giRoadSitY+2;
	        }
	  
	        if(giRoadSitY>=80&&giRoadSitY<85)
	        {
	           giRoadSit[0]=giRoadSitY+3;
	        }
	            
	        if(giRoadSitY>=85&&giRoadSitY<90)
	        {
	           giRoadSit[0]=giRoadSitY+4;
	        }
	            
	        if(giRoadSitY>=90&&giRoadSitY<100)
	        {
	           giRoadSit[0]=giRoadSitY+5;
	        }
	            
	        if(giRoadSitY>=100&&giRoadSitY<110)
	        {
	           giRoadSit[0]=giRoadSitY+6;
	        }
	            
	        if(giRoadSitY>=110&&giRoadSitY<127)
	        {
	           giRoadSit[0]=giRoadSitY+7;
	        }          
            giRoadSit[0]=giRoadSit[0];
            giRoadEdgeL[0]=giRoadSit[0]-HalfWidth;
            giRoadEdgeR[0]=giRoadSit[0]+HalfWidth;
            
            if (giRoadEdgeL[0] <= 0 )
            {
                 giRoadEdgeL[0] = 0;
            }
            if (giRoadEdgeR[0] >= 127)
            {
                giRoadEdgeR[0] = 127;
            }
            //TEXT2=11;
        	giExpectPlaceUpdate=giRoadSit[0];   
        }
        else if(giRoadSitY==0)
        {
            FindEdgeFar();
            FindEdge();
            gcSingleLostFlagL=0;
        	gcSingleLostFlagR=0;
        	
        	if(giRoadSitY_Old!=0&&giRoadSitY==0
        	&&(giRoadSitY_Old-giRoadSit[0])>=10&&(giRoadSitY_Old-giRoadSit[0])<=-10)
        	{
        		giRoadSit[0]=giRoadSitY_Old;
        	}
        	
            /************强制转弯标志位清除,改用CCD控制*********/
            if(TurnLeft==1)
            { 
              for(i=6;i<127;i++)
              {
                if((CCD_M[i-3]-CCD_M[i])>EYELight&&(CCD_M[i-4]-CCD_M[i-1])>EYELight)
                {
                      giAnR = i;
                      break;
                }
                else
                {
                      giAnR = 0;
                }
              }
              
              if(giAnR!=0 && giAnR!=127 && giAnR <90 && giAnR_Old<90 && giAnR_Old>giAnR)// && giAnR_Old<=50 && giAnR_Old<giAnR//&&giAnR_Old<90 && giAnR_Old>giAnR
              {
                 giRoadEdgeR[0]=giAnR;
                 giRoadSit[0]=giRoadEdgeR[0]-HalfWidth;
                 TurnLeft=0;
                 //giLostFlagFarR=0;
                 giLostFlagFarL=0;
                 giLostFlagL=0;
                 TurnFlagL=0;
                 giAnR=0;
                 TurnTest=0;
              }
              
               else
               {
                 giRoadEdgeR[0]=giRoadEdgeR[0];
               }
               
              FarJudgeL=0;
              FarJudgeR=0;
              LostFarR=0;
              LostFarL=0;
            }
			
			/************强制转弯标志位清除,改用CCD控制*********/
            if(TurnRight==1)
           {
             for(i=121;i>=0;i--)
              {
                   if((CCD_M[i + 3]-CCD_M[i])>EYELight && (CCD_M[i + 4]-CCD_M[i + 1])>EYELight)
                   {
                      giAnL = i;
                      break;
                   }
                   
                   else
                   {
                   		giAnL=0;
                   }
               }        
               
         
               if(giAnL!=0 && giAnL!=127 && giAnL>40&& giAnL_Old>40 && giAnL_Old<giAnL)// && giAnL_Old>80 && giAnL_Old>giAnL//&& giAnL_Old>40 && giAnL_Old<giAnL
               {
                 giRoadEdgeL[0]=giAnL;
                 giRoadSit[0]=giRoadEdgeL[0]+HalfWidth;
                 TurnRight=0;
                 giLostFlagFarR=0;
                 //giLostFlagFarL=0;
                 giLostFlagR=0;
                 TurnFlagR=0;
                 giAnL=0;
                 TurnTest=0;
               }
             
             else
               {
                 giRoadEdgeL[0]=giRoadEdgeL[0];
               }
               
              FarJudgeL=0;
              FarJudgeR=0;
              LostFarR=0;
              LostFarL=0;
           } 
           TEXT1=22;
           giAnR_Old=giAnR;
           giAnL_Old=giAnL;
        }
    }
    
    else if(giExpectPlaceUpdate>=122)//||giExpectPlaceUpdate<=6
    {
      		FindEdgeFar();
      		FindEdge();
    }
    
    giRoadSitYL_Old=giRoadSitYL;
    giRoadSitYR_Old=giRoadSitYR;
    giRoadSitY_Old=giRoadSitY;
    
    if(giRoadSit[0]>=127)
    {
        giRoadSit[0]=127;
    }
    if(giRoadSit[0]<=0)
    {
        giRoadSit[0]=0;
    }
    
    giRoadSit[0]=giRoadSit[0]*0.8+giRoadSit[1]*0.15+giRoadSit[2]*0.05;//平滑中心
       
    for (j = 19; j > 0; j--) //记历史
    {
        giRoadEdgeL[j] = giRoadEdgeL[j - 1];  
        
        giRoadEdgeR[j] = giRoadEdgeR[j - 1];
        
        giRoadSit[j]=giRoadSit[j-1];
    }
}