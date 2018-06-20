#include "SD.h"
#include "myfun.h"
//宏定义
//--------------------------------------------------------------
#define u8 unsigned char
#define SD_SPI_Write(x) SPI_Byte(x)
#define SD_SPI_Read() SPI_Byte(0xff)
//变量定义
//--------------------------------------------------------------

u8 SD1_Addr_Mode=0; //SD1的寻址方式，1为块寻址，0为字节寻址
u8 SD1_Ver=SD_VER_ERR; //SD卡1的版本
/************************************************************/
/*                       初始化SIU                          */
/************************************************************/
void SIU_init(void)
{
	SIU.PCR[13].R = 0x0604;        //设置PA[13]为SOUT接口
	SIU.PCR[12].R = 0x0103;        //设置PA[12]为SIN接口
	SIU.PCR[14].R = 0x0604;        //设置PA[14]为SCK接口
	SIU.PCR[15].R = 0x0223;        //设置PA[15]为开漏输出，作为CS，使能内部上拉电阻
}

/*************************************************************/
/*                      初始化SPI模块                        */
/*************************************************************/
void SPI_Init(void) 
{
  	DSPI_0.MCR.R = 0x80013001;     //设置DSPI0为主模式，CS信号低有效，禁止FIFO
  	DSPI_0.CTAR[0].R = 0x3E08772B; //配置CTAR[0]，设置为每帧数据为8位，高位在前，波特率为50KHz
  	DSPI_0.MCR.B.HALT = 0x0;	     //DSPI0处于运行状态
}

/*************************************************************/
/*                    设置SPI时钟为4MHz                      */
/*************************************************************/
void SPI_4M(void) 
{ 
  	DSPI_0.MCR.B.HALT = 0x1;	     //DSPI0停止运行
  	DSPI_0.CTAR[0].R = 0x3E087721; //配置CTAR[0]，设置为每帧数据为7位，高位在前，波特率为4MHz
  	DSPI_0.MCR.B.HALT = 0x0;	     //DSPI0处于运行状态
}

/*************************************************************/
/*                      SPI读写一个字节                      */
/*************************************************************/
uint8_t SPI_Byte(uint8_t value)  // : SPI读写一个字节（发送完成后返回本次通讯读取的数据）
{	
	uint8_t input;
	DSPI_0.PUSHR.R = 0x08000000|value;    //赋值需要发送的数据		
	while(DSPI_0.SR.B.TCF==0)            //等待发送完成
	{}
	DSPI_0.SR.B.TCF=1;    //清除发送完成标志位 
    
   	while(DSPI_0.SR.B.RFDF==0)           //等待接收到数据
	{}
	 input=(uint8_t)(DSPI_0.POPR.R);     //读取接收到的数据
	DSPI_0.SR.B.RFDF=1;                  //清除接收标志位

	return(input);      //返回接收到的数据
}
/******************************************************************
 - 功能描述：向SD卡写命令
 - 参数说明：SD卡的命令是6个字节，cmd是指向命令字节序列的指针
 - 返回说明：命令写入不成功，将返回0xff
 ******************************************************************/
unsigned char SD_Write_Cmd(unsigned char *cmd)
{
    unsigned  char temp,time=0;
    SD_deselect();
    SPI_Byte(0XFF);//提高兼容性，如果没有有些卡可能不支持
    SD_select();
	
	
    SD_SPI_Write(cmd[0]);
    SD_SPI_Write(cmd[1]);
    SD_SPI_Write(cmd[2]);
    SD_SPI_Write(cmd[3]);
    SD_SPI_Write(cmd[4]);
    SD_SPI_Write(cmd[5]);
	if(cmd[0]==0X1C) SD_SPI_Read(); //如果是停止命令，跳过多余的字节
    do
    {
        temp=SD_SPI_Read();
        time++;  
    }
    while((temp&0x80)&&(time<200));
    return(temp);
}
/******************************************************************
 - 功能描述：SD卡初始化，针对于不同的SD卡，如MMC、SD或SDHC，初始化
             方法是不同的
 - 参数说明：无
 - 返回说明：调用成功，返回0x00，否则返回错误码
 ******************************************************************/
