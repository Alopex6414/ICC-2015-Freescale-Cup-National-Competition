#include "SD.h"
#include "myfun.h"
//�궨��
//--------------------------------------------------------------
#define u8 unsigned char
#define SD_SPI_Write(x) SPI_Byte(x)
#define SD_SPI_Read() SPI_Byte(0xff)
//��������
//--------------------------------------------------------------

u8 SD1_Addr_Mode=0; //SD1��Ѱַ��ʽ��1Ϊ��Ѱַ��0Ϊ�ֽ�Ѱַ
u8 SD1_Ver=SD_VER_ERR; //SD��1�İ汾
/************************************************************/
/*                       ��ʼ��SIU                          */
/************************************************************/
void SIU_init(void)
{
	SIU.PCR[13].R = 0x0604;        //����PA[13]ΪSOUT�ӿ�
	SIU.PCR[12].R = 0x0103;        //����PA[12]ΪSIN�ӿ�
	SIU.PCR[14].R = 0x0604;        //����PA[14]ΪSCK�ӿ�
	SIU.PCR[15].R = 0x0223;        //����PA[15]Ϊ��©�������ΪCS��ʹ���ڲ���������
}

/*************************************************************/
/*                      ��ʼ��SPIģ��                        */
/*************************************************************/
void SPI_Init(void) 
{
  	DSPI_0.MCR.R = 0x80013001;     //����DSPI0Ϊ��ģʽ��CS�źŵ���Ч����ֹFIFO
  	DSPI_0.CTAR[0].R = 0x3E08772B; //����CTAR[0]������Ϊÿ֡����Ϊ8λ����λ��ǰ��������Ϊ50KHz
  	DSPI_0.MCR.B.HALT = 0x0;	     //DSPI0��������״̬
}

/*************************************************************/
/*                    ����SPIʱ��Ϊ4MHz                      */
/*************************************************************/
void SPI_4M(void) 
{ 
  	DSPI_0.MCR.B.HALT = 0x1;	     //DSPI0ֹͣ����
  	DSPI_0.CTAR[0].R = 0x3E087721; //����CTAR[0]������Ϊÿ֡����Ϊ7λ����λ��ǰ��������Ϊ4MHz
  	DSPI_0.MCR.B.HALT = 0x0;	     //DSPI0��������״̬
}

/*************************************************************/
/*                      SPI��дһ���ֽ�                      */
/*************************************************************/
uint8_t SPI_Byte(uint8_t value)  // : SPI��дһ���ֽڣ�������ɺ󷵻ر���ͨѶ��ȡ�����ݣ�
{	
	uint8_t input;
	DSPI_0.PUSHR.R = 0x08000000|value;    //��ֵ��Ҫ���͵�����		
	while(DSPI_0.SR.B.TCF==0)            //�ȴ��������
	{}
	DSPI_0.SR.B.TCF=1;    //���������ɱ�־λ 
    
   	while(DSPI_0.SR.B.RFDF==0)           //�ȴ����յ�����
	{}
	 input=(uint8_t)(DSPI_0.POPR.R);     //��ȡ���յ�������
	DSPI_0.SR.B.RFDF=1;                  //������ձ�־λ

	return(input);      //���ؽ��յ�������
}
/******************************************************************
 - ������������SD��д����
 - ����˵����SD����������6���ֽڣ�cmd��ָ�������ֽ����е�ָ��
 - ����˵��������д�벻�ɹ���������0xff
 ******************************************************************/
