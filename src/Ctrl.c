/**********************************************************
Copyright (B), 2014-2015, TYUT JBD TEAM B.
File name: Ctrl.c 
Author: TEAM B
Version: 1.0              Date: 2015.4.2
Description: 
Others:
Function List:
History: Helium				2015.4.2
***********************************************************/
#include "Declare.h"

/**************全局变量定义***************/
int giFlagStart = 0;                               								//直立发车延时标志
float gfAnglePWM = 0;
float gfSpeedPWM=0;
float gfSpeedPWMOld=0;
/***************控制PID参数***************/
int giAp=0;//直立P
int giAd=0;//直立D
int giSpExpedite=0;//键盘输入速度P
int giS_constant=0;//常速期最大速度P
int giV_constant=0;//常速期最小速度P
int giSi=0;//速度I
int giSd=0;//速度D
int giDp=0;//方向P
int giDd=0;//方向D
int giDesirDd=0;//D期望
int giDk=0;//方向K
int giDaa=0;//角速度微分项
int giDpp=0;//直角转向
int giDesirSpeed=0;//期望速度
int giError=0;
int gun=0;
int dan=0;
int giCircle=0;
int snailcount=0;
/******速度偏差转化角度******/
float gfAngle_Err_I = 0; 
float gfSpeed_Err_Old = 0; 
float gfAngle_Err_New=0;

/*********方向控制**********/
float gfDirection_PWM_L = 0;//方向计算输出PWM
float gfDirection_PWM_R = 0;

float gfDirection_Old_L = 0;//上次方向计算输出PWM    
float gfDirection_Old_R = 0;

float gfDirection_AngularVelocity=0;//当前转向角速度，右转负，左转正
char  gnDirectCtrlPeriod=0;
char  gnSpeedCtrlPeriod=0;

int TurnTest=0;//固定偏差保持周期
int TurnPeriod=0;//直角转向周期
/**************直立控制*************************************
*  函数名称：AngleControl
*  功能说明：计算输出直立PWM
*  参数说明：
*  函数返回：无
*  修改时间：2015-1-8
*  备    注：
			(局部变量)
					lfAngle_Err_CENTER：速度差转化为角度差
			(全局变量)
					giFlagStart：直立发车延时
					gfAnglePWM：直立控制PWM
					giAp:直立控制比例
					giAd:直立控制微分
*************************************************************/
void AngleControl (void) 
{	
   gfAnglePWM = (gfAngle) * giAp + gfAngleSpeedCurrent * giAd * 0.01;  //得到角度PWM值 
}

