#include "MPC5604C_M07N.h"


#define SD_select()        SIU.GPDO[15].B.PDO=0        //QS3�͵�ƽ
#define SD_deselect()      SIU.GPDO[15].B.PDO=1        //QS3�ߵ�ƽ
//�����붨��
//----------------------------------------------------------------------
#define SD_VER_ERR     0X00
#define SD_VER_MMC     0X01
#define SD_VER_V1      0X02
#define SD_VER_V2      0X03
#define SD_VER_V2HC    0X04

#define INIT_ERROR                  0x01 //��ʼ������
#define INIT_CMD0_ERROR             0x02 //CMD0����
#define INIT_CMD1_ERROR             0x03 //CMD1����
#define INIT_SDV2_ACMD41_ERROR	    0x04 //ACMD41����
#define INIT_SDV1_ACMD41_ERROR	    0x05 //ACMD41����

#define WRITE_CMD24_ERROR           0x06 //д��ʱ����CMD24����
#define WRITE_BLOCK_ERROR           0x07 //д�����

#define READ_BLOCK_ERROR            0x08 //�������

#define WRITE_CMD25_ERROR           0x09 //���������дʱ����CMD25����
#define WRITE_NBLOCK_ERROR          0x0A //�������дʧ��

#define READ_CMD18_ERROR            0x0B //����������ʱ����CMD18����
 
#define GET_CSD_ERROR               0x0C //��CSDʧ��

//----------------------------------------------------------------------
//                     ��������
//----------------------------------------------------------------------
void  SIU_init(void);//SPI�˿ڳ�ʼ��
void SPI_Init(void);//SPI�Ĵ�������
uint8_t SPI_Byte(uint8_t value);
void SPI_Init(void);//SPI���õ���256����
void SPI_4M(void);//SPI���õ���2����
unsigned char SD_Write_Cmd(unsigned char *cmd);//д����

unsigned char SD_Init(void);//SD����ʼ��
unsigned char  SD_Write_Sector(unsigned  long addr, unsigned char *buffer);
unsigned char  SD_Read_Sector(unsigned  long addr, unsigned char *buffer);
unsigned char SD_Erase_nSector(unsigned long addr_sta,unsigned long addr_end);
unsigned char SD_Write_nSector(unsigned long nsec,unsigned long addr,unsigned char *buffer);
unsigned char SD_Read_nSector(unsigned long nsec,unsigned long addr,unsigned char *buffer);
unsigned long SD_GetTotalSec(void);




 