unsigned char SD_Write_Cmd(unsigned char *cmd)
{
    unsigned  char temp,time=0;
    SD_deselect();
    SPI_Byte(0XFF);//��߼����ԣ����û����Щ�����ܲ�֧��
    SD_select();
	
	
    SD_SPI_Write(cmd[0]);
    SD_SPI_Write(cmd[1]);
    SD_SPI_Write(cmd[2]);
    SD_SPI_Write(cmd[3]);
    SD_SPI_Write(cmd[4]);
    SD_SPI_Write(cmd[5]);
	if(cmd[0]==0X1C) SD_SPI_Read(); //�����ֹͣ�������������ֽ�
    do
    {
        temp=SD_SPI_Read();
        time++;  
    }
    while((temp&0x80)&&(time<200));
    return(temp);
}
/******************************************************************
 - ����������SD����ʼ��������ڲ�ͬ��SD������MMC��SD��SDHC����ʼ��
             �����ǲ�ͬ��
 - ����˵������
 - ����˵�������óɹ�������0x00�����򷵻ش�����
 ******************************************************************/
unsigned char SD_Init(void)//��ʼ��
{
    unsigned char time=0,r=0,i=0;
	
    unsigned char rbuf[4]={0};
    	
    unsigned char CMD0[6] ={0x40,0x00,0x00,0x00,0x00,0x95}; //CMD0����SD����Ĭ���ϵ���SDģʽ�л���SPIģʽ��ʹSD������IDLE״̬
    unsigned char CMD1[6] ={0x41,0x00,0x00,0x00,0x00,0x01}; //CMD1��MMC��ʹ��CMD1������г�ʼ��
    unsigned char CMD8[6] ={0x48,0x00,0x00,0x01,0xAA,0x87}; //CMD8�����ڼ���SD���İ汾�����ɴ�Ӧ���֪SD���Ĺ�����ѹ
    unsigned char CMD16[6]={0x50,0x00,0x00,0x02,0x00,0x01}; //CMD16������������СΪ512�ֽڣ������������ڳ�ʼ�����֮�������̽�ԵĲ�����
                                                              //��������ɹ���˵����ʼ��ȷʵ�ɹ�
    unsigned char CMD55[6]={0x77,0x00,0x00,0x00,0x00,0x01}; //CMD55�����ڸ�֪SD��������ACMD����Ӧ�ò����� CMD55+ACMD41���ʹ��
                                                              //MMC��ʹ��CMD1�����г�ʼ������SD����ʹ��CMD55+ACMD41�����г�ʼ��
    unsigned char ACMD41H[6]={0x69,0x40,0x00,0x00,0x00,0x01}; //ACMD41,���������ڼ��SD���Ƿ��ʼ����ɣ�MMC���������ô�������2.0��SD��
    unsigned char ACMD41S[6]={0x69,0x00,0x00,0x00,0x00,0x01}; //ACMD41,���������ڼ��SD���Ƿ��ʼ����ɣ�MMC���������ô�������1.0��SD��
    
    unsigned char CMD58[6]={0x7A,0x00,0x00,0x00,0x00,0x01}; //CMD58�����ڼ���SD2.0������SDHC��������ͨ��SD�������߶�������ַ��Ѱַ��ʽ��ͬ
    SPI_Init(); //���Ƚ�SPI��Ϊ����
    SD_deselect();//�ر�Ƭѡ
    SciTX(0x01);
	for(i=0;i<10;i++)//��λ���ȷ�������74��ʱ���ź�
    {
        SciTX(0x03);
        SPI_Byte(0xff);//80��ʱ���ź�
    }
    SD_select();
    time=0;

	do
    {
        r=SD_Write_Cmd(CMD0);//д��CMD0
        time++;

        if(time==200)
        {
           // SD_CS_H;//�ر�Ƭѡ

           return (INIT_CMD0_ERROR);//CMD0д��ʧ��
        } 
         
    }while((r!=0x01));
    //SD_CS_SET(1);//�ر�Ƭѡ
    if(1==SD_Write_Cmd(CMD8))//д��CMD8���������ֵΪ1����SD���汾Ϊ2.0
    {
        rbuf[0]=SD_SPI_Read(); rbuf[1]=SD_SPI_Read(); //��ȡ4���ֽڵ�R7��Ӧ��ͨ������֪��SD���Ƿ�֧��2.7~3.6V�Ĺ�����ѹ
        rbuf[2]=SD_SPI_Read(); rbuf[3]=SD_SPI_Read();
    
        if(rbuf[2]==0X01 && rbuf[3]==0XAA)//SD���Ƿ�֧��2.7~3.6V
        {		
            time=0;
            do
            {
                SD_Write_Cmd(CMD55);//д��CMD55
                r=SD_Write_Cmd(ACMD41H);//д��ACMD41�����SD2.0
                time++;
                if(time>=200) 
                { 
                    return(INIT_SDV2_ACMD41_ERROR);//��SD2.0ʹ��ACMD41���г�ʼ��ʱ��������
                }
             }while(r!=0);	

            if(0==SD_Write_Cmd(CMD58)) //д��CMD58����ʼ����SD2.0
            {
                rbuf[0]=SD_SPI_Read(); rbuf[1]=SD_SPI_Read(); //��ȡ4���ֽڵ�OCR������CCSָ������SDHC������ͨ��SD
                rbuf[2]=SD_SPI_Read(); rbuf[3]=SD_SPI_Read();	
                
                if(rbuf[0]&0x40) 
                {
                    SD1_Ver=SD_VER_V2HC; //SDHC��	
                    SD1_Addr_Mode=1; //SDHC��������Ѱַ��ʽ��������ַ
                }	
                else SD1_Ver=SD_VER_V2; //��ͨ��SD����2.0�Ŀ�����SDHC��һЩ��ͨ�Ŀ�				
             }
         }
    }
    else //SD V1.0��MMC 
    {
      
        //SD��ʹ��ACMD41���г�ʼ������MMCʹ��CMD1�����г�ʼ������������һ���ж���SD����MMC
        SD_Write_Cmd(CMD55);//д��CMD55
        r=SD_Write_Cmd(ACMD41S);//д��ACMD41�����SD1.0
        
        if(r<=1) //��鷵��ֵ�Ƿ���ȷ�������ȷ��˵��ACMD41������ܣ���ΪSD��
        {
            SD1_Ver=SD_VER_V1; //��ͨ��SD1.0����һ����˵�������ᳬ��2G
            
            time=0;
            do
            {
                SD_Write_Cmd(CMD55);//д��CMD55
                r=SD_Write_Cmd(ACMD41S);//д��ACMD41�����SD1.0
                time++;
                if(time>=200) 
                { 
                    return(INIT_SDV1_ACMD41_ERROR);//��SD1.0ʹ��ACMD41���г�ʼ��ʱ��������
                }
             }while(r!=0);			 
         }
	     else //����ΪMMC	
	     {
	         SD1_Ver=SD_VER_MMC; //MMC��������֧��ACMD41�������ʹ��CMD1���г�ʼ��
			
	         time=0;
             do
             { 
                 r=SD_Write_Cmd(CMD1);//д��CMD1

                 time++;
                 if(time>=200) 
                 { 
                
                     return(INIT_CMD1_ERROR);//MMC��ʹ��CMD1������г�ʼ���в�������

                 }
              }while(r!=0);			
          }
      }
 
    if(0!=SD_Write_Cmd(CMD16)) //SD���Ŀ��С����Ϊ512�ֽ�
    {
        SD1_Ver=SD_VER_ERR; //������ɹ�����˿�Ϊ�޷�ʶ��Ŀ�
        
        return INIT_ERROR;
        
    }	
    
    SD_deselect();//�ر�Ƭѡ
    SD_SPI_Write(0xFF); //����SD���Ĳ���ʱ�������ﲹ8��ʱ�� 
    
    SPI_4M();//��ʼ����ϣ���SPI����Ϊ���٣���������ݴ����ٶ�
    
    return 0;//����0,˵����λ�����ɹ�
}
/**********************SD��д����**************************************/
unsigned char  SD_Write_Sector(unsigned  long addr, unsigned char *buffer)
{
    unsigned  char r,time;
    unsigned int i=0;
    unsigned char CMD24[]={0x58,0x00,0x00,0x00,0x00,0xff};//CMD24
    
    if(!SD1_Addr_Mode) addr<<=9; //addr = addr * 512	�����ַ��������ַ��תΪ�ֽڵ�ַ
    CMD24[1]=((addr&0xff000000)>>24);//���ֽڵ�ַд��CMD24
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
	while(0XFF!=SD_SPI_Read())   //�ȴ�SD��׼���ã������䷢���������������
    {
        ;
    }
    SD_SPI_Write(0xfe);//��ʼ�ֽڣ��������Ҫд���512�ֽ�����
    for(i=0;i<512;i++)//���������е�512�ֽ�д��SD��
    {
        SD_SPI_Write(buffer[i]);
    }
 
    SD_SPI_Write(0xff);//�����ֽڵ�CRCУ����
    SD_SPI_Write(0xff);
    
    r=SD_SPI_Read();//��ȡ����ֵ
    if((r&0x1f)!=0x05)//
    {
        SD_deselect();
        return  (WRITE_BLOCK_ERROR);//д������ʧ��
    }
    while(SD_SPI_Read()!=0xff)//�������Ӧ����0xff,˵��д��ɹ�
    {
        ;
    }
    SD_deselect();//
    SD_SPI_Write(0xff);//����SD���Ĳ���ʱ�������ﲹ8��ʱ��
    return(0);//����0��˵��д�����ɹ�
}
/**********************SD��������**************************************/
unsigned char  SD_Read_Sector(unsigned  long addr, unsigned char *buffer)
{
    unsigned  char r,time;
    unsigned int j=0;
    unsigned char CMD17[]={0x51,0x00,0x00,0x00,0x00,0xff};//CMD17
    if(!SD1_Addr_Mode) addr<<=9;//sector = sector * 512	   �����ַ��������ַ��תΪ�ֽڵ�ַ
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
            return (READ_BLOCK_ERROR);//��������ʧ��
        }   
    } while(r!=0);
     
    while(SD_SPI_Read()!=0xfe);//�������Ӧ����0xfe,��ʼ�������
    
    for(j=0;j<512;j++)//������д�뵽���ݻ�����
    {
        buffer[j]=SD_SPI_Read();//
    }
    SD_SPI_Read();//��ȡ�����ֽڵ�CRCУ����
    SD_SPI_Read();
    SD_deselect();//�ر�Ƭѡ
    SD_SPI_Write(0XFF);//����8��ʱ���ź�
    return 0;//����0��˵���������ɹ�
}