/*************************************************
Function: AngleErrCalculate
Description: 速度控制偏角计算
Calls:   无
Called By: PORTB2_IRQHandler
           PORTA13_IRQHandler
Table Accessed: 无
Table Updated: 无
Input: giDesirSpeed
       gfCurrentSpeed
       giSpExpedite
       giSpConstant
       giSi
       giSd
Output: gfAngleErrCenter 
Return: 无
Others:无
*************************************************/
static float gfSpeedErrOld = 0;//上次速度偏差值
float gfSpeedControlOut=0;
void AngleErrCalculate(void)
{ 
    int liSp = 0;                                            //速度P参数项 用做P值切换
    float lfAngleErrP = 0;                                   //速度控制P项
    float lfAngleErrD = 0;                                   //速度控制D项
    float lfSpeedErrNew = 0;                                 //此次速度偏差值       
    //static float gfSpeedPWMOld = 0;
    int lispeed=0;
    
    gnSpeedCtrlPeriod=0;
    
    /**/
    if(dan==0)
    {
    	lispeed=0;
    }
    
    else if(dan==1)
    {
    	if(snailcount<timer)
    	{
    		snailcount++;
    		lispeed=0;
    	}
    	if(snailcount>=timer)
    	{
    		snailcount=timer;
    		lispeed=giDesirSpeed;
    	}
    }
    
    else if(dan==2)
    {
    	lispeed=giDesirSpeed;
    }
    
    /*if(gfCurrent_Speed>=giDesirSpeed)
    {
    	SIU.GPDO[18].R=1;
    }
    
    else if(gfCurrent_Speed<giDesirSpeed)
    {
    	SIU.GPDO[18].R=0;
    }*/
    
    lfSpeedErrNew = lispeed-gfCurrent_Speed;
    
    if (STARTFLAG == 1)
    {
        if(lfSpeedErrNew < 45)
        {
            /*if(giV_constant<=giS_constant)
            {
            	liSp=giV_constant+(giS_constant-giV_constant)*0.0222*abs(lfSpeedErrNew);
            	
            	if(liSp>giS_constant)
            	{
            		liSp=giS_constant;
            	}
            	
            	if(liSp<giV_constant)
            	{
            		liSp=giV_constant;
            	}
            }
            
            else if(giV_constant>giS_constant)
            {
            	liSp=giS_constant+(giV_constant-giS_constant)*0.0222*abs(lfSpeedErrNew);
            	
            	if(liSp>giV_constant)
            	{
            		liSp=giV_constant;
            	}
            	
            	if(liSp<giS_constant)
            	{
            		liSp=giS_constant;
            	}
            }*/
            
            liSp = giSpExpedite; //giS_constant                                       
            gfAngle_Err_I += (giSi) * lfSpeedErrNew * 0.001; 
          
        }
        else
        { 
            liSp=giSpExpedite;//切换速度P参数giSpExpedite
            gfAngle_Err_I=0;
        }//小偏差范围内应用I项
    }

    lfAngleErrP = liSp * lfSpeedErrNew * 0.1;       
    lfAngleErrD = giSd * (lfSpeedErrNew - gfSpeedErrOld) * 0.01; 
       
    if (gfAngle_Err_I > 6500)
    {
        gfAngle_Err_I = 6500;
    }
    else if (gfAngle_Err_I < -6500)
    {
        gfAngle_Err_I = -6500;
    }//速度I项限幅
                                             
    gfSpeedPWMOld=gfSpeedPWM;
    
    gfSpeedPWM = lfAngleErrP + gfAngle_Err_I + lfAngleErrD;
    
    gfSpeedErrOld=lfSpeedErrNew;

    if (STARTFLAG == 1) 
    {
        giFlagStart ++;
    }
   
    if (giFlagStart  > 80)
    {
        giFlagStart  = 80;
        
    }//延时计数                                            
   
    if (giFlagStart  < 30)
    {
        gfSpeedPWM = 0;   
       
    }
    else if(giFlagStart  < 50)
    {
        gfSpeedPWM=(giFlagStart-30)*0.05*gfSpeedPWMOld;
    }
    else
    {
        gfSpeedPWM = gfSpeedPWM;
    }//直立发车
       
   if(gfSpeedPWM<-4000)//3000
   {
       gfSpeedPWM=-4000;
   }
   else if(gfSpeedPWM>4000)
   {
       gfSpeedPWM=4000;
   }
       
}