unsigned char SD_Init(void)//初始化
{
    unsigned char time=0,r=0,i=0;
	
    unsigned char rbuf[4]={0};
    	
    unsigned char CMD0[6] ={0x40,0x00,0x00,0x00,0x00,0x95}; //CMD0，将SD卡从默认上电后的SD模式切换到SPI模式，使SD卡进入IDLE状态
    unsigned char CMD1[6] ={0x41,0x00,0x00,0x00,0x00,0x01}; //CMD1，MMC卡使用CMD1命令进行初始化
    unsigned char CMD8[6] ={0x48,0x00,0x00,0x01,0xAA,0x87}; //CMD8，用于鉴别SD卡的版本，并可从应答得知SD卡的工作电压
    unsigned char CMD16[6]={0x50,0x00,0x00,0x02,0x00,0x01}; //CMD16，设置扇区大小为512字节，此命令用于在初始化完成之后进行试探性的操作，
                                                              //如果操作成功，说明初始化确实成功
    unsigned char CMD55[6]={0x77,0x00,0x00,0x00,0x00,0x01}; //CMD55，用于告知SD卡后面是ACMD，即应用层命令 CMD55+ACMD41配合使用
                                                              //MMC卡使用CMD1来进行初始化，而SD卡则使用CMD55+ACMD41来进行初始化
    unsigned char ACMD41H[6]={0x69,0x40,0x00,0x00,0x00,0x01}; //ACMD41,此命令用于检测SD卡是否初始化完成，MMC卡，不适用此命令，针对2.0的SD卡
    unsigned char ACMD41S[6]={0x69,0x00,0x00,0x00,0x00,0x01}; //ACMD41,此命令用于检测SD卡是否初始化完成，MMC卡，不适用此命令，针对1.0的SD卡
    
    unsigned char CMD58[6]={0x7A,0x00,0x00,0x00,0x00,0x01}; //CMD58，用于鉴别SD2.0到底是SDHC，还是普通的SD卡，二者对扇区地址的寻址方式不同
    SPI_Init(); //首先将SPI切为低速
    SD_deselect();//关闭片选
    SciTX(0x01);
	for(i=0;i<10;i++)//复位首先发送最少74个时钟信号
    {
        SciTX(0x03);
        SPI_Byte(0xff);//80个时钟信号
    }
    SD_select();
    time=0;

	do
    {
        r=SD_Write_Cmd(CMD0);//写入CMD0
        time++;

        if(time==200)
        {
           // SD_CS_H;//关闭片选

           return (INIT_CMD0_ERROR);//CMD0写入失败
        } 
         
    }while((r!=0x01));
    //SD_CS_SET(1);//关闭片选
    if(1==SD_Write_Cmd(CMD8))//写入CMD8，如果返回值为1，则SD卡版本为2.0
    {
        rbuf[0]=SD_SPI_Read(); rbuf[1]=SD_SPI_Read(); //读取4个字节的R7回应，通过它可知此SD卡是否支持2.7~3.6V的工作电压
        rbuf[2]=SD_SPI_Read(); rbuf[3]=SD_SPI_Read();
    
        if(rbuf[2]==0X01 && rbuf[3]==0XAA)//SD卡是否支持2.7~3.6V
        {		
            time=0;
            do
            {
                SD_Write_Cmd(CMD55);//写入CMD55
                r=SD_Write_Cmd(ACMD41H);//写入ACMD41，针对SD2.0
                time++;
                if(time>=200) 
                { 
                    return(INIT_SDV2_ACMD41_ERROR);//对SD2.0使用ACMD41进行初始化时产生错误
                }
             }while(r!=0);	

            if(0==SD_Write_Cmd(CMD58)) //写入CMD58，开始鉴别SD2.0
            {
                rbuf[0]=SD_SPI_Read(); rbuf[1]=SD_SPI_Read(); //读取4个字节的OCR，其中CCS指明了是SDHC还是普通的SD
                rbuf[2]=SD_SPI_Read(); rbuf[3]=SD_SPI_Read();	
                
                if(rbuf[0]&0x40) 
                {
                    SD1_Ver=SD_VER_V2HC; //SDHC卡	
                    SD1_Addr_Mode=1; //SDHC卡的扇区寻址方式是扇区地址
                }	
                else SD1_Ver=SD_VER_V2; //普通的SD卡，2.0的卡包含SDHC和一些普通的卡				
             }
         }
    }
    else //SD V1.0或MMC 
    {
      
        //SD卡使用ACMD41进行初始化，而MMC使用CMD1来进行初始化，依此来进一步判断是SD还是MMC
        SD_Write_Cmd(CMD55);//写入CMD55
        r=SD_Write_Cmd(ACMD41S);//写入ACMD41，针对SD1.0
        
        if(r<=1) //检查返回值是否正确，如果正确，说明ACMD41命令被接受，即为SD卡
        {
            SD1_Ver=SD_VER_V1; //普通的SD1.0卡，一般来说容量不会超过2G
            
            time=0;
            do
            {
                SD_Write_Cmd(CMD55);//写入CMD55
                r=SD_Write_Cmd(ACMD41S);//写入ACMD41，针对SD1.0
                time++;
                if(time>=200) 
                { 
                    return(INIT_SDV1_ACMD41_ERROR);//对SD1.0使用ACMD41进行初始化时产生错误
                }
             }while(r!=0);			 
         }
	     else //否则为MMC	
	     {
	         SD1_Ver=SD_VER_MMC; //MMC卡，它不支持ACMD41命令，而是使用CMD1进行初始化
			
	         time=0;
             do
             { 
                 r=SD_Write_Cmd(CMD1);//写入CMD1

                 time++;
                 if(time>=200) 
                 { 
                
                     return(INIT_CMD1_ERROR);//MMC卡使用CMD1命令进行初始化中产生错误

                 }
              }while(r!=0);			
          }
      }
 
    if(0!=SD_Write_Cmd(CMD16)) //SD卡的块大小必须为512字节
    {
        SD1_Ver=SD_VER_ERR; //如果不成功，则此卡为无法识别的卡
        
        return INIT_ERROR;
        
    }	
    
    SD_deselect();//关闭片选
    SD_SPI_Write(0xFF); //按照SD卡的操作时序在这里补8个时钟 
    
    SPI_4M();//初始化完毕，将SPI设置为高速，以提高数据传输速度
    
    return 0;//返回0,说明复位操作成功
}
/**********************SD卡写扇区**************************************/
unsigned char  SD_Write_Sector(unsigned  long addr, unsigned char *buffer)
{
    unsigned  char r,time;
    unsigned int i=0;
    unsigned char CMD24[]={0x58,0x00,0x00,0x00,0x00,0xff};//CMD24
    
    if(!SD1_Addr_Mode) addr<<=9; //addr = addr * 512	将块地址（扇区地址）转为字节地址
    CMD24[1]=((addr&0xff000000)>>24);//将字节地址写入CMD24
    CMD24[2]=((addr&0x00ff0000)>>16);
    CMD24[3]=((addr&0x0000ff00)>>8);
    CMD24[4]=addr;
    SD_select();
    time=0;
    do
    {
        r=SD_Write_Cmd(CMD24);
        time++;
        if(time==200)
        {
            SD_deselect();
            return (r);
        }   
    } while(r!=0);
    
    for(i=0;i<100;i++)
    {
        SD_SPI_Write(0xff);
    }
	while(0XFF!=SD_SPI_Read())   //等待SD卡准备好，再向其发送命令及后续的数据
    {
        ;
    }
    SD_SPI_Write(0xfe);//开始字节，后面就是要写入的512字节数据
    for(i=0;i<512;i++)//将缓冲区中的512字节写入SD卡
    {
        SD_SPI_Write(buffer[i]);
    }
 
    SD_SPI_Write(0xff);//两个字节的CRC校验码
    SD_SPI_Write(0xff);
    
    r=SD_SPI_Read();//读取返回值
    if((r&0x1f)!=0x05)//
    {
        SD_deselect();
        return  (WRITE_BLOCK_ERROR);//写块数据失败
    }
    while(SD_SPI_Read()!=0xff)//如果读到应答是0xff,说明写入成功
    {
        ;
    }
    SD_deselect();//
    SD_SPI_Write(0xff);//按照SD卡的操作时序在这里补8个时钟
    return(0);//返回0，说明写扇区成功
}
/**********************SD卡读扇区**************************************/
unsigned char  SD_Read_Sector(unsigned  long addr, unsigned char *buffer)
{
    unsigned  char r,time;
    unsigned int j=0;
    unsigned char CMD17[]={0x51,0x00,0x00,0x00,0x00,0xff};//CMD17
    if(!SD1_Addr_Mode) addr<<=9;//sector = sector * 512	   将块地址（扇区地址）转为字节地址
    CMD17[1]=((addr&0xff000000)>>24);//0x02
    CMD17[2]=((addr&0x00ff0000)>>16);
    CMD17[3]=((addr&0x0000ff00)>>8);
	CMD17[4]=addr;
    SD_select();
    time=0;
    do
    {
        r=SD_Write_Cmd(CMD17);
        time++;
        if(time==200)
        {
            return (READ_BLOCK_ERROR);//读块数据失败
        }   
    } while(r!=0);
     
    while(SD_SPI_Read()!=0xfe);//如果读到应答是0xfe,开始输出数据
    
    for(j=0;j<512;j++)//将数据写入到数据缓冲区
    {
        buffer[j]=SD_SPI_Read();//
    }
    SD_SPI_Read();//读取两个字节的CRC校验码
    SD_SPI_Read();
    SD_deselect();//关闭片选
    SD_SPI_Write(0XFF);//补充8个时钟信号
    return 0;//返回0，说明读扇区成功
}

