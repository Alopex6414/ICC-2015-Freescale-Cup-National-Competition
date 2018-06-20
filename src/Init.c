/**********************************************************
Copyright (B), 2014-2015, TYUT JBD TEAM B.
File name: Init.c 
Author: TEAM B
Version: 1.0              Date: 2015.4.2
Description: 
Others:
Function List:
History: Helium				2015.4.2
***********************************************************/
#include "Declare.h"
/********PLL START*********/
void initModesAndClock(void) 								//137
{
  ME.MER.R = 0x0000001D;          						//Enable DRUN, RUN0, SAFE, RESET modes 
                                 			      		// Initialize PLL before turning it on: 
						      							// Use 1 of the next 2 lines depending on crystal frequency: 
  CGM.FMPLL_CR.R = 0x02400100;    						// 8 MHz xtal: Set PLL0 to 64 MHz    
  //CGM.FMPLL_CR.R = 0x12400100;   						// 40 MHz xtal: Set PLL0 to 64 MHz    
  ME.RUN[0].R = 0x001F0074;       						// RUN0 cfg: 16MHzIRCON,OSC0ON,PLL0ON,syclk=PLL 
  ME.RUNPC[1].R = 0x00000010; 	 						 // Peri. Cfg. 1 settings: only run in RUN0 mode 
 
  // Mode Transition to enter RUN0 mode (the follw)  		// Page138
  ME.PCTL[4].R = 0x01;
  ME.PCTL[32].R = 0x01; 	      							// ADC_0: ME.RUNPC[1] 
  ME.PCTL[48].R = 0x01;           							// LINFlex 0: select ME.RUNPC[1]  UART(SCI)
  ME.PCTL[68].R = 0x01;           							// SIUL
  ME.PCTL[72].R = 0x01;          							 // EMIOS 0:  select ME.RUNPC[1] 
  ME.PCTL[92].R = 0x01;           							// PIT, RTI: select ME_RUN_PC[1]
  ME.PCTL[44].R = 0x01;  									// I2C: select ME_RUN_PC[1]	    	
 
  ME.MCTL.R = 0x40005AF0;        						 // Enter RUN0 Mode & Key 
  ME.MCTL.R = 0x4000A50F;         						 // Enter RUN0 Mode & Inverted Key   
  while (ME.GS.B.S_MTRANS) {}     					        // Wait for mode transition to complete     
                                  									// Note: could wait here using timer and/or I_TC IRQ 
  while(ME.GS.B.S_CURRENTMODE != 4) {} 				// Verify RUN0 is the current mode 
}

void initPeriClkGen(void)            							//page128
{
  CGM.SC_DC[0].R = 0x80;           						// MPC56xxB/S: Enable peri set 1 sysclk divided by 1
  CGM.SC_DC[2].R = 0x80;          							// MPC56xxB: Enable peri set 3 sysclk divided by 1 
}

void disableWatchdog(void) 
{
  SWT.SR.R = 0x0000c520;     
  SWT.SR.R = 0x0000d928; 
  SWT.CR.R = 0x8000010A;     
}
/***********PLL END************/

/***********PWM START**********/
void initEMIOS_0(void) 
{                                           								//PAGE536
     EMIOS_0.MCR.B.GPRE = 0;            						//Divide 64 MHz sysclk by  0 for 64 MHz eMIOS clk
                                            
     EMIOS_0.MCR.B.GPREN = 1;   	        					//Enable eMIOS clock 
     EMIOS_0.MCR.B.GTBE = 1;   
     EMIOS_0.MCR.B.FRZ = 1;    
}

void setPWM_EMIOS_0ch5(void)               						//��� PWM����page534
{   
     EMIOS_0.CH[5].CADR.R = 100;           						//ռ�ձ�        
     EMIOS_0.CH[5].CBDR.R = 4223;           						//15KHz	// ����3200/64=50us,20KHz
     // EMIOS_0.CH[5].CCR.B.BSL = 0x3;	                  
     
     EMIOS_0.CH[5].CCR.B.UCPRE=0;	     						// Set channel prescaler to divide by 1(8M) 
     EMIOS_0.CH[5].CCR.B.UCPEN = 1;      						// Enable prescaler; uses default divide by 1
     EMIOS_0.CH[5].CCR.B.FREN = 1; 	     						// Freeze channel counting when in debug mode
     
     EMIOS_0.CH[5].CCR.B.EDPOL = 0;	     					// Polarity-leading edge sets output/trailing clears
     EMIOS_0.CH[5].CCR.B.MODE = 0x58;    					// Mode is OPWFMB  
     SIU.PCR[5].R = 0x0600;              							// PA[5] 
     
}
     