/******************************************************************
 - ������������SD�����ɸ��������в����������������е����ݴ󲿷����
             ��Ϊȫ0����Щ��������Ϊȫ0XFF����Ҫʹ�ô˺�������ȷ�ϣ�
 - ����˵����addr_sta����ʼ������ַ   addr_end������������ַ
 - ����˵�������óɹ�������0x00�����򷵻ش�����
 ******************************************************************/

unsigned char SD_Erase_nSector(unsigned long addr_sta,unsigned long addr_end)
{
    unsigned char r,time;
   // unsigned char i=0;
    unsigned char CMD32[]={0x60,0x00,0x00,0x00,0x00,0xff}; //���ò����Ŀ�ʼ������ַ
    unsigned char CMD33[]={0x61,0x00,0x00,0x00,0x00,0xff}; //���ò����Ľ���������ַ
    unsigned char CMD38[]={0x66,0x00,0x00,0x00,0x00,0xff}; //��������
    
    if(!SD1_Addr_Mode) addr_sta<<=9;addr_end<<=9;//addr = addr * 512	�����ַ��������ַ��תΪ�ֽڵ�ַ
    
    CMD32[1]=addr_sta>>24; //����ʼ��ַд�뵽CMD32�ֽ�������
    CMD32[2]=addr_sta>>16;
    CMD32[3]=addr_sta>>8;
    CMD32[4]=addr_sta;	 
    
    CMD33[1]=addr_end>>24; //����ʼ��ַд�뵽CMD32�ֽ�������
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
            return(r); //����д��ʧ��
        }
    }while(r!=0);  
    
    time=0;
    do
    {  
        r=SD_Write_Cmd(CMD33);
        time++;
        if(time==200) 
        { 
            return(r); //����д��ʧ��
        }
    }while(r!=0);  
    
    time=0;
    do
    {  
        r=SD_Write_Cmd(CMD38);
        time++;
        if(time==200) 
        { 
            return(r); //����д��ʧ��
        }
    }while(r!=0);
    
    return 0; 

}
/****************************************************************************
 - ������������addr������ʼ��nsec������д�����ݣ���Ӳ��������д�룩
 - ����˵����nsec:������
             addr:��ʼ������ַ
             buffer:ָ�����ݻ�������ָ��
 - ����˵�������óɹ�������0x00�����򷵻ش�����
 
 ****************************************************************************/