/******************************************************************
 - 功能描述：对SD卡若干个扇区进行擦除，擦除后扇区中的数据大部分情况
             下为全0（有些卡擦除后为全0XFF，如要使用此函数，请确认）
 - 参数说明：addr_sta：开始扇区地址   addr_end：结束扇区地址
 - 返回说明：调用成功，返回0x00，否则返回错误码
 ******************************************************************/

unsigned char SD_Erase_nSector(unsigned long addr_sta,unsigned long addr_end)
{
    unsigned char r,time;
   // unsigned char i=0;
    unsigned char CMD32[]={0x60,0x00,0x00,0x00,0x00,0xff}; //设置擦除的开始扇区地址
    unsigned char CMD33[]={0x61,0x00,0x00,0x00,0x00,0xff}; //设置擦除的结束扇区地址
    unsigned char CMD38[]={0x66,0x00,0x00,0x00,0x00,0xff}; //擦除扇区
    
    if(!SD1_Addr_Mode) addr_sta<<=9;addr_end<<=9;//addr = addr * 512	将块地址（扇区地址）转为字节地址
    
    CMD32[1]=addr_sta>>24; //将开始地址写入到CMD32字节序列中
    CMD32[2]=addr_sta>>16;
    CMD32[3]=addr_sta>>8;
    CMD32[4]=addr_sta;	 
    
    CMD33[1]=addr_end>>24; //将开始地址写入到CMD32字节序列中
    CMD33[2]=addr_end>>16;
    CMD33[3]=addr_end>>8;
    CMD33[4]=addr_end;	
    
    time=0;
    do
    {  
        r=SD_Write_Cmd(CMD32);
        time++;
        if(time==200) 
        { 
            return(r); //命令写入失败
        }
    }while(r!=0);  
    
    time=0;
    do
    {  
        r=SD_Write_Cmd(CMD33);
        time++;
        if(time==200) 
        { 
            return(r); //命令写入失败
        }
    }while(r!=0);  
    
    time=0;
    do
    {  
        r=SD_Write_Cmd(CMD38);
        time++;
        if(time==200) 
        { 
            return(r); //命令写入失败
        }
    }while(r!=0);
    
    return 0; 

}
/****************************************************************************
 - 功能描述：向addr扇区开始的nsec个扇区写入数据（★硬件多扇区写入）
 - 参数说明：nsec:扇区数
             addr:开始扇区地址
             buffer:指向数据缓冲区的指针
 - 返回说明：调用成功，返回0x00，否则返回错误码
 
 ****************************************************************************/

