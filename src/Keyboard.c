/**********************************************************
Copyright (B), 2014-2015, TYUT JBD TEAM B.
File name: Keyboard.c 
Author: TEAM B
Version: 1.0              Date: 2015.4.2
Description: 
Others:
Function List:
History: Helium				2015.4.2
***********************************************************/
#include "Declare.h"

//使用端口定义
#define STB      		SIU.GPDO[68].R
#define CLK      		SIU.GPDO[44].R
#define DO       		SIU.GPDO[45].R      			//如果输入输出寄存器相同则可以用DIO代替
#define DI         		SIU.GPDI[45].R
#define DGPDO    		SIU.PCR[45].R = 0x0200     		//DIO设置为输出
#define DGPDI    		SIU.PCR[45].R = 0x0103    		//DIO设置为输入

//定义命令常量
#define WRITE_MODE_A     0x40   					//0100 0000 写数码管模式A，地址自动加一
#define WRITE_MODE_B     0x44   					//0100 0100 写数码管模式B，选定定地址
#define READ_MODE        	0x42   					//0100 0010 读取键盘命令
#define START_ADD        	0xc0  					 //1100 0000 开始地址。C0为始地址。
#define LED_CTRL         	0x88   					//1000 1100 数码管显示控制。后三位从000~111共8级辉度递增。倒数第4位：0关；1开
#define DATA_COUNT       	16
#define PRESS_DELAY      	1900
                                                                             
//功能函数声明
unsigned char ReadKeyValue(int bitnum);  				//读键值
void KeyScan();   									//键扫描函数
void InEnter(void);
int Str_to_Int(unsigned char *str);

unsigned char KeyDecode(unsigned char KeyCode);
unsigned char LedEncode(unsigned char LedCode);

//全局变量声明
unsigned char KeyValue[8];
unsigned char MyChar[8]={'-','-','-','-','-','-','-','-'};
int ValueTest;

unsigned char ShowText[8]={0x3d,0x10,0x50,0x38,0x00,0x00,0x00,0x00};  //当前显示内容
unsigned char InputString[5]={'0','0','0','0','0'};

char atemp='1';
unsigned char KeyCode;
int ConClick=0;                     	 						//连续点击标志
int PointFlag=0;                     							//小数点标志
int EnterFlag=0;                         						//回车键按下标志
unsigned char TranString[5];

//端口初始化
void PORT_init() 
{ 												//I/O端口初始化，M口负责显示输出和键盘输入
    SIU.PCR[45].R = 0x0200;     						//  PA[15]
    SIU.PCR[68].R = 0x0200;     						//PA[12]  使用内部拉电阻
    SIU.PCR[44].R = 0x0200;    						//PA[4]   上拉                                             
    CLK=1;    										//CLK，设置初始高电平
    DO=1;    										//DIO
    STB=1;    										//STB
}
/***********将字符串转化为整数型数据**************/
int Str_to_Int(unsigned char *str)
{  
	int sum=0,i=0,sign=1;
	for(i=0;i<5;i++)
	{   
		if(str[i]=='-')   {sign=-1;continue;}
		sum=sum*10+(str[i]-'0');	
	}
	sum=sum*sign;
	return sum;
}
/************将字符串转化成浮点型数据*************/
float Str_to_Float(unsigned char *str)    					//未测试（不用）
{
	float frac=0,sum=0,j=1;
	int i=0,sign=1,flag=0;
	for(i=0;i<5;i++)
	{   
		if(str[i]=='-')   {sign=-1;continue;}
		if(str[i]=='.')   {flag=1;continue;}
		if(flag==0)
		{
			sum=sum*10+(str[i]-'0');					//整数部分
		}
		else
		{
		    j=j/10;
			frac=frac+(str[i]-'0')*j;					//小数部分
		}
	}
	sum=(sum+frac)*sign;
	return sum;
}
//================================功能函数======================
//==================延迟===========
void delayms(int ms) 
{
  int ii,jj;
  if (ms<1) ms=1;
  for(ii=0;ii<ms;ii++)
  for(jj=0;jj<3338;jj++)
  {};    									//40MHz--1ms      
} 
/*void delay(int ms) 
{
  int ii,jj;
  if (ms<1) ms=1;
  for(ii=0;ii<ms;ii++)
  for(jj=0;jj<2000;jj++);          
} */
//==================送数据函数======================================
void SendData(unsigned char data) {
  int i;
  STB=0;                     								//STB低电平，允许数据操作
  for(i=0;i<8;i++)
  {
    CLK=0;                   								//CLK低电平，传输1bit
    if(data&0x01!=0)              							//0,1操作
    {
      DO=1;    
    } 
    else
    {
      DO=0;
    }
    CLK=1;
    data=data>>1;                 							//移位
  }
}