unsigned char SD_Write_nSector(unsigned long nsec,unsigned long addr,unsigned char *buffer)	
{  
    unsigned char r,time;
    unsigned int i=0;
    unsigned long j=0;
    
    unsigned char CMD25[6]={0x59,0x00,0x00,0x00,0x00,0x01}; //CMD25������ɶ������д
    unsigned char CMD55[6]={0x77,0x00,0x00,0x00,0x00,0x01}; //CMD55�����ڸ�֪SD��������ACMD,CMD55+ACMD23
    unsigned char ACMD23[6]={0x57,0x00,0x00,0x00,0x00,0x01};//CMD23���������Ԥ����
    
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
    if(SD1_Ver!=SD_VER_MMC) //�������MMC����������д��Ԥ�������CMD55+ACMD23�����������������д���ٶȻ����
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
            return(WRITE_CMD25_ERROR); //����д��ʧ��
        }
    }while(r!=0); 
    while(0XFF!=SD_SPI_Read()); //�ȴ�SD��׼���ã������䷢���������������
    for(j=0;j<nsec;j++)
    {
        SD_SPI_Write(0xFC);//д�뿪ʼ�ֽ� 0xfc���������Ҫд���512���ֽڵ�����	
        for(i=0;i<512;i++) //����������Ҫд���512���ֽ�д��SD��
        {
            SD_SPI_Write(buffer[i]);
        }
        SD_SPI_Write(0xFF); 
        SD_SPI_Write(0xFF); //�����ֽڵ�CRCУ���룬���ù���  
        r=SD_SPI_Read();   //��ȡ����ֵ
        if((r & 0x1F)!=0x05) //�������ֵ�� XXX00DELAY_TIME1 ˵�������Ѿ���SD��������
        {
            return(WRITE_NBLOCK_ERROR); //д������ʧ��
        }
        while(0xFF!=SD_SPI_Read());//�ȵ�SD����æ�����ݱ������Ժ�SD��Ҫ����Щ����д�뵽�����FLASH�У���Ҫһ��ʱ�䣩
        	                   //æʱ����������ֵΪ0x00,��æʱ��Ϊ0xff
     }
     SD_SPI_Write(0xFD);
     while(0xFF!=SD_SPI_Read());
     SD_deselect();//�ر�Ƭѡ
     SD_SPI_Write(0xFF);//����SD���Ĳ���ʱ�������ﲹ8��ʱ��
     return(0);		 //����0,˵��д���������ɹ�
} 
/****************************************************************************
 - ������������ȡaddr������ʼ��nsec�����������ݣ���Ӳ����������ȡ��
 - ����˵����nsec:������
             addr:��ʼ������ַ
             buffer:ָ�����ݻ�������ָ��
 - ����˵�������óɹ�������0x00�����򷵻ش�����
 - ע��SD����ʼ���ɹ��󣬶�д����ʱ��������SPI�ٶ������������Ч��
 ****************************************************************************/