unsigned char SD_Write_nSector(unsigned long nsec,unsigned long addr,unsigned char *buffer)	
{  
    unsigned char r,time;
    unsigned int i=0;
    unsigned long j=0;
    
    unsigned char CMD25[6]={0x59,0x00,0x00,0x00,0x00,0x01}; //CMD25用于完成多块连续写
    unsigned char CMD55[6]={0x77,0x00,0x00,0x00,0x00,0x01}; //CMD55，用于告知SD卡后面是ACMD,CMD55+ACMD23
    unsigned char ACMD23[6]={0x57,0x00,0x00,0x00,0x00,0x01};//CMD23，多块连续预擦除
    
    if(!SD1_Addr_Mode) addr<<=9; 
    CMD25[1]=addr>>24;
    CMD25[2]=addr>>16;
    CMD25[3]=addr>>8;
    CMD25[4]=addr;
    
    ACMD23[1]=nsec>>24;
    ACMD23[2]=nsec>>16;
    ACMD23[3]=nsec>>8;
    ACMD23[4]=nsec; 
    SD_select();
    if(SD1_Ver!=SD_VER_MMC) //如果不是MMC卡，则首先写入预擦除命令，CMD55+ACMD23，这样后面的连续块写的速度会更快
    {
        SD_Write_Cmd(CMD55);
        SD_Write_Cmd(ACMD23);
    }
    time=0;
    do
    {  
        r=SD_Write_Cmd(CMD25);
        time++;
        if(time==200) 
        { 
            return(WRITE_CMD25_ERROR); //命令写入失败
        }
    }while(r!=0); 
    while(0XFF!=SD_SPI_Read()); //等待SD卡准备好，再向其发送命令及后续的数据
    for(j=0;j<nsec;j++)
    {
        SD_SPI_Write(0xFC);//写入开始字节 0xfc，后面就是要写入的512个字节的数据	
        for(i=0;i<512;i++) //将缓冲区中要写入的512个字节写入SD卡
        {
            SD_SPI_Write(buffer[i]);
        }
        SD_SPI_Write(0xFF); 
        SD_SPI_Write(0xFF); //两个字节的CRC校验码，不用关心  
        r=SD_SPI_Read();   //读取返回值
        if((r & 0x1F)!=0x05) //如果返回值是 XXX00DELAY_TIME1 说明数据已经被SD卡接受了
        {
            return(WRITE_NBLOCK_ERROR); //写块数据失败
        }
        while(0xFF!=SD_SPI_Read());//等到SD卡不忙（数据被接受以后，SD卡要将这些数据写入到自身的FLASH中，需要一个时间）
        	                   //忙时，读回来的值为0x00,不忙时，为0xff
     }
     SD_SPI_Write(0xFD);
     while(0xFF!=SD_SPI_Read());
     SD_deselect();//关闭片选
     SD_SPI_Write(0xFF);//按照SD卡的操作时序在这里补8个时钟
     return(0);		 //返回0,说明写扇区操作成功
} 
/****************************************************************************
 - 功能描述：读取addr扇区开始的nsec个扇区的数据（★硬件多扇区读取）
 - 参数说明：nsec:扇区数
             addr:开始扇区地址
             buffer:指向数据缓冲区的指针
 - 返回说明：调用成功，返回0x00，否则返回错误码
 - 注：SD卡初始化成功后，读写扇区时，尽量将SPI速度提上来，提高效率
 ****************************************************************************/
