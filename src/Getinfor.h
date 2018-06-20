/**********************************************************
Copyright (B), 2014-2015, TYUT JBD TEAM B.
File name: Getinfor.h 
Author: TEAM B
Version: 1.0              Date: 2015.4.2
Description: 
Others:
Function List:
History: Helium				2015.4.2
***********************************************************/
#ifndef _GETINFOR_H_
#define _GETINFOR_H_

#define Position_Center 64
#define gfAngleSpeedCenter 0
#define EYELight 200//新校区//40//北区
#define EYELightF 250//新校区//35//北区
#define EYELightSingle 200//新校区//中心线的阈值//40//北区
#define HalfADFar 140

#define HalfWidth 35
#define HalfWidthFar 23 
#define HalfWidthNear 53

extern float gfSendRed1;
extern float gfSendGreen1;
extern float gfAngleAD;
extern float gfAngleSpeedAD;
extern float gfAngle;
extern float gfAngleSpeedCurrent;
extern float gfAngleCenter;
extern char STARTFLAG;//过零启动标志
extern int giRoadEdgeL[20];
extern int giRoadEdgeR[20];
extern int giRoadSit[20];

extern int FlagL;
extern int FlagR;
extern int giWhiteCountL,giWhiteCountR;
extern int giFlagCurveL;
extern int giFlagCurveR;
extern int TurnLeft;
extern int TurnRight;
extern int LostFarL;
extern int LostFarR;
extern int FarJudgeL;//预判左边丢失直角
extern int FarJudgeR;//预判右边丢失直角
extern int giLostFlagFarL;
extern int giLostFlagFarR;
extern int giAnR;
extern int giAnL;

extern char giRoadEdgeFarL[20];
extern char giRoadEdgeFarR[20];
extern char giRoadSitFar[20];
extern int giWhiteCountFarL,giWhiteCountFarR;
extern int giExpectPlaceUpdateFar;
extern int SingleLine;
extern int giExpectPlaceUpdate;
extern int giRoadSitYL;
extern int giRoadSitYR;

extern int CCD_M[128];
extern int CCD_H[128];
extern int giRoadSitY;
extern int giRoadSitY_Old;
extern char TempFlag;//标志位
extern int giBlockBlocL;
extern int giBlockBlocR;

extern char gcSingleLostFlagL;
extern char gcSingleLostFlagR;
extern int giLostFlagL;
extern int giLostFlagR;
extern int giRoadSitSL;
extern int giRoadSitSR;
extern int giAnL;//直角左边标志
extern int giAnR;//直角右边标志
extern int giAnL_Old;
extern int giAnR_Old;

extern int gihindEdgeL;
extern int gihindEdgeR;
extern float gfActualCurvature;
extern int giPreLostEdgeL;
extern int giPreLostEdgeR;

extern int SwitchStop;
extern int TurnFlagL;
extern int TurnFlagR;
//extern int TurnRight;
//extern int TurnLeft;

#endif