unsigned char SD_Read_nSector(unsigned long nsec,unsigned long addr,unsigned char *buffer)
{
    unsigned char r,time;
    unsigned int i=0;
    unsigned long j=0;
    unsigned char CMD18[6]={0x52,0x00,0x00,0x00,0x00,0x01}; //CMD18���ֽ�����
    unsigned char CMD12[6]={0x1C,0x00,0x00,0x00,0x00,0x01}; //CMD12��ǿ��ֹͣ����
    
    if(!SD1_Addr_Mode) addr<<=9; //sector = sector * 512	   �����ַ��������ַ��תΪ�ֽڵ�ַ
    CMD18[1]=addr>>24; //���ֽڵ�ַд�뵽CMD17�ֽ�������
    CMD18[2]=addr>>16;
    CMD18[3]=addr>>8;
    CMD18[4]=addr;	
    
    SD_select();
    time=0;
    do
    {  
        r=SD_Write_Cmd(CMD18); //д��CMD18
        time++;
        if(time==200) 
        {
            return(READ_CMD18_ERROR); //д��CMD18ʧ��
        }
    }while(r!=0); 
    for(j=0;j<nsec;j++)
    {  
        while(SD_SPI_Read()!= 0xFE); //һֱ����������0xfeʱ��˵���������512�ֽڵ�������
        for(i=0;i<512;i++)	 //������д�뵽���ݻ�������
        {	
             buffer[i]=SD_SPI_Read();//
        }
        SD_SPI_Read();
        SD_SPI_Read();//��ȡ�����ֽڵ�CRCУ���룬���ù�������
    }
    SD_Write_Cmd(CMD12); //д��CMD12���ֹͣ���ݶ�ȡ 
    SD_deselect();
    SD_SPI_Write(0xFF); //����SD���Ĳ���ʱ�������ﲹ8��ʱ�� 
    return 0;
}
/****************************************************************************
 - ������������ȡSD��������������ͨ����ȡSD����CSD���������õ�����������
 - ����˵������
 - ����˵��������SD������������
 - ע����
 ****************************************************************************/