unsigned char SD_Read_nSector(unsigned long nsec,unsigned long addr,unsigned char *buffer)
{
    unsigned char r,time;
    unsigned int i=0;
    unsigned long j=0;
    unsigned char CMD18[6]={0x52,0x00,0x00,0x00,0x00,0x01}; //CMD18的字节序列
    unsigned char CMD12[6]={0x1C,0x00,0x00,0x00,0x00,0x01}; //CMD12，强制停止命令
    
    if(!SD1_Addr_Mode) addr<<=9; //sector = sector * 512	   将块地址（扇区地址）转为字节地址
    CMD18[1]=addr>>24; //将字节地址写入到CMD17字节序列中
    CMD18[2]=addr>>16;
    CMD18[3]=addr>>8;
    CMD18[4]=addr;	
    
    SD_select();
    time=0;
    do
    {  
        r=SD_Write_Cmd(CMD18); //写入CMD18
        time++;
        if(time==200) 
        {
            return(READ_CMD18_ERROR); //写入CMD18失败
        }
    }while(r!=0); 
    for(j=0;j<nsec;j++)
    {  
        while(SD_SPI_Read()!= 0xFE); //一直读，当读到0xfe时，说明后面的是512字节的数据了
        for(i=0;i<512;i++)	 //将数据写入到数据缓冲区中
        {	
             buffer[i]=SD_SPI_Read();//
        }
        SD_SPI_Read();
        SD_SPI_Read();//读取两个字节的CRC校验码，不用关心它们
    }
    SD_Write_Cmd(CMD12); //写入CMD12命令，停止数据读取 
    SD_deselect();
    SD_SPI_Write(0xFF); //按照SD卡的操作时序在这里补8个时钟 
    return 0;
}
/****************************************************************************
 - 功能描述：获取SD卡的总扇区数（通过读取SD卡的CSD寄器组计算得到总扇区数）
 - 参数说明：无
 - 返回说明：返回SD卡的总扇区数
 - 注：无
 ****************************************************************************/

