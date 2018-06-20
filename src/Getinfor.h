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
#define EYELight 200//��У��//40//����
#define EYELightF 250//��У��//35//����
#define EYELightSingle 200//��У��//�����ߵ���ֵ//40//����
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
extern char STARTFLAG;//����������־
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
extern int FarJudgeL;//Ԥ����߶�ʧֱ��
extern int FarJudgeR;//Ԥ���ұ߶�ʧֱ��
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
extern char TempFlag;//��־λ
extern int giBlockBlocL;
extern int giBlockBlocR;

extern char gcSingleLostFlagL;
extern char gcSingleLostFlagR;
extern int giLostFlagL;
extern int giLostFlagR;
extern int giRoadSitSL;
extern int giRoadSitSR;
extern int giAnL;//ֱ����߱�־
extern int giAnR;//ֱ���ұ߱�־
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