//=================显示函数===========================================
//=================显示模式A，地址自动加一模式
void DisplayA(unsigned char text)
{
  int i;
  DO=1;                   //端口准备
  CLK=1;
  STB=1;
  
  SendData(WRITE_MODE_A);       					//写数据模式
  STB=1;                   								//STB高电平
  SendData(START_ADD);          						//设置起始地址
  for(i=0;i<DATA_COUNT;i++) 
  {   												//传输显示数据，共16位
    SendData(text);
  }
  STB=1;
  SendData(LED_CTRL);             						//设置显示亮度
  STB=1;
}         

//================显示模式B，制定地址显示===============================
void DisplayB(unsigned char address,unsigned char text) 
{
  
  DO=1;                   								//端口准备
  CLK=1;
  STB=1;
  
  SendData(WRITE_MODE_B);       					//显示模式
  STB=1;                   								//STB高电平
  SendData(address);              						 //传输地址
  SendData(text);                 							//传输内容
  STB=1;  
}         

//================键盘扫描程序============================================
void KeyScan() 
{
  int i;
  STB=1;                   								//端口STB高
  SendData(READ_MODE);          						//传送读数据命令
  for (i=0;i<4;i++)
  {            										//读数据，共8位，每位代表两行六个按键
    KeyValue[i]=ReadKeyValue(8); 
//===============添加
  }
  STB=1;                   								//结束扫描，抬高STB电平
}

void KeyDo(){
    int i,j,k;
    for (i=0;i<4;i++){
      if (i==1) KeyValue[i]=KeyValue[i]|0x80;
      if (KeyValue[i]==0x01) InEnter();
      
      KeyCode=LedEncode(KeyDecode(KeyValue[i]));
      
      if ((KeyDecode(KeyValue[i])==InputString[4]) && (ConClick==0))
      {
        ConClick=1;
        delayms(PRESS_DELAY);
      }
      else
      {
        if ((KeyValue[i]!=0x00) && (KeyValue[i]!=0x80) && (KeyValue[i]!=0x01) && ((ShowText[4]==0x00)||(ShowText[4]==0x3f)))
        { 
          
          if ((KeyCode==0x80) && (PointFlag==0)){
                    
            for (k=0;k<5;k++){
              InputString[k]=InputString[k+1];
            }
            ShowText[7]=ShowText[7]|KeyCode;
            InputString[4]='.';
            PointFlag=1;
          } 
          else if(KeyCode!=0x80) 
          {           
            for (j=4;j<8;j++)
            {
              ShowText[j]=ShowText[j+1];  
            }
            ShowText[7]=KeyCode;
                    
            for (k=0;k<5;k++){
              InputString[k]=InputString[k+1];
            }
            InputString[4]=KeyDecode(KeyValue[i]);
            ConClick=0;
          }
        }
      }
    }
}

                                                                
//=================读取键值===================================
unsigned char ReadKeyValue(int bitnum) 
{
  unsigned char kt=0;
  int i=0;
  DO=1;
  DGPDI;                								//DIO设为输入口，读数据
  STB=0;                  								//STB放低，开始读数据
  for (i=0;i<bitnum;i++){         							//循环读bit
    CLK=0;                								//时钟下降沿，开始读一个bit
    
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
        asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
     asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
     asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
     asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
     asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
     asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
     asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
     asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);
    asm (nop);

        
    
    kt=kt>>1;
    
    if (DI==0)
    {
      kt=kt&0x7f;
    } 
    else
    {
      kt=kt|0x80;
    }
    CLK=1;              									//时钟上升沿，1bit读取完成
  }                          
   DGPDO;             									//设置DIO为输出口
  return kt;
}




//=======================显示程序=========================
void MyDisplay()             //双排数码管
{
int i;
unsigned char add,DisChar;
add=0xC0;
for (i=0;i<8;i++) 
{
  DisChar=ShowText[i];
  DisplayB(add,DisChar);
  add=add+0x02;
  }
}                     
/*void MyDisplay()         //单排数码管
{//四个数码管显示
    int i;
    unsigned char add,DisChar;
    add=0xC0;
 
    if(ShowText[7]!=0)
    {
        for(i=7;i>=4;i--)
        ShowText[i-4]=ShowText[i];
    }    
    
    for (i=0;i<4;i++) 
    {
      DisChar=ShowText[i];
      DisplayB(add,DisChar);
      add=add+0x02;
    }
}*/


//==================按下回车键处理========================
void InEnter(void){
int i;

for (i=0;i<4;i++){
  ShowText[i]=ShowText[i+4];
  ShowText[i+4]=0x00;
}

ValueTest=Str_to_Int(InputString);

for (i=0;i<5;i++) {
  InputString[i]='0';
}
PointFlag=0;
EnterFlag=1;  
  delayms(1500);
}