void SpeedControlOutput(void)
{
	float fValue=0;
	fValue = gfSpeedPWM - gfSpeedPWMOld;
	gfSpeedControlOut = fValue * gnSpeedCtrlPeriod / 5 + gfSpeedPWMOld;
}
/**********************方向控制******************************
*  函数名称：DirectionControl
*  功能说明：计算输出两轮所需方向PWM
*  参数说明：
*  函数返回：方向PWM
*  修改时间：2015-1-10
*  备    注：
		(局部变量)
				lfPosition_R：右偏差
				lfPosition_L：左偏差
				lfDirection_New_R：当前右方向计算值
				lfDirection_New_L：当前左方向计算值
				lfDirection_SpeedError_New_R:右当前速度差
				lfDirection_SpeedError_New_L:左当前速度差
				lfDirection_Position_New_R:右当前速度差
				lfDirection_Position_New_L:左当前速度差
		(全局变量)
				giSpExpedite：键盘输入速度参数P
				giDesirSpeed:  期望速度
				gfCurrent_Speed：当前速度
				lfSpeed_Err_New：期望速度与当前速度差
*************************************************************/
 float gfD_Gyro=0;
 float gfDirectCtrlOutL=0;
 float gfDirectCtrlOutR=0;
 float gfDirection_PWMOld_L=0;//上次PWML
 float gfDirection_PWMOld_R=0;//上次PWMR
 float gfPositionOld=0;

 void DirectionControl(void)
 {  
   float lfPosition_R = 0;
   float lfPosition_L = 0;
   float lfDirection_New_R = 0;
   float lfDirection_New_L = 0;
   float lfDirection_AngularVelocity=0;//当前转向陀螺仪值
   gnDirectCtrlPeriod=0;
   gfD_Gyro=0;
   
   lfDirection_AngularVelocity=gfDirection_AngularVelocity;
   
   gfPosition=giRoadSit[0]-Position_Center;//去大赛道试一下偏差是多少
   //gfPosition=giCircle;				//转圆圈偏差
   /****************直角拐弯**************************/
    if(TurnLeft==1&&giDpp!=0)    
    { 	
    	gfPosition=gfPositionOld-2;
    	TurnPeriod++;
    	if(gfPosition<=-giDpp)
    	{
    		gfPosition=-giDpp;
    		TurnTest++;
    		if(TurnTest>15)
    		{
    			TurnTest=15;
    		}
    	}
    	
    	if(TurnTest==15)
    	{
    		gfPosition=gfPositionOld+2;
    		if(gfPosition>=0)
    		{
    			gfPosition=0;
    			TurnTest=0;
    			TurnLeft=0;
    		}
    	}
    	
        giDesirDd=giDd;
        //gfPosition=-giDpp;
        gfD_Gyro=giDaa;
        //SIU.GPDO[18].R=1;
    }
    else if(TurnRight==1&&giDpp!=0)
    {
    	gfPosition=gfPositionOld+2;
    	TurnPeriod++;
    	if(gfPosition>=giDpp)
    	{
    		gfPosition=giDpp;
    		TurnTest++;
    		if(TurnTest>15)
    		{
    			TurnTest=15;
    		}
    	}
    	
    	if(TurnTest==15)
    	{
    		gfPosition=gfPositionOld-2;
    		if(gfPosition<=0)
    		{
    			gfPosition=0;
    			TurnTest=0;
    			TurnRight=0;
    		}
    	}
    	
        giDesirDd=giDd;
        //gfPosition=giDpp;
        gfD_Gyro=giDaa;
        //SIU.GPDO[18].R=1;
    }
    else if(TurnLeft==0||TurnRight==0)
    {
       //SIU.GPDO[18].R=0;
       giDesirDd=giDd;
       gfD_Gyro=0;
    }
    
   lfPosition_L = gfPosition;            
   lfPosition_R = gfPosition;
   
   /*if(gfDirection_AngularVelocity>-100&&gfDirection_AngularVelocity<100)
   {
   		gfD_Gyro=(1-abs(gfDirection_AngularVelocity)*0.01)*giDaa;
   }
   
   else if(gfDirection_AngularVelocity<=-100||gfDirection_AngularVelocity>=100)
   {
   		gfD_Gyro=0;
   }
   
   if(gfD_Gyro<0)
   {
   		gfD_Gyro=0;
   }
   
   if(gfD_Gyro>giDaa)
   {
   		gfD_Gyro=giDaa;
   }*/
   gfDirection_PWMOld_L=gfDirection_PWM_L;
   gfDirection_PWMOld_R=gfDirection_PWM_R;
   
   lfDirection_New_L = (-gfDirection_AngularVelocity + lfPosition_L * gfCurrent_Speed * giDk * 0.001)* giDp * 0.01;//-gfDirection_AngularVelocity//(gfCurrent_Speed - gfCurrent_Speed_Left * 2)
   lfDirection_New_R = (gfDirection_AngularVelocity - lfPosition_R * gfCurrent_Speed * giDk * 0.001)* giDp * 0.01;//gfDirection_AngularVelocity//(gfCurrent_Speed - gfCurrent_Speed_Right* 2)
        
   gfDirection_PWM_L = lfDirection_New_L + (lfDirection_New_L - gfDirection_Old_L) * giDesirDd * 0.01-gfDirection_AngularVelocity*gfD_Gyro*0.01;//1000
   gfDirection_PWM_R = lfDirection_New_R + (lfDirection_New_R - gfDirection_Old_R) * giDesirDd * 0.01+gfDirection_AngularVelocity*gfD_Gyro*0.01;//giDd
   
   gfDirection_Old_L = lfDirection_New_L;
   gfDirection_Old_R = lfDirection_New_R;
   
   gfPositionOld=gfPosition;  	
 }

 void DirectionCtrlOutput(void) 
{ 
    float fValueL=0;
    float fValueR=0;
    
    
        fValueL =  gfDirection_PWM_L-gfDirection_PWMOld_L;
        fValueR =  gfDirection_PWM_R-gfDirection_PWMOld_R;
        
        gfDirectCtrlOutL = fValueL * gnDirectCtrlPeriod / 4 + gfDirection_PWMOld_L;
        gfDirectCtrlOutR = fValueR * gnDirectCtrlPeriod / 4 + gfDirection_PWMOld_R;
}
/**************电机控制*************************************
*  函数名称：SetMotorVoltage
*  功能说明：将PWM输出给电机控制
*  参数说明：
*  函数返回：无
*  修改时间：2015-1-8
*  备    注：
			(局部变量)
					PWMtempleft：左电机PWM
					PWMtempright：右电机PWM
			(全局变量)
					giFlagStart：直立发车延时
					gfAnglePWM：直立控制PWM
					giAp:直立控制比例
					giAd:直立控制微分
*************************************************************/
int giStopCarCount=0;

