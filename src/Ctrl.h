/**********************************************************
Copyright (B), 2014-2015, TYUT JBD TEAM B.
File name: Ctrl.h
Author: TEAM B
Version: 1.0              Date: 2015.4.2
Description: 
Others:
Function List:
History: Helium				2015.4.2
***********************************************************/
#ifndef _CTRL_H_
#define _CTRL_H_

/*********最大最小角度限制***********/
#define MAX_ANGLE  20                     
#define MIN_ANGLE  -18

//键盘输入变量
extern int giAp;
extern int giAd;
extern int giSpExpedite;
extern int giSi;
extern int giSd;
extern int giDp;
extern int giDd;
extern int giDesirDd;
extern int giDk;
extern int giDaa;
extern int giDpp;
extern int giDesirSpeed;
extern int giFlagStart;
extern int giError;

extern float gfAnglePWM;
extern float gfDirection_PWM_L;
extern float gfDirection_PWM_R;
extern float gfDirectCtrlOutL;
extern float gfDirectCtrlOutR;
extern float gfDirection_AngularVelocity;
extern float gfDirection_PWMOld_L;//上次PWML
extern float gfDirection_PWMOld_R;//上次PWMR

extern float gfSpeedPWM;
extern float gfSpeedPWMOld;
extern float gfD_Gyro;
extern int giCircle;
extern int giS_constant;
extern int giV_constant;
extern float gfSpeedControlOut;
extern int TurnTest; 
extern int TurnPeriod;

#endif