void setPWM_EMIOS_0ch23(void)            						//��� PWM����page534
{
    
     EMIOS_0.CH[23].CADR.R = 100;          						//ռ�ձ�        
     EMIOS_0.CH[23].CBDR.R = 4223;          					//15KHz// ����4223/64=50us,20KHz
     // EMIOS_0.CH[22].CCR.B.BSL = 0x3;	    			             
     
     EMIOS_0.CH[23].CCR.B.UCPRE=0;	      					// Set channel prescaler to divide by 1(8M) 
     EMIOS_0.CH[23].CCR.B.UCPEN = 1;      					// Enable prescaler; uses default divide by 1
     EMIOS_0.CH[23].CCR.B.FREN = 1; 	     					// Freeze channel counting when in debug mode
     
     EMIOS_0.CH[23].CCR.B.EDPOL = 0;	    					// Polarity-leading edge sets output/trailing clears
     EMIOS_0.CH[23].CCR.B.MODE = 0x58;  				 	// Mode is OPWFMB
     SIU.PCR[71].R = 0x0600;             						 	// PE[7] //71
     
     
}

void initEMIOS_0ModulusCounter1(void) 						//PA[0]ģ����������ڣ�������//PD11�������תleft
{
    EMIOS_0.CH[0].CCR.B.MODE = 0x51; 					// Mode is MCB, 
	//EMIOS_0.CH[24].CCR.B.MODE = 0x13; 					// Mode is MCB, 
	EMIOS_0.CH[0].CCR.B.BSL = 0x3; 						// Use internal counter
	EMIOS_0.CH[0].CCR.B.UCPRE=0; 						// Set channel prescaler to divide by 1
	EMIOS_0.CH[0].CCR.B.UCPEN = 1; 						// Enable prescaler; uses default divide by 1 
	//EMIOS_0.CH[24].CCR.B.FREN = 0;						// Freeze channel counting when in debug mode 
	EMIOS_0.CH[0].CCR.B.EDPOL=1; 						//Edge Select rising edge
	EMIOS_0.CH[0].CADR.R=0xffff;

	SIU.PCR[0].R = 0x0103;  								// Initialize pad for eMIOS channel Initialize pad for input
	SIU.PCR[40].R=0x0100;								//PC[8]�������ת
}

void initEMIOS_0ModulusCounter2(void) 						//PE[0]ģ����������ڣ�������//PD11�������תright
{
	EMIOS_0.CH[16].CCR.B.MODE = 0x51; 					// Mode is MCB, 
	//EMIOS_0.CH[3].CCR.B.MODE = 0x13; 					// Mode is MCB, 
	EMIOS_0.CH[16].CCR.B.BSL = 0x3; 						// Use internal counter
	EMIOS_0.CH[16].CCR.B.UCPRE=0; 						// Set channel prescaler to divide by 1
	EMIOS_0.CH[16].CCR.B.UCPEN = 1; 						// Enable prescaler; uses default divide by 1 
	//EMIOS_0.CH[16].CCR.B.FREN = 0;						// Freeze channel counting when in debug mode 
	EMIOS_0.CH[16].CCR.B.EDPOL=1; 						//Edge Select rising edge
	EMIOS_0.CH[16].CADR.R=0xffff;

	SIU.PCR[64].R = 0x0103;  								// Initialize pad for eMIOS channel Initialize pad for input
	SIU.PCR[46].R=0x0100;								//PC[14]�������ת
}

void Init_PWM(void)
{
    initEMIOS_0();
    setPWM_EMIOS_0ch5();
    setPWM_EMIOS_0ch23(); 
    initEMIOS_0ModulusCounter1();
    initEMIOS_0ModulusCounter2(); 
}
/***********PWM END**********/

/***********SCI START********/
void initUART1(void)                    								//page385 387
{
	LINFLEX_0.LINCR1.B.INIT = 1;    					// Put LINFlex hardware in init mode 
	LINFLEX_0.LINCR1.B.SLEEP= 0; 						// Configure module as LIN master & header
	LINFLEX_0.LINIBRR.B.DIV_M= 34; 						// Mantissa baud rate divider component ������������������  115200  
	LINFLEX_0.LINFBRR.B.DIV_F= 12; 						// Fraction baud rate divider comonent  ������С����������  �ο��ֲ����� //page399 
	LINFLEX_0.UARTCR.B.UART =1;   						//UART mode enable  This bit can be programmed in Initialization mode only.
	LINFLEX_0.UARTCR.B.TDFL=0;     						//00 Transmit buffer size = 1.   43    6
	LINFLEX_0.UARTCR.B.RDFL=0;     						//00 Receive buffer size = 1.//page394
	LINFLEX_0.UARTCR.B.RXEN=1;     						//Receiver Enable
	LINFLEX_0.UARTCR.B.TXEN=1;     						//Transmitter Enable
    LINFLEX_0.UARTCR.B.PCE =0;     						//Parity Control Disable
    LINFLEX_0.UARTCR.B.WL =1;     						// 1:8-bit data (or 9-bit if PCE is set)
    LINFLEX_0.LINCR1.B.INIT=0;
    SIU.PCR[18].R=0X0403;           							//TX PB[2]
    SIU.PCR[19].R=0X0103;           							//RX PB[3]	
}
/***********SCI END********/

