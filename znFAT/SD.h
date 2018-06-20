#include "MPC5604C_M07N.h"


#define SD_select()        SIU.GPDO[15].B.PDO=0        //QS3低电平
#define SD_deselect()      SIU.GPDO[15].B.PDO=1        //QS3高电平
//错误码定义
//----------------------------------------------------------------------
#define SD_VER_ERR     0X00
#define SD_VER_MMC     0X01
#define SD_VER_V1      0X02
#define SD_VER_V2      0X03
#define SD_VER_V2HC    0X04

#define INIT_ERROR                  0x01 //初始化错误
#define INIT_CMD0_ERROR             0x02 //CMD0错误
#define INIT_CMD1_ERROR             0x03 //CMD1错误
#define INIT_SDV2_ACMD41_ERROR	    0x04 //ACMD41错误
#define INIT_SDV1_ACMD41_ERROR	    0x05 //ACMD41错误

#define WRITE_CMD24_ERROR           0x06 //写块时产生CMD24错误
#define WRITE_BLOCK_ERROR           0x07 //写块错误

#define READ_BLOCK_ERROR            0x08 //读块错误

#define WRITE_CMD25_ERROR           0x09 //在连续多块写时产生CMD25错误
#define WRITE_NBLOCK_ERROR          0x0A //连续多块写失败

#define READ_CMD18_ERROR            0x0B //在连续多块读时产生CMD18错误
 
#define GET_CSD_ERROR               0x0C //读CSD失败

//----------------------------------------------------------------------
//                     函数声明
//----------------------------------------------------------------------
void  SIU_init(void);//SPI端口初始化
void SPI_Init(void);//SPI寄存器配置
uint8_t SPI_Byte(uint8_t value);
void SPI_Init(void);//SPI设置低速256分配
void SPI_4M(void);//SPI设置低速2分配
unsigned char SD_Write_Cmd(unsigned char *cmd);//写命令

unsigned char SD_Init(void);//SD卡初始化
unsigned char  SD_Write_Sector(unsigned  long addr, unsigned char *buffer);
unsigned char  SD_Read_Sector(unsigned  long addr, unsigned char *buffer);
unsigned char SD_Erase_nSector(unsigned long addr_sta,unsigned long addr_end);
unsigned char SD_Write_nSector(unsigned long nsec,unsigned long addr,unsigned char *buffer);
unsigned char SD_Read_nSector(unsigned long nsec,unsigned long addr,unsigned char *buffer);
unsigned long SD_GetTotalSec(void);




 