void SetMotorVoltage(void)        
{ 
    float PWMtempleft=0;
    float PWMtempright=0;
       
    if(STARTFLAG==1)															//StartLineCount是停车延时时间
    {  
    
        if(gfAngle<-40||gfAngle>40||dan==4)    							//角度在40度以外
        { 
            SIU.GPDO[35].R=0; 
            gfAngle_Err_I=0;
        }
        
        else if(dan==3)
        {
        	giStopCarCount++;
        	if(giStopCarCount>100)
        	{
        		giStopCarCount=100;
        	}
        	
        	if(giStopCarCount<100)
        	{
	        	SIU.GPDO[35].R=1;
	        	SIU.GPDO[66].R=0;
	        	SIU.GPDO[37].R=0;
	        	EMIOS_0.CH[5].CADR.R=1500;//left
		     	EMIOS_0.CH[23].CADR.R=1500;//right
        	}
        	
        	else if(giStopCarCount==100)
        	{
        		dan=4;
        	}

        }
        
        else 
        { 
            SIU.GPDO[35].R=0;
            
            gnDirectCtrlPeriod++;
            if(gnDirectCtrlPeriod>4)
            {
            	gnDirectCtrlPeriod=4;
            }
            DirectionCtrlOutput();
             
            gnSpeedCtrlPeriod++;
            if(gnSpeedCtrlPeriod>5)
            {
            	gnSpeedCtrlPeriod=5;
            }
            SpeedControlOutput();
              
            PWMtempleft=gfAnglePWM-gfSpeedControlOut-gfDirection_PWM_L+giError;
            PWMtempright=gfAnglePWM-gfSpeedControlOut-gfDirection_PWM_R;
                               
          
            if(PWMtempleft<=0)     							//获取左侧电机的PWM值
              { 
                 SIU.GPDO[35].R=1;    							//打开电机使能开关
                 SIU.GPDO[66].R=0;     						// 1正转 0反转 
                            
                 PWMtempleft=-PWMtempleft;
              }
               
            else if(PWMtempleft>0)
              {
                 SIU.GPDO[35].R=1; 
                 SIU.GPDO[66].R=1;
                       
                 PWMtempleft=4224-PWMtempleft;     
              }
                    
             if (PWMtempright<=0)          					//获取右侧电机的PWM值
             {
                SIU.GPDO[35].R=1; 
                SIU.GPDO[37].R=0; 
                           
                PWMtempright=-PWMtempright;
             }
              
             else if (PWMtempright>0)
             {
                SIU.GPDO[35].R=1; 
                SIU.GPDO[37].R=1;
                        
                PWMtempright=4224-PWMtempright;
              }
                      
             if (PWMtempleft>4220)   PWMtempleft=4220;
             if (PWMtempright>4220)  PWMtempright=4220;
             if (PWMtempleft<4)  PWMtempleft=4;
             if (PWMtempright<4) PWMtempright=4;
          
	     EMIOS_0.CH[5].CADR.R=PWMtempleft;//left
	     EMIOS_0.CH[23].CADR.R=PWMtempright;//right
	     
            }                
    	}
        else 
        {
          SIU.GPDO[35].R=0; 
          EMIOS_0.CH[5].CADR.R=0;
          EMIOS_0.CH[23].CADR.R=0;
        } 
}