/******INTERRUPT START*****/
void initPIT(void) 
{
  PIT.PITMCR.R = 0x00000001;      
  PIT.CH[1].LDVAL.R = 64000;        //�ж�ʱ��=(1/64)*64000=1ms
  PIT.CH[1].TCTRL.R = 0x000000003; 
  INTC.PSR[60].R = 0x01;  
  INTC_InstallINTCInterruptHandler(Pit1ISR,60,1); 
        
}

void enableIrq(void) 
{
  INTC.CPR.B.PRI = 0;          
  asm(" wrteei 1");	    	   
}
/******INTERRUPT END*****/

/******ADC START*******/
void Init_ADC(void)//page526  page623
{
     ADC.MCR.R = 0x20000000;         						// ��ʼ��ADC0              //PAGE608
     ADC.NCMR[1].R = 0x000000F0;     						// ANS4 ANS5 ANS7                 //PAGE623
     ADC.CTR[1].R = 0x00008606;      						// ����ADCת��ʱ��32MHz  //PAGE622
     ADC.MCR.B.NSTART=1;             						// ����ת��ADC0         //PAGE608
     SIU.PCR[60].R = 0x2000;         						// AD������:PD[12]��ΪANS[4] ���ٶȼ�             
     SIU.PCR[62].R = 0x2000;         						// AD������:PD[14]��ΪANS[6] CCD1
     SIU.PCR[63].R = 0x2000;         						// AD������:PD[15]��ΪANS[7] CCD2
}
/******ADC END*******/

/******GPIO START*******/
void initGPIO (void) //page339
{
	SIU.PCR[35].R = 0x0203;    							//pC[3],���ʹ���ź�   out
	SIU.PCR[37].R = 0x0203;   							//pC[5],����������   out 
	SIU.PCR[66].R = 0x0203;    							//pE[2],����������   out 
	SIU.GPDO[35].R=0;
	SIU.GPDO[37].R=0;				   				//0��ת,1��ת
	SIU.GPDO[66].R=0;

	//���Կڳ�ʼ��
	SIU.PCR[69].R = 0x0203;            					//pE[5],    ���Կ�    out
	SIU.GPDO[69].R=0;									//PE[5]���Կ�
	SIU.PCR[11].R = 0x0200;            					//pA[11],    ���Ե�    out
	SIU.GPDO[11].R=0;									//PA[11],    ���Ե�
	
	//CCD1��ʼ��
 	SIU.PCR[8].R = 0x0203;            					//PA[8],	SI
 	SIU.PCR[3].R = 0x0203;            					//PA[3],	CLK
 	SIU.GPDO[8].R=0;								    //PA[8],	SI	��ʼ��Ϊ�͵�ƽ
	SIU.GPDO[3].R=0;								    //PA[3],	CLK	��ʼ��Ϊ�͵�ƽ
 	
 	//CCD2��ʼ��
 	SIU.PCR[6].R = 0x0203;            					//PA[6],	SI
 	SIU.PCR[34].R = 0x0203;            					//PC[2],	CLK
 	SIU.GPDO[6].R=0;									//PA[6],	SI	��ʼ��Ϊ�͵�ƽ
	SIU.GPDO[34].R=0;									//PC[2],	CLK	��ʼ��Ϊ�͵�ƽ 
	
 	SIU.PCR[18].R=0x0203;								//PB[2],	������
	SIU.GPDO[18].R=0;									//PB[2],	�������ر�
	
	SIU.PCR[17].R=0x0100;
	SIU.PCR[74].R=0x0203;
	SIU.GPDO[74].R=1;
	
	SIU.PCR[42].R=0x0100;
	SIU.PCR[75].R=0x0203;
	SIU.GPDO[75].R=1;
	
	SIU.PCR[1].R=0x0100;
	SIU.PCR[72].R=0x0203;
	SIU.GPDO[72].R=1;
}
/******GPIO END*******/
/****ALL START******/
void Init_All(void)
{
    initModesAndClock();  								// MPC56xxP/B/S: Initialize mode entries, set sysclk = 64 MHz
    initPeriClkGen(); 
    disableWatchdog();    									// Disable watchdog
  
    Init_PWM();
    initGPIO();           			
    Init_ADC();        		
    //initUART1();										//����������ʱ���ô���
    InitL3G4200D();             			
    
}
/****ALL END******/

/*****ALL INTERRUPT START****/
void Init_interrupt(void)
 {
	initPIT();
	enableIrq();
 }
/*****ALL INTERRUPT END******/
        