/*********************************************************************************
Name:int gCommand(unsigned char ch1,unsigned char ch2,unsigned char ch3)--赋值命令
Parameter:the name of variable that you want to chage the value(你要改变的变量的名字)
Output:get the value from the key(通过键盘输入的值)
 
**********************************************************************************/
int gCommand(unsigned char ch1,unsigned char ch2,unsigned char ch3)
{
  unsigned char comStr[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  int value=0;
  int i;
  comStr[0]=LedEncode(ch1);
  comStr[1]=LedEncode(ch2);
  comStr[2]=LedEncode(ch3);
  
  for (i=0;i<8;i++)
  {
     ShowText[i]=comStr[i];
  }
  
  EnterFlag=0;
  
  while (EnterFlag!=1)
  {
     KeyScan();
     KeyDo();
     MyDisplay();
     value=ValueTest;
  }
  
  return value;
}



//=====================键盘、显示编解码程序
unsigned char KeyDecode(unsigned char KeyCode){
   
  switch (KeyCode){
    case 0x01:return ('#');
              break;
    case 0x02:return ('0');
              break;
    case 0x04:return ('.');
              break;
    case 0x10:return ('9');
              break;
    case 0x20:return ('8');
              break;
    case 0x40:return ('7');
              break;
    case 0x81:return ('6');
              break;
    case 0x82:return ('5');
              break;
    case 0x84:return ('4');
              break;
    case 0x90:return ('3');
              break;
    case 0xa0:return ('2');
              break;
    case 0xc0:return ('1');
              break;
    default  :return ('-');
              break;
  }
}

unsigned char LedEncode(unsigned char LedCode){
  switch (LedCode){
    case '3':return (0x4f);
              break;
    case '2':return (0x5b);
              break;
    case '1':return (0x06);
              break;
    case '6':return (0x7d);
              break;
    case '5':return (0x6d);
              break;
    case '4':return (0x66);
              break;
    case '9':return (0x6f);
              break;
    case '8':return (0x7f);
              break;
    case '7':return (0x07);
              break;
    case '#':return (0x0c);
              break;
    case '0':return (0x3f);
              break;
    case '.':return (0x80);
              break;
    case 'a':return (0x77);
              break;
    case 'b':return (0x7c);
              break;
    case 'c':return (0x58);
              break;
    case 'd':return (0x5e);
              break;
    case 'e':return (0x79);
              break;
    case 'f':return (0x71);
              break;
    case 'g':return (0x6f);
              break;
    case 'h':return (0x74);
              break;
    case 'i':return (0x30);
              break;
    case 'j':return (0x1e);
              break;
    case 'l':return (0x38);
              break;
    case 'n':return (0x54);
              break;
    case 'o':return (0x5c);
              break;
    case 'p':return (0x73);
              break;
    case 'q':return (0x67);
              break;
    case 'r':return (0x50);
              break;
    case 's':return (0x6d);
              break;
    case 't':return (0x78);
              break;
    case 'u':return (0x1c);
              break;
    case 'y':return (0x6e);
              break;
    case 'A':return (0x77);
              break;
    case 'C':return (0x39);
              break;
    case 'H':return (0x76);
              break;
    case 'J':return (0x1f);
              break;
    case 'N':return (0x80);
              break;
    case 'O':return (0x3f);
              break;
    case 'U':return (0x3e);
              break;
    case 'G':return (0x3d);
              break;
    case ' ':return (0x00);
              break;
    case '-':return (0x40);
              break;
    default  :return (0x08);
              break;
  }
}
void DisplayNum(unsigned char add,unsigned char ch)  // 
{
    ch=ch+'0';
    ch=LedEncode(ch);
    DisplayB(add,ch);  
}
/*******************测试程序********************/
/***************************************************************************************************
DisplayA(0x40):所有数码管显示‘-’ 函数LedEncode中0x40对应'-'
DisplayB(0xc0,0x40):控制某个数码管显示，地址加2换一个数码管，0xc0为起始地址，参考函数MyDisplay()
sp=gCommand('a','b','1'):变量输入，先显示变量名，接着输入参数,按Enter结束本次输入   
****************************************************************************************************/
void keyboard() 
{
  unsigned char comStr[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  //int temp;
  int add=0xC0;
  char scrtest='0';
  
  PORT_init(); 
  DisplayA(0x40);      
  delayms(200);
  //SwitchStop=gCommand('s','s','s');
  giAp=150;//gCommand('a','p','p');//;//100//150//200
  giAd=1200;//gCommand('a','d','d');//;//700//900//1400
  gfAngleCenter=gCommand('a','c','t');//438
  giDesirSpeed=gCommand('s','p','d');//
  giSpExpedite=gCommand('s','p','0');//450//400
  //giS_constant=gCommand('s','p','1');//常速期最大值P
  //giV_constant=gCommand('s','p','2');//常速期最小值P
  //giSi=gCommand('s','i',' ');
  //giSd=gCommand('s','d',' ');
  giDp=gCommand('d','p','_');//100
  giDd=gCommand('d','d','_');//
  giDk=gCommand('d','-','-');//200
  giDaa=150;//gCommand('d','a','a');//陀螺仪项
  //SDTIME=gCommand('s', '-', '-');
  //giError=gCommand('e','r','r');
  giDpp=43;//gCommand('d','p','p');
  //giCircle=gCommand('c','i','r');//转圈圈固定偏差
  
  //giDpp =gCommand('d', 'p', 'p');                                   
/*****************测试程序结束*******************/
  
                                            
/* loop forever */
/* please make sure that you never leave main */
}
