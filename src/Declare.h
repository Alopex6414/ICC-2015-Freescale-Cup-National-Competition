/**********************************************************
Copyright (B), 2014-2015, TYUT JBD TEAM B.
File name: Declare.h 
Author: TEAM B
Version: 1.0              Date: 2015.4.2
Description: 
Others:
Function List:
History: Helium				2015.4.2
***********************************************************/
#ifndef _DECLARE_H_
#define _DECLARE_H_

#include "MPC5604C_M07N.h"
#include "typedefs.h"
#include "INTCInterrupts.h"
#include "string.h"
#include "math.h"
#include "stdlib.h"
#include "float.h"
#include "stdio.h"
#include "Ctrl.h"
#include "Func.h"
#include "Getinfor.h"
#include "Gyro.h"
#include "Init.h"
#include "Keyboard.h"
#include "znfat/znFAT.h"
#include "myfun.h"

#define SpeL SIU.GPDI[40].R 
#define SpeR SIU.GPDI[46].R 

extern float gfCurrent_Speed;
extern float gfCurrent_Speed_Right;
extern float gfCurrent_Speed_Left;
extern int TEXT1;
extern int TEXT2;
extern int BeTatest;
extern int SDTIME;
extern int PWMtestL;
extern int PWMtestR;
extern int gun;
extern int dan;
extern int timer;

#endif