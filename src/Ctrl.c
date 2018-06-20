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

/**************ȫ�ֱ�������***************/
int giFlagStart = 0;                               								//ֱ��������ʱ��־
float gfAnglePWM = 0;
float gfSpeedPWM=0;
float gfSpeedPWMOld=0;
/***************����PID����***************/
int giAp=0;//ֱ��P
int giAd=0;//ֱ��D
int giSpExpedite=0;//���������ٶ�P
int giS_constant=0;//����������ٶ�P
int giV_constant=0;//��������С�ٶ�P
int giSi=0;//�ٶ�I
int giSd=0;//�ٶ�D
int giDp=0;//����P
int giDd=0;//����D
int giDesirDd=0;//D����
int giDk=0;//����K
int giDaa=0;//���ٶ�΢����
int giDpp=0;//ֱ��ת��
int giDesirSpeed=0;//�����ٶ�
int giError=0;
int gun=0;
int dan=0;
int giCircle=0;
int snailcount=0;
/******�ٶ�ƫ��ת���Ƕ�******/
float gfAngle_Err_I = 0; 
float gfSpeed_Err_Old = 0; 
float gfAngle_Err_New=0;

/*********�������**********/
float gfDirection_PWM_L = 0;//����������PWM
float gfDirection_PWM_R = 0;

float gfDirection_Old_L = 0;//�ϴη���������PWM    
float gfDirection_Old_R = 0;

float gfDirection_AngularVelocity=0;//��ǰת����ٶȣ���ת������ת��
char  gnDirectCtrlPeriod=0;
char  gnSpeedCtrlPeriod=0;

int TurnTest=0;//�̶�ƫ�������
int TurnPeriod=0;//ֱ��ת������
/**************ֱ������*************************************
*  �������ƣ�AngleControl
*  ����˵�����������ֱ��PWM
*  ����˵����
*  �������أ���
*  �޸�ʱ�䣺2015-1-8
*  ��    ע��
			(�ֲ�����)
					lfAngle_Err_CENTER���ٶȲ�ת��Ϊ�ǶȲ�
			(ȫ�ֱ���)
					giFlagStart��ֱ��������ʱ
					gfAnglePWM��ֱ������PWM
					giAp:ֱ�����Ʊ���
					giAd:ֱ������΢��
*************************************************************/
void AngleControl (void) 
{	
   gfAnglePWM = (gfAngle) * giAp + gfAngleSpeedCurrent * giAd * 0.01;  //�õ��Ƕ�PWMֵ 
}

/*************************************************
Function: AngleErrCalculate
Description: �ٶȿ���ƫ�Ǽ���
Calls:   ��
Called By: PORTB2_IRQHandler
           PORTA13_IRQHandler
Table Accessed: ��
Table Updated: ��
Input: giDesirSpeed
       gfCurrentSpeed
       giSpExpedite
       giSpConstant
       giSi
       giSd
Output: gfAngleErrCenter 
Return: ��
Others:��
*************************************************/
static float gfSpeedErrOld = 0;//�ϴ��ٶ�ƫ��ֵ
float gfSpeedControlOut=0;
void AngleErrCalculate(void)
{ 
    int liSp = 0;                                            //�ٶ�P������ ����Pֵ�л�
    float lfAngleErrP = 0;                                   //�ٶȿ���P��
    float lfAngleErrD = 0;                                   //�ٶȿ���D��
    float lfSpeedErrNew = 0;                                 //�˴��ٶ�ƫ��ֵ       
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
            liSp=giSpExpedite;//�л��ٶ�P����giSpExpedite
            gfAngle_Err_I=0;
        }//Сƫ�Χ��Ӧ��I��
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
    }//�ٶ�I���޷�
                                             
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
        
    }//��ʱ����                                            
   
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
    }//ֱ������
       
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
/**********************�������******************************
*  �������ƣ�DirectionControl
*  ����˵������������������跽��PWM
*  ����˵����
*  �������أ�����PWM
*  �޸�ʱ�䣺2015-1-10
*  ��    ע��
		(�ֲ�����)
				lfPosition_R����ƫ��
				lfPosition_L����ƫ��
				lfDirection_New_R����ǰ�ҷ������ֵ
				lfDirection_New_L����ǰ�������ֵ
				lfDirection_SpeedError_New_R:�ҵ�ǰ�ٶȲ�
				lfDirection_SpeedError_New_L:��ǰ�ٶȲ�
				lfDirection_Position_New_R:�ҵ�ǰ�ٶȲ�
				lfDirection_Position_New_L:��ǰ�ٶȲ�
		(ȫ�ֱ���)
				giSpExpedite�����������ٶȲ���P
				giDesirSpeed:  �����ٶ�
				gfCurrent_Speed����ǰ�ٶ�
				lfSpeed_Err_New�������ٶ��뵱ǰ�ٶȲ�
*************************************************************/
 float gfD_Gyro=0;
 float gfDirectCtrlOutL=0;
 float gfDirectCtrlOutR=0;
 float gfDirection_PWMOld_L=0;//�ϴ�PWML
 float gfDirection_PWMOld_R=0;//�ϴ�PWMR
 float gfPositionOld=0;

 void DirectionControl(void)
 {  
   float lfPosition_R = 0;
   float lfPosition_L = 0;
   float lfDirection_New_R = 0;
   float lfDirection_New_L = 0;
   float lfDirection_AngularVelocity=0;//��ǰת��������ֵ
   gnDirectCtrlPeriod=0;
   gfD_Gyro=0;
   
   lfDirection_AngularVelocity=gfDirection_AngularVelocity;
   
   gfPosition=giRoadSit[0]-Position_Center;//ȥ��������һ��ƫ���Ƕ���
   //gfPosition=giCircle;				//תԲȦƫ��
   /****************ֱ�ǹ���**************************/
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
/**************�������*************************************
*  �������ƣ�SetMotorVoltage
*  ����˵������PWM������������
*  ����˵����
*  �������أ���
*  �޸�ʱ�䣺2015-1-8
*  ��    ע��
			(�ֲ�����)
					PWMtempleft������PWM
					PWMtempright���ҵ��PWM
			(ȫ�ֱ���)
					giFlagStart��ֱ��������ʱ
					gfAnglePWM��ֱ������PWM
					giAp:ֱ�����Ʊ���
					giAd:ֱ������΢��
*************************************************************/
int giStopCarCount=0;

void SetMotorVoltage(void)        
{ 
    float PWMtempleft=0;
    float PWMtempright=0;
       
    if(STARTFLAG==1)															//StartLineCount��ͣ����ʱʱ��
    {  
    
        if(gfAngle<-40||gfAngle>40||dan==4)    							//�Ƕ���40������
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
                               
          
            if(PWMtempleft<=0)     							//��ȡ�������PWMֵ
              { 
                 SIU.GPDO[35].R=1;    							//�򿪵��ʹ�ܿ���
                 SIU.GPDO[66].R=0;     						// 1��ת 0��ת 
                            
                 PWMtempleft=-PWMtempleft;
              }
               
            else if(PWMtempleft>0)
              {
                 SIU.GPDO[35].R=1; 
                 SIU.GPDO[66].R=1;
                       
                 PWMtempleft=4224-PWMtempleft;     
              }
                    
             if (PWMtempright<=0)          					//��ȡ�Ҳ�����PWMֵ
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