unsigned long SD_GetTotalSec(void)
{
    unsigned char pCSD[16];
    unsigned long Capacity;  
    unsigned char n,i;
    unsigned int csize; 
    
    unsigned char CMD9[6]={0x49,0x00,0x00,0x00,0x00,0x01}; //CMD9	
    
    if(SD_Write_Cmd(CMD9)!=0) //写入CMD9命令
    {
        return GET_CSD_ERROR; //获取CSD时产生错误
    }
    
    while(SD_SPI_Read()!= 0xFE); //一直读，当读到0xfe时，说明后面的是16字节的CSD数据
    
    for(i=0;i<16;i++) pCSD[i]=SD_SPI_Read(); //读取CSD数据
    
    SD_SPI_Read();
    SD_SPI_Read(); //读取两个字节的CRC校验码，不用关心它们
    
    SD_deselect();
    SD_SPI_Write(0xFF); //按照SD卡的操作时序在这里补8个时钟 

    //如果为SDHC卡，即大容量卡，按照下面方式计算
    if((pCSD[0]&0xC0)==0x40)	 //SD2.0的卡
    {	
        csize=pCSD[9]+(((unsigned long)(pCSD[8]))<<8)+1;
        Capacity=((unsigned long)csize)<<10;//得到扇区数	 		   
    }
    else //SD1.0的卡
    {	
        n=(pCSD[5]&0x0F)+((pCSD[10]&0x80)>>7)+((pCSD[9]&0x03)<<1)+2;
        csize=(pCSD[8]>>6)+((unsigned int)pCSD[7]<<2)+((unsigned int)(pCSD[6]&0x03)<<0x0A)+1;
        Capacity=(unsigned long)csize<<(n-9);//得到扇区数   
    }
    return Capacity;
}