unsigned long SD_GetTotalSec(void)
{
    unsigned char pCSD[16];
    unsigned long Capacity;  
    unsigned char n,i;
    unsigned int csize; 
    
    unsigned char CMD9[6]={0x49,0x00,0x00,0x00,0x00,0x01}; //CMD9	
    
    if(SD_Write_Cmd(CMD9)!=0) //д��CMD9����
    {
        return GET_CSD_ERROR; //��ȡCSDʱ��������
    }
    
    while(SD_SPI_Read()!= 0xFE); //һֱ����������0xfeʱ��˵���������16�ֽڵ�CSD����
    
    for(i=0;i<16;i++) pCSD[i]=SD_SPI_Read(); //��ȡCSD����
    
    SD_SPI_Read();
    SD_SPI_Read(); //��ȡ�����ֽڵ�CRCУ���룬���ù�������
    
    SD_deselect();
    SD_SPI_Write(0xFF); //����SD���Ĳ���ʱ�������ﲹ8��ʱ�� 

    //���ΪSDHC�����������������������淽ʽ����
    if((pCSD[0]&0xC0)==0x40)	 //SD2.0�Ŀ�
    {	
        csize=pCSD[9]+(((unsigned long)(pCSD[8]))<<8)+1;
        Capacity=((unsigned long)csize)<<10;//�õ�������	 		   
    }
    else //SD1.0�Ŀ�
    {	
        n=(pCSD[5]&0x0F)+((pCSD[10]&0x80)>>7)+((pCSD[9]&0x03)<<1)+2;
        csize=(pCSD[8]>>6)+((unsigned int)pCSD[7]<<2)+((unsigned int)(pCSD[6]&0x03)<<0x0A)+1;
        Capacity=(unsigned long)csize<<(n-9);//�õ�������   
    }
    return Capacity;
}
