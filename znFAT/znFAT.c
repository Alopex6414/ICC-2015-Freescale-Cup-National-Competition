#include "znfat.h"
#include "template.h"
#include "deviceio.h"


/*==================================================================================
  ���ϵ�znFAT һ�ֽ�Ϊ�걸��Ƕ��ʽƽ̨�ϵ�FAT32�ļ�ϵͳ������� V11.21
               �����ע ���ϵ�znFAT ��վ www.znfat.com
	           QQ 987582714
===================================================================================*/
/*----------------------------------------------------------------------------/
/  Here is znFAT -- a complete FAT32 FileSystem Solution  ver 11.21   
/-----------------------------------------------------------------------------/
/ znFAT is a complete FAT FileSystem Code Module for embeded system platform.
/ znFAT is Developped and Coded By ZN China who own the full copyright on it.
/ You are allowed to use it for study,research,and commerce purpose,to modify
/ the code and publish it freely.
/ znFAT is pleasure to be recommended to more E-amateur and Engineer.Thanks!!
/  
/              Copyright (C) 2010, ZN, all right reserved.
/
/               Technology Support http://www.znfat.com
/                  Welcome to ZN's FAT32 FS World!!
/-----------------------------------------------------------------------------/

/-----------------------------------------------------------------------------/
Function Table(���ܺ�����): 
 znFAT_Device_Init  : Storage device initialize (�洢�豸��ʼ��)
 znFAT_Init         : File system initialize (�ļ�ϵͳ��ʼ��)
 znFAT_Select_Device: Select storage device (ѡ��洢�豸)
 znFAT_Open_File    : Open a file (���ļ�)
 znFAT_ReadData     : Read data in a file (��ȡ�ļ�����)
 znFAT_ReadDataX    : Read data and redirect it (��ȡ�ļ�����+�����ض���)
 znFAT_Enter_Dir    : Enter a dir (����Ŀ¼)
 znFAT_WriteData    : Write data to a file,append it to the end (���ļ�д������)
 znFAT_Modify_Data  : Modify data in a file (�޸��ļ�����)
 znFAT_Dump_Data    : Dump data of a file (�������ض��ļ�����)
 znFAT_Create_File  : Create file (�����ļ�)
 znFAT_Create_Dir   : Create dir (����Ŀ¼)
 znFAT_Delete_File  : Delete file (ɾ���ļ�)
 znFAT_Delete_Dir   : Delete dir (ɾ��Ŀ¼)
 znFAT_Make_FS      : Make a FAT32 FS on a storage device,even Format (��ʽ��)
 znFAT_Close_File   : Close file (�ر��ļ�)
 znFAT_Flush_FS     : Flush FS,Update FS information from RAM to Disk (ˢ��FS)
/----------------------------------------------------------------------------/
Configuration for znFAT's Functions,is necessary before the usage of them.
        (��znFAT�й��ܺ��������ã���ʹ������֮ǰ����ض����������)

When you use a function in znFAT(but znFAT_Device_Init znFAT_Init and znFAT_\
Select_Device,because them must be used in every project),you must firstly
OPEN the MACROS as "#define ZNFAT_XXXX",So the relevant code of the function
is added to the compiling.Or,You will get a warning like "XXXX is undefined .."
For example:You now wanna use znFAT_Open_File,You must open the header file
config.h,OPEN the MACRO ZNFAT_OPEN_FILE,even delete the "//" before it.

(����Ҫʹ��znFAT��һ������ʱ(znFAT_Device_Init znFAT_Init and znFAT_Select_\
Device�������������⣬��Ϊ�������κ�ʱ���Ǳ�Ȼ��ʹ�õ�)���������Ȱ���Ӧ�ĺ�
�򿪣�����"#define ZNFAT_XXXX"�����������������صĴ���Żᱻ���뵽����֮�У�
��������ܻ�õ��������ľ���"XXXX is undefined.."������˵����Ҫʹ��znFAT_\
Open_File�����������Ҫ��config.h���������Ӧ�ĺ�ZNFAT_OPEN_FILE����ȥ��
ǰ���"//"��)

Option for znFAT (znFAT�еĹ�����ʽѡ��)

 USE_LFN : Use the Long File Name (ʹ�ó��ļ���)
  MAX_LFN_LEN : Define the max Long File Name length (���峤�ļ�����󳤶�)
  USE_OEM_CHAR: Use OEM charactor in LFN,as CHN (�ڳ��ļ�����ʹ��OEM�ַ���������)
 USE_MULTISEC_R : Use hardware continuous Sector Read (ʹ��Ӳ��������������)
 USE_MULTISEC_W : Use hardware continuous Sector Write (ʹ��Ӳ������������д)
 USE_MULTISEC_CLEAR : Use hardware continuous Sector Clear (ʹ��Ӳ��������������0)
 RT_UPDATE_FSINFO : Realtime update the information of FS (ʵʱˢ���ļ�ϵͳ��Ϣ)
 RT_UPDATE_FILESIZE : Realtime update the file size (ʵʱ�����ļ���С)
 RT_UPDATE_CLUSTER_CHAIN : Realtime update the cluster chain (ʵʱ���´���)
                           if not define it ,znFAT will use CCCB algorithm
						   to store the cluster chain in CCCB buffer 
						   (���û�ж�������꣬��znFAT�����ݶ�д�����У�ʹ��
						    CCCB�㷨����������ʱ�ԵĴ洢��RAM�Ļ������У�����
							��Ϊ��������ݶ�д���ʣ��׶����ڻ����еĴ�������
							д�����������У���������ݶ�ʧ�����Ҫ��ʱ��д��)
  CCCB_LEN (XXX) : Define the size of CCCB buffer (����CCCB ��������С)
  USE_ALONE_CCCB : Use the alone CCCB buffer (ʹ�ö���CCCB ����������ÿһ���ļ�
                                              �������һ��������CCCB������������
											  ��дʱ�����ļ����ø��Ե�CCCB���壬
											  �������档)
		           if not define it,use the Shared CCCB buffer
				                             (����˺�û�ж��壬��znFAT��ʹ�ù�
											  CCCB ����������Ҫ��Ϊ�˽�ʡRAM��
											  Դ����������Ȼ���¶���ļ��Թ���
											  CCCB��������ᣬ��ͬʱ�����ļ�
											  �϶��ʱ�����ݶ�дЧ�ʲ����ߡ�)
 USE_EXCHANGE_BUFFER : Use the exchange buffer and relevant algorithm (EXB)
                                             (ʹ��EXB�����������弰���㷨��EXB
											  ������Ϊ�˼����������ݵĶ�-��-д
											  ��������)
  USE_ALONE_EXB	: Use alone exchange buffer (ʹ�ö�����EXB�����������壬��ÿ��
                                             �ļ����и��Զ�����EXB���壬������
											 �Ἣ�������RAM����)
	              if no define it,use the Shared exchange buffer
				                            (����˺�û�ж��壬��ʹ�ù���EXB
											 ���壬����ļ���ʱ����һ����������
											 ���壬�������������)
 Data_Redirect : Redirect function name macro defination for read_dataX
                                            (ΪznFAT_ReadDataX���������������
											 �ض���λ�ֽڴ�����)

/---------------------------------------------------------------------------*/


//#pragma udata directive
//#pragma udata BUFFER
UINT8 tmpBuf[ZNFAT_BUF_SIZE];
//#pragma udata

UINT8 *znFAT_Buffer=tmpBuf; //znFAT���ڲ���������ʹ���߲���˽��ʹ��
                            //�ȶ���tmpBuf������znFAT_Bufferָ����������Ϊ��һЩ�ܹ���CPU��
                            //������RAM������ṹ��ֻ����ָ�������ʴ����飬����PIC

//--------------------------------------------------------------------------------------------------
struct znFAT_Init_Args *pInit_Args; //��ʼ�������ṹ��ָ�룬����ָ��ĳһ�洢�豸�ĳ�ʼ����������
                                    //ʹ��֮ǰ*����*��ָ��ṹ������
extern struct znFAT_IO_Ctl ioctl; 

UINT8 Dev_No=0; //�豸�ţ�����ʵ�ֶ��豸

//--------------------------------------------------------------------------------------------------

struct FileInfo *just_file=(struct FileInfo *)0; //���ڼ�¼����������ļ�

//-------------------SCCCB��ر�������----------------------
#ifndef RT_UPDATE_CLUSTER_CHAIN //���ڶ��干��CCCB�ı���������ʵ��
#ifndef USE_ALONE_CCCB
UINT32 scccb_buf[CCCB_LEN]; //CCCB�Ļ��������������ضεķ�ʽ����¼����
UINT8  scccb_counter=0; 
UINT32 scccb_curval=0;

UINT8  scccb_curdev=(UINT8)(-1);
#endif
#endif

#ifndef RT_UPDATE_CLUSTER_CHAIN

#ifndef USE_ALONE_CCCB //��ʹ�ö����������壬����ʹ�ù���������壬���±���������ɹ���CCCBʹ�ù����е�����
UINT32 *pcccb_buf=scccb_buf;
UINT32 *pcccb_curval=&scccb_curval;
UINT8  *pcccb_counter=&scccb_counter;

UINT8  *pcccb_curdev=&scccb_curdev;
struct FileInfo *pcccb_cur_oc=(struct FileInfo *)0;
struct znFAT_Init_Args *pcccb_cur_initargs=(struct znFAT_Init_Args *)0;
#else
UINT32 *pcccb_buf=(UINT32 *)0;
UINT32 *pcccb_curval=(UINT32 *)0;
UINT8  *pcccb_counter=(UINT8 *)0;
#endif
 
UINT8 get_next_cluster_in_cccb=0; //���Ա�־�Ƿ���CCCB�в�����һ��
#endif
//----------------------------------------------------------

//------------------EXB��ر�������-------------------------
#ifdef USE_EXCHANGE_BUFFER
#ifndef USE_ALONE_EXB
//#pragma udata directive
//#pragma udata SEXB_BUF
UINT8  sexb_buf[ZNFAT_BUF_SIZE];
//#pragma udata

UINT8  sexb_cur_dev=(UINT8)(-1);
UINT32 sexb_cur_sec=0;
struct FileInfo *psexb_cur_oc=(struct FileInfo *)0; //ָʾ��ǰEXB���ĸ��ļ�ռ��
#endif
#endif

#ifdef USE_EXCHANGE_BUFFER

#ifndef USE_ALONE_EXB
UINT8 *pexb_buf=sexb_buf;
#else
UINT8 *pexb_buf=(UINT8 *)0;
#endif

#endif
//----------------------------------------------------------
//====================һЩ���ú���================================================================

//znFAT��ʹ�õ��Ĺ������������а����˶�ROM���� �ֽڡ��֡�˫�ֵĶ�ȡ�Լ�ROM��RAM֮�俽������
//���Ҫʹ���ļ������ʽ�����ܣ�����ЩROM��صĺ����Ǳ�����ȷ����ʵ�ֵ�

UINT8 Memory_Set(UINT8 *pmem,UINT32 len,UINT8 value)
{
 	UINT32 i=0;
 	for(i=0;i<len;i++)
 	{ 
  		pmem[i]=value;
 	}
 	return 0;
}

UINT8 Memory_Compare(UINT8 *psmem,UINT8 *pdmem,UINT32 len)
{
 	UINT32 i=0;
 	for(i=0;i<len;i++)
 	{
  		if(psmem[i]!=pdmem[i])
  		{
   			return 0;
  		}
  	}
 	return 1;
}

UINT8 * Memory_Copy(UINT8 *pdmem,UINT8 *psmem,UINT32 len)
{
 	UINT32 i=0;
 	for(i=0;i<len;i++)
 	{
  		pdmem[i]=psmem[i];
 	}
 	return pdmem;
}

INT8 * StringCopy(INT8 *dest_str,INT8 *src_str)
{
 	UINT8 i=0;
 	while('\0'!=src_str[i])
 	{
  		dest_str[i]=src_str[i];
  		i++;
 	}
 	dest_str[i]='\0';
 	return dest_str;
}

UINT32 StringLen(INT8 *pstr)
{
 	UINT32 len=0;
 	while('\0'!=pstr[len]) 
 	{
  		len++;
 	}
 	return len;
}

UINT32 WStringLen(UINT16 *str)
{
 	UINT32 i=0;
 	while(0!=str[i])
 	{
  		i++;
 	}
 	return i;
}
//=============================FLASHROM ������غ���=====================

UINT8 PGM_BYTE_FUN(ROM_TYPE_UINT8 *ptr)
{
 	return *(ptr); 
}

UINT16 PGM_WORD_FUN(ROM_TYPE_UINT16 *ptr)
{
 	return *(ptr);
}

UINT32 PGM_DWORD_FUN(ROM_TYPE_UINT32 *ptr)
{
 	return *(ptr);
}

UINT8 * PGM2RAM(UINT8 *pdmem,ROM_TYPE_UINT8 *psmem,UINT32 len)
{
 	UINT32 i=0;
 	for(i=0;i<len;i++)
 	{
  		pdmem[i]=PGM_BYTE_FUN((psmem+i));
 	}
 	return pdmem;
}

//================================================================================================

/*****************************************************************************
 ���ܣ�ѡ��һ���洢�豸
 �βΣ�devno:�豸�� pinitargs:ָ��洢�豸����Ӧ���ļ�ϵͳ��ʼ���������ϵ�ָ��
 ���أ�0
 ��⣺znFAT��֧�ֶ��豸�ģ�����ڶ��豸�����ļ�ϵͳ��ز���֮ǰ������Ҫ��ѡ��
       һ���洢�豸���˺�����Dev_No���ȫ�ֵ��������ִ洢�豸�����ı���������
       ����ͬʱ��pInit_Argsָ��洢�豸��Ӧ���ļ�ϵͳ��ʼ����������
*****************************************************************************/
UINT8 znFAT_Select_Device(UINT8 devno,struct znFAT_Init_Args *pinitargs) //ѡ���豸
{
 	pInit_Args=pinitargs; //��znFAT�ĳ�ʼ����������ָ��ָ���豸�ĳ�ʼ����������
 	Dev_No=devno; //�����豸��
 	return 0;
}
/***********************************************************************************
 ���ܣ���һ��С�����е��ֽ����У�����õ�����ĳһ�ַ�����������������ֵ
 �βΣ�dat:ָ���ֽ����е�ָ�� len:���ֽ����е�ǰlen���ֽڼ�������ֵ
 ���أ�����õ�������ֵ
 ��⣺��һ���������β�ͬCPU�ڴ�С���ϵĲ������Ҫ�ֶΡ��������һ��С�˵�4�ֽ�����
       unsigned char *p={0X12,0X34,0X56,0X78} �������������ϳ�Ϊһ��4�ֽ����ͣ���
       unsigned long����ô������������ *((unsigned long *)p)�������ڲ�ͬ��CPU�����
       ����RAM�е��ֽ�����˳��ͬ������С�����⣬��������������ֵ����Ϊ0X12345678
       �� 0X78563412���⽫����ִ���Ϊ���������ֲ��죬�����˴˺�����ͨ�����ֽ�����
       ���м��㣬��󽫿��Եõ���ȷ��ֵ�����������е��ֽ����У�ͨ���˺����ļ���Bytes
       2Value(p,4)ֵһ��Ϊ0X12345678��
***********************************************************************************/
UINT32 Bytes2Value(UINT8 *dat,UINT8 len)
{
 	UINT32 temp=0;
 	if(len>=1) temp|=((UINT32)(dat[0]))    ;
 	if(len>=2) temp|=((UINT32)(dat[1]))<<8 ;
 	if(len>=3) temp|=((UINT32)(dat[2]))<<16;
 	if(len>=4) temp|=((UINT32)(dat[3]))<<24;
 	return temp;
}
/***********************************************************************************
 ���ܣ�����FSINFO�����������ַ
 �βΣ�fsinfosec:һ��ָ�����ڼ�¼FSINFO���������ַ�ı�����ָ��
 ���أ����н�����ɹ���ʧ��
 ��⣺һ����˵FSINFO������DBR�����ĺ�һ���������������������������������DBR+1��
       FAT1��ǰһ���������б�������FSINFO�����ı�־����Ϊ�϶���FSINFO������������
***********************************************************************************/
UINT8 Find_FSINFO_Sec(UINT32 *fsinfosec) //Ѱ��FSINFO������������ʣ���������õĿմ�
{
 	UINT32 iSec=0;
 	struct FSInfo *pfsinfo;
 	UINT8 head[4]={'R','R','a','A'};
 	UINT8 sign[4]={'r','r','A','a'}; //FSINFO�����ı�־
 	for(iSec=(pInit_Args->BPB_Sector_No+1);iSec<(pInit_Args->FirstFATSector);iSec++)
 	{
  		znFAT_Device_Read_Sector(iSec,znFAT_Buffer);
  		pfsinfo=((struct FSInfo *)znFAT_Buffer);
  		if(Memory_Compare(pfsinfo->Head,head,4) //�ж������Ƿ���FSINFO����
	 	&& Memory_Compare(pfsinfo->Sign,sign,4))
  		{
   			*fsinfosec=iSec;
   			return ERR_SUCC;
  		}
 	}
 	return ERR_FAIL;
}
/***********************************************************************************
 ���ܣ���FAT�����ͷ�Ͽ�ʼ���ҿ��õĿմأ����ҵ���һ�����ÿմ�
 �βΣ�nFreeCluster:ָ�����ڼ�¼���õĿմصı�����ָ��
 ���أ����н�����ɹ���ʧ��
 ��⣺��FAT����ʼλ�ÿ�ʼ˳�β��ң�ֱ����һ�����õĿմس��֣��˿մ�ֵ���ᱻ��¼
       ���ļ�ϵͳ��ʼ�����������е�pInit_Args->Next_Free_Cluster��Ϊ�ļ�������������
       ʹ�ÿմ�ʱ�ṩ���ÿմصĲο�ֵ��
***********************************************************************************/
UINT8 Search_Free_Cluster_From_Start(UINT32 *nFreeCluster)
{
 	UINT32 iSec=0;
 	UINT8  iItem=0;
 	struct FAT_Sec *pFAT_Sec;
 	for(iSec=0;iSec<pInit_Args->FATsectors;iSec++)
 	{
  		znFAT_Device_Read_Sector(pInit_Args->FirstFATSector+iSec,znFAT_Buffer); //��ȡFAT����
  		pFAT_Sec=(struct FAT_Sec *)znFAT_Buffer;
  		for(iItem=0;iItem<NITEMSINFATSEC;iItem++) //�������д��Ѱ�ҿ��д�
  		{
   			if((0==(((pFAT_Sec->items[iItem]).Item)[0])) && (0==(((pFAT_Sec->items[iItem]).Item)[1])) &&
	  		(0==(((pFAT_Sec->items[iItem]).Item)[2])) && (0==(((pFAT_Sec->items[iItem]).Item)[3])))
   			{
    			*nFreeCluster=((iSec*NITEMSINFATSEC)+(UINT32)iItem);
				return ERR_SUCC;
   			}
  		}
 	}
 
 	return	ERR_FAIL;
}
/***********************************************************************************
 ���ܣ�����FSINFO��������
 �βΣ���
 ���أ�0
 ��⣺����FSINFO��������ʵ��Ҫ��Ϊ��ά��ʣ��մ�������һ�����ÿմزο�ֵ������������
***********************************************************************************/
#ifdef UPDATE_FSINFO
UINT8 Update_FSINFO(void) //����FSINFO����
{
 	struct FSInfo *pfsinfo;
 	znFAT_Device_Read_Sector(pInit_Args->FSINFO_Sec,znFAT_Buffer);
 	pfsinfo=((struct FSInfo *)znFAT_Buffer); 
 	//д��ʣ��մ���
 	pfsinfo->Free_Cluster[0]=(UINT8)( pInit_Args->Free_nCluster&0X000000FF)    ;
 	pfsinfo->Free_Cluster[1]=(UINT8)((pInit_Args->Free_nCluster&0X0000FF00)>>8);
 	pfsinfo->Free_Cluster[2]=(UINT8)((pInit_Args->Free_nCluster&0X00FF0000)>>16);
 	pfsinfo->Free_Cluster[3]=(UINT8)((pInit_Args->Free_nCluster&0XFF000000)>>24);
 	//д����һ���дزο�ֵ�����޶������
 	pfsinfo->Next_Free_Cluster[0]=(UINT8)( pInit_Args->Next_Free_Cluster&0X000000FF)    ;
 	pfsinfo->Next_Free_Cluster[1]=(UINT8)((pInit_Args->Next_Free_Cluster&0X0000FF00)>>8);
 	pfsinfo->Next_Free_Cluster[2]=(UINT8)((pInit_Args->Next_Free_Cluster&0X00FF0000)>>16);
 	pfsinfo->Next_Free_Cluster[3]=(UINT8)((pInit_Args->Next_Free_Cluster&0XFF000000)>>24);
 	znFAT_Device_Write_Sector(pInit_Args->FSINFO_Sec,znFAT_Buffer);
 	return 0;
}
#endif

/***********************************************************************************
 ���ܣ�znFAT�е��ļ�ϵͳ��ʼ������
 �βΣ���
 ���أ����н�����ɹ���ʧ�ܣ����������2����˵������FAT32�ļ�ϵͳ����У����󣬼���
       �洢�ϵ��ļ�ϵͳ��FAT32��
 ��⣺�ļ�ϵͳ��ʼ��������������ļ�ϵͳ��ʼ���������ϵ�װ�룬Ϊ�Ժ���ļ���������
       ׼����
***********************************************************************************/
UINT8 znFAT_Init(void)
{
 	struct DBR *pdbr;

 	UINT8 dm[3]=DBR_MARK;

 	znFAT_Device_Read_Sector(MBR_SECTOR,znFAT_Buffer);  
 	if(!(znFAT_Buffer[0]==dm[0] && znFAT_Buffer[1]==dm[1] && znFAT_Buffer[2]==dm[2])) //���0�����Ƿ�ΪDBR����
 	{
  		pInit_Args->BPB_Sector_No=Bytes2Value(((((struct MBR *)(znFAT_Buffer))->Part[0]).StartLBA),4);
 	}
 	else
 	{
  		pInit_Args->BPB_Sector_No=0;
 	}
 	znFAT_Device_Read_Sector((pInit_Args->BPB_Sector_No),znFAT_Buffer); //��ȡDBR����
 	pdbr=(struct DBR *)znFAT_Buffer;
 if(!IS_FAT32_TYPE((pdbr->BS_FilSysType1))) return FSTYPE_NOT_FAT32; //FAT32�ļ�ϵͳ���ͼ���
 pInit_Args->BytesPerSector  =Bytes2Value((pdbr->BPB_BytesPerSec),2);//װ��ÿ�����ֽ�����BytesPerSector��
 pInit_Args->FATsectors      =Bytes2Value((pdbr->BPB_FATSz32)    ,4);//װ��FAT��ռ�õ���������FATsectors��
 pInit_Args->SectorsPerClust =pdbr->BPB_SecPerClus;//װ��ÿ����������SectorsPerClust ��

 pInit_Args->FirstFATSector  =Bytes2Value((pdbr->BPB_RsvdSecCnt) ,2)+pInit_Args->BPB_Sector_No;//װ���һ��FAT�������ŵ�FirstFATSector ��

 pInit_Args->FirstDirSector  =(pInit_Args->FirstFATSector)+(pdbr->BPB_NumFATs)*(pInit_Args->FATsectors); //װ���һ��Ŀ¼������FirstDirSector��

 pInit_Args->Total_SizeKB    =Bytes2Value((pdbr->BPB_TotSec32),4)/2;  //���̵�����������λ��KB

 if(Find_FSINFO_Sec(&(pInit_Args->FSINFO_Sec))) //����FSINFO��Ϣ����
 {
  return ERR_FAIL;
 }

 znFAT_Device_Read_Sector((pInit_Args->FSINFO_Sec),znFAT_Buffer);
 pInit_Args->Free_nCluster=Bytes2Value(((struct FSInfo *)znFAT_Buffer)->Free_Cluster,4); //��ȡʣ�����

 if(0XFFFFFFFF==pInit_Args->Free_nCluster) //���һ�����̸�ʽ����û�о�꣬����洢�ռ��û��һ��ռ�ã���ʱFSINFO��¼��ʣ��մ���Ϊ0XFFFFFFFF
 {
  pInit_Args->Free_nCluster=(((pInit_Args->Total_SizeKB*2)-(pInit_Args->FirstDirSector))/(pInit_Args->SectorsPerClust))-1;
 }

 if(Search_Free_Cluster_From_Start(&(pInit_Args->Next_Free_Cluster)))//��������FAT���������õĿ��д�
 {                                                                   //�˲������ܻ�ķѽ϶�ʱ�䣬��û�а취
  return ERR_FAIL;                                                   //FSINFO�еĿ��дزο�ֵ������֤��ȷ
 }                                                                   //������ά��������������鷳

 #ifdef RT_UPDATE_FSINFO //��ʱ��FSINFO�������и���
 Update_FSINFO();
 #endif

 #ifndef RT_UPDATE_CLUSTER_CHAIN
 #ifndef USE_ALONE_CCCB
 Memory_Set((UINT8 *)pcccb_buf,sizeof(UINT32)*CCCB_LEN,0);
 #endif
 #endif

 #ifdef USE_EXCHANGE_BUFFER
 #ifndef USE_ALONE_EXB
 Memory_Set(pexb_buf,512,0);
 #endif
 #endif

 return ERR_SUCC;
}

#ifndef RT_UPDATE_CLUSTER_CHAIN

#ifdef USE_ALONE_CCCB
UINT8 CCCB_To_Alone(void) //���򵽵�ǰ�ļ���CCCB	 
{
 pcccb_buf=(just_file->acccb_buf);
 pcccb_curval=&(just_file->acccb_curval);
 pcccb_counter=&(just_file->acccb_counter);

 return 0;
}
#endif

UINT32 CCCB_Get_Next_Cluster(UINT32 cluster)
{
 UINT32 pos=CCCB_LEN-1;
 UINT32 i=0,temp=0;

 if(pcccb_buf==(UINT32 *)0) return 0;
 
 if(0==pcccb_buf[0]) return 0; //���CCCBδ��ռ�ã���ֱ�ӷ���0

 #ifndef USE_ALONE_CCCB
 if(Dev_No!=(*pcccb_curdev)) return 0; //�����ǰռ��SCCCB���豸��������ѡ�����豸����ֱ�ӷ���0
 if(just_file!=pcccb_cur_oc) return 0; //�����ǰռ��SCCCB���ļ������������ڲ������ļ�����ֱ�ӷ���0
 #endif

 while(0==pcccb_buf[pos]) pos--;

 if(cluster>=pcccb_buf[pos] && cluster<=(*pcccb_curval))
 {
  if(cluster==(*pcccb_curval)) return 0X0FFFFFFF;
  if(cluster==pcccb_buf[pos])
  {
   if(pcccb_buf[pos]==(*pcccb_curval)) return 0X0FFFFFFF;
  }
  return (cluster+1);
 }

 temp=pos/2;
 for(i=0;i<temp;i++)
 {
  if(cluster>=pcccb_buf[2*i] && cluster<=pcccb_buf[2*i+1])
  {
   if(cluster==pcccb_buf[2*i+1]) return pcccb_buf[2*i+2];
   if(cluster==pcccb_buf[2*i])
   {
    if(pcccb_buf[2*i]==pcccb_buf[2*i+1]) return pcccb_buf[2*i+2];
   }
   return (cluster+1);
  }  
 }

 return 0;
}

UINT8 CCCB_Update_FAT(void)
{
 UINT32 i=0,j=0,temp=0,temp1=0;
 UINT32 old_clu=0,cur_clu=0,clu_sec=0;

 #ifndef USE_ALONE_CCCB
 UINT8 old_devno=Dev_No;
 struct znFAT_Init_Args *old_pinit_args=pInit_Args;
 #endif

 struct FAT_Sec *pFAT_Sec=(struct FAT_Sec *)znFAT_Buffer; //�����ݻ������׵�ַǿתΪFAT_Sec�ṹ���ָ������

 if(pcccb_buf==(UINT32 *)0) return 0;

 if(0==pcccb_buf[0]) return 0; //CCCB��δ��ռ�ã��޴����ɸ���

 #ifndef USE_ALONE_CCCB
 Dev_No=(*pcccb_curdev); pInit_Args=pcccb_cur_initargs;
 #endif

 old_clu=cur_clu=pcccb_buf[0];
 clu_sec=(old_clu/NITEMSINFATSEC); //����ǰһ�صĴ������ڵ�FAT����
 znFAT_Device_Read_Sector(clu_sec+(pInit_Args->FirstFATSector),znFAT_Buffer);
 
 pcccb_buf[(*pcccb_counter)]=(*pcccb_curval); //�����һ�����䲹��

 temp1=((*pcccb_counter)+1)/2;
 for(i=0;i<temp1;)
 {
  for(j=pcccb_buf[2*i]+1;j<=pcccb_buf[2*i+1];j++)
  {
   cur_clu++;

   if(clu_sec!=(old_clu/NITEMSINFATSEC))
   {
    znFAT_Device_Write_Sector(clu_sec+(pInit_Args->FirstFATSector),znFAT_Buffer);
    znFAT_Device_Write_Sector(clu_sec+(pInit_Args->FirstFATSector+pInit_Args->FATsectors),znFAT_Buffer);
     
	clu_sec=(old_clu/NITEMSINFATSEC);
    znFAT_Device_Read_Sector(clu_sec+(pInit_Args->FirstFATSector),znFAT_Buffer);
   }

   temp=(UINT8)(old_clu%NITEMSINFATSEC);
   (((pFAT_Sec->items)[temp]).Item)[0]=(UINT8)(cur_clu&0X000000FF)      ;  //��������ǰ��Ĵ�����   
   (((pFAT_Sec->items)[temp]).Item)[1]=(UINT8)((cur_clu&0X0000FF00)>>8) ;
   (((pFAT_Sec->items)[temp]).Item)[2]=(UINT8)((cur_clu&0X00FF0000)>>16);
   (((pFAT_Sec->items)[temp]).Item)[3]=(UINT8)((cur_clu&0XFF000000)>>24);
	
   old_clu=cur_clu;
  }

  cur_clu=((i==(temp1-1))?(0X0FFFFFFF):(pcccb_buf[2*i+2])); //Ŀ���ȡ��һ�ضεĿ�ʼ��;

  if(clu_sec!=(old_clu/NITEMSINFATSEC))
  {
   znFAT_Device_Write_Sector(clu_sec+(pInit_Args->FirstFATSector),znFAT_Buffer);
   znFAT_Device_Write_Sector(clu_sec+(pInit_Args->FirstFATSector+pInit_Args->FATsectors),znFAT_Buffer);
     
   clu_sec=(old_clu/NITEMSINFATSEC);
   znFAT_Device_Read_Sector(clu_sec+(pInit_Args->FirstFATSector),znFAT_Buffer);
  }

  temp=(UINT8)(old_clu%NITEMSINFATSEC);
  (((pFAT_Sec->items)[temp]).Item)[0]=(UINT8)(cur_clu&0X000000FF)      ;  //��������ǰ��Ĵ�����   
  (((pFAT_Sec->items)[temp]).Item)[1]=(UINT8)((cur_clu&0X0000FF00)>>8) ;
  (((pFAT_Sec->items)[temp]).Item)[2]=(UINT8)((cur_clu&0X00FF0000)>>16);
  (((pFAT_Sec->items)[temp]).Item)[3]=(UINT8)((cur_clu&0XFF000000)>>24);

  old_clu=cur_clu;
  i++;
 }
 znFAT_Device_Write_Sector(clu_sec+(pInit_Args->FirstFATSector),znFAT_Buffer);
 znFAT_Device_Write_Sector(clu_sec+(pInit_Args->FirstFATSector+pInit_Args->FATsectors),znFAT_Buffer);

 //============================================================================================
 Memory_Set((UINT8 *)pcccb_buf,sizeof(UINT32)*CCCB_LEN,0); //���CCCB
 (*pcccb_counter)=0;

 #ifndef USE_ALONE_CCCB
 pcccb_cur_oc=(struct FileInfo *)0;
 *pcccb_curdev=(UINT8)(-1);
 pcccb_cur_initargs=(struct znFAT_Init_Args *)0;

 Dev_No=old_devno; pInit_Args=old_pinit_args; //�ָ��豸���� ���豸��ص��ļ�ϵͳ��������
 #endif

 return 0;
}
#endif

/***********************************************************************************
 ���ܣ���ȡ��һ��
 �βΣ���ǰ��
 ���أ���һ�صĴغ�
 ��⣺�˺�����znFAT��Ƶ������
***********************************************************************************/
#ifdef GET_NEXT_CLUSTER 
UINT32 Get_Next_Cluster(UINT32 cluster)
{
 UINT32 clu_sec=0;
 struct FAT_Sec *pFAT_Sec;
 struct FAT_Item *pFAT_Item;

 #ifndef RT_UPDATE_CLUSTER_CHAIN
 UINT32 next_clu=0;
 #endif

 #ifndef RT_UPDATE_CLUSTER_CHAIN //���ʹ����CCCB�����ȡ��һ��ʱ���ȵ�CCCB��Ѱ�ң�Ȼ���ٵ�FAT��ȥ��
 if(0!=get_next_cluster_in_cccb)
 {
  next_clu=CCCB_Get_Next_Cluster(cluster);
  if(0!=next_clu) return next_clu;
 }
 #endif

 clu_sec=(cluster/NITEMSINFATSEC)+(pInit_Args->FirstFATSector); //ָ���صĴ������ڵ�����Ϊ��FAT���ڵ�ƫ��������
                                                          
 znFAT_Device_Read_Sector(clu_sec,znFAT_Buffer); //���������ڵ��������ݶ��뻺����

 pFAT_Sec=(struct FAT_Sec *)znFAT_Buffer; //�����ݻ������׵�ַǿתΪFAT_Sec�ṹ���ָ������

 pFAT_Item=&((pFAT_Sec->items)[cluster%NITEMSINFATSEC]); //��ȡָ���صĴ��������������еĵ�ַ

 return Bytes2Value((UINT8 *)pFAT_Item,NFATITEMBYTES); //���ش����ֵ����ָ������һ�صĴغ�
}
#endif

/***********************************************************************************
 ���ܣ��ļ����ݶ�λ
 �βΣ�pfi:ָ���ļ���Ϣ���ϵ�ָ�� offset:Ŀ��ƫ����
 ���أ�0
 ��⣺���offset�����ļ���С����λ���ļ�ĩβ���˺����Ѿ������ݶ�д�������ɣ�ʹ��
       ��һ�㲻��Ҫ���ô˺�����
***********************************************************************************/
#ifdef ZNFAT_SEEK
UINT8 znFAT_Seek(struct FileInfo *pfi,UINT32 offset)
{
 UINT32 Cluster_Size=((pInit_Args->SectorsPerClust)*(pInit_Args->BytesPerSector)); //����ص����ֽ����ݣ���������ظ�����
 UINT32 temp=0,temp1=0,temp2=0,len=0,k=0,ncluster=0,have_read=0;

 just_file=pfi;

 #ifndef RT_UPDATE_CLUSTER_CHAIN
 get_next_cluster_in_cccb=1;
 #ifdef USE_ALONE_CCCB
 CCCB_To_Alone();
 #endif
 #endif

 if(offset<(pfi->File_Size)) //���Ҫ��λ����ƫ����С���ļ���С����ض������ļ�ĩβ
 {
  pfi->File_IsEOF=BOOL_FALSE;
 }

 if(offset==(pfi->File_CurOffset)) return 0; //���Ҫ��λ��λ�������ǵ�ǰƫ��������ֱ�ӷ���

 if(offset<(pfi->File_CurOffset)) //���Ҫ��λ��λ���ڵ�ǰƫ����֮ǰ�����Ȼص��ļ���㣬��Ϊ�����ǵ����
 {
  pfi->File_CurClust=pfi->File_StartClust;
  pfi->File_CurSec=SOC(pfi->File_CurClust);
  pfi->File_CurPos=0;
  pfi->File_CurOffset=0; 
  pfi->File_IsEOF=BOOL_FALSE;
 }
 
 len=offset-(pfi->File_CurOffset); //����Ŀ��ƫ��������ǰƫ����֮������ݳ���
 
 if(offset>=(pfi->File_Size)) //����ӵ�ǰλ�ÿ�ʼҪ��ȡ�����ݳ���len��С���ļ���С
 {
  len=(pfi->File_Size-pfi->File_CurOffset);    //��len������������Ϊ�ļ�ʣ��ɶ���������
  pfi->File_IsEOF=BOOL_TRUE;    //��������£��ļ���Ȼ�����ĩβ��                    
 }
 
 //=================================================================== 
 if((pfi->File_CurOffset%Cluster_Size)!=0) //�����ǰƫ�����Ǵش�С��������˵����λ�ü�Ϊ���ؿ�ʼ
 {                                         //��Ҫ�ٽ��е�ǰ�������ݴ���ֱ�ӽ����-����-�ֽڽ׶�
  if(len<=(pInit_Args->BytesPerSector-pfi->File_CurPos))
  {
   //���µ�ǰλ�ò���
   if((pInit_Args->BytesPerSector-pfi->File_CurPos)==len) //������ö�����ǰ������ĩβ
   {
    if(IS_END_SEC_OF_CLU(pfi->File_CurSec,pfi->File_CurClust))//�����ǰ�����ǵ�ǰ�ص����һ������                     
    {
     if(!pfi->File_IsEOF) 
	 {
	  pfi->File_CurClust=Get_Next_Cluster(pfi->File_CurClust); 
	 }
     pfi->File_CurSec=SOC(pfi->File_CurClust);
    }
    else
    {
     pfi->File_CurSec++;	
    }
    pfi->File_CurPos=0; 
   }
   else
   {
    pfi->File_CurPos+=((UINT16)len); 
   }	
   pfi->File_CurOffset+=len;

   return NUL_RET;
  }
  //===================================================================
  else
  {
   temp=(pInit_Args->BytesPerSector-pfi->File_CurPos); //����ǰ������ʣ�������������м����temp
   have_read+=temp;
 	
   if(!(IS_END_SEC_OF_CLU(pfi->File_CurSec,pfi->File_CurClust))) //�����ǰ�������ǵ�ǰ�ص����һ������
   {
    pfi->File_CurSec++;
    pfi->File_CurPos=0; 

    temp2=(len-have_read); //����ʣ��������
    temp1=((LAST_SEC_OF_CLU(pfi->File_CurClust)-(pfi->File_CurSec-1))*(pInit_Args->BytesPerSector)); //ʣ����������������
    if(temp2<=temp1) //���ʣ��������xxx
    {
	 //��˵��Ҫ���������ڵ�ǰ���ڣ�û�п絽��һ��	   
     temp=temp2/(pInit_Args->BytesPerSector); //���㵱ǰ������������ȡ�Ľ�������
   	 have_read+=((pInit_Args->BytesPerSector)*temp);

     if(temp2==temp1)
     {
      if(!pfi->File_IsEOF) 
	  {
	   pfi->File_CurClust=Get_Next_Cluster(pfi->File_CurClust); 
	  }
      pfi->File_CurSec=SOC(pfi->File_CurClust); 
      pfi->File_CurPos=0;
     }
     else
     {
      pfi->File_CurSec+=temp; 
      //���µ�ǰλ�ò���
      pfi->File_CurPos=(UINT16)(len-have_read);
     }
     pfi->File_CurOffset+=len; 
    
     return NUL_RET;
    }
    else //���ʣ�����ݵ�����������С�ڵ�ǰ�ص�ʣ������������Ҫ�������ݲ����ڵ�ǰ���ڣ��Ѿ������
    {
   	 temp=LAST_SEC_OF_CLU(pfi->File_CurClust)-(pfi->File_CurSec)+1; //���㵱ǰ�ص�ʣ����������
   	 have_read+=((pInit_Args->BytesPerSector)*temp);
    }
   }
  
   //���µ�ǰλ�ò�������ʱ�Ѿ����굱ǰ�ص�����ʣ�����ݣ��絽��һ��
   pfi->File_CurClust=Get_Next_Cluster(pfi->File_CurClust);
   pfi->File_CurSec=SOC(pfi->File_CurClust); 
   pfi->File_CurPos=0;    
  }
 }
 //----------------------------�����Ǵ���ǰ���ڵ�����-------------------------------------
 if(len-have_read>0) 
 {
  ncluster=(len-have_read)/Cluster_Size; //����ʣ�����ݵ�������

  //���µ�ǰλ�ò�������ʱ�Ѿ��������е���������

  for(k=0;k<ncluster;k++) //��ȡ��������
  {
   have_read+=(Cluster_Size);
   if(!((len-have_read)==0 && pfi->File_IsEOF))  
   {
	pfi->File_CurClust=Get_Next_Cluster(pfi->File_CurClust);
   }
  }

  pfi->File_CurSec=SOC(pfi->File_CurClust);
 
  //----------------------------�����Ǵ�����������------------------------------------------  
  if(len-have_read>0)
  {
   temp=(len-have_read)/(pInit_Args->BytesPerSector); //��������ʣ�����ݵ���������
   have_read+=((pInit_Args->BytesPerSector)*temp);   

   pfi->File_CurSec+=temp;
   //----------------------------�����Ǵ�������������----------------------------------------
   if(len-have_read>0)
   {  
    //���µ�ǰλ�ò�������ʱ���ݶ�ȡ�����Ѿ�����
    pfi->File_CurPos=(UINT16)(len-have_read);    
   }
   //----------------------------�����Ǵ�����������ڵ�ʣ���ֽ�----------------------------------------
  }
 }

 pfi->File_CurOffset+=len;

 return 0;
}
#endif

/******************************************************************************************
 ���ܣ��ļ����ݶ�ȡ
 �βΣ�pfi:ָ���ļ���Ϣ���ϵ�ָ�� offset:���ݶ�ȡ�Ŀ�ʼƫ���� len:Ҫ��ȡ���ֽ���
       app_Buffer:Ӧ�����ݻ�����ָ��
 ���أ�ʵ�ʶ�ȡ�������ݳ��� 
 ��⣺�����offsetλ�ö�ȡ�����ݳ����Ѿ���Խ���ļ���С�������ȡoffsetλ�õ��ļ�ĩβ������
       ��ˣ�����ļ���ƫ��λ���Ѿ����ļ�ĩβ���ٶ�����ж�ȡ�����Ȼ���������ݣ�������0��
       �������Ϊ�ļ�������ȫ������ı�־����ȻҲ���Կ�File_IsEOF�����־�����Ϊ1��˵���ļ�
       �Ѿ���ĩβ�ˣ��ٻ��߿��ԱȽ��ļ���ǰƫ����File_CurOffset���ļ���С�Ĳ�ֵ�����Ϊ0����
       �ļ��ѵ�ĩβ�����������ݽ�������app_Bufferָ���Ӧ�����ݻ������У�Ҫע�⻺������С��
       ��ֹ�ڴ������
******************************************************************************************/
#ifdef ZNFAT_READDATA 
UINT32 znFAT_ReadData(struct FileInfo *pfi,UINT32 offset,UINT32 len,UINT8 *app_Buffer)//
{
 UINT32 Cluster_Size=0,iClu=0,next_clu=0,start_clu=0,end_clu=0;
 UINT32 temp=0,temp1=0,temp2=0,ncluster=0,have_read=0;

 just_file=pfi;

 #ifndef RT_UPDATE_CLUSTER_CHAIN
 get_next_cluster_in_cccb=1;
 #ifdef USE_ALONE_CCCB
 CCCB_To_Alone();
 #endif
 #endif

 znFAT_Seek(pfi,offset); //�ļ���λ

 if(0==len) return 0; //���Ҫ��ȡ�����ݳ���Ϊ0����ֱ�ӷ���
 
 Cluster_Size=(pInit_Args->SectorsPerClust*pInit_Args->BytesPerSector); //����ص����ֽ����ݣ���������ظ�����
 
 if((pfi->File_CurOffset+len)>=(pfi->File_Size)) //����ӵ�ǰλ�ÿ�ʼҪ��ȡ�����ݳ���len��С���ļ���С
 {
  len=(pfi->File_Size-pfi->File_CurOffset);    //��len������������Ϊ�ļ�ʣ��ɶ���������
  pfi->File_IsEOF=BOOL_TRUE;    //��������£��ļ���Ȼ�����ĩβ��                    
 }
 
 //============================================================================================================ 
 if((pfi->File_CurOffset%Cluster_Size)!=0) //�����ǰƫ�����Ǵش�С��������˵����λ�ü�Ϊ���ؿ�ʼ
 {                                         //��Ҫ�ٽ��е�ǰ�������ݴ���ֱ�ӽ����-����-�ֽڽ׶�
  znFAT_Device_Read_Sector(pfi->File_CurSec,znFAT_Buffer); //����ǰ���������ڲ�������

  temp=pInit_Args->BytesPerSector-pfi->File_CurPos; //���㵱ǰ�����е�ʣ��������

  if(len<=temp)
  {
   Memory_Copy(app_Buffer,znFAT_Buffer+(pfi->File_CurPos),len);//���ڲ���������Ҫ�������ݿ���Ӧ�û�����
   
   //���µ�ǰλ�ò���
   if(temp==len) //������ö�����ǰ������ĩβ
   {
    if(IS_END_SEC_OF_CLU(pfi->File_CurSec,pfi->File_CurClust))//�����ǰ�����ǵ�ǰ�ص����һ������                     
    {
     if(!pfi->File_IsEOF) //��������ļ�ĩβ
	 {
	  pfi->File_CurClust=Get_Next_Cluster(pfi->File_CurClust); //�����С����ء� 
	 }
     pfi->File_CurSec=SOC(pfi->File_CurClust);
    }
    else
    {
     pfi->File_CurSec++;	
    }
    pfi->File_CurPos=0; 
   }
   else
   {
    pfi->File_CurPos+=(UINT16)len; 
   }	
   pfi->File_CurOffset+=len;

   return len;
  }
  //===========================================================================================================
  else
  {
   temp=(pInit_Args->BytesPerSector-pfi->File_CurPos); //����ǰ������ʣ�������������м����temp

   Memory_Copy(app_Buffer,znFAT_Buffer+(pfi->File_CurPos),temp); //����ǰ����ʣ�������ܵ�Ӧ�û�����
   have_read+=temp;
 	
   if(!(IS_END_SEC_OF_CLU(pfi->File_CurSec,pfi->File_CurClust))) //�����ǰ�������ǵ�ǰ�ص����һ������
   {
    pfi->File_CurSec++;
    pfi->File_CurPos=0; 

    temp2=(len-have_read); //����ʣ��������
    temp1=((LAST_SEC_OF_CLU(pfi->File_CurClust)-(pfi->File_CurSec-1))*(pInit_Args->BytesPerSector)); //ʣ����������������
    if(temp2<=temp1) //���Ҫ����ʣ��������С�ڵ��ڵ�ǰ��ʣ�������� 
    {
	 //��˵��Ҫ���������ڵ�ǰ���ڣ�û�п絽��һ��
	   
     temp=temp2/(pInit_Args->BytesPerSector); //����Ҫ��ȡ��ʣ�������㹻���������� 
     
	 znFAT_Device_Read_nSector(temp,pfi->File_CurSec,app_Buffer+have_read);
	 have_read+=((pInit_Args->BytesPerSector)*temp);

     if(temp2==temp1)
     {
      if(!pfi->File_IsEOF) //��������ļ�ĩβ
	  {
	   pfi->File_CurClust=Get_Next_Cluster(pfi->File_CurClust); //�����С����ء� 
	  }
      pfi->File_CurSec=SOC(pfi->File_CurClust); 
      pfi->File_CurPos=0;
     }
     else
     {
      pfi->File_CurSec+=temp; 
	  temp=len-have_read;

      znFAT_Device_Read_Sector(pfi->File_CurSec,znFAT_Buffer); //��ǰ�����п��ܻ��в���ʣ������Ҫ��
      Memory_Copy(app_Buffer+have_read,znFAT_Buffer,temp); //���������������������Ӧ�û�����
      //���µ�ǰλ�ò���
      pfi->File_CurPos=(UINT16)temp;
      
     }
     pfi->File_CurOffset+=len; 
    
     return len;
    }
    else //���ʣ�����ݵ�����������С�ڵ�ǰ�ص�ʣ������������Ҫ�������ݲ����ڵ�ǰ���ڣ��Ѿ������
    {
   	 temp=(LAST_SEC_OF_CLU(pfi->File_CurClust))-(pfi->File_CurSec)+1; //���㵱ǰ�ػ��м��������� 

     znFAT_Device_Read_nSector(temp,(pfi->File_CurSec),app_Buffer+have_read);
     have_read+=((pInit_Args->BytesPerSector)*temp);
    }
   }
  
   //���µ�ǰλ�ò�������ʱ�Ѿ����굱ǰ�ص�����ʣ�����ݣ��絽��һ��
   pfi->File_CurClust=Get_Next_Cluster(pfi->File_CurClust); //���ﲻ����������ء� 
   pfi->File_CurSec=SOC(pfi->File_CurClust); 
   pfi->File_CurPos=0;    
  }
 }
 //----------------------------�����Ǵ���ǰ���ڵ�����-------------------------------------

 temp1=len-have_read;
 ncluster=temp1/Cluster_Size; //����ʣ�����ݵ�������
 if(ncluster>0) //ʣ�����������㹻һ����
 {
  //���¼��������ضΣ��Ծ���ʹ�ö�������ȡ����
  start_clu=end_clu=pfi->File_CurClust;

  for(iClu=1;iClu<ncluster;iClu++)
  {
   next_clu=Get_Next_Cluster(end_clu);
   if((next_clu-1)==end_clu)
   {
    end_clu=next_clu;
   }
   else
   {
    znFAT_Device_Read_nSector(((end_clu-start_clu+1)*(pInit_Args->SectorsPerClust)),SOC(start_clu),app_Buffer+have_read);
	have_read+=((end_clu-start_clu+1)*Cluster_Size);
	start_clu=end_clu=next_clu;
   }
  }

  //----------------------------�����Ǵ�����������------------------------------------------  
  temp=temp1%Cluster_Size; //�������ض���֮���Ƿ�������Ҫ��
  if(temp>0) //�������ݺ��滹������Ҫ��
  {
   temp=temp/(pInit_Args->BytesPerSector); //�������ղ������ص�ʣ�����ݵ���������

   

   next_clu=Get_Next_Cluster(end_clu);
   if((next_clu-1)==end_clu) //������һ������Ȼ��ǰ��������ض�����
   {
    znFAT_Device_Read_nSector(((end_clu-start_clu+1)*(pInit_Args->SectorsPerClust)+temp),SOC(start_clu),app_Buffer+have_read);
	have_read+=((pInit_Args->BytesPerSector)*((end_clu-start_clu+1)*(pInit_Args->SectorsPerClust)+temp));
   }
   else //������һ������ǰ��������ضβ�������
   {
    znFAT_Device_Read_nSector(((end_clu-start_clu+1)*(pInit_Args->SectorsPerClust)),SOC(start_clu),app_Buffer+have_read);
	have_read+=(Cluster_Size*(end_clu-start_clu+1));
    znFAT_Device_Read_nSector(temp,SOC(next_clu),app_Buffer+have_read);
	have_read+=(temp*(pInit_Args->BytesPerSector));
   }

   pfi->File_CurClust=next_clu;
   pfi->File_CurSec=(SOC(next_clu)+temp); 

   //----------------------------�����Ǵ�������������----------------------------------------
   temp=len-have_read;
   if(temp>0)
   {
    znFAT_Device_Read_Sector(pfi->File_CurSec,znFAT_Buffer); //�����Ŀ��ܰ���һ����Ҫ�������ݵ����������ڲ�������
    Memory_Copy(app_Buffer+have_read,znFAT_Buffer,temp); //�������������������������Ӧ�û�����
  
    //���µ�ǰλ�ò�������ʱ���ݶ�ȡ�����Ѿ�����
    pfi->File_CurPos=(UINT16)temp;    
   }
   //----------------------------�����Ǵ�����������ڵ�ʣ���ֽ�----------------------------------------
  }
  else //�������ݶ���֮����������Ҫ��
  {
   znFAT_Device_Read_nSector(((end_clu-start_clu+1)*(pInit_Args->SectorsPerClust)),SOC(start_clu),app_Buffer+have_read);

   pfi->File_CurClust=end_clu;
   if(!pfi->File_IsEOF) 
   {
    pfi->File_CurClust=Get_Next_Cluster(end_clu);
   }
   pfi->File_CurSec=SOC(pfi->File_CurClust); 
  }
 }
 else //ʣ������ݲ���һ����
 {
  temp=temp1/(pInit_Args->BytesPerSector); //����ʣ��������㹻��������
  znFAT_Device_Read_nSector(temp,SOC(pfi->File_CurClust),app_Buffer+have_read);
  have_read+=temp*(pInit_Args->BytesPerSector);

  pfi->File_CurSec+=temp;
  
  temp=temp1%(pInit_Args->BytesPerSector); //�������յĲ���һ������������
  if(temp>0) //������������
  {
   znFAT_Device_Read_Sector(pfi->File_CurSec,znFAT_Buffer);
   Memory_Copy(app_Buffer+have_read,znFAT_Buffer,temp);

   pfi->File_CurPos=(UINT16)temp;
  }
 }

 //----------------------------------------------------------------------------------------
 pfi->File_CurOffset+=len;

 return len;
}
#endif

/******************************************************************************************
 ���ܣ��ļ����ݶ�ȡ+�����ض���
 �βΣ�pfi:ָ���ļ���Ϣ���ϵ�ָ�� offset:���ݶ�ȡ�Ŀ�ʼƫ���� len:Ҫ��ȡ���ֽ���
       app_Buffer:Ӧ�����ݻ�����ָ��
 ���أ�ʵ�ʶ�ȡ�������ݳ��� 
 ��⣺�˺�����ʵ�ֹ�������������ݶ�ȡ�������ƣ���ͬ���ڿ��ԶԶ��������ݽ������¶���
       ��������ݶ�ȡ�����ǽ�����ֱ�ӷŵ���Ӧ�����ݻ������У�������ĳЩӲ��ƽ̨��RAM��Դ
       �ȽϽ��ţ����ٳ��ϴ��Ӧ�����ݻ�������̫��ʵ����������������ض���ĸ�����Խ�
       ������ÿһ���ֽ�ʹ��ĳ������ֱ�ӽ��д����������ڻ������С�������ڴ����ֽ����ݵ�
       ������ʹ�����ṩ����������ĺ�������znfat.h�б��궨��ΪData_Redirect������������
       ����������ͨ�����ڷ���������znfat.h�н��궨���Ϊ #define Data_Redirect Send_Byte
       Send_Byte���Ǵ��ڷ����ֽڵĺ����ĺ�������
******************************************************************************************/
#ifdef ZNFAT_READDATAX 
UINT32 znFAT_ReadDataX(struct FileInfo *pfi,UINT32 offset,UINT32 len) //�����ض����ȡ���������ݶ�ȡ������������ʹ�ö���������
{                                                                     //��֮û���㹻��RAM��Դ����ÿ�δ���һ���ֽڣ����Ч�ʲ���
 UINT32 Cluster_Size=0,iClu=0,iSec=0,next_clu=0,start_clu=0,end_clu=0;
 UINT32 temp=0,temp1=0,temp2=0,k=0,ncluster=0,have_read=0;
 UINT32 i=0;

 just_file=pfi;

 #ifndef RT_UPDATE_CLUSTER_CHAIN
 get_next_cluster_in_cccb=1;
 #ifdef USE_ALONE_CCCB
 CCCB_To_Alone();
 #endif
 #endif

 znFAT_Seek(pfi,offset); //�ļ���λ
 
 Cluster_Size=(pInit_Args->SectorsPerClust*pInit_Args->BytesPerSector); //����ص����ֽ����ݣ���������ظ�����
 
 if((pfi->File_CurOffset+len)>=(pfi->File_Size)) //����ӵ�ǰλ�ÿ�ʼҪ��ȡ�����ݳ���len��С���ļ���С
 {
  len=(pfi->File_Size-pfi->File_CurOffset);    //��len������������Ϊ�ļ�ʣ��ɶ���������
  pfi->File_IsEOF=BOOL_TRUE;    //��������£��ļ���Ȼ�����ĩβ��                    
 }
 
 //=================================================================== 
 if((pfi->File_CurOffset%Cluster_Size)!=0) //�����ǰƫ�����Ǵش�С��������˵����λ�ü�Ϊ���ؿ�ʼ
 {                                         //��Ҫ�ٽ��е�ǰ�������ݴ���ֱ�ӽ����-����-�ֽڽ׶�
  znFAT_Device_Read_Sector(pfi->File_CurSec,znFAT_Buffer); //����ǰ���������ڲ�������

  temp=pInit_Args->BytesPerSector-pfi->File_CurPos; //���㵱ǰ�����е�ʣ��������
  if(len<=temp)
  {
   for(i=0;i<len;i++)
   {
	Data_Redirect(znFAT_Buffer[i+(pfi->File_CurPos)]);
   }
   
   //���µ�ǰλ�ò���
   if(temp==len) //������ö�����ǰ������ĩβ
   {
    if(IS_END_SEC_OF_CLU(pfi->File_CurSec,pfi->File_CurClust))//�����ǰ�����ǵ�ǰ�ص����һ������                     
    {
     if(!pfi->File_IsEOF) //��������ļ�ĩβ
	 {
	  pfi->File_CurClust=Get_Next_Cluster(pfi->File_CurClust); //�����С����ء� 
	 }
     pfi->File_CurSec=SOC(pfi->File_CurClust);
    }
    else
    {
     pfi->File_CurSec++;	
    }
    pfi->File_CurPos=0; 
   }
   else
   {
    pfi->File_CurPos+=(UINT16)len; 
   }	
   pfi->File_CurOffset+=len;

   return len;
  }
  //===================================================================
  else
  {
   temp=(pInit_Args->BytesPerSector-pfi->File_CurPos); //����ǰ������ʣ�������������м����temp

   for(i=0;i<temp;i++)
   {
	Data_Redirect(znFAT_Buffer[i+(pfi->File_CurPos)]);
   }
   
   have_read+=temp;
 	
   if(!(IS_END_SEC_OF_CLU(pfi->File_CurSec,pfi->File_CurClust))) //�����ǰ�������ǵ�ǰ�ص����һ������
   {
    pfi->File_CurSec++;
    pfi->File_CurPos=0; 

    temp2=(len-have_read); //����ʣ��������
    temp1=((LAST_SEC_OF_CLU(pfi->File_CurClust)-(pfi->File_CurSec-1))*(pInit_Args->BytesPerSector)); //ʣ����������������
    if(temp2<=temp1) //���Ҫ����ʣ��������С�ڵ��ڵ�ǰ��ʣ�������� 
    {
	 //��˵��Ҫ���������ڵ�ǰ���ڣ�û�п絽��һ��
	   
     temp=temp2/(pInit_Args->BytesPerSector); //����Ҫ��ȡ��ʣ�������㹻���������� 

	 for(iSec=0;iSec<temp;iSec++)
	 {
	  znFAT_Device_Read_Sector(pfi->File_CurSec+iSec,znFAT_Buffer);
	  for(i=0;i<(pInit_Args->BytesPerSector);i++)
	  {
	   Data_Redirect(znFAT_Buffer[i]);
	  }
	 }
	 
	 have_read+=temp*(pInit_Args->BytesPerSector);

     if(temp2==temp1)
     {
      if(!pfi->File_IsEOF) //��������ļ�ĩβ
	  {
	   pfi->File_CurClust=Get_Next_Cluster(pfi->File_CurClust); //�����С����ء� 
	  }
      pfi->File_CurSec=SOC(pfi->File_CurClust); 
      pfi->File_CurPos=0;
     }
     else
     {
      pfi->File_CurSec+=temp; 
	  temp=len-have_read;

      znFAT_Device_Read_Sector(pfi->File_CurSec,znFAT_Buffer); //��ǰ�����п��ܻ��в���ʣ������Ҫ��
      for(i=0;i<temp;i++)
	  {
	   Data_Redirect(znFAT_Buffer[i]);
	  }
      //���µ�ǰλ�ò���
      pfi->File_CurPos=(UINT16)temp;
      
     }
     pfi->File_CurOffset+=len; 
    
     return len;
    }
    else //���ʣ�����ݵ�����������С�ڵ�ǰ�ص�ʣ������������Ҫ�������ݲ����ڵ�ǰ���ڣ��Ѿ������
    {
   	 temp=(LAST_SEC_OF_CLU(pfi->File_CurClust))-(pfi->File_CurSec)+1; //���㵱ǰ�ػ��м��������� 

	 for(iSec=0;iSec<temp;iSec++)
	 {
	  znFAT_Device_Read_Sector(pfi->File_CurSec+iSec,znFAT_Buffer);
	  for(i=0;i<(pInit_Args->BytesPerSector);i++)
	  {
	   Data_Redirect(znFAT_Buffer[i]);
	  }
	 }
	 have_read+=temp*(pInit_Args->BytesPerSector);
    }
   }
  
   //���µ�ǰλ�ò�������ʱ�Ѿ����굱ǰ�ص�����ʣ�����ݣ��絽��һ��
   pfi->File_CurClust=Get_Next_Cluster(pfi->File_CurClust); //���ﲻ����������ء� 
   pfi->File_CurSec=SOC(pfi->File_CurClust); 
   pfi->File_CurPos=0;    
  }
 }
 //----------------------------�����Ǵ���ǰ���ڵ�����-------------------------------------
 temp1=len-have_read;
 ncluster=temp1/Cluster_Size; //����ʣ�����ݵ�������

 if(ncluster>0) //ʣ�����������㹻һ����
 {
  //���¼��������ضΣ��Ծ���ʹ�ö�������ȡ����
  start_clu=end_clu=pfi->File_CurClust;

  for(iClu=1;iClu<ncluster;iClu++)
  {
   next_clu=Get_Next_Cluster(end_clu);
   if((next_clu-1)==end_clu)
   {
    end_clu=next_clu;
   }
   else
   {
    temp=((end_clu-start_clu+1)*(pInit_Args->SectorsPerClust));
    for(iSec=0;iSec<temp;iSec++)
	{
	 znFAT_Device_Read_Sector(SOC(start_clu)+iSec,znFAT_Buffer);
	 for(i=0;i<(pInit_Args->BytesPerSector);i++)
	 {
	  Data_Redirect(znFAT_Buffer[i]);
	 }
	}
    have_read+=temp*(pInit_Args->BytesPerSector);
	start_clu=end_clu=next_clu;
   }
  }

  //----------------------------�����Ǵ�����������------------------------------------------  
  temp=temp1%Cluster_Size; //�������ض���֮���Ƿ�������Ҫ��
  if(temp>0) //�������ݺ��滹������Ҫ��
  { 
   temp=temp/(pInit_Args->BytesPerSector); //�������ղ������ص�ʣ�����ݵ���������
   next_clu=Get_Next_Cluster(end_clu);
   if((next_clu-1)==end_clu) //������һ������Ȼ��ǰ��������ض�����
   {
    temp2=((end_clu-start_clu+1)*(pInit_Args->SectorsPerClust)+temp);
    for(iSec=0;iSec<temp2;iSec++)
	{
	 znFAT_Device_Read_Sector(SOC(start_clu)+iSec,znFAT_Buffer);

	 for(i=0;i<(pInit_Args->BytesPerSector);i++)
	 {
	  Data_Redirect(znFAT_Buffer[i]);
	 }
	}
   }
   else //������һ������ǰ��������ضβ�������
   {
	temp2=((end_clu-start_clu+1)*(pInit_Args->SectorsPerClust));
    for(iSec=0;iSec<temp2;iSec++)
	{
	 znFAT_Device_Read_Sector(SOC(start_clu)+iSec,znFAT_Buffer);

	 for(i=0;i<(pInit_Args->BytesPerSector);i++)
	 {
	  Data_Redirect(znFAT_Buffer[i]);
	 }
	}

    for(iSec=0;iSec<temp;iSec++)
	{
	 znFAT_Device_Read_Sector(SOC(next_clu)+iSec,znFAT_Buffer);

	 for(i=0;i<(pInit_Args->BytesPerSector);i++)
	 {
	  Data_Redirect(znFAT_Buffer[i]);
	 }
	}
   }
   
   have_read+=((end_clu-start_clu+1)*(pInit_Args->SectorsPerClust)+temp)*(pInit_Args->BytesPerSector);

   pfi->File_CurClust=next_clu;
   pfi->File_CurSec=(SOC(next_clu)+temp); 

   //----------------------------�����Ǵ�������������----------------------------------------
   temp=len-have_read;
   if(temp>0)
   {
    znFAT_Device_Read_Sector(pfi->File_CurSec,znFAT_Buffer); //�����Ŀ��ܰ���һ����Ҫ�������ݵ����������ڲ�������
  
	for(i=0;i<temp;i++)
	{
	 Data_Redirect(znFAT_Buffer[i]);
	}

    //���µ�ǰλ�ò�������ʱ���ݶ�ȡ�����Ѿ�����
    pfi->File_CurPos=(UINT16)temp;    
   }
   //----------------------------�����Ǵ�����������ڵ�ʣ���ֽ�----------------------------------------
  }
  else //�������ݶ���֮����������Ҫ��
  {
   temp2=((end_clu-start_clu+1)*(pInit_Args->SectorsPerClust));
   for(iSec=0;iSec<temp2;iSec++)
   {
	znFAT_Device_Read_Sector(SOC(start_clu)+iSec,znFAT_Buffer);
	for(i=0;i<(pInit_Args->BytesPerSector);i++)
	{
	 Data_Redirect(znFAT_Buffer[i]);
	}
   }

   pfi->File_CurClust=end_clu;
   if(!pfi->File_IsEOF) 
   {
    pfi->File_CurClust=Get_Next_Cluster(end_clu);
   }
   pfi->File_CurSec=SOC(pfi->File_CurClust); 
  }
 }
 else //ʣ������ݲ���һ����
 {
  temp=temp1/(pInit_Args->BytesPerSector); //����ʣ��������㹻��������
  for(iSec=0;iSec<temp;iSec++)
  {
   znFAT_Device_Read_Sector(SOC(pfi->File_CurClust)+iSec,znFAT_Buffer);
   for(i=0;i<(pInit_Args->BytesPerSector);i++)
   {
	Data_Redirect(znFAT_Buffer[i]);
   }
  }

  pfi->File_CurSec+=temp;
  
  temp=temp1%(pInit_Args->BytesPerSector); //�������յĲ���һ������������
  if(temp>0) //������������
  {
   znFAT_Device_Read_Sector(pfi->File_CurSec,znFAT_Buffer);
   for(i=0;i<temp;i++)
   {
	Data_Redirect(znFAT_Buffer[i]);
   }

   pfi->File_CurPos=temp;
  }
 }

 //----------------------------------------------------------------------------------------
 pfi->File_CurOffset+=len;

 return len;
}
#endif

//----------------------------���º������ڼ����ļ����ĺϷ���----------------------------------

/******************************************************************************************
 ���ܣ�����ļ������Ƿ��зǷ��ַ����Ƿ��ַ��� \/:"<>| 
 �βΣ�pfn:ָ���ļ�����ָ��
 ���أ����н�����ɹ���ʧ��  �ɹ����ļ������޷Ƿ��ַ�����֮���� 
 ��⣺�����ļ����зǷ��ַ��ļ�飬��ʵ���������ַ�*?������znFAT��֧���ļ�����ͨ��ģ����
       ���ﲻ����*��?���ڴ��ļ���Ŀ¼��ʱ����ʹ�õ��ļ������Ǵ���*��?���������ļ���Ŀ¼ʱ
       �ļ������򲻿���*��?����ʹ����ע�⣡
******************************************************************************************/
#ifdef CHECK_ILLEGAL_CHAR
UINT8 Check_Illegal_Char(INT8 *pfn) 
{
 UINT32 i=0;
 while(pfn[i])
 {
  if(('\\'==pfn[i]) || ('/'==pfn[i]) || (':'==pfn[i])  
   || /*('*'==pfn[i]) || ('?'==pfn[i]) ||*/ ('"'==pfn[i])  
   || ('<'==pfn[i]) || ('>'==pfn[i]) || ('|'==pfn[i]))
  return ERR_FAIL;
  i++;
 }
 return ERR_SUCC;
}
#endif

/******************************************************************************************
 ���ܣ�����ļ������Ƿ��������ַ���+[],;=space
 �βΣ�pfn:ָ���ļ�����ָ��
 ���أ����н�����ɹ���ʧ��  �ɹ����ļ������������ַ�����֮���� 
 ��⣺����8.3��ʽ���ļ�����������а����������ַ������䱻��Ϊ���ļ��������Ƕ�������Ϊ����
       �볤���ڴ������ǲ�ͬ�ģ���˶��������ֿ�����SFN��Short FileName����д������SFN�ĺ�
       ����Զ����ļ��
******************************************************************************************/
#ifdef CHECK_SFN_SPECIAL_CHAR
UINT8 Check_SFN_Special_Char(INT8 *pfn) 
{
 UINT32 i=0;
 UINT32 pos=(StringLen(pfn)-1);

 while(' '==pfn[pos]) pos--;
 
 while(i<=pos)
 {
  if( ('+'==pfn[i]) || ('['==pfn[i]) 
   || (']'==pfn[i]) || (','==pfn[i])
   || (' '==pfn[i]) || (';'==pfn[i])
   || ('='==pfn[i]))
  return ERR_FAIL;
  i++;
 }

 return ERR_SUCC;
}
#endif

/******************************************************************************************
 ���ܣ�����ļ����е�.�Ƿ����SFNҪ��
 �βΣ�pfn:ָ���ļ�����ָ��
 ���أ����н�����ɹ���ʧ��
 ��⣺SFN�е�.���ö���1������A..A.TXT��A.A.A��AA.A..A�Ⱦ���SFN���ļ���������.��ʼ
       ���ɸ�.�����ĩβ���������
******************************************************************************************/
#ifdef CHECK_SFN_DOT
UINT8 Check_SFN_Dot(INT8 *pfn) 
{
 UINT32 pos=(StringLen(pfn)-1);
 UINT32 dot_counter=0;

 if('.'==pfn[0]) return ERR_FAIL;

 while(pos>0) 
 {
  if('.'==pfn[pos])
  {
   dot_counter++;
  }
  pos--;
 }

 if(dot_counter>1) return ERR_FAIL;
 else return ERR_SUCC;
}
#endif

/******************************************************************************************
 ���ܣ�����ļ����Ĵ�Сд�Ƿ����SFNҪ��
 �βΣ�pfn:ָ���ļ�����ָ��
 ���أ����н�����ɹ���ʧ��
 ��⣺SFN���ǲ������д�Сд���ŵģ�����AabcC.txt ABC.Txt AbCdef.TXT������SFN�����ǳ���
       ����֮��SFN�����ļ�������չ��������Ǵ�д��Сд������aaa.TXT AAA.txt aaa.txt AA.TXT
       ����SFN
******************************************************************************************/
#ifdef CHECK_SFN_ILLEGAL_LOWER
UINT8 Check_SFN_Illegal_Lower(INT8 *pfn)
{
 UINT32 i=(StringLen(pfn)-1);
 UINT8 flag1=2,flag2=2; 

 while('.'!=pfn[i] && i>0)
 {
  if(pfn[i]>='a' && pfn[i]<='z') 
  {
   if(0==flag1)
   {
	return (UINT8)-1;
   }
   flag1=1;
  }

  if(pfn[i]>='A' && pfn[i]<='Z') 
  {
   if(1==flag1)
   {
	return (UINT8)-1;
   }
   flag1=0;
  }

  i--;
 }

 if(0==i) //���û��.˵��û����չ��
 { 
  if(pfn[0]>='a' && pfn[0]<='z') 
  {
   if(0==flag1)
   {
	return (UINT8)-1;
   }
   flag1=1;
  }

  flag2=flag1; 
  flag1=0;
 }

 if('.'==pfn[i])
 {
  i--;
 }

 while(i>0)
 {
  if(pfn[i]>='a' && pfn[i]<='z') 
  {
   if(0==flag2)
   {
	return (UINT8)-1;
   }
   flag2=1;
  }

  if(pfn[i]>='A' && pfn[i]<='Z') 
  {
   if(1==flag2)
   {
	return (UINT8)-1;
   }
   flag2=0;
  }

  i--;  
 }

 if(2==flag2)
 {
  if(pfn[0]>='a' && pfn[0]<='z') 
  {
   flag2=1;
  }
  if(pfn[0]>='A' && pfn[0]<='Z')
  {
   flag2=0;
  }
 }
 
 return (UINT8)((flag1<<4)|(flag2));
}
#endif

/******************************************************************************************
 ���ܣ�����ļ����ĳ����Ƿ����SFNҪ��
 �βΣ�pfn:ָ���ļ�����ָ��
 ���أ����н�����ɹ���ʧ��
 ��⣺8.3��ʽ��SFN��Ҫ�����ļ����ĳ���0<len<=8 ��չ������0<=len<=3����������һҪ��ľ�Ϊ��
       ��
******************************************************************************************/
#ifdef CHECK_SFN_ILLEGAL_LENGTH
UINT8 Check_SFN_Illegal_Length(INT8 *pfn)
{
 UINT8 dot_pos=0,have_dot=0,i=0;
 UINT8 mainfn_len=0,extfn_len=0;
 UINT32 fn_len=StringLen(pfn);

 while(' '==pfn[fn_len-1]) fn_len--;

 if(fn_len>12) return ERR_FAIL; //fn is longer than 8.3

 for(i=(UINT8)(fn_len-1);i>0;i--) //����Ѱ��. ��һ��.�����ļ�����չ���ķֽ�
 {
  if('.'==pfn[i]) 
  {
   dot_pos=i;
   have_dot=1;
   break;
  }
 }
 
 if(0==have_dot) //û�е�
 {
  mainfn_len=(UINT8)fn_len;
  extfn_len=0;

  if((mainfn_len>0 && mainfn_len<=8))
  {
   return ERR_SUCC;
  }
  else
  {
   return ERR_FAIL;
  }
 }
 else //�е�
 {
  mainfn_len=dot_pos;
  extfn_len=(UINT8)(((UINT8)fn_len)-(UINT8)(dot_pos+1));

  if(( mainfn_len>0  && mainfn_len<=8) 
   && (/*extfn_len>=0  && */extfn_len <=3))
  {
   return ERR_SUCC;
  }
  else
  {
   return ERR_FAIL;
  }
 }	
}
#endif
//--------------------------------���Ϻ������ڼ���SFN�ĺϷ���----------------------------------

/******************************************************************************************
 ���ܣ���һ���ļ�Ŀ¼���е�ǰ11���ֽڣ����ڼ�¼8.3���ļ�����תΪ�ļ���
 �βΣ�name_in_fdi:ָ���ļ�Ŀ¼�������ڼ�¼�ļ������ֽ����е�ָ�� pfilename:���ڼ�¼ת�����
       �ļ����������ָ��
 ���أ����н�����ɹ���ʧ��
 ��⣺�ڴ��ļ���Ŀ¼�Ⱥ����У����ļ�Ŀ¼���е��ļ����ֶ�תΪ�ļ������Է�����Ŀ���ļ�����
       ƥ��ͱȶԡ�����Ҫ��ABC.TXT ������Ӧ���ļ�Ŀ¼���¼����ʽΪABC     TXT�������ǲ���
       ֱ�ӽ��бȽϵģ�Ҫ�Ƚ�ABC     TXTתΪABC.TXT��Ȼ���ٱȽϡ�
******************************************************************************************/
#ifdef TO_FILE_NAME
UINT8 To_File_Name(INT8 *name_in_fdi,INT8 *pfileName)
{
 UINT8 i=0,n=7,m=10;

 while(' '==name_in_fdi[n])
 {
  n--;
 }
 n++;

 while(' '==name_in_fdi[m] && m>=8)
 {
  m--;
 }
 m-=7;

 for(i=0;i<n;i++)
 {
  pfileName[i]=name_in_fdi[i];
 }
 pfileName[i]='.';

 for(i=0;i<m;i++)
 {
  pfileName[n+i+1]=name_in_fdi[8+i];
 }

 if('.'==pfileName[n+m]) pfileName[n+m]=0;
 else pfileName[n+m+1]=0;

 return 0;
}
#endif

/******************************************************************************************
 ���ܣ�����һ���ļ�Ŀ¼��������õ��Ĳ���װ�뵽�ļ���Ϣ������
 �βΣ�pfi:ָ���ļ���Ϣ���ϵ�ָ�� pFDI:ָ���ļ�Ŀ¼���ָ��
 ���أ�0
 ��⣺�˺��������ļ��ĺ������ã��Է���ƥ���������ļ�Ŀ¼����н�������������װ�뵽�ļ���
       Ϣ�����У��Ա���ļ����н�һ���Ĳ���ʱʹ�á�
******************************************************************************************/
#ifdef ANALYSE_FDI
UINT8 Analyse_FDI(struct FileInfo *pfi,struct FDI *pFDI)
{
 UINT32 temp=0,i=0;

 //==========================================================

 just_file=pfi;

 To_File_Name((INT8 *)pFDI,pfi->File_Name);
 
 temp=(StringLen(pfi->File_Name)-1);
 while('.'!=(pfi->File_Name)[temp] && temp>0)
 {
  if(((pFDI->LowerCase)&0x10)!=0) 
  {
   (pfi->File_Name)[temp]=(INT8)Upper2Low((pfi->File_Name)[temp]);
  }
  temp--;
 }
 if(((pFDI->LowerCase)&0x08)!=0) 
 {
  for(i=0;i<temp;i++)
  {
   (pfi->File_Name)[i]=(INT8)Upper2Low((pfi->File_Name)[i]);   
  }
 }

 temp=(StringLen(pfi->File_Name)-1); 
 if(CHK_ATTR_DIR(pFDI->Attributes)) //�����Ŀ¼������.ȥ��
 {
  (pfi->File_Name)[temp+1]='\0';
 }
 //==�����ǰ���LowerCase�ֽڶ����ļ�������չ�ļ�������Сд��

 pfi->File_Attr=pFDI->Attributes; //�ļ�����
 pfi->File_StartClust=Bytes2Value(pFDI->LowClust,2)+Bytes2Value(pFDI->HighClust,2)*65536;
 pfi->File_Size=Bytes2Value(pFDI->FileSize,4);
  
 //�����ļ�����ʱ��������
 temp=Bytes2Value(pFDI->CTime,2);
 pfi->File_CTime.sec=(UINT8)((temp&TIME_SEC_MARK)*2); temp>>=TIME_SEC_NBITS;  //����ʱ���2��λ
 pfi->File_CTime.min=(UINT8)(temp&TIME_MIN_MARK);   temp>>=TIME_MIN_NBITS; //����ʱ��ķ�λ
 pfi->File_CTime.hour=(UINT8)(temp&TIME_HOUR_MARK); //����ʱ���ʱλ
 pfi->File_CTime.sec+=(UINT8)((UINT16)(pFDI->CTime10ms)/100); //�����ϼ���10����λ

 temp=Bytes2Value(pFDI->CDate,2);
 pfi->File_CDate.day=(UINT8)(temp&DATE_DAY_MARK);     temp>>=DATE_DAY_NBITS;   //�������ڵ���λ
 pfi->File_CDate.month=(UINT8)(temp&DATE_MONTH_MARK); temp>>=DATE_MONTH_NBITS; //�������ڵ���λ
 pfi->File_CDate.year=(UINT16)((temp&DATE_YEAR_MARK)+DATE_YEAR_BASE); //�������ڵ���λ��������ݻ�����

 //�����ļ��޸�ʱ��������
 //temp=Bytes2Value(pFDI->MTime,2);
 //pfi->File_MTime.sec=(UINT8)((temp&TIME_SEC_MARK)*2); temp>>=TIME_SEC_NBITS;  //����ʱ���2��λ
 //pfi->File_MTime.min=(UINT8)(temp&TIME_MIN_MARK);   temp>>=TIME_MIN_NBITS; //����ʱ��ķ�λ
 //pfi->File_MTime.hour=(UINT8)(temp&TIME_HOUR_MARK); //����ʱ���ʱλ
 //�ļ����޸�ʱ��û��10����λ��������ֻ�ܱ��ż����

 //temp=Bytes2Value(pFDI->MDate,2);
 //pfi->File_MDate.day=(UINT8)(temp&DATE_DAY_MARK);     temp>>=DATE_DAY_NBITS;   //�������ڵ���λ
 //pfi->File_MDate.month=(UINT8)(temp&DATE_MONTH_MARK); temp>>=DATE_MONTH_NBITS; //�������ڵ���λ
 //pfi->File_MDate.year=(UINT8)((temp&DATE_YEAR_MARK)+DATE_YEAR_BASE); //�������ڵ���λ

 //�����ļ���������
 //temp=Bytes2Value(pFDI->ADate,2);
 //pfi->File_ADate.day=(UINT8)(temp&DATE_DAY_MARK);     temp>>=DATE_DAY_NBITS;   //�������ڵ���λ
 //pfi->File_ADate.month=(UINT8)(temp&DATE_MONTH_MARK); temp>>=DATE_MONTH_NBITS; //�������ڵ���λ
 //pfi->File_ADate.year=(UINT8)((temp&DATE_YEAR_MARK)+DATE_YEAR_BASE); //�������ڵ���λ

 pfi->File_CurClust=pfi->File_StartClust;
 pfi->File_CurSec=(pfi->File_CurClust)?SOC(pfi->File_CurClust):0;
 pfi->File_CurPos=0;
 pfi->File_CurOffset=0;
 pfi->File_IsEOF=(UINT8)((pfi->File_StartClust)?BOOL_FALSE:BOOL_TRUE);

 return 0;
}
#endif

/******************************************************************************************
 ���ܣ�����ļ����Ƿ���ͨ������ͨ�����к���*��?
 �βΣ�pfn:ָ���ļ�����ָ��
 ���أ����н�� �ǻ��
 ��⣺znFAT��֧��ͨ�����ģ�����ͨ����znFAT���ǲ���������ļ����Ϸ��Լ��ġ���ʹ��������
       ע�⡣
******************************************************************************************/
#ifdef IS_WILDFILENAME
UINT8 Is_WildFileName(INT8 *pfn)
{ 
 UINT8 i=0;
 while('\0'!=pfn[i])
 {
  if('*'==pfn[i] || '?'==pfn[i])
  {
   return 1;
  }
  i++;
 }
 return 0;
}
#endif

/******************************************************************************************
 ���ܣ���һ���ַ����в���һ���Ӵ�
 �βΣ�str:�ַ��� substr:�Ӵ� pos:���ַ�����posλ�ÿ�ʼ�����Ӵ�
 ���أ��Ӵ����ַ����еĿ�ʼλ��
 ��⣺
******************************************************************************************/
#ifdef FINDSUBSTR
UINT8 FindSubStr(INT8 *str,INT8 *substr,UINT8 pos)
{
 UINT8 i=pos,j=0,lens=(UINT8)StringLen(str),lent=(UINT8)StringLen(substr);

 while(i<lens && j<lent)
 {
  if(str[i]==substr[j] || '?'==substr[j])
  {
   i++;
   j++;
  }
  else
  {
   i=(UINT8)(i-j+1);
   j=0;
  }
 }
 if(j==lent) return (UINT8)(i-lent); 
 else return (UINT8)0XFF;
}
#endif

/*********************************************************************************************
 ���ܣ��ļ���ƥ�䣬֧��*��?ͨ��
 �βΣ�t:ģ���ļ�������ʹ����������ļ��� s:Ŀ���ļ���  ����tΪa*b.txt sΪaxxb.txt��������ƥ���
       ����tΪa???x.t* sΪaxyzx.txt��������ƥ��ġ�
 ���أ����н�� �ǻ��
 ��⣺��һƥ���㷨�����Ӵ�ƥ�䣬������֤����һ����������㷨ǿ�Ȳ�����DOS�ϵ�ͨ�����Ტ�ۡ�
**********************************************************************************************/
#ifdef SFN_MATCH
UINT8 SFN_Match(INT8 *t,INT8 *s)
{
    UINT8 i=0,j=0,lens=(UINT8)StringLen(s);
    UINT8 lent=(UINT8)StringLen(t);
    INT8 buf[20];
    UINT8 bufp=0;
    //======================================================
    while(j<lent && '*'!=t[j])
    {
        buf[bufp]=(INT8)Lower2Up(t[j]);
        bufp++;
        j++;
    }
    if('\0'==t[j] && (lent!=lens)) return ERR_FAIL;
    buf[bufp]='\0';
    if(FindSubStr(s,buf,0)!=0) return ERR_FAIL;
    i=bufp;
    while(1)
    {
        while(j<lent && '*'==t[j]) j++;
        if(j==lent) return ERR_SUCC;
        bufp=0;
    
        while(j<lent && '*'!=t[j])
        {
            buf[bufp]=(INT8)Lower2Up(t[j]);
            bufp++;
            j++;
        }
        buf[bufp]='\0';
        if(j==lent)
        {
            if(FindSubStr(s,buf,i)!=(lens-bufp)) return ERR_FAIL;
            return ERR_SUCC;
        }
        
        i=FindSubStr(s,buf,i);
        if(0XFF==i) return ERR_FAIL;
        i+=bufp;
    }

}
#endif

/*********************************************************************************************
 ���ܣ��޸�FAT���ĳ������
 �βΣ�cluster:Ҫ�޸ĵĴ���� next_cluster:Ҫ�������д���ֵ 
 ���أ����н�� �ɹ���ʧ��
 ��⣺�˺���ʹcluster����next_cluster ���clusterΪ0��1�������������Ϊ��Ч�Ĵغ��Ǵ�2��ʼ��
**********************************************************************************************/
#ifdef MODIFY_FAT
UINT8 Modify_FAT(UINT32 cluster,UINT32 next_cluster) //����FAT����
{
 UINT32 temp1=0,temp2=0;

 if(0==cluster || 1==cluster) return ERR_FAIL; //����0��1�ǲ��ܸ��ĵ�

 temp1=pInit_Args->FirstFATSector+(cluster*4/pInit_Args->BytesPerSector);
 temp2=((cluster*4)%pInit_Args->BytesPerSector);

 znFAT_Device_Read_Sector(temp1,znFAT_Buffer);
 znFAT_Buffer[temp2+0]=(UINT8)( next_cluster&0x000000ff)     ;
 znFAT_Buffer[temp2+1]=(UINT8)((next_cluster&0x0000ff00)>>8 );
 znFAT_Buffer[temp2+2]=(UINT8)((next_cluster&0x00ff0000)>>16);
 znFAT_Buffer[temp2+3]=(UINT8)((next_cluster&0xff000000)>>24);
 znFAT_Device_Write_Sector(temp1,znFAT_Buffer);
 
 znFAT_Device_Read_Sector(temp1+pInit_Args->FATsectors,znFAT_Buffer);
 znFAT_Buffer[temp2+0]=(UINT8)( next_cluster&0x000000ff)     ;
 znFAT_Buffer[temp2+1]=(UINT8)((next_cluster&0x0000ff00)>>8 );
 znFAT_Buffer[temp2+2]=(UINT8)((next_cluster&0x00ff0000)>>16);
 znFAT_Buffer[temp2+3]=(UINT8)((next_cluster&0xff000000)>>24);
 znFAT_Device_Write_Sector(temp1+pInit_Args->FATsectors,znFAT_Buffer);

 return ERR_SUCC;
}
#endif

/********************************************************************************************
 ���ܣ���cluster�ؽ�����0
 �βΣ�cluster:Ҫ������0�����Ĵ�
 ���أ�0
 ��⣺��ĳ��Ŀ¼�´����ļ���Ŀ¼ʱ����Ŀ¼�����д����޿��пռ���д���ļ�Ŀ¼��ʱ����Ҫ��չ��
       �أ������մ�����Ŀ¼�Ĵ����ϣ���������д���ļ�Ŀ¼�������ļ���Ŀ¼�Ĵ���������մ�
       д���ļ�Ŀ¼����ǰ���������մ����Ϊ0����Ϊ��Ŀ¼�Ĵ���д���ļ�Ŀ¼�����0���ж���
       ���п��е�λ�á�
********************************************************************************************/
#ifdef CLEAR_CLUSTER
UINT8 Clear_Cluster(UINT32 cluster)
{
 znFAT_Device_Clear_nSector((pInit_Args->SectorsPerClust),SOC(cluster));

 return 0;
}
#endif

/********************************************************************************************
 ���ܣ�������һ�մزο�ֵ ��znFAT���ļ�ϵͳ��ʼ�����������е�Next_Free_Cluster
 �βΣ���
 ���أ����н�� 
 ��⣺��ÿ����Ҫʹ�ÿմص�ʱ�����Ƕ�����ֱ��ȥȡpInit_Args->Next_Free_Cluster����������֮
       Ҫ��ʱ������ֵ����Ϊ��һ��ʹ�ÿմ�����׼����
********************************************************************************************/
#ifdef UPDATE_NEXT_FREE_CLUSTER
UINT8 Update_Next_Free_Cluster(void)
{
 UINT32 clu_sec,iItem,iSec;
 struct FAT_Sec *pFAT_Sec;

 if(0!=pInit_Args->Free_nCluster) //�������пռ�
 {
  pInit_Args->Free_nCluster--; //���д�����1

  clu_sec=(pInit_Args->Next_Free_Cluster/NITEMSINFATSEC); //ָ���صĴ������ڵ�����Ϊ��FAT���ڵ�ƫ����                                                                                 //FAT��������                                                         
  znFAT_Device_Read_Sector(clu_sec+(pInit_Args->FirstFATSector),znFAT_Buffer); //���������ڵ��������ݶ��뻺����
  pFAT_Sec=(struct FAT_Sec *)znFAT_Buffer; //�����ݻ������׵�ַǿתΪFAT_Sec�ṹ���ָ������

  for(iItem=((pInit_Args->Next_Free_Cluster)%NITEMSINFATSEC)+1;iItem<NITEMSINFATSEC;iItem++) //����ڵ�ǰFAT�����ڵ�ǰ����֮���Ƿ��пմ�
  {
   if(0==(((pFAT_Sec->items)[iItem]).Item)[0]
   && 0==(((pFAT_Sec->items)[iItem]).Item)[1]
   && 0==(((pFAT_Sec->items)[iItem]).Item)[2]
   && 0==(((pFAT_Sec->items)[iItem]).Item)[3])
   {
    pInit_Args->Next_Free_Cluster=(clu_sec*NITEMSINFATSEC)+iItem;
    #ifdef RT_UPDATE_FSINFO
    Update_FSINFO();
    #endif
	return ERR_SUCC;
   }
  }

  for(iSec=(clu_sec+1);iSec<(pInit_Args->FATsectors);iSec++) //�ں����FAT�����м�������
  {
   znFAT_Device_Read_Sector(iSec+(pInit_Args->FirstFATSector),znFAT_Buffer);
   pFAT_Sec=(struct FAT_Sec *)znFAT_Buffer;
   for(iItem=0;iItem<NITEMSINFATSEC;iItem++) //����ڵ�ǰFAT�����ڵ�ǰ����֮���Ƿ��пմ�
   {
    if(0==(((pFAT_Sec->items)[iItem]).Item)[0]
    && 0==(((pFAT_Sec->items)[iItem]).Item)[1]
    && 0==(((pFAT_Sec->items)[iItem]).Item)[2]
    && 0==(((pFAT_Sec->items)[iItem]).Item)[3])
    {
     pInit_Args->Next_Free_Cluster=(iSec*NITEMSINFATSEC)+iItem;
     #ifdef RT_UPDATE_FSINFO
     Update_FSINFO();
     #endif
	 return ERR_SUCC;
    }
   }
  }
 }
 pInit_Args->Next_Free_Cluster=2; //������޿ռ䣬��nFreeClusterΪ0������һ�մ���Ϊ2
                                  //WINDOWS������������
 return ERR_NO_SPACE;
}
#endif

//=================for LFN=====���´�������ʵ�ֳ��ļ���===Ҫʹ�ó������� ��Ҫ��USE_LFN�������ж���===

/********************************************************************************************
 ���ܣ��ж�һ���ļ����Ƿ��ǳ��ļ���
 �βΣ�filename:ָ���ļ�����ָ��
 ���أ����н�����ǻ��
 ��⣺�ж�һ���ļ����Ƿ��ļ������ǳ���Ҫ��FAT32�жԳ����Ķ���Ƚ���������ӣ�һЩ���ƶ���
       ���ļ���ʵΪ����������ֻʹ�ó����жϵ���Ҫ������������ʹ�ó���ʱ��ʹ��������ȷ����ʽ
       ����������
********************************************************************************************/
#ifdef IS_LFN
UINT8 Is_LFN(INT8 *filename)
{
 UINT8 is_lfn=BOOL_FALSE;

 if(Check_SFN_Illegal_Length(filename)) is_lfn=BOOL_TRUE;
 if(Check_SFN_Dot(filename))            is_lfn=BOOL_TRUE;
 if(Check_SFN_Special_Char(filename))   is_lfn=BOOL_TRUE;
 if(((UINT8)(-1))==Check_SFN_Illegal_Lower(filename)) is_lfn=BOOL_TRUE;

 return is_lfn;
}
#endif

/***********************************************************************************************
 ���ܣ��ӳ���������ȡ���������Ĳ��ֳ�����UNI�룬�˺���ͨ����ε��ã�ʵ���˶���������UNI���ƴ��
 �βΣ�lfn_buf:ָ������װ�س���UNI��Ļ����� plfndi:ָ�������ָ�� n:˵�����ǳ����ĵ�n����
 ���أ����н��
 ��⣺FAT32�еĳ������ɶ�������ͬ���ģ�ÿ���������¼�ų�����ĳһ�Σ��˺������ǽ���������
       �ĳ���UNI����ȡ�������ŵ�����������Ӧλ���ϡ�������ε��ô˺��������տ���ƴ�ӵõ�������
************************************************************************************************/
#ifdef GET_PART_NAME
UINT8 Get_Part_Name(UINT16 *lfn_buf,struct LFN_FDI *plfndi,UINT8 n)
{
 UINT8 i=0;
 UINT16 temp=0;

 if((plfndi->AttrByte[0])&0X40) Memory_Set(((UINT8 *)lfn_buf),2*(MAX_LFN_LEN+1),0); //��������һ������������lfn_buf

 for(i=0;i<5;i++) //��һ���ֳ���
 {
  if(n>=MAX_LFN_LEN) return ERR_LFN_BUF_OUT;
  temp=(((UINT16)(((plfndi->Name1)+i*2)[0]))&0X00FF)|((((UINT16)(((plfndi->Name1)+i*2)[1]))&0X00FF)<<8);
  if(0==temp)
  {
   lfn_buf[n]=0;
   return 0;
  }
  else
  {
   lfn_buf[n]=temp;n++;
  }
 }

 for(i=0;i<6;i++) //�ڶ����ֳ���
 {
  if(n>=MAX_LFN_LEN) return ERR_LFN_BUF_OUT;  
  temp=(((UINT16)(((plfndi->Name2)+i*2)[0]))&0X00FF)|((((UINT16)(((plfndi->Name2)+i*2)[1]))&0X00FF)<<8);
  if(0==temp)
  {
   lfn_buf[n]=0;
   return 0;
  }
  else
  {
   lfn_buf[n]=temp;n++;
  }
 }

 for(i=0;i<2;i++) //�������ֳ���
 {
  if(n>=MAX_LFN_LEN) return ERR_LFN_BUF_OUT;  
  temp=(((UINT16)(((plfndi->Name3)+i*2)[0]))&0X00FF)|((((UINT16)(((plfndi->Name3)+i*2)[1]))&0X00FF)<<8);
  if(0==temp)
  {
   lfn_buf[n]=0;
   return 0;
  }
  else
  {
   lfn_buf[n]=temp;n++;
  }
 }

 n--;
 if((plfndi->AttrByte[0])&0X40)
 {
  while(((UINT16)(0XFFFF))==lfn_buf[n]) n--;
  lfn_buf[n+1]=0;
 }

 return 0;
}
#endif

/***********************************************************************************************
 ���ܣ���OEM�루���ں�����˵��OEM�����GB2312��תΪUNICODE��
 �βΣ�oem_code:OEM�� uni_code:ָ�����ڼ�¼oem_code����Ӧ��UNICODE����ı�����ָ��
 ���أ����н��  �ɹ���ʧ��
 ��⣺OEM���Ǹ������ļ�����������ڼ�����̻��ı������ֱ��룬�������й�OEM�����GB2312�룬����
       λ�룬����FAT32�ĳ��ļ�������UNICODE������ģ������Ҫһ������ת���Ĺ��̡���������ʵ�ֻ���
       �����������ɿ��ٲ��ҵ�OEM���Ӧ��UNICODE���롣
************************************************************************************************/
#ifdef OEMTOUNI
UINT8 OEM2UNI(UINT16 oem_code,UINT16 *uni_code) //ͨ�����ַ����OEM��תΪUNI��
{  
 UINT32 low=0,high=MAX_UNI_INDEX-1,mid;//�õ�ǰ�����������½�ĳ�ֵ 
 
 if(oem_code<GET_PGM_WORD(&(oem_uni[0][1]))) return ERR_FAIL;
 if(oem_code>GET_PGM_WORD(&(oem_uni[MAX_UNI_INDEX-1][1]))) return ERR_FAIL; //��������oem_code���Ǳ�ķ�Χ�ڣ���ֱ�ӷ���

 while(low<=high) //��ǰ��������[low..high]�ǿ�
 {
  mid=low+(high-low)/2;

  if(oem_code==GET_PGM_WORD(&(oem_uni[mid][1])))
  {
   *uni_code=GET_PGM_WORD(&(oem_uni[mid][0]));

   return ERR_SUCC; //���ҳɹ�����
  }

  if(GET_PGM_WORD(&(oem_uni[mid][1]))>oem_code)
  {
   high=mid-1;  //������[low..mid-1]�в���
  }
  else
  {
   low=mid+1; //������[mid+1..high]�в���
  }
 }											

 return ERR_FAIL; //��low>highʱ��ʾ��������Ϊ�գ�����ʧ��
}
#endif

/***********************************************************************************************
 ���ܣ���OEM������ַ���תΪ��UNICODE������ַ���
 �βΣ�oemstr:ָ��oem������ַ���  unistr:ָ��UNICODE������ַ���
 ���أ����н��  �ɹ� �ַ��������� �� �����������
 ��⣺���볤�ļ���֮���漰�����ļ����ıȶԺ�ƥ�䣬��˱�ȻҪ��oem������ַ���תΪUNICODE����
       ���ַ������Ա����ɳ���������ȡƴ�Ӷ��ɵ�UNICODE�ַ������бȶԡ�
************************************************************************************************/
#ifdef OEMSTRTOUNISTR
UINT8 oemstr2unistr(INT8 *oemstr,UINT16 *unistr)
{
 UINT32 len=StringLen(oemstr);
 UINT32 i=0,pos=0;
 UINT8 res=0;
 UINT16 temp=0;

 for(i=0;i<len;i++)
 {
  if(IS_ASC(oemstr[i])) //����Ƿ���ASCII�룬ASCII�����ֵ��ΧΪ0X00~0X7F��OEM����ֵ���ڴ˷�Χ���Դ�������ASCII����OEM
  {
   unistr[pos]=(UINT16)(((UINT16)oemstr[i])&0X00FF);
   pos++;
  }
  #ifdef USE_OEM_CHAR
  else //����ASCII�룬����OEM����
  {
   temp=((((UINT16)oemstr[i])<<8)&0xff00);
   temp|=(((UINT16)oemstr[i+1])&0x00ff);
   res=OEM2UNI(temp,unistr+pos);
   if(res) 
   {
	  unistr[0]=0;
	  return ERR_OEM_CHAR_NOT_COMPLETE;
   }
   pos++;i++;
  }
  #endif

  if(pos>MAX_LFN_LEN) 
  {
   unistr[0]=0;
   return ERR_LFN_BUF_OUT; 
  }
 }

 unistr[pos]=0;

 return ERR_SUCC;
}
#endif

/***********************************************************************************************
 ���ܣ��Կ��ַ�������ÿ���ַ���ռ�������ֽڣ���UNICODE�������Ӵ�����
 �βΣ�str:ָ����ַ�����ָ��  substr:ָ���Ӵ���ָ��  pos:Ҫ���ҵ���ʼλ��
 ���أ��Ӵ��ڿ��ַ����е���ʼλ�ã����δ�ҵ����򷵻�-1
 ��⣺�˺���������ǰ��SFN��ƥ���еĲ����Ӵ��ĺ���ͬ��ֻ�����ַ�������չΪ�˿��ַ���
************************************************************************************************/
#ifdef WFINDSUBSTR
UINT32 WFindSubStr(UINT16 *str,UINT16 *substr,UINT32 pos)
{
 UINT32 i=pos,j=0,lens=WStringLen(str),lent=WStringLen(substr);

 while(i<lens && j<lent)
 {
  if(WLower2Up(str[i])==substr[j] || (UINT16)'?'==substr[j])
  {
   i++;
   j++;
  }
  else
  {
   i=i-j+1;
   j=0;
  }
 }

 if(j==lent) return i-lent; 
 else return (UINT32)-1;
}
#endif

/***********************************************************************************************
 ���ܣ����������ַ�������ƥ�䣬ʵΪ���ļ�����ͨ��ȶ�
 �βΣ�t:ָ��ģ����ַ�����ָ��  s:ָĿ���ַ�����ָ��  ��t:����*����.txt s:���ϵ���ԭ������.txt
       ������ƥ��ġ�
 ���أ����н�� �ɹ���ʧ��
 ��⣺�˺���������ǰ��SFN��ƥ�亯��SFN_Matchͬ��
************************************************************************************************/
#ifdef LFN_MATCH
UINT8 LFN_Match(UINT16 *t,UINT16 *s)
{
 UINT32 i=0,j=0,lens=WStringLen(s),lent=WStringLen(t);
 UINT16 bufp=0;

 UINT16 buf[MAX_LFN_LEN+1];

 //======================================================
 
 while(j<lent && (UINT16)'*'!=t[j])
 {
  buf[bufp]=WLower2Up(t[j]);
  bufp++;
  j++;
 }

 if(0==t[j] && (lent!=lens)) return ERR_FAIL;

 buf[bufp]=0;
 
 if(WFindSubStr(s,buf,0)!=0) return ERR_FAIL;
 i=bufp;
 
 while(1)
 {
  while(j<lent && (UINT16)'*'==t[j]) j++;
  if(j==lent) return ERR_SUCC;
  bufp=0;

  while(j<lent && (UINT16)'*'!=t[j])
  {
   buf[bufp]=(UINT16)WLower2Up(t[j]);
   bufp++;
   j++;
  }
  buf[bufp]=0;
  
  if(j==lent)
  {
   if(WFindSubStr(s,buf,i)!=(lens-bufp)) return ERR_FAIL;
   return 0;
  }

  i=WFindSubStr(s,buf,i);
  if(((UINT32)(-1))==i) return ERR_FAIL;
  i+=bufp;
 }
}
#endif

/***********************************************************************************************
 ���ܣ��Ӷ����ļ�Ŀ¼���л�ȡ��У���
 �βΣ�pdi:ָ������ļ�Ŀ¼���ָ�� 
 ���أ���У��͵�ֵ
 ��⣺�˺�����ʹ�õ�У���㷨������΢���FAT32����ĵ���һ���ļ����ļ������Ϊ���ļ�������������
       ���ɸ�����������ͬ��ﳤ����ͬʱ������֮��Ӧ�Ķ�������� ABCDEFGHI.RMVB ��2�������ÿ
       ��������ɼ�¼13��UNICODE�룩����֮��Ӧ�Ķ�������ΪABCDEF~1.RMV��ͨ������£�������������
       ��������棬������֮�䲢���⿿λ�ù�ϵ����֤����ϵ������һ�ְ�У����㷨��ͨ����������
       ����õ�һ��ֵ������������ר�ŵ��ֶ�Ҳ��¼�����ֵ�����������ֵ��ȣ�����Ϊ�����������
       �����й����ģ���Գɹ���
************************************************************************************************/
#ifdef GET_BINDING_SUMCHK
UINT8 Get_Binding_SumChk(struct FDI *pdi)
{
 UINT8 i=0,c=0;

 for(i=0;i<11;i++)
 {
  c=(UINT8)(((c&0X01)?0X80:0)+(c>>1)+((pdi->Name)[i]));
 }

 return c;
}
#endif

/***********************************************************************************************
 ���ܣ�ELF��ϣ�㷨��������һ���ַ�������õ�һ��ֵ�����ֵ���ַ�����Ψһ��Ӧ��
 �βΣ�str:ָ���ַ�����ָ��
 ���أ�����õ��Ĺ�ϣֵ
 ��⣺�ڴ����ļ���Ŀ¼ʱ������ǳ���������Ŀ¼����д�볤�����ͬʱ��ҲҪд�����Ӧ�Ķ������
       �������ȷ����FAT32��ʹ�ü����ֺ�׺�ķ�������XXXXXX~n.YYY����n<=5��n>5��ʱ�����Ҫһ����
       ���ɳ���ֱ�Ӽ���õ�һ��Ψһ��֮��Ӧ�Ķ��ļ��������㷨������HASH�㷨��ʵ�֣���ELF��ϣ��
       �ڶ��ϣ�㷨���õñȽϹ㷺�ģ�����Ч���ұ�֤�˹�ϣֵ���ַ�����Ψһ��Ӧ����ͬ���ַ���ӵ��
       ��ͬ�Ĺ�ϣֵ�ĸ����Ǽ�С�ģ����������Ψһ�Ĺ�ϣֵ���ǿ��Կ��ٹ����������Ӧ�Ķ�����
************************************************************************************************/
#ifdef	ELFHASH
UINT32 ELFHash(INT8 *str)
{
 UINT32 hash=0;
 UINT32 x=0;

 while(*str)
 {
  hash=(hash<<4)+(*str++); //hash����4λ����ǰ�ַ�ASCII����hash����λ�� 
  x=hash&0xF0000000;
  if(0!=x)
  { //�����ߵ���λ��Ϊ0����˵���ַ�����7��������������ټӵھŸ��ַ�ʱ����һ���ַ��ᱻ�Ƴ������Ҫ�����´���
    //�ô�����������ַ���(a-z ����A-Z)�ͻ����Ӱ��5-8λ�������Ӱ��5-31λ����ΪC����ʹ�õ�������λ
   hash ^= (x >> 24);
   //���28-31λ��
   hash &= ~x;
  }
 }

 //����һ������λΪ0���������������λ�����⺯�������Ӱ�졣(���ǿ��Կ��ǣ����ֻ���ַ�������λ������Ϊ��)
 return (hash&0X7FFFFFFF);
}
#endif

/***********************************************************************************************
 ���ܣ���һ��32λ������ֵתΪ16�����ַ��� ����0X12345678תΪ"12345678"
 �βΣ�hex:һ��32λ�������� str:ָ���ַ�����ָ��
 ���أ�0
 ��⣺�˺������ڽ�����Ĺ�ϣ��������õ���32λ�Ĺ�ϣֵתΪ�ַ������Ա���ж��ļ����Ĺ��졣
************************************************************************************************/
#ifdef HEX2STR_32B
UINT8 Hex2Str_32b(UINT32 hex,INT8 *str)
{
 UINT8 i=0,temp=0;
 for(i=0;i<8;i++)
 {
  temp=((unsigned char)((hex&(0X0000000F<<(i*4)))>>(i*4)));
  str[7-i]=(INT8)((temp>=10)?('A'+temp-10):(temp+0X30));	
 }
 str[i]=0;
 
 return 0;
}
#endif

/***********************************************
 ���ܣ��ɳ����������Ӧ�Ķ���
 �βΣ�pfn:ָ��������ָ�� psfn:ָ�������ָ��
 ���أ�0
 ��⣺�˺�������Ҫ��һ���޸ģ����ݿ��á�
************************************************/
#ifdef MAKE_SHORT_NAME
UINT8 Make_Short_Name(char *pfn,char *psfn) //�˺����������ɳ�����Ӧ�Ķ�����ʹ����ELFHASH�㷨��������Ψһ�Ķ���
{
 //psfn[0]=Lower2Up(pfn[0]);
 //psfn[1]='~';
 UINT32 temp=ELFHash(pfn);
 Hex2Str_32b(temp,psfn);
 //psfn[0]='*';
 psfn[8]='.';
 psfn[9]=psfn[10]=psfn[11]='A';
 psfn[12]=0;

 return 0;
}
#endif

/******************************************************************************
 ���ܣ������n��������
 �βΣ�unifn:ָ��UNICODE����ĳ��ļ��� plfni:ָ�����ڴ洢����ĳ�����ı���ָ�� 
       ChkSum:�ɶ��������õ��İ�У��� n:ָʾ�ǵڼ���������
 ���أ�0
 ��⣺���쳤������znFAT��ʵ�ֳ��ļ�����һ���ؼ����ܺ���
*******************************************************************************/
#ifdef FILL_LFN_FDI
UINT8 Fill_LFN_FDI(UINT16 *unifn,struct LFN_FDI *plfni,UINT8 ChkSum,UINT8 n)
{
 UINT8 temp=(UINT8)(n*13),i=0,j=0;
 UINT16 len=(UINT16)WStringLen(unifn+temp);

 (plfni->AttrByte[0])=(UINT8)(n+1); //�����������ֽ�
 if(len<=13) (plfni->AttrByte[0])|=0X40; //������ִӵ�ǰλ�ÿ�ʼ��UNI������С�ڵ���13����˵�����������һ��������

 (plfni->ChkVal[0])=ChkSum; //д����SFN�İ�У��ֵ
 (plfni->LFNSign[0])=0X0F; //������ı�־

 (plfni->Resv[0])=(plfni->StartClu[0])=(plfni->StartClu[1])=0;

 for(i=0;i<10;i++) (plfni->Name1)[i]=0XFF; 
 for(i=0;i<12;i++) (plfni->Name2)[i]=0XFF;
 for(i=0;i<4;i++)  (plfni->Name3)[i]=0XFF; //�Ȱѳ���UNI���ֶ���0XFF���
 
 for(i=0;i<5;i++)
 {
  (plfni->Name1)[2*i]=(UINT8)(unifn[j+temp]&0X00FF);
  (plfni->Name1)[2*i+1]=(UINT8)((unifn[j+temp]&0XFF00)>>8);
  j++;
  if(j>=len) 
  {
   if(4==i) 
   {
    (plfni->Name2)[0]=(plfni->Name2)[1]=0;
   }
   else
   {
	i++;
    (plfni->Name1)[2*i]=(plfni->Name1)[2*i+1]=0;
   }
   return ERR_SUCC; 
  }
 }

 for(i=0;i<6;i++)
 {
  (plfni->Name2)[2*i]=(UINT8)(unifn[j+temp]&0X00FF);
  (plfni->Name2)[2*i+1]=(UINT8)((unifn[j+temp]&0XFF00)>>8);
  j++;
  if(j>=len)
  {
   if(5==i) 
   {
    (plfni->Name3)[0]=(plfni->Name3)[1]=0;
   }
   else
   {
	i++;
    (plfni->Name2)[2*i]=(plfni->Name2)[2*i+1]=0;
   }
   return ERR_SUCC; 
  }
 }

 for(i=0;i<2;i++)
 {
  (plfni->Name3)[2*i]=(UINT8)(unifn[j+temp]&0X00FF);
  (plfni->Name3)[2*i+1]=(UINT8)((unifn[j+temp]&0XFF00)>>8);
  j++;
  if(j>=len)
  {
   if(1!=i) 
   {
	i++;
    (plfni->Name3)[2*i]=(plfni->Name3)[2*i+1]=0;
   }
   return ERR_SUCC; 
  }
 }
 
 return ERR_SUCC;
}
#endif

/********************************************************************************
 ���ܣ���Ŀ¼����ע�᳤������Ӧ�Ķ���������ض��������ڵ������������ڵ�λ��
 �βΣ�cluster:Ҫ�������ļ���Ŀ¼���ڵ�Ŀ¼���״� pfdi:ָ��������ָ�� 
       unifn:ָ��UNICODE����ĳ�����ָ�� psec:ָ�����ڴ洢���������������ı���ָ��
       pn:ָ�����ڼ�¼�������������������е�λ�õı�����ָ��
 ���أ����н�����ɹ�������
 ��⣺��Ϊ�������ɶ���������һ������������ͬ�����б��ģ���˴����������ļ���
       Ŀ¼������Ĳ������ǿ���һ������Ŀ¼����ע�����ļ�Ŀ¼��˺�����������
       ��Refigster_FDI��������ʵ��Ϊ����ע���ļ�Ŀ¼�����չ��
*********************************************************************************/
#ifdef REGISTER_LFN_FDI
UINT8 Register_LFN_FDI(UINT32 cluster,struct FDI *pfdi,UINT16 *unifn,UINT32 *psec,UINT8 *pn)
{
 UINT32 temp_sec=0,old_cluster=0;
 UINT8 iClu=0,iSec=0,iFDI=0,res=0;
 struct FDIesInSEC *pitems;
 struct FDI *pitem;

 struct LFN_FDI *plfni;

 UINT8 have_lfn=0; //���ļ�Ŀ¼��ɨ������֣����ڱ���Ƿ��г���
 UINT8 is_lfn_buf_err=0,cur_binding_sumchk=0,flag=0,chksum=Get_Binding_SumChk(pfdi);
 UINT16 lfn_buf[MAX_LFN_LEN+1]; //����װ�س���UNI��Ļ���
 struct LFN_FDI temp_lfni; //���ڹ��쳤�������ʱ�ṹ�����

 UINT16 len=(UINT16)WStringLen(unifn),xlen=0; //����UNI������
 UINT8 nclu=0,nsec=0,nlfni=(UINT8)(len/13);
 if(len%13) nlfni++; //����UNI�����Ϊ���ٸ�������

 //===========================================================================================

 if(0==pInit_Args->Free_nCluster) return ERR_NO_SPACE; //���û�пռ䣬��ֱ�ӷ���
 
 //���´������Ƿ��Ѿ������ظ����ļ���Ŀ¼���Լ���ѯ��λ������Ŀ¼��
 do
 {
  temp_sec=SOC(cluster);
  for(iSec=0;iSec<(pInit_Args->SectorsPerClust);iSec++)
  {
   znFAT_Device_Read_Sector(temp_sec+(UINT32)iSec,znFAT_Buffer);
   pitems=((struct FDIesInSEC *)znFAT_Buffer);

   for(iFDI=0;iFDI<NFDI_PER_SEC;iFDI++)
   {
	pitem=&(pitems->FDIes[iFDI]); //ָ��һ���ļ�Ŀ¼������

	if((0X08!=pitem->Attributes) && (0X0F!=pitem->Attributes)  //���Ǿ�ꡢ���ǳ����û��ɾ��������.��..
       && (0XE5!=pitem->Name[0]) && ('.'!=pitem->Name[0])) //������һ���Ϸ���SFN��
	{
	 if(have_lfn && !is_lfn_buf_err) //����SFN�����ǰ������г������������������UNI���ȶԣ���ȷ���Ƿ���ͬ���ļ���Ŀ¼
	 {                               //����û�з���lfn_buf���������
      if(cur_binding_sumchk==Get_Binding_SumChk(pitem)) //���LFN��SFNĿ¼���У�����ȣ�����Ϊ������Ч
	  { 
	   xlen=(UINT16)WStringLen(lfn_buf); //����Ӹ�������ϳɺ��UNI���ĳ���
	   if(xlen==len && Memory_Compare(((UINT8 *)unifn),((UINT8 *)lfn_buf),2*((UINT32)len))) //��UNI�����бȶԣ����Ⱥ�����Ҫ���
	   {
            *psec=temp_sec+(UINT32)iSec;
            *pn=iFDI; //��¼�����ļ�Ŀ¼���λ�ã��Ա������н���
	    return ERR_FDI_ALREADY_EXISTING; //����ͬ���ļ���Ŀ¼
	   }
	  }
	 }
         if(is_lfn_buf_err) is_lfn_buf_err=0; //�ָ�������UNI��ȡ�Ĵ�����
	}

    if(CHK_ATTR_LFN(pitem->Attributes) && (0XE5!=pitem->Name[0])) //�ǳ��������û�б�ɾ��
	{
	 have_lfn=1;
	 
	 plfni=(struct LFN_FDI *)pitem;

     cur_binding_sumchk=(plfni->ChkVal)[0]; //��ȡ������İ�У���

     res=Get_Part_Name(lfn_buf,plfni,(UINT8)((((plfni->AttrByte[0])&0XBF)-1)*13)); //����ǰLFN���е��ļ���UNICODE��ƴ����ʱ����
                                                                      //����ʱ���峤��ΪMAX_LFN_LEN�����Խ�磬��
	                                                                  //����װ�룬�������LFN�Ľضϡ�
	 if(res) is_lfn_buf_err=1; //����LFN_BUF�������
	}
	else
	{
	 have_lfn=0;
	}

	if(0==(pitem->Name)[0])
	{
     flag=1; //��Ǵ������Ѿ����޸�

	 if(nlfni>0)
	 {
	  Fill_LFN_FDI(unifn,&temp_lfni,chksum,(UINT8)(nlfni-1));
      Memory_Copy(((UINT8 *)pitem),((UINT8 *)(&temp_lfni)),FDI_NBYTES); //���ϳɵĳ������������������
	  nlfni--;
	 }
	 else
	 {
	  *psec=temp_sec+(UINT32)iSec;
	  *pn=iFDI; //��¼��λ��λ��
	  Memory_Copy(((UINT8 *)pitem),((UINT8 *)pfdi),FDI_NBYTES); //���������������������
	  znFAT_Device_Write_Sector(temp_sec+(UINT32)iSec,znFAT_Buffer);
	  return ERR_SUCC;
	 }	 
	}
   }

   if(flag) 
   {
	znFAT_Device_Write_Sector(temp_sec+(UINT32)iSec,znFAT_Buffer);
	flag=0;
   }
  }
  old_cluster=cluster;
  cluster=Get_Next_Cluster(cluster);
 }while(!IS_END_CLU(cluster)); //����������һ���أ������ѭ��
 //===========================================================
 //������е������˵����ǰ�������޿�λ
 if(0!=pInit_Args->Free_nCluster) //���ʣ��մ���Ϊ0����˵���������޿ռ�
 {
  nsec=(UINT8)((nlfni+1)/NFDI_PER_SEC); //ʣ��ĳ������Ҫ���ٸ�����,+1����Ϊ�����һ��������
  if((nlfni+1)%NFDI_PER_SEC) nsec++; 

  nclu=(UINT8)(nsec/(pInit_Args->SectorsPerClust));
  if(nsec%(pInit_Args->SectorsPerClust)) nclu++;

  for(iClu=0;iClu<nclu;iClu++)
  {
   Modify_FAT(old_cluster,pInit_Args->Next_Free_Cluster);
   Modify_FAT(pInit_Args->Next_Free_Cluster,0X0FFFFFFF); //����FAT����
   Clear_Cluster(pInit_Args->Next_Free_Cluster); //��տ��д�

   temp_sec=SOC(pInit_Args->Next_Free_Cluster);

   old_cluster=pInit_Args->Next_Free_Cluster;
   Update_Next_Free_Cluster();

   for(iSec=0;iSec<(pInit_Args->SectorsPerClust);iSec++)
   {
    znFAT_Device_Read_Sector(temp_sec+(UINT32)iSec,znFAT_Buffer);
	pitems=((struct FDIesInSEC *)znFAT_Buffer);
    
	for(iFDI=0;iFDI<NFDI_PER_SEC;iFDI++)
	{
	 pitem=&(pitems->FDIes[iFDI]); //ָ��һ���ļ�Ŀ¼������

	 if(nlfni>0)
	 {
	  Fill_LFN_FDI(unifn,&temp_lfni,chksum,(UINT8)(nlfni-1));
      Memory_Copy(((UINT8 *)pitem),((UINT8 *)(&temp_lfni)),FDI_NBYTES); //���ϳɵĳ������������������
	  nlfni--;
	 }
	 else
	 {
	  *psec=temp_sec+(UINT32)iSec;
	  *pn=iFDI; //��¼��λ��λ��
	  Memory_Copy(((UINT8 *)pitem),((UINT8 *)pfdi),FDI_NBYTES); //���������������������
	  znFAT_Device_Write_Sector(temp_sec+(UINT32)iSec,znFAT_Buffer);
	  return ERR_SUCC;
	 }
	}
	znFAT_Device_Write_Sector(temp_sec+(UINT32)iSec,znFAT_Buffer);
   }   
  }
  return ERR_SUCC;  
 }
 else
 {
  return ERR_NO_SPACE;
 } 
}
#endif

//===================���ϴ�������ʵ�ֳ��ļ�������===for LFN=======================================

/************************************************************************************
 ���ܣ����ļ�
 �βΣ�pfi:ָ���ļ���Ϣ���ϵ�ָ�� filepath:�ļ���·�� n:��� is_file:Ҫ���Ƿ����ļ�
 ���أ����н�����ɹ� �� ������
 ��⣺�˺����Ĺ��ܱȽ϶����ԣ��ļ���·��֧���������Ŀ¼ �� /a/b/c/d/e/f/g/..../a.txt
       ·���е�Ŀ¼�ָ�������Ϊ/��\����C���ַ����б��\Ҫ����д\\�����ļ�����֧������
       ������/a/b/c/d/.../a*.txt /??abc.txt ���ļ���֧�ֳ��ļ������������۳���������֧
       ��ͨ�䣬��ʹ��ͨ���ļ���ʱ����֮ƥ����ļ����ܻ��ж����n����ָ��Ҫ�򿪵��ǵڼ�
       ���ļ����˺�����ɴ�Ŀ¼���ļ���Ŀ¼�򿪺�����Ϣ����װ�뵽pfi��ָ����ļ�
       ��Ϣ�����С�ע������Ŀ¼��Ȼ�洢��ʽ�����ļ����ƣ��������ļ�Ŀ¼�����洢������
       û���ļ���С���ֶΣ��㶨Ϊ0����
*************************************************************************************/
#ifdef ZNFAT_OPEN_FILE 
UINT8 znFAT_Open_File(struct FileInfo *pfi,INT8 *filepath,UINT32 n,UINT8 is_file)
{
 UINT8 result=0,flag=0;
 UINT32 sec_temp=0,Cur_Cluster=0,fn_pos=0,item=0;
 UINT32 iSec=0,iFDI=0;

 //===========for LFN=======
 #ifdef USE_LFN
 struct LFN_FDI *plfni;

 UINT8 cur_binding_sumchk=0;
 UINT8 is_wfn=0;
 UINT16 temp_lfn[MAX_LFN_LEN+1]; //����װ�س���UNICODE�����ʱ����
 UINT8 is_lfn=0; //������ļ����Ƿ��ǳ���
 UINT8 is_lfn_buf_err=0; //������������������󣬴˱��Ϊ1��ֱ�����������ȶ�
 #endif
 //===========for LFN=======

 INT8 temp_filename[13];
 INT8 *filename;

 struct FDIesInSEC *pitems; //ָ���ļ�Ŀ¼���������ݵ�ָ��
 struct FDI *pitem; //ָ���ļ�Ŀ¼�����ݵ�ָ��

 #ifdef USE_LFN
 pfi->have_lfn=BOOL_FALSE; //���趨�ļ��޳���
 #endif

 just_file=pfi;

 #ifndef RT_UPDATE_CLUSTER_CHAIN
 get_next_cluster_in_cccb=0;
 #ifdef USE_ALONE_CCCB
 CCCB_To_Alone();
 Memory_Set((UINT8 *)pcccb_buf,sizeof(UINT32)*CCCB_LEN,0);
 #endif
 #endif

 #ifdef USE_EXCHANGE_BUFFER
 #ifdef USE_ALONE_EXB
 //Memory_Set(just_file->exb_buf,512,0);
 just_file->exb_cursec=0;
 #endif
 #endif

 result=znFAT_Enter_Dir(filepath,&Cur_Cluster,&fn_pos); //��ȡ·�����һ��Ŀ¼�Ŀ�ʼ��
 if(result) return result;

 filename=filepath+fn_pos; //filenameָ��filepath�е��ļ���
                           //ע������򿪵���Ŀ¼����·��filepath����/��\���������Ŀ¼\a\b\c 
                           //��Ҫд��\a\b\c\����������ɴ�ʧ��
                           //znFAT��Ϊ��Ҫ��cĿ¼�µ�һ������Ϊ�յ�Ŀ¼
                           //��Ŀ¼�����Ŀ¼��ͬ������Ŀ¼����ȡĿ¼�״أ�����Ŀ¼
                           //���Ŀ¼���н��������ļ���ز��������ļ���Ϣ���ϣ���Ŀ¼����ʱ���

 if(Check_Illegal_Char(filename)) return ERR_ILL_CHAR; //����ļ������Ƿ��зǷ��ַ������۳������Ƕ���������ͨ���� 

 //������Ҫ�����LFN��SFN��ͨ����������ؼ��
 if(!Is_WildFileName(filename)) //�������ͨ���ļ�������ȷ����������Ҫ�����ļ����Ϸ��Լ��
 {
  #ifdef USE_LFN
  if(!Is_LFN(filename)) //������ǳ���,���Ƕ���
  {
  #endif
   //���SFN�Ϸ��ԣ����Ƿ���ֱ�ӷ��أ����ٽ��к���Ĵ���(�˴���SFN�ĺϷ��Լ��Ƚ��ϸ�)
   //���ȼ��SFN�ĺϷ��ԣ����ٺ��洦���ϵ��鷳

   if(Check_SFN_Illegal_Length(filename)) return ERR_SFN_ILL_LEN; //���SFN�Ƿ����8.3����
   if(Check_SFN_Dot(filename)) return ERR_SFN_DOT; //���SFN��.�Ƿ�Ϸ� 
   if(Check_SFN_Special_Char(filename)) return ERR_SFN_SPEC_CHAR; //���SFN���Ƿ��������ַ�
   if(((UINT8)(-1))==Check_SFN_Illegal_Lower(filename)) return ERR_SFN_ILL_LOWER; //���SFN���Ƿ��зǷ��Ĵ�Сд

  #ifdef USE_LFN
  }
  else //����ǳ���
  {
   is_lfn=1; //���������ļ���Ϊ����
   result=oemstr2unistr(filename,temp_lfn);//��filenameתΪUNICODE�룬����temp_lfn��Ա��������ļ����ȶ�
   if(result) return result;
  }
  #endif
 }
 else //�����ͨ������������*��?
 {
  #ifdef USE_LFN
  is_wfn=1; //��־������ļ���Ϊͨ����
  is_lfn=1; //����������£�Ҳ��Ϊ�ǳ�����ͨ����ҲҪ��LFN��ȶ�
  result=oemstr2unistr(filename,temp_lfn); //תΪ��ͨ�����UNI��
  if(result) return result; //OEM�ַ�������ȫ��OEM->UNIת�������ַ��Ҳ��� �� LFN�������
  #endif
 }

 //================================================
 do
 {
  sec_temp=SOC(Cur_Cluster); //��ǰ��������
  for(iSec=0;iSec<(pInit_Args->SectorsPerClust);iSec++) 
  {
   znFAT_Device_Read_Sector(sec_temp+(UINT32)iSec,znFAT_Buffer);
   pitems=(struct FDIesInSEC *)znFAT_Buffer; 

   for(iFDI=0;iFDI<NFDI_PER_SEC;iFDI++) //���������и��ļ�Ŀ¼��
   {
    pitem=&(pitems->FDIes[iFDI]); //ָ��һ���ļ�Ŀ¼������
     
    if((is_file?CHK_ATTR_FILE(pitem->Attributes):CHK_ATTR_DIR(pitem->Attributes)) 
		&& (0XE5!=pitem->Name[0]) && ('.'!=pitem->Name[0])) //��is_file������ԣ���û�б�ɾ��
		                                                    //����.��..
    {
     To_File_Name((INT8 *)(pitem->Name),temp_filename); //��FDI�е��ļ����ֶ�תΪ8.3�ļ���

	 #ifdef USE_LFN	
     if(!is_lfn || is_wfn) //���������ļ������ǳ��ļ�������Ϊ���ļ�������������ͨ������Ҫ����SFN�ȶ�
	 #endif                //��Ҫ��Ϊ�˷�ֹ����Ĳ�������������ļ���Ϊ���ļ��������������SFN�ȶԣ���
		                   //��ȶԣ����Ҳһ���ǲ�ƥ���
     {
      if(!SFN_Match(filename,temp_filename)) //���ļ���ͨ��
	  {
	   if(n==item)
	   {
        Analyse_FDI(pfi,pitem); //����ƥ����ļ�Ŀ¼��
	    pfi->FDI_Sec=sec_temp+iSec; //�ļ�Ŀ¼�����ڵ�����
	    pfi->nFDI=(UINT8)iFDI; //�ļ�Ŀ¼���������е�����

        #ifdef USE_LFN
	    if(!pfi->have_lfn) (pfi->longname)[0]=0;
        #endif

	    return ERR_SUCC;
	   } 
	   flag=1;
	  }
	 }

     #ifdef USE_LFN
     if(is_lfn && (pfi->have_lfn) && !is_lfn_buf_err) //���������ļ���Ϊ����������Ҳ�����˳���
	 {
	  if(cur_binding_sumchk==Get_Binding_SumChk(pitem)) //���LFN��SFNĿ¼���У�����ȣ�����Ϊ
	  {	                                                //������Ч
       if(!LFN_Match(temp_lfn,(pfi->longname))) //���ļ���ͨ��
	   {
	    if(n==item)
		{
         Analyse_FDI(pfi,pitem); //����ƥ����ļ�Ŀ¼��
	     pfi->FDI_Sec=sec_temp+iSec; //�ļ�Ŀ¼�����ڵ�����
	     pfi->nFDI=(UINT8)iFDI; //�ļ�Ŀ¼���������е�����
	     return ERR_SUCC;
		}
	    flag=1;
	   }	
	  }
	 }

	 if(is_lfn_buf_err) is_lfn_buf_err=0;
     #endif

	 if(flag) {item++;flag=0;} //���LFN���������LFN�Ļ�����SFN��һ��ȶԳɹ�����item��ƥ�䣬��item++
	}

    #ifdef USE_LFN
		if((CHK_ATTR_DIR(pitem->Attributes))||(CHK_ATTR_FILE(pitem->Attributes))) //�������������ָ����ȴ�����
			if(is_lfn_buf_err) is_lfn_buf_err=0;

    if(CHK_ATTR_LFN(pitem->Attributes) && (0XE5!=pitem->Name[0]) && is_lfn) //�ǳ��������û�б�ɾ��������������ļ����ǳ���
	{                                                                       //��Ϊ�������������ļ�������Ͳ��ǳ����������Ǿ�û��Ҫȥ���ĳ�����
     pfi->have_lfn=1;
	 
	 plfni=(struct LFN_FDI *)pitem;

	 cur_binding_sumchk=(plfni->ChkVal)[0]; //��ȡ������İ�У���
    
     result=Get_Part_Name(pfi->longname,plfni,(UINT8)((((plfni->AttrByte[0])&0XBF)-1)*13)); //����ǰLFN���е��ļ���UNICODE��ƴ����ʱ����
                                                                      //����ʱ���峤��ΪMAX_LFN_LEN�����Խ�磬��
	                                                                  //����װ�룬�������LFN�Ľضϡ�
	 if(result) is_lfn_buf_err=1; //����LFN_BUF�������
	}
	else
	{
	 pfi->have_lfn=0;
	}
    #endif

   }
  }

  Cur_Cluster=Get_Next_Cluster(Cur_Cluster); //��ȡ��һ��
 }while(!IS_END_CLU(Cur_Cluster)); //����������һ���أ������ѭ��

 return ERR_NO_FILE;
}
#endif

/************************************************************************************
 ���ܣ���ȡһ��Ŀ¼�Ŀ�ʼ��
 �βΣ�dir_name:Ŀ¼�� pCluster:ָ�����ڴ洢�غŵı�����ָ�� ע����ָ�벻�����ڽ�Ŀ¼
       ���ڵ�Ŀ¼�غŴ��룬ͬʱ�ֽ���Ŀ¼�Ŀ�ʼ�ء����磺����Ҫ��ȡ /a/b/c cĿ¼�Ŀ�
       �أ�pCluster���ȴ���b�Ŀ�ʼ�أ�Ȼ�������������ʼ�ؼ����̴���Ѱ��Ϊ��dir_n
       ame��Ŀ¼��ҵ��󣬽��俪ʼͨ��pCluster���أ���pCluster��ָ��ı��������˸�
       �䡣
 ���أ����н��   �ɹ�������
 ��⣺�˺�����ʵ�ֹ�������ļ���࣬���ұ�������Щ��dir_nameͬ��֧�ֳ��ļ�����
       ͨ�䡣
*************************************************************************************/
#ifdef GET_DIR_START_CLUSTER
UINT8 Get_Dir_Start_Cluster(INT8 *dir_name,UINT32 *pCluster) //��ȡĿ¼����Ϊdir_name����Ŀ¼�Ŀ�ʼ��
{                                                            //Ŀ¼���״ش���pCluster��ָ��ı���
 UINT32 sec_temp=0;
 UINT8 iSec=0,iFDI=0;
 UINT32 Cur_Clust=*pCluster;

 //===========for LFN=======
 #ifdef USE_LFN
 struct LFN_FDI *plfni;

 UINT8 cur_binding_sumchk=0,result=0;
 UINT16 temp_lfn[MAX_LFN_LEN+1]; //����װ�س���UNICODE�����ʱ����
 UINT16 lfn_buf[MAX_LFN_LEN+1]; //�ӳ���������ȡ��UNI��װ�����������
 UINT8 is_lfn=0; //������ļ����Ƿ��ǳ���
 UINT8 have_lfn=0; //������Ŀ¼��ʱ�����˳�����
 UINT8 is_lfn_buf_err=0; //������������������󣬴˱��Ϊ1��ֱ�����������ȶ�
 #endif

 //===========for LFN=======

 INT8 temp_dirname[13];

 struct FDIesInSEC *pitems; //ָ���ļ�Ŀ¼���������ݵ�ָ��
 struct FDI *pitem; //ָ���ļ�Ŀ¼�����ݵ�ָ��

 #ifndef RT_UPDATE_CLUSTER_CHAIN
 get_next_cluster_in_cccb=0;
 #endif

 if(Check_Illegal_Char(dir_name)) return ERR_ILL_CHAR; //����ļ������Ƿ��зǷ��ַ�

 #ifdef USE_LFN
 if(!Is_LFN(dir_name)) //������ǳ���,���Ƕ���
 {
 #endif
  //�����ļ����Ϸ��ԣ����Ƿ���ֱ�ӷ��أ����ٽ��к���Ĵ���(�˴���SFN�ĺϷ��Լ��ǳ��ϸ�)
  //���ȼ��SFN�ĺϷ��ԣ����ٺ��洦���ϵ��鷳
  if(Check_SFN_Illegal_Length(dir_name)) return ERR_SFN_ILL_LEN; //���SFN�Ƿ����8.3����
  if(Check_SFN_Dot(dir_name)) return ERR_SFN_DOT; //���SFN��.�Ƿ�Ϸ�  
  if(Check_SFN_Special_Char(dir_name)) return ERR_SFN_SPEC_CHAR; //���SFN���Ƿ��������ַ�
  if(((UINT8)(-1))==Check_SFN_Illegal_Lower(dir_name)) return ERR_SFN_ILL_LOWER; //���SFN���Ƿ��зǷ��Ĵ�Сд
 #ifdef USE_LFN
 }
 else //����ǳ���
 {
  is_lfn=1; //���������ļ���Ϊ����
  result=oemstr2unistr(dir_name,temp_lfn);//��filenameתΪUNICODE�룬����temp_lfn��Ա��������ļ����ȶ�
  if(result) return result;
 }
 #endif

 //================================================
 do
 {
  sec_temp=SOC(Cur_Clust); //��ǰ��������
  for(iSec=0;iSec<(pInit_Args->SectorsPerClust);iSec++) 
  {
   znFAT_Device_Read_Sector(sec_temp+(UINT32)iSec,znFAT_Buffer);
   pitems=(struct FDIesInSEC *)znFAT_Buffer; 

   for(iFDI=0;iFDI<NFDI_PER_SEC;iFDI++) //���������и��ļ�Ŀ¼��
   {
    pitem=&(pitems->FDIes[iFDI]); //ָ��һ���ļ�Ŀ¼������
     
    if((CHK_ATTR_DIR(pitem->Attributes)) && (0XE5!=pitem->Name[0])) //�ļ�����ΪĿ¼����û�б�ɾ��
    {
     To_File_Name((INT8 *)(pitem->Name),temp_dirname); //��FDI�е�Ŀ¼���ֶ�תΪ8.3�ļ���
     #ifdef USE_LFN
     if(!is_lfn)
     #endif
	 {
      if(!SFN_Match(dir_name,temp_dirname)) //Ŀ¼��ƥ��
	  {
	   //��ȡĿ¼�Ŀ�ʼ�� 
	   *pCluster=(Bytes2Value(pitem->LowClust,2))|(Bytes2Value(pitem->HighClust,2)<<16); 
	   return ERR_SUCC;
	  }
	 }

     #ifdef USE_LFN
     if(is_lfn && have_lfn && !is_lfn_buf_err) //���������ļ���Ϊ����������Ҳ�����˳���
	 {                                         //ͬʱ��û�з������������������
	  if(cur_binding_sumchk==Get_Binding_SumChk(pitem)) //���LFN��SFNĿ¼���У�����ȣ�����Ϊ
	  {	                                                //������Ч
       if(!LFN_Match(temp_lfn,lfn_buf)) //���ļ���ͨ��
	   {
	    //��ȡĿ¼�Ŀ�ʼ�� 
	    *pCluster=(Bytes2Value(pitem->LowClust,2))|(Bytes2Value(pitem->HighClust,2)<<16); 
		return ERR_SUCC;
	   }	
	  }
	 }
     #endif

    }

    #ifdef USE_LFN
	if((CHK_ATTR_DIR(pitem->Attributes))||(CHK_ATTR_FILE(pitem->Attributes))) //�������������ָ����ȴ�����
			if(is_lfn_buf_err) is_lfn_buf_err=0;

    if(CHK_ATTR_LFN(pitem->Attributes) && (0XE5!=pitem->Name[0]) && is_lfn) //�ǳ��������û�б�ɾ��,���������Ϊ����
	{
     have_lfn=1;
	 
	 plfni=(struct LFN_FDI *)pitem;

	 cur_binding_sumchk=(plfni->ChkVal)[0]; //��ȡ������İ�У���
    
     result=Get_Part_Name(lfn_buf,plfni,(UINT8)((((plfni->AttrByte[0])&0XBF)-1)*13)); //����ǰLFN���е��ļ���UNICODE��ƴ����ʱ����
                                                                      //����ʱ���峤��ΪMAX_LFN_LEN�����Խ�磬��
	                                                                  //����װ�룬�������LFN�Ľضϡ�
	 if(result) is_lfn_buf_err=1; //�������LFN_BUF�������
	}
    #endif
   }
  }

  Cur_Clust=Get_Next_Cluster(Cur_Clust); //��ȡ��һ��
 }while(!IS_END_CLU(Cur_Clust)); //����������һ���أ������ѭ��

 return ERR_NO_DIR; 
}
#endif

/************************************************************************************
 ���ܣ�����һ��Ŀ¼��ʵΪ��ȡĿ¼�Ŀ�ʼ��
 �βΣ�dirpath:Ŀ¼��·�� pCluster:ָ�����ڼ�¼�غŵı�����ָ�� pos:·�����ļ�����λ��
 ���أ����н�� �ɹ��������
 ��⣺�˺�����Ȼʹ���߿ɼ������ô�����������Ŀ¼�Ŀ�ʼ�ء�dirpath���Դ����ļ�������
       /a/b/c/d/e/test.txt ��������֮��pos��ָ��ı�����ֵΪtest.txt��·���е�λ�á���
       Ҫ�ǿ��ǵ��˺�������ļ���Ŀ¼�ĺ�������ϣ����Ժܷ���Ĵ�·������ȡ���ļ���
*************************************************************************************/
#ifdef ZNFAT_ENTER_DIR
UINT8 znFAT_Enter_Dir(INT8 *dirpath,UINT32 *pCluster,UINT32 *pos) 
{
 UINT8 index=0,res=0;
 UINT32 i=1;

 #ifndef USE_LFN
 INT8 dirname[13];
 #else
 INT8 dirname[MAX_LFN_LEN+1];
 #endif

 *pos=1;
 *pCluster=2;

 if(('\\'==dirpath[0] || '/'==dirpath[0]) && '\0'==dirpath[1]) //�����"\\"����ֱ��ȡ��Ŀ¼�أ�����2��
 {
  return ERR_SUCC;
 }

 while('\0'!=dirpath[i])
 {
  if('\\'==dirpath[i] || '/'==dirpath[i])
  {
   dirname[index]='\0';
   index=0;
   
   res=Get_Dir_Start_Cluster(dirname,pCluster);
   if(res) 
   { 
	return res;  //���ش�����  
   }
   *pos=i+1;
  } 
  else
  {
   dirname[index]=dirpath[i];
   index++;
   #ifndef USE_LFN
   if(index>12) //�����ʹ�ó�������Ŀ¼���Լ��ļ�������ܳ���8+1+3
   {
	return ERR_SFN_ILL_LEN; //Ŀ¼������8.3�����ֹdirname���
   }
   #else
   if(index>MAX_LFN_LEN) //���ʹ�ó�������Ŀ¼���Լ��ļ�������ܳ����趨�ĳ��������
   {
	return ERR_LFN_BUF_OUT; //Ŀ¼������MAX_LFN_LEN�����ֹdirname���
   }   
   #endif
  }
  i++;
 }
   
 return ERR_SUCC; //�ɹ�
}
#endif

/************************************************************************************
 ���ܣ��ɸ����Ķ��ļ�����ʱ����Ϣ�������һ���ļ�Ŀ¼��
 �βΣ�pfdi:ָ������װ���ļ�Ŀ¼��ı�����ָ�� pfn:ָ���ļ��� pdt:ָ��ʱ����Ϣ 
       is_file:������ļ�Ŀ¼�����ڱ���ļ�����Ŀ¼
 ���أ�0
 ��⣺�ڹ���Ŀ¼���ļ�Ŀ¼��ʱ��Ҫ����Ϊ�����ÿմأ����������is_file�������֡�
*************************************************************************************/
#ifdef FILL_FDI
UINT8 Fill_FDI(struct FDI *pfdi,INT8 *pfn,struct DateTime *pdt,UINT8 is_file)
{
 UINT8 dot_pos=0,have_dot=0,lowcase=0;
 UINT8 i=0,j=0,fn_len=(UINT8)StringLen(pfn);
 UINT16 time=0,date=0;

 Memory_Set(((UINT8 *)pfdi),FDI_NBYTES,0); //���ļ�Ŀ¼����0

 for(i=(UINT8)(fn_len-1);i>0;i--) //����Ѱ��. ��һ��.�����ļ�����չ���ķֽ�
 {
  if('.'==pfn[i]) 
  {
   dot_pos=i;
   have_dot=1;
   break;
  }
 } 

 if(have_dot) //����е�
 {
  //�������ļ���
  for(i=0;i<dot_pos;i++)
  {
   (pfdi->Name)[i]=(INT8)Lower2Up(pfn[i]); //תΪ��д
  }
  for(;i<8;i++)
  {
   (pfdi->Name)[i]=' '; //����8�ֽڲ�������ո�
  }

  //������չ��
  for(i=(UINT8)(dot_pos+1);i<fn_len;i++)
  {
   (pfdi->Extension)[j]=(UINT8)Lower2Up(pfn[i]); //תΪ��д
   j++;
  }
  for(;j<3;j++)
  {
   (pfdi->Extension)[j]=' '; //����8�ֽڲ�������ո�
  }
 }
 else //���û�е�
 {
  //�������ļ���
  for(i=0;i<fn_len;i++)
  {
   (pfdi->Name)[i]=(UINT8)Lower2Up(pfn[i]); //תΪ��д
  }
  for(;i<8;i++)
  {
   (pfdi->Name)[i]=' '; //����8�ֽڲ�������ո�
  } 
  
  //������չ��
  for(j=0;j<3;j++)
  {
   (pfdi->Extension)[j]=' '; //��չ������ո�
  }
 }
 //=======================�������ļ�������չ��========

 pfdi->Attributes=(UINT8)(is_file?0X20:0X30); //��������
 //=======================��������========

 lowcase=Check_SFN_Illegal_Lower(pfn); //��ȡ���ļ�������չ���Ĵ�Сд״̬
 if((lowcase&0X0F)==0X01) //������ļ���ΪСд
 {
  pfdi->LowerCase|=0X08;
 }
 if((lowcase&0XF0)==0X10) //�����չ��ΪСд
 {
  pfdi->LowerCase|=0X10;
 }
 //=======================�����Сд������============

 pfdi->CTime10ms=(UINT8)((((pdt->time).sec)%2)?0X78:0X00);
 //=======================���봴��ʱ��10MSλ==========

 time=(UINT16)(MAKE_TIME((pdt->time).hour,(pdt->time).min,(pdt->time).sec));
 (pfdi->CTime)[0]=(UINT8)time;
 (pfdi->CTime)[1]=(UINT8)(time>>8);
 //=======================���봴��ʱ��================

 date=(UINT16)(MAKE_DATE((pdt->date).year,(pdt->date).month,(pdt->date).day));
 (pfdi->CDate)[0]=(UINT8)date;
 (pfdi->CDate)[1]=(UINT8)(date>>8);
 //=======================���봴������================

 (pfdi->ADate)[0]=(UINT8)date;
 (pfdi->ADate)[1]=(UINT8)(date>>8);
 //=======================�����������================
 
 (pfdi->MTime)[0]=(UINT8)time;
 (pfdi->MTime)[1]=(UINT8)(time>>8);
 //=======================�����޸�ʱ��================ 

 (pfdi->MDate)[0]=(UINT8)date;
 (pfdi->MDate)[1]=(UINT8)(date>>8);
 //=======================�����޸�����================

 //��俪ʼ��
 if(!is_file) //�����Ŀ¼������fdi�����մ�
 {
  pfdi->HighClust[0]=(UINT8)(((pInit_Args->Next_Free_Cluster)>>16)&0X000000FF);//Ŀ¼���ļ���ͬ��Ŀ¼�ڴ���֮��
  pfdi->HighClust[1]=(UINT8)(((pInit_Args->Next_Free_Cluster)>>24)&0X000000FF);//��ҪΪ�����մأ�Ϊ����д��.��..
  pfdi->LowClust [0]=(UINT8)(((pInit_Args->Next_Free_Cluster)    )&0X000000FF);//������������ļ�Ŀ¼��
  pfdi->LowClust [1]=(UINT8)(((pInit_Args->Next_Free_Cluster)>>8 )&0X000000FF);                                                                                                                                                                  
 }

 //�ļ���С�ֶ�����Ϊ0
 return 0;
}
#endif

/************************************************************************************
 ���ܣ�����õ��ļ�Ŀ¼��ע�ᵽĿ¼����ȥ
 �βΣ�cluster:Ŀ¼�� pfdi:ָ���ļ�Ŀ¼���ָ�� psec:�ļ�Ŀ¼�д����������ַ 
       pn:�ļ�Ŀ¼�����������е�λ�� 
 ���أ����н�����ɹ��������
 ��⣺�˺����Ǵ����ļ���Ŀ¼����Ҫ���Ĳ�����������Զ����ģ���Register_LFN_FDI���Ӧ
*************************************************************************************/
#ifdef REGISTER_FDI
UINT8 Register_FDI(UINT32 cluster,struct FDI *pfdi,UINT32 *psec,UINT8 *pn)
{
 UINT32 temp_sec=0,old_cluster=0;
 UINT8 iSec=0,iFDI=0;
 struct FDIesInSEC *pfdis;

 if(0==pInit_Args->Free_nCluster) return ERR_NO_SPACE; //���û�пռ䣬��ֱ�ӷ���
 
 //���´������Ƿ��Ѿ������ظ����ļ�Ŀ¼��Լ���ѯ��λ
 do
 {
  temp_sec=SOC(cluster);
  for(iSec=0;iSec<(pInit_Args->SectorsPerClust);iSec++)
  {
   znFAT_Device_Read_Sector(temp_sec+(UINT32)iSec,znFAT_Buffer);
   pfdis=((struct FDIesInSEC *)znFAT_Buffer);
   for(iFDI=0;iFDI<NFDI_PER_SEC;iFDI++)
   {
    if(Memory_Compare((UINT8*)((pfdis->FDIes)+iFDI),(UINT8*)pfdi,11))  //�Ƚ��ļ���
    {
     *psec=temp_sec+(UINT32)iSec;
     *pn=iFDI; //��¼�����ļ�Ŀ¼���λ�ã��Ա������н���
     return ERR_FDI_ALREADY_EXISTING;
    }
    else
    {
	 if(0==((((pfdis->FDIes)[iFDI]).Name)[0]))
	 {
	  *psec=temp_sec+(UINT32)iSec;
	  *pn=iFDI; //��¼��λ��λ��

      znFAT_Device_Read_Sector(*psec,znFAT_Buffer);
      Memory_Copy((UINT8*)((((struct FDIesInSEC *)znFAT_Buffer)->FDIes)+*pn),(UINT8 *)pfdi,FDI_NBYTES);
      znFAT_Device_Write_Sector(*psec,znFAT_Buffer);

	  return ERR_SUCC;
	 }
    }
   }
  }
  old_cluster=cluster;
  cluster=Get_Next_Cluster(cluster);
 }while(!IS_END_CLU(cluster)); //����������һ���أ������ѭ��
 //===========================================================
 //������е������˵����ǰ�������޿�λ
 if(0!=pInit_Args->Free_nCluster) //���ʣ��մ���Ϊ0����˵���������޿ռ�
 {
  Modify_FAT(old_cluster,pInit_Args->Next_Free_Cluster);
  Modify_FAT(pInit_Args->Next_Free_Cluster,0X0FFFFFFF); //����FAT����
  Clear_Cluster(pInit_Args->Next_Free_Cluster); //��տ��д�

  *psec=SOC(pInit_Args->Next_Free_Cluster);
  *pn=0; //��¼��λ��λ��

  znFAT_Device_Read_Sector(*psec,znFAT_Buffer);
  Memory_Copy((UINT8*)((((struct FDIesInSEC *)znFAT_Buffer)->FDIes)),(UINT8 *)pfdi,FDI_NBYTES);
  znFAT_Device_Write_Sector(*psec,znFAT_Buffer);

  Update_Next_Free_Cluster();

  return ERR_SUCC;  
 }
 else
 {
  return ERR_NO_SPACE;
 } 
}
#endif

/************************************************************************************
 ���ܣ��ļ�����
 �βΣ�pfi:ָ���ļ���Ϣ����ָ�� pfn:ָ���ļ�·�� pdt:ָ��ʱ����Ϣ
 ���أ����н�� �ɹ��������
 ��⣺�˺�������֧�ֶ����볤�����������Ŀ¼���ļ��������֮���½��ļ�����Ϣ����
       װ���ļ���Ϣ���ϣ��漴���ԶԴ��ļ����в������������ε�open_file���������
       Ҫ�������ļ��Ѿ����������򷵻��������󣬲����Ѿ����ڵ��ļ�����Ϣװ���ļ���Ϣ
       ���ϡ������ĳЩ�����Ҳ���������䵱open_file������û�������ļ��ʹ���һ���µģ�
       ����������ļ���򿪡�
*************************************************************************************/
#ifdef ZNFAT_CREATE_FILE
UINT8 znFAT_Create_File(struct FileInfo *pfi,INT8 *pfn,struct DateTime *pdt)
{
 UINT32 Cur_Cluster=0,pos=0,sec=0;
 UINT8 res=0,n=0;
 struct FDI fdi;
 INT8 *filename;

 #ifdef USE_LFN
 UINT8 is_lfn=0;
 INT8 temp_filename[13];
 #endif

 just_file=pfi;

 #ifndef RT_UPDATE_CLUSTER_CHAIN
 get_next_cluster_in_cccb=0;
 #ifdef USE_ALONE_CCCB
 CCCB_To_Alone();
 Memory_Set((UINT8 *)pcccb_buf,sizeof(UINT32)*CCCB_LEN,0);
 #endif
 #endif

 #ifdef USE_EXCHANGE_BUFFER
 #ifdef USE_ALONE_EXB
 //Memory_Set(just_file->exb_buf,512,0);
 just_file->exb_cursec=0;
 #endif
 #endif

 res=znFAT_Enter_Dir(pfn,&Cur_Cluster,&pos); //����Ŀ¼
 if(res)
 {
 
  return res;
 }

 filename=pfn+pos;

 if(Check_Illegal_Char(filename)) return ERR_ILL_CHAR; //����ļ������Ƿ��зǷ��ַ�

 #ifdef USE_LFN
 if(!Is_LFN(filename))

 #endif
  //�����ļ����Ϸ��ԣ����Ƿ���ֱ�ӷ��أ����ٽ��к���Ĵ���(�˴���SFN�ĺϷ��Լ��ǳ��ϸ�)
  //���ȼ��SFN�ĺϷ��ԣ����ٺ��洦���ϵ��鷳
  if(Check_SFN_Illegal_Length(filename)) return ERR_SFN_ILL_LEN; //���SFN�Ƿ����8.3����
  if(Check_SFN_Dot(filename)) return ERR_SFN_DOT; //���SFN��.�Ƿ�Ϸ� 
  if(Check_SFN_Special_Char(filename)) return ERR_SFN_SPEC_CHAR; //���SFN���Ƿ��������ַ�
  if(((UINT8)(-1))==Check_SFN_Illegal_Lower(filename)) return ERR_SFN_ILL_LOWER; //���SFN���Ƿ��зǷ��Ĵ�Сд
 #ifdef USE_LFN
 }
 else
 {
  is_lfn=1;
  res=oemstr2unistr(filename,pfi->longname); //����ǳ�������filenameתΪUNI��
  if(res) ��  return res;��
 }

 if(!is_lfn) //������ǳ���
 {
 #endif
  Fill_FDI(&fdi,filename,pdt,BOOL_TRUE); //�����ļ�Ŀ¼��
  res=Register_FDI(Cur_Cluster,&fdi,&sec,&n);//�ڵ�ǰ���н����ļ�Ŀ¼���"ע��"

 #ifdef USE_LFN
 }
 else //����ǳ���
 {
  Make_Short_Name(filename,temp_filename);
  Fill_FDI(&fdi,temp_filename,pdt,BOOL_TRUE); //�����ļ�Ŀ¼��
  res=Register_LFN_FDI(Cur_Cluster,&fdi,(pfi->longname),&sec,&n); //�ڵ�ǰ���н��г������Ӧ�����ע�ᡱ
 }
 #endif
 
 if(!res)
 {
  //���½��ļ�����Ϣװ���ļ���Ϣ����
  #ifdef USE_LFN
  if(is_lfn)
  {
   StringCopy(pfi->File_Name,temp_filename);
   pfi->have_lfn=1;
  }
  else
  #endif
  {
   StringCopy(pfi->File_Name,filename);
  }
  pfi->File_Attr=0X20;
  (pfi->File_CTime).hour=(pdt->time).hour;
  (pfi->File_CTime).min=(pdt->time).min;
  (pfi->File_CTime).sec=(pdt->time).sec;
  (pfi->File_CDate).year=(pdt->date).year;
  (pfi->File_CDate).month=(pdt->date).month;
  (pfi->File_CDate).day=(pdt->date).day;
  //(pfi->File_ADate).year=(pdt->date).year;
  //(pfi->File_ADate).month=(pdt->date).month;
  //(pfi->File_ADate).day=(pdt->date).day;
  //(pfi->File_MTime).hour=(pdt->time).hour;
  //(pfi->File_MTime).min=(pdt->time).min;
  //(pfi->File_MTime).sec=(pdt->time).sec;
  //(pfi->File_MDate).year=(pdt->date).year;
  //(pfi->File_MDate).month=(pdt->date).month;
  //(pfi->File_MDate).day=(pdt->date).day;

  pfi->File_StartClust=0;
  pfi->File_Size=0;

  pfi->File_CurClust=0;
  pfi->File_CurSec=0;
  pfi->File_CurPos=0;

  pfi->File_CurOffset=0;
  pfi->File_IsEOF=BOOL_TRUE;

  pfi->FDI_Sec=sec;
  pfi->nFDI=n;

  return ERR_SUCC;
 }
 else
 {
  if(res==ERR_FDI_ALREADY_EXISTING) //����ļ��Ѿ����ڣ���ֱ�ӽ�����
  {
   znFAT_Device_Read_Sector(sec,znFAT_Buffer); //��������ļ�Ŀ¼�����ڵ�����
   Analyse_FDI(pfi,(((struct FDIesInSEC *)znFAT_Buffer)->FDIes)+n); //����ƥ����ļ�Ŀ¼��
   pfi->FDI_Sec=sec;
   pfi->nFDI=n;
   
   #ifdef USE_LFN
   if(is_lfn)
   {
    pfi->have_lfn=1;
   }
   else
   {
    pfi->have_lfn=0;
   }
   #endif 
  }
  return res;
 }
}
#endif

/************************************************************************************
 ���ܣ���һ��Ŀ¼�д���һ��Ŀ¼
 �βΣ�cluster:Ŀ¼�� pdn:ָ��Ŀ¼�� pdt:ָ��ʱ����Ϣ
 ���أ����н�� �ɹ��������
 ��⣺֧�ֳ�Ŀ¼������ cluster���ջ᷵���´�����Ŀ¼�Ŀ�ʼ��
       �˺�����znFATʵ�ֶ༶Ŀ¼��������Ҫ����
*************************************************************************************/
#ifdef CREATE_DIR_IN_CLUSTER
UINT8 Create_Dir_In_Cluster(UINT32 *cluster,INT8 *pdn,struct DateTime *pdt)
{
 UINT8 res=0,i=0;
 UINT32 dummy=0;
 struct FDI fdi;

 #ifdef USE_LFN
 UINT8 is_lfn=0; //��������Ŀ¼���Ƿ���
 UINT16 temp_lfn_buf[MAX_LFN_LEN+1]; //����װ�س���UNI������ʱ����
 INT8 temp_dirname[13];
 #endif

 #ifndef RT_UPDATE_CLUSTER_CHAIN
 get_next_cluster_in_cccb=0;
 #endif

 if(Check_Illegal_Char(pdn)) return ERR_ILL_CHAR; //���Ŀ¼�����Ƿ��зǷ��ַ�
 
 #ifdef USE_LFN
 if(!Is_LFN(pdn))
 {
 #endif
  //�����ļ����Ϸ��ԣ����Ƿ���ֱ�ӷ��أ����ٽ��к���Ĵ���(�˴���SFN�ĺϷ��Լ��ǳ��ϸ�)
  //���ȼ��SFN�ĺϷ��ԣ����ٺ��洦���ϵ��鷳
  if(Check_SFN_Illegal_Length(pdn)) return ERR_SFN_ILL_LEN; //���SFN�Ƿ����8.3����
  if(Check_SFN_Dot(pdn)) return ERR_SFN_DOT; //���SFN��.�Ƿ�Ϸ� 
  if(Check_SFN_Special_Char(pdn)) return ERR_SFN_SPEC_CHAR; //���SFN���Ƿ��������ַ�
  if(((UINT8)(-1))==Check_SFN_Illegal_Lower(pdn)) return ERR_SFN_ILL_LOWER; //���SFN���Ƿ��зǷ��Ĵ�Сд
 #ifdef USE_LFN
 }
 else
 {
  is_lfn=1;
  res=oemstr2unistr(pdn,temp_lfn_buf); //����ǳ�������pdnתΪUNI��
  if(res) return res;
 }

 if(!is_lfn) //������ǳ���
 {
 #endif
  Fill_FDI(&fdi,pdn,pdt,BOOL_FALSE); //����Ŀ¼��
  res=Register_FDI(*cluster,&fdi,&dummy,&i);//�ڵ�ǰ���н����ļ�Ŀ¼���"ע��"

 #ifdef USE_LFN
 }
 else //����ǳ���
 {
  Make_Short_Name(pdn,temp_dirname);
  Fill_FDI(&fdi,temp_dirname,pdt,BOOL_FALSE); //�����ļ�Ŀ¼��
  res=Register_LFN_FDI(*cluster,&fdi,temp_lfn_buf,&dummy,&i); //�ڵ�ǰ���н��г������Ӧ�����ע�ᡱ
 }
 #endif

 if(res)
 {
  return res;
 }

 //====================================================================================

 //��Ŀ¼����д��.��..
 Modify_FAT(pInit_Args->Next_Free_Cluster,0X0FFFFFFF); //����FAT����
 Clear_Cluster(pInit_Args->Next_Free_Cluster); //��տ��д� 

 //��fdi�е������滻Ϊ. ��Ϊ.��Ŀ¼ָ������ǰ��
 fdi.Name[0]='.';
 for(i=1;i<11;i++)
 {
  fdi.Name[i]=' '; //.����ȫ�����ո�
 }

 Memory_Copy(znFAT_Buffer,((UINT8 *)(&fdi)),FDI_NBYTES); //���ļ�Ŀ¼��.װ�뵽�ڲ���������

 //��fdi�е������滻Ϊ.. ��Ϊ..��Ŀ¼ָ������һ��(DOS��ʹ��CD..���Իص���һ��Ŀ¼�����������)
 fdi.Name[1]='.';
 
 //��fdi�еĴ�ֵ��Ϊ��һ��Ŀ¼���״� 
 if(2!=(*cluster)) //�����һ��Ŀ¼�״ز���"��Ŀ¼"(��Ŀ¼)
 {
  fdi.HighClust[0]=(UINT8)(((*cluster)>>16)&0X000000FF);
  fdi.HighClust[1]=(UINT8)(((*cluster)>>24)&0X000000FF);
  fdi.LowClust [0]=(UINT8)(((*cluster)    )&0X000000FF);
  fdi.LowClust [1]=(UINT8)(((*cluster)>>8 )&0X000000FF); 
 }
 else
 {
  fdi.HighClust[0]=fdi.HighClust[1]=fdi.LowClust[0]=fdi.LowClust[1]=0;
 }

 Memory_Copy(znFAT_Buffer+FDI_NBYTES,((UINT8 *)(&fdi)),FDI_NBYTES); //���ļ�Ŀ¼��..װ�뵽�ڲ���������

 znFAT_Device_Write_Sector(SOC(pInit_Args->Next_Free_Cluster),znFAT_Buffer);

 *cluster=(pInit_Args->Next_Free_Cluster); //�����´�����Ŀ¼�״�

 Update_Next_Free_Cluster();

 return ERR_SUCC;
}
#endif

/************************************************************************************
 ���ܣ�����Ŀ¼
 �βΣ�pdp:ָ��Ŀ¼·�� pdt:ָ��ʱ����Ϣ 
 ���أ����н��  �ɹ��������
 ��⣺֧��������Ŀ¼���� �� Ŀ¼·�������� /a/b/c/d/e/f/g/h/ ע������һ��Ҫ��/��"\\"
       ����������znFAT����Ϊ���һ��Ŀ¼�����ļ������������Դ�����֧�ֳ�Ŀ¼��
*************************************************************************************/
#ifdef ZNFAT_CREATE_DIR
UINT8 znFAT_Create_Dir(INT8 *pdp,struct DateTime *pdt)
{
 UINT32 Cur_Cluster=0,i=0;
 UINT8 index=0,res=0;

 #ifndef USE_LFN
 UINT8 dirname[13];
 #else
 UINT8 dirname[MAX_LFN_LEN+1];
 #endif

 #ifndef RT_UPDATE_CLUSTER_CHAIN
 get_next_cluster_in_cccb=0;
 #endif

 if(znFAT_Enter_Dir(pdp,&Cur_Cluster,&i)) //����Ŀ¼�������;��������
 {                                        //����ĳһ��Ŀ¼�����Ϸ�����
  while('\0'!=pdp[i])                     //Ŀ¼�����ڣ��������һ��
  {                                       //Ŀ¼���״ؼ�¼��Cur_Cluster��
   if('\\'==pdp[i] || '/'==pdp[i])                       //����ʧ�ܵ���һ��Ŀ¼���ĵ�һ
   {                                      //�ַ���λ�ü�¼��i��
    dirname[index]='\0';                  //����\a1\b2\c3\e4\f5\��Ŀ¼c3������
    index=0;                              //��Cur_ClusterΪĿ¼b2���״أ�i��¼����'c'��λ�ü�7
   
    res=Create_Dir_In_Cluster(&Cur_Cluster,(INT8 *)dirname,pdt); //�ڴ�Cur_Cluster�д���Ŀ¼
    if(res) 
    { 
	 return res;  //���ش�����  
    }
   }
   else
   {
    dirname[index]=pdp[i];
    index++;
    #ifndef USE_LFN
    if(index>12) //�����ʹ�ó�������Ŀ¼���Լ��ļ�������ܳ���8+1+3
    {
	 return ERR_SFN_ILL_LEN; //Ŀ¼������8.3�����ֹdirname���
    }
    #else
    if(index>MAX_LFN_LEN) //���ʹ�ó�������Ŀ¼���Լ��ļ�������ܳ����趨�ĳ��������
    {
	 return ERR_LFN_BUF_OUT; //Ŀ¼������MAX_LFN_LEN�����ֹdirname���
    }   
    #endif
   }
   i++;
  }
  
  return ERR_SUCC; //�ɹ�  
 }
 else
 {
  return ERR_DIR_ALREADY_EXISTING; //Ҫ������Ŀ¼�Ѿ�������
 }
}
#endif

//========================���´��볢��ʵ��Ŀ¼ɾ������=====================

/************************************************************************************
 ���ܣ�����һ��FAT����
 �βΣ�cluster:�����Ŀ�ʼ�غ�
 ���أ�0
 ��⣺����ԭ��ʹ��Modify_FAT+ѭ����ʵ�ִ˺�����Ч�ʼ��ͣ��ٶȺ��������ڽ����˸Ľ���
       Ч�ʽϸߡ�
*************************************************************************************/
#ifdef DESTROY_FAT_CHAIN
UINT8 Destroy_FAT_Chain(UINT32 cluster)
{
 UINT32 clu_sec=0,temp1=0,temp2=0,old_clu=0,nclu=1;

 struct FAT_Sec *pFAT_Sec;

 if(cluster<(pInit_Args->Next_Free_Cluster)) //���Ҫ���ٵĴ���ͷ�ر���һ�մ�ֵС������һ�մ�ֵ��Ϊ��
 {
  pInit_Args->Next_Free_Cluster=cluster;
 }

 old_clu=cluster;

 znFAT_Device_Read_Sector((old_clu/NITEMSINFATSEC)+(pInit_Args->FirstFATSector),znFAT_Buffer);

 pFAT_Sec=(struct FAT_Sec *)znFAT_Buffer;

 cluster=Bytes2Value(((pFAT_Sec->items)[cluster%NITEMSINFATSEC]).Item,4);

 while(!IS_END_CLU(cluster))
 {
  nclu++;

  clu_sec=cluster/NITEMSINFATSEC;

  temp1=old_clu%NITEMSINFATSEC;
  temp2=old_clu/NITEMSINFATSEC;

  ((pFAT_Sec->items)[temp1]).Item[0]=0;
  ((pFAT_Sec->items)[temp1]).Item[1]=0;
  ((pFAT_Sec->items)[temp1]).Item[2]=0;
  ((pFAT_Sec->items)[temp1]).Item[3]=0;

  if(temp2!=clu_sec)
  {     
   znFAT_Device_Write_Sector(temp2+(pInit_Args->FirstFATSector),znFAT_Buffer);
   znFAT_Device_Write_Sector(temp2+(pInit_Args->FirstFATSector+pInit_Args->FATsectors),znFAT_Buffer);

   znFAT_Device_Read_Sector(clu_sec+(pInit_Args->FirstFATSector),znFAT_Buffer);
  }

  old_clu=cluster;
  cluster=Bytes2Value(((pFAT_Sec->items)[cluster%NITEMSINFATSEC]).Item,4);
 }

 temp1=old_clu%NITEMSINFATSEC;
 temp2=old_clu/NITEMSINFATSEC;

 ((pFAT_Sec->items)[temp1]).Item[0]=0;
 ((pFAT_Sec->items)[temp1]).Item[1]=0;
 ((pFAT_Sec->items)[temp1]).Item[2]=0;
 ((pFAT_Sec->items)[temp1]).Item[3]=0;

 znFAT_Device_Write_Sector(temp2+(pInit_Args->FirstFATSector),znFAT_Buffer);
 znFAT_Device_Write_Sector(temp2+(pInit_Args->FirstFATSector+pInit_Args->FATsectors),znFAT_Buffer);

 pInit_Args->Free_nCluster+=nclu; //����ʣ��մ���

 return ERR_SUCC;
}
#endif

/************************************************************************************
 ���ܣ�����һ���ļ�Ŀ¼������Ӧ����������
 �βΣ�pitem:ָ���ļ�Ŀ¼���ָ��
 ���أ�0
 ��⣺��Ҫ�������ļ���Ŀ¼��ɾ��
*************************************************************************************/
#ifdef DESTROY_FDI
UINT8 Destroy_FDI(struct FDI *pitem)
{
 UINT32 start_cluster=Bytes2Value(pitem->LowClust,2)+Bytes2Value(pitem->HighClust,2)*65536;

 if(0==start_cluster) return ERR_SUCC;

 Destroy_FAT_Chain(start_cluster); //���ٴ���

 return ERR_SUCC;
} 
#endif

/************************************************************************************
 ���ܣ���ĳ��Ŀ¼�ؽ����������������Ƿ���Ŀ¼�����Ŀ¼������Ŀ¼������������������
       �ļ��ȫ�����١�
 �βΣ�cluster:Ҫ����������Ŀ¼��ʼ�غ� 
       for_del_dir:������Ŀ¼��֮���Ƿ����俪ʼ�أ����ǽ���ֱ������
 ���أ����н�� �ҵ���Ŀ¼��δ�ҵ� ע�������Ŀ¼��δ�ҵ���Ŀ¼�����Ŀ¼�е������ļ�
       ����ȫ�����٣�����������£����Ŀ¼�漴Ҳ�ᱻ���٣�Ȼ�����Ŀ¼����.....
 ��⣺Ŀ¼ɾ��������㷨����Ϊ���ӣ���������������ñȽϼ򵥡�Ŀ¼��Ϊ����Ŀ¼
       ������һ�����εĽṹ�����ɾ��Ŀ¼�Ĺ�����ʵ��һ�����ݹ���ݡ��Ĺ��̣�ֱ���ص�
       ����Ŀ¼��
*************************************************************************************/
#ifdef HAVE_ANY_SUBDIR_WITH_DEL_FOREFILE
UINT8 Have_Any_SubDir_With_Del_ForeFile(UINT32 *cluster,UINT8 for_del_dir)
{
 UINT8 iSec=0,iFDI=0;
 UINT32 sec_temp=0;
 UINT32 temp=*cluster;

 struct FDIesInSEC *pitems; //ָ���ļ�Ŀ¼���������ݵ�ָ��
 struct FDI *pitem; //ָ���ļ�Ŀ¼�����ݵ�ָ��

 #ifndef RT_UPDATE_CLUSTER_CHAIN
 get_next_cluster_in_cccb=0;
 #endif

 do
 {
  sec_temp=SOC(temp); //��ǰ��������
  for(iSec=0;iSec<(pInit_Args->SectorsPerClust);iSec++) 
  {
   znFAT_Device_Read_Sector(sec_temp+(UINT32)iSec,znFAT_Buffer);
   pitems=(struct FDIesInSEC *)znFAT_Buffer; 

   for(iFDI=0;iFDI<NFDI_PER_SEC;iFDI++) //���������и��ļ�Ŀ¼��
   {
    pitem=&(pitems->FDIes[iFDI]); //ָ��һ���ļ�Ŀ¼������

	if((CHK_ATTR_FILE(pitem->Attributes)) && (0XE5!=pitem->Name[0])) //�ļ�����Ϊ�ļ�����û�б�ɾ��
    {
     Destroy_FDI(pitem); //�����ļ�Ŀ¼����Ӧ�Ĵ��� ע���ڲ����������ݱ����
	 znFAT_Device_Read_Sector(sec_temp+(UINT32)iSec,znFAT_Buffer); //���¶�ȡ�������ݣ��ָ��ڲ�������������

	 pitem->Name[0]=0XE5; //���ļ�Ŀ¼�����"��ɾ��"�ı��
     pitem->HighClust[0]=pitem->HighClust[1]=0; //��ʼ�صĸ�����0

     znFAT_Device_Write_Sector(sec_temp+(UINT32)iSec,znFAT_Buffer); //��������ٲ��������д����
	}

    if((CHK_ATTR_DIR(pitem->Attributes)) && (0XE5!=pitem->Name[0]) //�ļ�����ΪĿ¼����û�б�ɾ��
	   && ('.'!=pitem->Name[0]))                          //����.��..
    { 
	 if(!for_del_dir) //����Ϊ�˰���Ŀ¼����ɾ��������Ϊ�˻�ȡ���״أ��������������Ŀ¼
	 {
	  *cluster=Bytes2Value(pitem->LowClust,2)+Bytes2Value(pitem->HighClust,2)*65536;
	 }
     else
	 {
      Destroy_FDI(pitem); //�����ļ�Ŀ¼����Ӧ�Ĵ��� ע���ڲ����������ݱ����
	  znFAT_Device_Read_Sector(sec_temp+(UINT32)iSec,znFAT_Buffer); //���¶�ȡ�������ݣ��ָ��ڲ�������������

	  pitem->Name[0]=0XE5; //���ļ�Ŀ¼�����"��ɾ��"�ı��
      pitem->HighClust[0]=pitem->HighClust[1]=0; //��ʼ�صĸ�����0

      znFAT_Device_Write_Sector(sec_temp+(UINT32)iSec,znFAT_Buffer); //��������ٲ��������д����	
	 }
     return BOOL_TRUE;
    }
   }
  }

  temp=Get_Next_Cluster(temp); //��ȡ��һ��
 }while(!IS_END_CLU(temp)); //����������һ���أ������ѭ��
  
 return BOOL_FALSE;
}
#endif

/************************************************************************************
 ���ܣ������ĳ��Ŀ¼��ʼ��������󡱵�Ŀ¼
 �βΣ�cluster:ָ��Ŀ¼�ص�ָ��
 ���أ�0
 ��⣺�˺�����Ŀ¼ɾ�����ܵ�һ���������ؼ��Ĳ��������������뵽���������󡱵�Ŀ¼��
       ͬʱ�ڴ˹����У�ɾ��������Ŀ¼����ߡ��������ļ������˺�����˼���Ϊ����
*************************************************************************************/
#ifdef ENTER_DEEP_AHEAD_DIR
UINT8 Enter_Deep_Ahead_Dir(UINT32 *cluster)
{
 UINT32 dir_cluster=*cluster; 

 while(Have_Any_SubDir_With_Del_ForeFile(&dir_cluster,BOOL_FALSE));

 *cluster=dir_cluster;

 return ERR_SUCC;
}
#endif

/************************************************************************************
 ���ܣ���ȡһ��Ŀ¼����һ��Ŀ¼��ʼ��
 �βΣ�cluster:Ŀ¼�Ŀ�ʼ�أ�ͬʱ�����ڽ��ռ���õ�����һ��Ŀ¼�Ŀ�ʼ��
 ���أ����н�� �ɹ�������� 
 ��⣺��Ŀ¼����ͷ�ϣ���.��..����..�м�¼����һ��Ŀ¼�Ŀ�ʼ�أ��˺������ǻ�����һ��
       ����ȡĿ¼��һ��Ŀ¼��ʼ�صġ�
*************************************************************************************/
#ifdef GET_UPPER_DIR
UINT8 Get_Upper_Dir(UINT32 *cluster)
{
 struct FDIesInSEC *pitems; //ָ���ļ�Ŀ¼���������ݵ�ָ��
 struct FDI *pitem; //ָ���ļ�Ŀ¼�����ݵ�ָ��

 znFAT_Device_Read_Sector(SOC(*cluster),znFAT_Buffer);
 pitems=(struct FDIesInSEC *)znFAT_Buffer; 
   
 pitem=&(pitems->FDIes[1]); //ָ��..�ļ�Ŀ¼����Ի�ȡ��һ��Ŀ¼���״�

 if('.'==pitem->Name[0] && '.'==pitem->Name[1])
  *cluster=Bytes2Value(pitem->LowClust,2)+Bytes2Value(pitem->HighClust,2)*65536;
 else
  return ERR_FS_DIR; //�����Ŀ¼����ͷ�ϲ���.��..����˵���ļ�ϵͳ�ѱ���
 
 if(0==(*cluster)) (*cluster)=2; //�����һ��Ŀ¼Ϊ��Ŀ¼(��Ŀ¼)������ֵ��Ϊ2

 return ERR_SUCC;
}
#endif

/************************************************************************************
 ���ܣ�ɾ��Ŀ¼
 �βΣ�dirpath:ָ����Ŀ¼·�� Ŀ¼��֧��ͨ��
 ���أ����н�� �ɹ��������
 ��⣺dirpath��\a\b\c��ɾ��Ŀ¼c�����Ҫ��\������ɾ��Ŀ¼����ɾ��Ŀ¼�µ�������Ŀ¼
       ���ļ�����Ŀ¼�¿���������Ŀ¼���ļ������εݹ顣dirpathҲ����Ϊ/a/b/c* ������
       ɾ��bĿ¼��������c��ͷ��Ŀ¼��
*************************************************************************************/
#ifdef ZNFAT_DELETE_DIR
UINT8 znFAT_Delete_Dir(INT8 *dirpath) 
{
 UINT32 top_dir_first_cluster=0,sub_dir_first_cluster=0;
 UINT8 res=0;

 struct FDIesInSEC *pitems; //ָ���ļ�Ŀ¼���������ݵ�ָ��

 struct FileInfo fi;

 res=znFAT_Open_File(&fi,dirpath,0,BOOL_FALSE); //���Դ�Ŀ¼

 if(res) return res; //�����Ŀ¼ʧ�ܣ���ֱ�ӷ��ش�����

 while(!res) //Ŀ¼�򿪳ɹ�
 {
  top_dir_first_cluster=fi.File_StartClust; //����Ŀ¼���״�
  sub_dir_first_cluster=top_dir_first_cluster;

  //���´��뽫����Ŀ¼����������(�ļ�����Ŀ¼����Ŀ¼�е����ݣ����ݹ�)����

  Enter_Deep_Ahead_Dir(&sub_dir_first_cluster); //��ȡ�����ǰ��Ŀ¼�״�

  while(sub_dir_first_cluster!=top_dir_first_cluster) //��������ǰĿ¼�״ؾ���Ҫɾ���Ķ���Ŀ¼
  {                                                   //��˵������Ŀ¼�µ��������ݶ��Ѿ����
   Get_Upper_Dir(&sub_dir_first_cluster); //��ȡ��һ��Ŀ¼�״� 
   
   Have_Any_SubDir_With_Del_ForeFile(&sub_dir_first_cluster,BOOL_TRUE); //������������ݵ���Ŀ¼���� 

   Enter_Deep_Ahead_Dir(&sub_dir_first_cluster); //��ȡ�����ǰ��Ŀ¼�״�
  }  

  //���ٶ���Ŀ¼��Ӧ���ļ�Ŀ¼������

  znFAT_Device_Read_Sector(fi.FDI_Sec,znFAT_Buffer); //��ȡ�ļ�Ŀ¼�����ڵ�����
  pitems=(struct FDIesInSEC *)znFAT_Buffer;

  Destroy_FDI((pitems->FDIes)+fi.nFDI); //���ٶ���Ŀ¼

  znFAT_Device_Read_Sector(fi.FDI_Sec,znFAT_Buffer); //��ȡ�ļ�Ŀ¼�����ڵ�����
  (pitems->FDIes)[fi.nFDI].Name[0]=0XE5;
  (pitems->FDIes)[fi.nFDI].HighClust[0]=(pitems->FDIes)[fi.nFDI].HighClust[1]=0;

  znFAT_Device_Write_Sector(fi.FDI_Sec,znFAT_Buffer); //��д����

  res=znFAT_Open_File(&fi,dirpath,0,BOOL_FALSE); //���Դ�Ŀ¼
 }
 
 znFAT_Close_File(&fi);

 #ifdef RT_UPDATE_FSINFO
 Update_FSINFO();
 #endif

 return ERR_SUCC; 
}
#endif

//========================���ϴ�������ʵ��Ŀ¼ɾ������====================

/************************************************************************************
 ���ܣ��ļ�ɾ��
 �βΣ�filepath:�ļ�·�� 
 ���أ����н�� �ɹ��������
 ��⣺ɾ���ļ���ɾ��Ŀ¼Ҫ�򵥵Ķࡣfilepath��\a\b\test.txt��ɾ���ļ�test.txt
       ����ͨ�䣬filepath����Ϊ/a/b/c/d/t*.txt ɾ��dĿ¼�����е�t��ͷ��txt�ļ�
*************************************************************************************/
#ifdef ZNFAT_DELETE_FILE
UINT8 znFAT_Delete_File(INT8 *filepath) 
{
 UINT8 res=0;
 struct FileInfo fi; 

 struct FDIesInSEC *pitems; //ָ���ļ�Ŀ¼���������ݵ�ָ��
 struct FDI *pitem; //ָ���ļ�Ŀ¼�����ݵ�ָ��

 res=znFAT_Open_File(&fi,filepath,0,BOOL_TRUE);
 if(res) return res;

 while(!res) //���ļ��ɹ�
 {
  znFAT_Device_Read_Sector(fi.FDI_Sec,znFAT_Buffer); //��ȡ�ļ����ļ�Ŀ¼����������
  pitems=(struct FDIesInSEC *)znFAT_Buffer;
  pitem=(pitems->FDIes)+fi.nFDI;

  if(0!=fi.File_StartClust) Destroy_FAT_Chain(fi.File_StartClust); //������������

  znFAT_Device_Read_Sector(fi.FDI_Sec,znFAT_Buffer); //��ȡ�ļ�Ŀ¼�����ڵ�����

  pitem->Name[0]=0XE5; //���ļ�Ŀ¼�����"��ɾ��"�ı��
  pitem->HighClust[0]=pitem->HighClust[1]=0; //��ʼ�صĸ�����0

  znFAT_Device_Write_Sector(fi.FDI_Sec,znFAT_Buffer); //��д����

  res=znFAT_Open_File(&fi,filepath,0,BOOL_TRUE);
 }

 znFAT_Close_File(&fi);

 #ifdef RT_UPDATE_FSINFO
 Update_FSINFO();
 #endif

 return ERR_SUCC; 
}
#endif

//==========================���´�������ʵ�ָ�ʽ������============================

/************************************************************************************
 ���ܣ���ȡ��ĳ�ִ��������´ش�С���Ƽ�ֵ
 �βΣ�nsec:��������
 ���أ�0
 ��⣺�������0����˵��������������FAT32�ļ�ϵͳ��˵̫С�ˣ��޷���FAT32����ʽ����
*************************************************************************************/
#ifdef GET_RECMD_SZCLU
UINT8 Get_Recmd_szClu(UINT32 nsec)
{
 if(nsec<(14336)) return 0;

 if((nsec>=(14336)) && (nsec<=(32767))) return 0;
 if((nsec>=(32768)) && (nsec<=(65535))) return 1;
 if((nsec>=(65536)) && (nsec<=(131071))) return 1;
 if((nsec>=(131072)) && (nsec<=(262143))) return 2;
 if((nsec>=(262144)) && (nsec<=(524287))) return 4;
 if((nsec>=(524288)) && (nsec<=(16777215))) return 8;
 if((nsec>=(16777216)) && (nsec<=(33554431))) return 16;
 if((nsec>=(33554432)) && (nsec<=(67108863))) return 32;
 if((nsec>=(67108864)) && (nsec<=(4294967295UL))) return 64;

 return 0;
}
#endif

/************************************************************************************
 ���ܣ��ڴ����ϴ���һ��FAT32���ļ�ϵͳ������ʽ��
 �βΣ�tt_sec:�������� clu_sz:ʹ����ָ���Ĵش�С�����Ϊ0����ȡ�Ƽ�ֵ
 ���أ�0
 ��⣺FAT32�ĸ�ʽ����Ϊ���ֲ��ԣ�FDISK��SFD��FDISK��֧�ֶ�����ģ������Ҫ����MBR
       ����MBR���漰��һЩ�Ƚϵײ�����ݣ������Ѷȣ����Ϊ�˼򵥶���Ч��znFAT��ʹ����
       SFD���ԣ������������̾͵���һ��Ĭ�ϵĴ�����������û��MBR����ʽ������̵ľ�
       ��ΪZN'ZNFATOK!
*************************************************************************************/
#ifdef ZNFAT_MAKE_FS
UINT8 znFAT_Make_FS(UINT32 tt_sec,UINT16 clu_sz) //��ʽ�� tt_sec �������� clu_sz ָ���Ĵش�С
{                                          //��Ϊ0�򰴴��̴�Сѡ��Ĭ��ֵ ��ʽ�����Բ��ñȽϼ򵥵�SFD����
	                                       //��MBR����֧�ַ�����ֱ�Ӵ�DBR��ʼ
 struct DBR      *pdbr;
 struct FSInfo   *pfsinfo;

 UINT32 temp=0,temp1=0,temp2=0;

 tt_sec/=(UINT32)(NSECPERCYLINDER); 
 tt_sec*=(UINT32)(NSECPERCYLINDER);//��ȥ��ʣ����������ʣ��������ָ����һ�������������

 //=================�ϳ�DBR��������=============================================================
 PGM2RAM(znFAT_Buffer,_dbr,512); //��ģ�������а����ݰᵽ�ڲ�������
 pdbr=(struct DBR *)znFAT_Buffer;

 pdbr->BPB_SecPerClus=(UINT8)(clu_sz/512); //ÿ��������
 if(0==pdbr->BPB_SecPerClus) pdbr->BPB_SecPerClus=Get_Recmd_szClu(tt_sec);
 if(0==pdbr->BPB_SecPerClus) return ERR_FMT_TOO_LOW_VOLUME; //����̫С��������FAT32���и�ʽ��

 temp1=pdbr->BPB_SecPerClus;

 pdbr->BPB_TotSec32[0]=(UINT8)((tt_sec)    &0X000000FF);
 pdbr->BPB_TotSec32[1]=(UINT8)((tt_sec>>8) &0X000000FF);
 pdbr->BPB_TotSec32[2]=(UINT8)((tt_sec>>16)&0X000000FF);
 pdbr->BPB_TotSec32[3]=(UINT8)((tt_sec>>24)&0X000000FF); //�÷�������������

 temp=(tt_sec-32)/(((UINT32)NITEMSINFATSEC)*((UINT32)(pdbr->BPB_SecPerClus)));
 if((tt_sec-32)%((UINT32)NITEMSINFATSEC)*((UINT32)pdbr->BPB_SecPerClus)) temp++; //((tt_sec-32)-2*FATsz)/(SecPerClus*128)=FATsz �������ʽ
 temp2=temp;
 
 pdbr->BPB_FATSz32[0]=(UINT8)((temp)    &0X000000FF);
 pdbr->BPB_FATSz32[1]=(UINT8)((temp>>8) &0X000000FF);
 pdbr->BPB_FATSz32[2]=(UINT8)((temp>>16)&0X000000FF);
 pdbr->BPB_FATSz32[3]=(UINT8)((temp>>24)&0X000000FF); //FAT���������

 znFAT_Device_Write_Sector(0,znFAT_Buffer); //���ϳɺõ�DBR����д�뵽0������ȥ

 //===============================���ϴ�����ɶ�DBR�������ݵĺϳ�===============================

 //===================================���´�����ɶ�FSINFO�������ݵĺϳ�========================

 Memory_Set(znFAT_Buffer,ZNFAT_BUF_SIZE,0); //���ڲ���������0
 PGM2RAM(znFAT_Buffer,_fsinfo_1,4); //��FSINFOģ�����ݵĵ�һ���ְ����
 PGM2RAM(znFAT_Buffer+484,_fsinfo_2,28); //��FSINFOģ�����ݵĵڶ����ְ����
                                             //ע��FSINFOģ�����ݷ�Ϊ�����֣���Ҫ����Ϊ�����о���
                                             //������0��Ϊ�˽�ʡ�̻�������������FLASHROM�ռ��ʹ����
 pfsinfo=(struct FSInfo *)znFAT_Buffer;
 
 temp=(tt_sec-32-2*temp)/((UINT32)(temp1))-1; //�ܴ���-1����Ϊ��2��Ϊ��Ŀ¼���Ѿ������ռ��
 pfsinfo->Free_Cluster[0]=(UINT8)((temp)    &0X000000FF);
 pfsinfo->Free_Cluster[1]=(UINT8)((temp>>8) &0X000000FF);
 pfsinfo->Free_Cluster[2]=(UINT8)((temp>>16)&0X000000FF);
 pfsinfo->Free_Cluster[3]=(UINT8)((temp>>24)&0X000000FF); //ʣ��մ���

 znFAT_Device_Write_Sector(1,znFAT_Buffer); //���ϳɺõ�DBR����д�뵽64������ȥ����DBR�����ĺ�һ������

 //=====================================���ϴ�����ɶ�FSINFO�������ݵĺϳ�=====================

 //=====================================���´�����ɶ�FAT��ĳ�ʼ��============================

 znFAT_Device_Clear_nSector(temp1,32+2*temp2);
 znFAT_Device_Clear_nSector(temp2-1,33);
 znFAT_Device_Clear_nSector(temp2-1,33+temp2);

 PGM2RAM(znFAT_Buffer,_fatsec,12); //��FAT��ģ�����ݰᵽ�ڲ�������
 znFAT_Device_Write_Sector(32,znFAT_Buffer); //��FAT��1��д��0
 znFAT_Device_Write_Sector(32+temp2,znFAT_Buffer); //��FAT��2��д��0

 //=====================================���ϴ�����ɶ�FAT��ĳ�ʼ��============================

 //=====================================���´�������������������г�ʼ����д����=============

 PGM2RAM(znFAT_Buffer,_1stsec,26); 
 znFAT_Device_Write_Sector(32+2*temp2,znFAT_Buffer); //����������һ������д������

 //=====================================����.....==============================================
 return ERR_SUCC;
}  
#endif

//===================���´�������ʵ���ļ�����д��=====================

/************************************************************************************
 ���ܣ������ļ��Ĵ�С������ǰ�ļ���Ϣ�����е�File_Sizeֵд�뵽�ļ�Ŀ¼����ȥ
 �βΣ�pfi:ָ���ļ�Ŀ¼���ָ��
 ���أ�0
 ��⣺�����ļ�д�����ݣ���ɾ������֮����������ô˺������ļ���С���µ�Ŀ¼����ȥ
       �������ڵ������ǿ�����д������ݵġ��˺�����znFAT���ܵ�RT_UPDATE_FILESIZE���
       ��Ŀ��ƣ��Ծ����Ƿ�ʵʱ�����ļ���С��
*************************************************************************************/
#ifdef UPDATE_FILE_SIZE
UINT8 Update_File_Size(struct FileInfo *pfi) //�����ļ���С
{
 struct FDI *pfdi;

 just_file=pfi;

 znFAT_Device_Read_Sector(pfi->FDI_Sec,znFAT_Buffer);

 pfdi=(((struct FDIesInSEC *)znFAT_Buffer)->FDIes)+(pfi->nFDI); //�ļ����ļ�Ŀ¼��

 (pfdi->FileSize)[0]=(UINT8)((pfi->File_Size)&0X000000FF)      ;
 (pfdi->FileSize)[1]=(UINT8)(((pfi->File_Size)&0X0000FF00)>>8) ;
 (pfdi->FileSize)[2]=(UINT8)(((pfi->File_Size)&0X00FF0000)>>16);
 (pfdi->FileSize)[3]=(UINT8)(((pfi->File_Size)&0XFF000000)>>24);

 znFAT_Device_Write_Sector(pfi->FDI_Sec,znFAT_Buffer);

 return 0;
}
#endif

/************************************************************************************
 ���ܣ������ļ��Ŀ�ʼ��
 �βΣ�pfi:ָ���ļ���Ϣ���ϵ�ָ�� clu:�ļ���ʼ�غ�
 ���أ�0
 ��⣺����һ���ոմ������ļ������ļ��������Ŀ�ʼ��Ϊ0���������д������ʱ������Ҫ��
       �´�������Ҫ�����ļ��Ŀ�ʼ�أ������ļ����������Ŀ�ʼ��
*************************************************************************************/
#ifdef UPDATE_FILE_SCLUST
UINT8 Update_File_sClust(struct FileInfo *pfi,UINT32 clu) //�����ļ���ʼ��
{
 struct FDI *pfdi;

 just_file=pfi;

 znFAT_Device_Read_Sector(pfi->FDI_Sec,znFAT_Buffer);

 pfdi=(((struct FDIesInSEC *)znFAT_Buffer)->FDIes)+(pfi->nFDI); //�ļ����ļ�Ŀ¼��

 pfi->File_StartClust=clu;

 (pfdi->HighClust)[0]=(UINT8)((clu&0X00FF0000)>>16);
 (pfdi->HighClust)[1]=(UINT8)((clu&0XFF000000)>>24);
 (pfdi->LowClust )[0]=(UINT8)((clu&0X000000FF))    ;
 (pfdi->LowClust )[1]=(UINT8)((clu&0X0000FF00)>>8) ;

 znFAT_Device_Write_Sector(pfi->FDI_Sec,znFAT_Buffer);

 return 0; 
}
#endif

/************************************************************************************
 ���ܣ�����һ������
 �βΣ�pfi:ָ���ļ���Ϣ���ϵ�ָ�� cluster:�����Ŀ�ʼ�� datalen:���ݳ���(�ֽ�)
 ���أ����н�� �ɹ��������
 ��⣺�˺����Ĺ�����Ϊ�����ļ���д�����ݶ�Ԥ�Ƚ����������Ӷ������ṩ����д���Ч��
       ��ʹ����д��Ĺ����У�����ǣ����������Ĳ�������֮�Դ����������εķ��������Ժ�
       ���������߶�����������д������ʹ���ʡ���һ������znFAT����ģʽ���Ļ�������ģ
       ʽ������Ϊ�ļ�Ԥ�Ƚ����ô�����Ȼ����ǵ���������д���ˣ��������������������д
       ����ٶȺ�Ч�ʣ������ݸ��ٴ洢�Ľ����������
*************************************************************************************/
#ifdef CREATE_CLUSTER_CHAIN
UINT8 Create_Cluster_Chain(struct FileInfo *pfi,UINT32 cluster,UINT32 datalen)
{
 UINT32 iSec=0,clu_sec=0,old_clu=cluster,ncluster=0,temp_ncluster=0;
 UINT8 iItem=0,temp=0;
 struct FAT_Sec *pFAT_Sec=(struct FAT_Sec *)znFAT_Buffer; //�����ݻ������׵�ַǿתΪFAT_Sec�ṹ���ָ������

 UINT32 Clu_Size=(pInit_Args->SectorsPerClust*pInit_Args->BytesPerSector); //�ش�С����������ظ�����

 just_file=pfi;

 #ifndef RT_UPDATE_CLUSTER_CHAIN
 #ifdef USE_ALONE_CCCB
 CCCB_To_Alone();
 #endif
 #endif

 ncluster=datalen/Clu_Size;
 if(0!=datalen%Clu_Size) ncluster++; //�������������������������������1

 temp_ncluster=ncluster; //��¼�´����Ĵ���

 if((pInit_Args->Free_nCluster)<ncluster) return ERR_NO_SPACE; //���㹻�Ĵ洢�ռ�

 #ifndef RT_UPDATE_CLUSTER_CHAIN
 #ifndef USE_ALONE_CCCB
 if(pfi!=pcccb_cur_oc) //�����ǰռ��CCCB�Ĳ�������Ҫ�������ļ�
 {
  CCCB_Update_FAT();
  pcccb_cur_oc=pfi;
  (*pcccb_curdev)=Dev_No;
  pcccb_cur_initargs=pInit_Args;
 }
 #endif
 (*pcccb_curval)=cluster;
 #endif

 //===================================������ܲ�������==========================================

 cluster=(pInit_Args->Next_Free_Cluster);

 pfi->File_CurClust=pInit_Args->Next_Free_Cluster; //��ǰ��Ϊ��һ�մ�
 pfi->File_CurSec=SOC(pfi->File_CurClust); //��ǰ�ص�������

 ncluster--;
 //============================

 if(0!=old_clu)
 {
  clu_sec=(old_clu/NITEMSINFATSEC); //����ǰһ�صĴ������ڵ�FAT����
  znFAT_Device_Read_Sector(clu_sec+(pInit_Args->FirstFATSector),znFAT_Buffer);

  #ifdef RT_UPDATE_CLUSTER_CHAIN
  temp=(UINT8)(old_clu%NITEMSINFATSEC);
  (((pFAT_Sec->items)[temp]).Item)[0]=(UINT8)(cluster&0X000000FF)      ;  //��������ǰ��Ĵ�����   
  (((pFAT_Sec->items)[temp]).Item)[1]=(UINT8)((cluster&0X0000FF00)>>8) ;
  (((pFAT_Sec->items)[temp]).Item)[2]=(UINT8)((cluster&0X00FF0000)>>16);
  (((pFAT_Sec->items)[temp]).Item)[3]=(UINT8)((cluster&0XFF000000)>>24);
  #else
  //---------------------------------CCCB�Ĵ���--------------------------------------
  if(0==(*pcccb_counter)) 
  {
   pcccb_buf[(*pcccb_counter)]=(*pcccb_curval);
   (*pcccb_counter)++;
  }

  if(cluster==((*pcccb_curval)+1))
  {
   (*pcccb_curval)++;
  }
  else
  {
   if(((*pcccb_counter)+1)!=CCCB_LEN) //CCCBû�����
   {
    pcccb_buf[(*pcccb_counter)]=(*pcccb_curval);

    (*pcccb_counter)++;
    pcccb_buf[(*pcccb_counter)]=cluster;
    (*pcccb_curval)=cluster;
    (*pcccb_counter)++;
   }
   else //CCCB�������ʱ��Ҫ��CCCB���µ�FAT
   {
	CCCB_Update_FAT();
	#ifndef USE_ALONE_CCCB
	pcccb_cur_oc=pfi;
    (*pcccb_curdev)=Dev_No;
    pcccb_cur_initargs=pInit_Args;
    #endif

    (*pcccb_counter)=0;
    pcccb_buf[(*pcccb_counter)]=pcccb_buf[(*pcccb_counter)+1]=(*pcccb_curval);
    pcccb_buf[(*pcccb_counter)+2]=cluster;
    (*pcccb_curval)=cluster;
    (*pcccb_counter)+=3;
   }
  }
  //---------------------------------CCCB�Ĵ���--------------------------------------
  #endif
 }
 else
 {
  clu_sec=(cluster/NITEMSINFATSEC); //����ǰһ�صĴ������ڵ�FAT����
  znFAT_Device_Read_Sector(clu_sec+(pInit_Args->FirstFATSector),znFAT_Buffer);
  
  #ifndef RT_UPDATE_CLUSTER_CHAIN
  //---------------------------------CCCB�Ĵ���--------------------------------------
  (*pcccb_counter)=0;
  pcccb_buf[(*pcccb_counter)]=cluster;
  (*pcccb_curval)=cluster;
  (*pcccb_counter)++;
  //---------------------------------CCCB�Ĵ���--------------------------------------
  #endif
 }

 #ifdef RT_UPDATE_CLUSTER_CHAIN
 if(clu_sec==(cluster/NITEMSINFATSEC)) //���Ŀ�������ǰһ������ͬһ����
 {
  if(0==ncluster) 
  {
   temp=(UINT8)(cluster%NITEMSINFATSEC);
   (((pFAT_Sec->items)[temp]).Item)[0]=0XFF;  //�������  
   (((pFAT_Sec->items)[temp]).Item)[1]=0XFF;
   (((pFAT_Sec->items)[temp]).Item)[2]=0XFF;
   (((pFAT_Sec->items)[temp]).Item)[3]=0X0F;   

   znFAT_Device_Write_Sector(clu_sec+(pInit_Args->FirstFATSector),znFAT_Buffer);
   znFAT_Device_Write_Sector(clu_sec+(pInit_Args->FirstFATSector+pInit_Args->FATsectors),znFAT_Buffer);
  }
 }
 else //���Ŀ�������ǰһ�����ͬһ����
 {
  znFAT_Device_Write_Sector(clu_sec+(pInit_Args->FirstFATSector),znFAT_Buffer);
  znFAT_Device_Write_Sector(clu_sec+(pInit_Args->FirstFATSector+pInit_Args->FATsectors),znFAT_Buffer);

  clu_sec=(cluster/NITEMSINFATSEC); //����ǰһ�صĴ������ڵ�FAT����
  znFAT_Device_Read_Sector(clu_sec+(pInit_Args->FirstFATSector),znFAT_Buffer);

  if(0==ncluster) 
  {
   temp=(UINT8)(cluster%NITEMSINFATSEC);

   (((pFAT_Sec->items)[temp]).Item)[0]=0XFF;  //�������  
   (((pFAT_Sec->items)[temp]).Item)[1]=0XFF;
   (((pFAT_Sec->items)[temp]).Item)[2]=0XFF;
   (((pFAT_Sec->items)[temp]).Item)[3]=0X0F;   

   znFAT_Device_Write_Sector(clu_sec+(pInit_Args->FirstFATSector),znFAT_Buffer);
   znFAT_Device_Write_Sector(clu_sec+(pInit_Args->FirstFATSector+pInit_Args->FATsectors),znFAT_Buffer);
  }
 }
 #endif
 
 if(0==ncluster) //��������������
 {
  pInit_Args->Free_nCluster-=temp_ncluster; //����ʣ��մ���

  Update_Next_Free_Cluster();

  #ifdef RT_UPDATE_FSINFO
  Update_FSINFO();
  #endif

  return ERR_SUCC;
 }

 old_clu=cluster; 

 //=============================================================================================

 clu_sec=(old_clu/NITEMSINFATSEC);

 if(((UINT8)((cluster%NITEMSINFATSEC)+1))!=((UINT8)NITEMSINFATSEC)) //�����ǰ�Ĵ����������FAT�����е����һ������
 {
  znFAT_Device_Read_Sector(clu_sec+(pInit_Args->FirstFATSector),znFAT_Buffer);

  for(iItem=(UINT8)((cluster%NITEMSINFATSEC)+1);iItem<NITEMSINFATSEC;iItem++) //����ڵ�ǰFAT�����ڵ�ǰ����֮���Ƿ��пմ�
  {
   cluster++; //�غ�����

   if(0==(((pFAT_Sec->items)[iItem]).Item)[0]  //������ֿմ�
   && 0==(((pFAT_Sec->items)[iItem]).Item)[1]
   && 0==(((pFAT_Sec->items)[iItem]).Item)[2]
   && 0==(((pFAT_Sec->items)[iItem]).Item)[3])
   { 
	#ifdef RT_UPDATE_CLUSTER_CHAIN
    temp=(UINT8)(old_clu%NITEMSINFATSEC);

    (((pFAT_Sec->items)[temp]).Item)[0]=(UINT8)(cluster&0X000000FF)      ;  //��������ǰ��Ĵ�����   
    (((pFAT_Sec->items)[temp]).Item)[1]=(UINT8)((cluster&0X0000FF00)>>8) ;
    (((pFAT_Sec->items)[temp]).Item)[2]=(UINT8)((cluster&0X00FF0000)>>16);
    (((pFAT_Sec->items)[temp]).Item)[3]=(UINT8)((cluster&0XFF000000)>>24);
    #else
	//---------------------------------CCCB�Ĵ���--------------------------------------
    if(cluster==((*pcccb_curval)+1))
	{
     (*pcccb_curval)++;
	}
    else
	{
     if(((*pcccb_counter)+1)!=CCCB_LEN) //CCCBû�����
     {
      pcccb_buf[(*pcccb_counter)]=(*pcccb_curval);

      (*pcccb_counter)++;
      pcccb_buf[(*pcccb_counter)]=cluster;
      (*pcccb_curval)=cluster;
      (*pcccb_counter)++;
     }
     else //CCCB�������ʱ��Ҫ��CCCB���µ�FAT�������CCCB���Ա��ٴ�����
     {
	  CCCB_Update_FAT();
	  #ifndef USE_ALONE_CCCB
	  pcccb_cur_oc=pfi;
      (*pcccb_curdev)=Dev_No;
      pcccb_cur_initargs=pInit_Args;
      #endif

	  (*pcccb_counter)=0;
	  pcccb_buf[(*pcccb_counter)]=pcccb_buf[(*pcccb_counter)+1]=(*pcccb_curval);
	  pcccb_buf[(*pcccb_counter)+2]=cluster;
      (*pcccb_curval)=cluster;
	  (*pcccb_counter)+=3;
     }
	}
	//---------------------------------CCCB�Ĵ���--------------------------------------
    #endif
    ncluster--;
    old_clu=cluster;
   }

   if(0==ncluster) 
   {
    #ifdef RT_UPDATE_CLUSTER_CHAIN
	(((pFAT_Sec->items)[iItem]).Item)[0]=0XFF;
	(((pFAT_Sec->items)[iItem]).Item)[1]=0XFF;
	(((pFAT_Sec->items)[iItem]).Item)[2]=0XFF;
	(((pFAT_Sec->items)[iItem]).Item)[3]=0X0F; //FAT�����
    
    znFAT_Device_Write_Sector(clu_sec+(pInit_Args->FirstFATSector),znFAT_Buffer);
    znFAT_Device_Write_Sector(clu_sec+(pInit_Args->FirstFATSector+pInit_Args->FATsectors),znFAT_Buffer);
    #endif
    pInit_Args->Free_nCluster-=temp_ncluster; //����ʣ��մ���
    pInit_Args->Next_Free_Cluster=cluster; 

    Update_Next_Free_Cluster();

    #ifdef RT_UPDATE_FSINFO
    Update_FSINFO();
    #endif

    return ERR_SUCC;
   }
  }
 }
 #ifdef RT_UPDATE_CLUSTER_CHAIN
 znFAT_Device_Write_Sector(clu_sec+(pInit_Args->FirstFATSector),znFAT_Buffer);
 znFAT_Device_Write_Sector(clu_sec+(pInit_Args->FirstFATSector+pInit_Args->FATsectors),znFAT_Buffer);
 #endif
 //=============================================================================================

 for(iSec=(clu_sec+1);iSec<(pInit_Args->FATsectors);iSec++) //�ں����FAT�����м�������
 {
  znFAT_Device_Read_Sector(iSec+(pInit_Args->FirstFATSector),znFAT_Buffer);

  for(iItem=0;iItem<NITEMSINFATSEC;iItem++) //����ڵ�ǰFAT�����ڵ�ǰ����֮���Ƿ��пմ�
  {
   cluster++;

   if(0==(((pFAT_Sec->items)[iItem]).Item)[0]
   && 0==(((pFAT_Sec->items)[iItem]).Item)[1]
   && 0==(((pFAT_Sec->items)[iItem]).Item)[2]
   && 0==(((pFAT_Sec->items)[iItem]).Item)[3])
   {
    #ifdef RT_UPDATE_CLUSTER_CHAIN
	clu_sec=(old_clu/NITEMSINFATSEC);
    temp=(UINT8)(old_clu%NITEMSINFATSEC);

    if(iSec!=clu_sec) //���Ҫ���µĴ������ڵ������뵱ǰ��������ͬһ����
	{                 //����Ҫ�ڸ��´���󣬻ָ��ڲ����������Ϊ��ǰ����
     Modify_FAT(old_clu,cluster);

     znFAT_Device_Read_Sector(iSec+(pInit_Args->FirstFATSector),znFAT_Buffer);	 
	}
	else //�����Ҫ���µĴ������������뵱ǰ����Ϊͬһ��������ֻ��Ҫ���ڲ������н��и���
	{    //�������������л�д�������ṩ���������ٶȵĺ���˼��
     (((pFAT_Sec->items)[temp]).Item)[0]=(UINT8)(cluster&0X000000FF)      ;  //��������ǰ��Ĵ�����   
     (((pFAT_Sec->items)[temp]).Item)[1]=(UINT8)((cluster&0X0000FF00)>>8) ;
     (((pFAT_Sec->items)[temp]).Item)[2]=(UINT8)((cluster&0X00FF0000)>>16);
     (((pFAT_Sec->items)[temp]).Item)[3]=(UINT8)((cluster&0XFF000000)>>24);
	}
    #else
	//---------------------------------CCCB�Ĵ���--------------------------------------
    if(cluster==((*pcccb_curval)+1))
	{
     (*pcccb_curval)++;
	}
    else
	{
     if(((*pcccb_counter)+1)!=CCCB_LEN) //CCCBû�����
     {
      pcccb_buf[(*pcccb_counter)]=(*pcccb_curval);

      (*pcccb_counter)++;
      pcccb_buf[(*pcccb_counter)]=cluster;
      (*pcccb_curval)=cluster;
      (*pcccb_counter)++;
     }
     else //CCCBû���������ʱ��Ҫ��CCCB���µ�FAT
     {
	  CCCB_Update_FAT();
	  #ifndef USE_ALONE_CCCB
	  pcccb_cur_oc=pfi;
      (*pcccb_curdev)=Dev_No;
      pcccb_cur_initargs=pInit_Args;
      #endif

	  (*pcccb_counter)=0;
	  pcccb_buf[(*pcccb_counter)]=pcccb_buf[(*pcccb_counter)+1]=(*pcccb_curval);
	  pcccb_buf[(*pcccb_counter)+2]=cluster;
      (*pcccb_curval)=cluster;
	  (*pcccb_counter)+=3;
     }
	}
	//---------------------------------CCCB�Ĵ���--------------------------------------
    #endif
	ncluster--;
	old_clu=cluster;
   }

   if(0==ncluster) 
   {
	#ifdef RT_UPDATE_CLUSTER_CHAIN
	clu_sec=(old_clu/NITEMSINFATSEC);

	(((pFAT_Sec->items)[iItem]).Item)[0]=0XFF;
	(((pFAT_Sec->items)[iItem]).Item)[1]=0XFF;
	(((pFAT_Sec->items)[iItem]).Item)[2]=0XFF;
	(((pFAT_Sec->items)[iItem]).Item)[3]=0X0F; //FAT�����

    znFAT_Device_Write_Sector(clu_sec+(pInit_Args->FirstFATSector),znFAT_Buffer);
    znFAT_Device_Write_Sector(clu_sec+(pInit_Args->FirstFATSector+pInit_Args->FATsectors),znFAT_Buffer);
    #endif
    pInit_Args->Free_nCluster-=temp_ncluster; //����ʣ��մ���
    pInit_Args->Next_Free_Cluster=cluster; 

    Update_Next_Free_Cluster();

    #ifdef RT_UPDATE_FSINFO
    Update_FSINFO();
    #endif

    return ERR_SUCC;
   }
  }
  #ifdef RT_UPDATE_CLUSTER_CHAIN
  znFAT_Device_Write_Sector(iSec+(pInit_Args->FirstFATSector),znFAT_Buffer);
  znFAT_Device_Write_Sector(iSec+(pInit_Args->FirstFATSector+pInit_Args->FATsectors),znFAT_Buffer);
  #endif
 } 

 return ERR_FAIL;
}
#endif

/************************************************************************************
 ���ܣ����ļ�����λ�ÿ�ʼд������
 �βΣ�pfi:ָ���ļ���Ϣ���ϵ�ָ�� len:Ҫд������ݳ��� pbuf:ָ�����ݻ�����
 ���أ�ʵ��д���������
 ��⣺znFAT�����ļ�д����������׷�ӷ�ʽд��ģ������Ǵ��ļ���ĩβ���д�����ݡ�д��
       ���ݵ��������ļ���ǰ��λ�ò��������ǵ��ļ�Ϊ���ļ�����������Ϊ0����������Ϊ��
       ��С������������ʱ�ļ���λ�ò�����������ʵ����ļ����ݵĵ�ǰλ�ã��ļ�Ϊ��ʱ
       ��ǰ��Ϊ0������ʱ��ǰ��Ϊ���һ����Ч�أ��������������znFAT�еġ���ء����˺�
       ��ר�����Դ������������
*************************************************************************************/
#ifdef WRITEDATA_FROM_NCLUSTER
UINT32 WriteData_From_nCluster(struct FileInfo *pfi,UINT32 len,UINT8 *pbuf)
{
 UINT32 CluSize=((pInit_Args->BytesPerSector)*(pInit_Args->SectorsPerClust)); //����ش�С����������ظ�����
 UINT32 temp=len/CluSize;//����Ҫд�������������������
 UINT32 iClu=0,start_clu=0,end_clu=0,next_clu=0; 
 UINT32 temp1=0,temp2=0;
 UINT8 res=0;

 #ifdef USE_EXCHANGE_BUFFER
 #ifndef USE_ALONE_EXB
 UINT8 old_devno=Dev_No;
 #else
 pexb_buf=(pfi->exb_buf);
 #endif
 #endif

 just_file=pfi;

 #ifndef RT_UPDATE_CLUSTER_CHAIN
 get_next_cluster_in_cccb=1;
 #ifdef USE_ALONE_CCCB
 CCCB_To_Alone();
 #endif
 #endif

 if(0==len) return 0; //���Ҫд������ݳ���Ϊ0����ֱ�ӷ���

 if(0==pfi->File_CurClust) //����ǿ��ļ�����ǰ��Ϊ0������δΪ������
 {
  pfi->File_StartClust=pInit_Args->Next_Free_Cluster;
  Update_File_sClust(pfi,pInit_Args->Next_Free_Cluster);
 }

 res=Create_Cluster_Chain(pfi,pfi->File_CurClust,len); //Ϊ��������Ԥ������
 if(res) return res;

 if(0==temp) //���Ҫд�����������һ����
 {
  temp=len/(pInit_Args->BytesPerSector); //Ҫд������ݹ�����������
  znFAT_Device_Write_nSector(temp,SOC(pfi->File_CurClust),pbuf);
  pfi->File_CurSec+=temp;
  pbuf+=(temp*(pInit_Args->BytesPerSector));

  temp=len%(pInit_Args->BytesPerSector);
  if(0!=temp) //��������Ҫд�룬�������������һ������
  {
   #ifndef USE_EXCHANGE_BUFFER
   Memory_Copy(znFAT_Buffer,pbuf,temp); //�Ѳ���һ�����������ȷ����ڲ���������
   znFAT_Device_Write_Sector(pfi->File_CurSec,znFAT_Buffer); //���ڲ��������е�����д��������
                                                         //���ֱ��ʹ��pbuf������Դ��д�룬����һ������
                                                         //�Ӷ�������ڴ����,�������
   #else
   #ifndef USE_ALONE_EXB
   if(Dev_No!=sexb_cur_dev) //������ڲ������豸���ǵ�ǰռ��EXB���豸
   {
	if(0!=sexb_cur_sec) //���EXB����ռ��
	{
	 Dev_No=sexb_cur_dev;
	 znFAT_Device_Write_Sector(sexb_cur_sec,pexb_buf); //���EXB�л������ݣ����Ƚ���Щ���ݻ�д������Ӧ������

     Dev_No=old_devno;
	}
   }
   else //������ڲ������豸���ǵ�ǰռ��EXB���豸
   {
	if(sexb_cur_sec!=(pfi->File_CurSec)) //ռ��EXB���������ǵ�ǰҪ����������
	{
	 if(0!=sexb_cur_sec) //���EXB����ռ��
	 {
	  znFAT_Device_Write_Sector(sexb_cur_sec,pexb_buf); //���EXB�л������ݣ����Ƚ���Щ���ݻ�д������Ӧ������
	 }	   
	}
   }
   #endif 

   Memory_Copy(pexb_buf,pbuf,temp);

   #ifndef USE_ALONE_EXB
   sexb_cur_sec=pfi->File_CurSec;   //��¼��ǰ��������������ӳ��������ַ
   sexb_cur_dev=Dev_No; //��¼��ǰ�����������������ڵ��豸��
   psexb_cur_oc=pfi; //��¼EXB�л�������������ĸ��ļ�
   #else
   just_file->exb_cursec=pfi->File_CurSec;
   #endif
   #endif
	  
   pfi->File_CurPos=(UINT16)temp;   
  }
 }
 else
 {
  //����������ضΣ��Ծ����ܵ�ʹ�ö�����д���������������д���ٶ�
  //start_clu��end_clu���ڼ�¼�����ضε�ʼĩ
  start_clu=end_clu=pfi->File_CurClust; 

  for(iClu=1;iClu<temp;iClu++)
  {
   next_clu=Get_Next_Cluster(end_clu);

   if((next_clu-1)==end_clu) //������������٣�������
   {
    end_clu=next_clu;
   }
   else //��������ز����٣������������ϵ�
   {
    znFAT_Device_Write_nSector(((end_clu-start_clu+1)*(pInit_Args->SectorsPerClust)),SOC(start_clu),pbuf);
	pbuf+=((end_clu-start_clu+1)*CluSize);
    start_clu=next_clu;
    end_clu=next_clu;
   }
  }

  temp1=(len%CluSize)/(pInit_Args->BytesPerSector); //ʣ�����ݹ�����������
  temp2=Get_Next_Cluster(end_clu);
  temp=(end_clu-start_clu+1)*(pInit_Args->SectorsPerClust);
 
  if(!IS_END_CLU(temp2)) //�����һ�ز��ǽ����أ������滹������Ҫд��
  {
   if((temp2-1)==end_clu) //������һ�����е�ʣ��������ǰ������������ض���������
   {
    znFAT_Device_Write_nSector((temp+temp1),SOC(start_clu),pbuf);
    pbuf+=((temp+temp1)*(pInit_Args->BytesPerSector));
   }
   else
   {
    znFAT_Device_Write_nSector(((end_clu-start_clu+1)*(pInit_Args->SectorsPerClust)),SOC(start_clu),pbuf); 
    pbuf+=(temp*(pInit_Args->BytesPerSector));
    znFAT_Device_Write_nSector(temp1,SOC(temp2),pbuf);
    pbuf+=(temp1*(pInit_Args->BytesPerSector));
   }

   pfi->File_CurClust=temp2;
   pfi->File_CurSec=(SOC(temp2)+temp1);
   //=======================================================================================
   temp=len%(pInit_Args->BytesPerSector);
   if(0!=temp) //��������Ҫд��
   {
	#ifndef USE_EXCHANGE_BUFFER
    Memory_Copy(znFAT_Buffer,pbuf,temp); //�Ѳ���һ�����������ȷ����ڲ���������
    znFAT_Device_Write_Sector(pfi->File_CurSec,znFAT_Buffer); //���ڲ��������е�����д��������
                                                         //���ֱ��ʹ��pbuf������Դ��д�룬����һ������
                                                         //�Ӷ�������ڴ����,�������
    #else
    #ifndef USE_ALONE_EXB
    if(Dev_No!=sexb_cur_dev) //������ڲ������豸���ǵ�ǰռ��EXB���豸
    {
	 if(0!=sexb_cur_sec) //���EXB����ռ��
	 {
	  Dev_No=sexb_cur_dev;
	  znFAT_Device_Write_Sector(sexb_cur_sec,pexb_buf); //���EXB�л������ݣ����Ƚ���Щ���ݻ�д������Ӧ������

      Dev_No=old_devno;
	 }
    }
    else //������ڲ������豸���ǵ�ǰռ��EXB���豸
    {
	 if(sexb_cur_sec!=(pfi->File_CurSec)) //ռ��EXB���������ǵ�ǰҪ����������
	 {
	  if(0!=sexb_cur_sec) //���EXB����ռ��
	  {
	   znFAT_Device_Write_Sector(sexb_cur_sec,pexb_buf); //���EXB�л������ݣ����Ƚ���Щ���ݻ�д������Ӧ������
	  }	   
	 }
    }
    #endif

    Memory_Copy(pexb_buf,pbuf,temp);

    #ifndef USE_ALONE_EXB
    sexb_cur_sec=pfi->File_CurSec;
	sexb_cur_dev=Dev_No; //��¼��ǰ�����������������ڵ��豸��
	psexb_cur_oc=pfi; //��¼EXB�л�������������ĸ��ļ�
    #else
    just_file->exb_cursec=pfi->File_CurSec;
    #endif
    #endif   
	
	pfi->File_CurPos=(UINT16)temp;
   }
  }
  else //�����һ����Ϊ�����أ�����������������Ҫд��
  {
   znFAT_Device_Write_nSector(temp,SOC(start_clu),pbuf);
   pbuf+=((temp+temp1)*(pInit_Args->BytesPerSector)); 

   pfi->File_CurClust=end_clu;
   pfi->File_CurSec=SOC(end_clu);
  }
 }

 //========================���ϴ������ڴ������������������ݵ�д�룬������������������������========================
 #ifdef RT_UPDATE_FSINFO
 Update_FSINFO();
 #endif

 pfi->File_CurOffset+=len;

 return len;
} 
#endif

/************************************************************************************
 ���ܣ����ļ���д������
 �βΣ�pfi:ָ���ļ���Ϣ���ϵ�ָ�� len:Ҫд��������� pbuf:ָ�����ݻ�������ָ��
 ���أ�ʵ�����ļ���д���������  ����ļ���С�Ѿ��ﵽFAT32�������Ƶļ��ޣ�����-2
 ��⣺�������չ�ʹ���ߵ��õ��ļ�����д�뺯���������Ǵ��ļ���ĩβ���д�����ݣ�������
       ����׷�ӵķ�ʽ��д��ġ�������д��֮��Ҫ��ʱ�ĸ����ļ���С������д������ݽ�
       ���ɼ������Դ�RT_UPDATE_FILESIZE��������ʵʱ�ļ���С���¹��ܣ���ÿ��д������
       znFAT����ȥ�����ļ���С�����ֹ�����ʽ�������м�ͻȻ�ϵ��������û�й�ϵ������
       ��֤�ļ���С��ʵ��ӳ�ļ�����Ч�������������ַ�ʽʹ����д����ٶȺ�Ч�ʱ�������
       ��ʹ�ô˺꣬����Ҫʹ����������д�����ݵĲ������֮�󣬵��ø����ļ���С�ĺ�����
*************************************************************************************/
#ifdef ZNFAT_WRITEDATA
UINT32 znFAT_WriteData(struct FileInfo *pfi,UINT32 len,UINT8 *pbuf)
{
 UINT32 temp=0,temp1=0,len_temp=len;
 UINT32 Cluster_Size=((pInit_Args->BytesPerSector)*(pInit_Args->SectorsPerClust));

 #ifdef USE_EXCHANGE_BUFFER
 #ifndef USE_ALONE_EXB
 UINT8 old_devno=Dev_No;
 #else
 pexb_buf=(pfi->exb_buf);
 #endif
 #endif

 just_file=pfi;

 #ifndef RT_UPDATE_CLUSTER_CHAIN
 get_next_cluster_in_cccb=1;
 #ifdef USE_ALONE_CCCB
 CCCB_To_Alone();
 #endif
 #endif

 if(0==len) return 0; //���Ҫд������ݳ���Ϊ0����ֱ�ӷ���0

 if(len>(0XFFFFFFFF-pfi->File_Size)) return (UINT32)-2; //�ļ���С��д�����ݺ󽫷����������
 
 znFAT_Seek(pfi,pfi->File_Size); //�ļ����ݶ�λ���ļ�ĩβ���ļ�λ�������Ϣ�漴�ı�

 //������ʣ��ռ��Ƿ���
 if((pfi->File_CurOffset%Cluster_Size)!=0)
 {
  temp=((pInit_Args->BytesPerSector)-(pfi->File_CurPos))+((LAST_SEC_OF_CLU(pfi->File_CurClust))-(pfi->File_CurSec))*(Cluster_Size);
  //��ǰ��ʣ��������

  if(len>temp) //���Ҫд�������������temp����˵����Ȼ�ᳬ����ǰ�أ�Ϊ����չ�մ�
  {
   temp1=(len-temp)/(Cluster_Size);
   if((len-temp)%(Cluster_Size)) temp1++; //������Ҫ���ٸ��մ�

   if(temp1>(pInit_Args->Free_nCluster)) return ((UINT32)-1); //�ռ䲻��
  }

 }
 else
 {
  temp1=len/(Cluster_Size);
  if(len%(Cluster_Size)) temp1++; //������Ҫ���ٸ��մ�  
  if(temp1>(pInit_Args->Free_nCluster)) return ((UINT32)-1); //�ռ䲻��
 }

 //===========================================================================================================

 temp=((pInit_Args->BytesPerSector)-(pfi->File_CurPos)); //���㸳����ʱ��������������ظ�����

 if((pfi->File_CurOffset%Cluster_Size)!=0)
 {
  if(len<=temp) //Ҫд�������С�ڵ��ڵ�ǰ����ʣ��������
  {
   #ifndef USE_EXCHANGE_BUFFER
   znFAT_Device_Read_Sector(pfi->File_CurSec,znFAT_Buffer); //��ȡ��ǰ�������ݣ��Ա�������������ƴ��
   Memory_Copy(znFAT_Buffer+pfi->File_CurPos,pbuf,len); //��������ƴ��
   znFAT_Device_Write_Sector(pfi->File_CurSec,znFAT_Buffer); //��д��������
   #endif

   if(len==temp) //���Ҫд�����������ռ����ǰ����
   {
	#ifdef USE_EXCHANGE_BUFFER
	if(0!=pfi->File_CurPos) 
	{
     #ifndef USE_ALONE_EXB
	 if(Dev_No!=sexb_cur_dev) //������ڲ������豸���ǵ�ǰռ��EXB���豸
	 {
	  if(0!=sexb_cur_sec) //���EXB����ռ��
	  {
	   Dev_No=sexb_cur_dev;
	   znFAT_Device_Write_Sector(sexb_cur_sec,pexb_buf); //���EXB�л������ݣ����Ƚ���Щ���ݻ�д������Ӧ������

       Dev_No=old_devno;
	  }
	  znFAT_Device_Read_Sector(pfi->File_CurSec,pexb_buf); 
	 }
	 else //������ڲ������豸���ǵ�ǰռ��EXB���豸
	 {
	  if(sexb_cur_sec!=(pfi->File_CurSec)) //ռ��EXB���������ǵ�ǰҪ����������
	  {
	   if(0!=sexb_cur_sec) //���EXB����ռ��
	   {
	    znFAT_Device_Write_Sector(sexb_cur_sec,pexb_buf); //���EXB�л������ݣ����Ƚ���Щ���ݻ�д������Ӧ������

	    znFAT_Device_Read_Sector(pfi->File_CurSec,pexb_buf); 
	   }	   
	  }
	 }

     #else
	 if(0==(just_file->exb_cursec)) 
	 {
	  znFAT_Device_Read_Sector(pfi->File_CurSec,pexb_buf);
	 }
     #endif

	 Memory_Copy(pexb_buf+pfi->File_CurPos,pbuf,len); //��������ƴ��

     znFAT_Device_Write_Sector(pfi->File_CurSec,pexb_buf); //��д��������
     
     #ifndef USE_ALONE_EXB
	 sexb_cur_sec=0; //ÿ��EXB�е����������������ݻ�д֮�����Ǳ���Ϊ�����ٱ�ռ����
	 sexb_cur_dev=(UINT8)(-1); //EXB�ĵ�ǰ�豸����Ϊ�գ�����ȡ-1�϶���Ϊ�գ�Ϊ������Ч�豸��0������
	 psexb_cur_oc=(struct FileInfo *)0; //��ʱEXB�����κ��ļ�����
     #else
	 (just_file->exb_cursec)=0; //�ļ��Ķ���EXBδռ��
     #endif
	}
	else
	{
	 znFAT_Device_Write_Sector(pfi->File_CurSec,pbuf); //��д��������
	}
    #endif

	if(IS_END_SEC_OF_CLU(pfi->File_CurSec,pfi->File_CurClust)) //�����ǰ�����ǵ�ǰ�ص����һ������
	{
	 pfi->File_CurSec=SOC(pfi->File_CurClust); //���µ�ǰ��������ʵ��Ч��Ϊ�˹���
	}
	else //��ǰ�������ǵ�ǰ�ص��������
	{
	 pfi->File_CurSec++;
	}

	pfi->File_CurPos=0;
	pfi->File_CurOffset+=len; //���µ�ǰƫ����
	pfi->File_Size+=len; //�����ļ���С
     
    #ifdef RT_UPDATE_FILESIZE
	Update_File_Size(pfi); //���ļ�Ŀ¼���е��ļ���С�ֶ�
    #endif

	return len;
   }
   else//lenС�ڵ�ǰ����ʣ��������
   {  
	#ifdef USE_EXCHANGE_BUFFER
    #ifndef USE_ALONE_EXB
	if(Dev_No!=sexb_cur_dev) //������ڲ������豸���ǵ�ǰռ��EXB���豸
	{
	 if(0!=sexb_cur_sec) //���EXB����ռ��
	 {
	  Dev_No=sexb_cur_dev;
	  znFAT_Device_Write_Sector(sexb_cur_sec,pexb_buf); //���EXB�л������ݣ����Ƚ���Щ���ݻ�д������Ӧ������

      Dev_No=old_devno; 
	 }
	 znFAT_Device_Read_Sector(pfi->File_CurSec,pexb_buf);
	}
	else //������ڲ������豸���ǵ�ǰռ��EXB���豸
	{
	 if(sexb_cur_sec!=(pfi->File_CurSec)) //ռ��EXB���������ǵ�ǰҪ����������
	 {
	  if(0!=sexb_cur_sec) //���EXB����ռ��
	  {
	   znFAT_Device_Write_Sector(sexb_cur_sec,pexb_buf); //���EXB�л������ݣ����Ƚ���Щ���ݻ�д������Ӧ������

	   znFAT_Device_Read_Sector(pfi->File_CurSec,pexb_buf); 
	  }	   
	 }
	}
    #else
	if((0==(just_file->exb_cursec)) && (0!=(pfi->File_CurPos))) //��ǰ�ļ��Ķ���EXBδ��ռ�ã����ļ���ǰ������ƫ�Ʋ�Ϊ0�����Ϊ0��û��Ҫ������
	{
	 znFAT_Device_Read_Sector(pfi->File_CurSec,pexb_buf);
	}
    #endif

	Memory_Copy(pexb_buf+pfi->File_CurPos,pbuf,len); //��������ƴ��

    #ifndef USE_ALONE_EXB
	sexb_cur_dev=Dev_No;
	sexb_cur_sec=pfi->File_CurSec;
	psexb_cur_oc=pfi; //��¼EXB�л�������������ĸ��ļ�
    #else
	(just_file->exb_cursec)=pfi->File_CurSec;
    #endif
    #endif
    //znFAT_Device_Write_Sector(pfi->File_CurSec,ex_buf); //��д��������

    pfi->File_CurPos+=(UINT16)len;
    pfi->File_CurOffset+=len; //���µ�ǰƫ����
    pfi->File_Size+=len; //�����ļ���С 
	
    #ifdef RT_UPDATE_FILESIZE
	Update_File_Size(pfi); //���ļ�Ŀ¼���е��ļ���С�ֶ�
    #endif

	return len;	 
   }
  }
  else 
  {
   #ifndef USE_EXCHANGE_BUFFER
   znFAT_Device_Read_Sector(pfi->File_CurSec,znFAT_Buffer); //��ȡ��ǰ����
   Memory_Copy(znFAT_Buffer+pfi->File_CurPos,pbuf,temp); //��������ƴ��
   znFAT_Device_Write_Sector(pfi->File_CurSec,znFAT_Buffer); //��д����
   #else

   if(0!=pfi->File_CurPos) 
   {
    #ifndef USE_ALONE_EXB
	if(Dev_No!=sexb_cur_dev) //������ڲ������豸���ǵ�ǰռ��EXB���豸
	{
	 if(0!=sexb_cur_sec) //���EXB����ռ��
	 {
	  Dev_No=sexb_cur_dev;
	  znFAT_Device_Write_Sector(sexb_cur_sec,pexb_buf); //���EXB�л������ݣ����Ƚ���Щ���ݻ�д������Ӧ������

      Dev_No=old_devno;
	 }
	 znFAT_Device_Read_Sector(pfi->File_CurSec,pexb_buf); 
	}
	else //������ڲ������豸���ǵ�ǰռ��EXB���豸
	{
	 if(sexb_cur_sec!=(pfi->File_CurSec)) //ռ��EXB���������ǵ�ǰҪ����������
	 {
	  if(0!=sexb_cur_sec) //���EXB����ռ��
	  {
	   znFAT_Device_Write_Sector(sexb_cur_sec,pexb_buf); //���EXB�л������ݣ����Ƚ���Щ���ݻ�д������Ӧ������

	   znFAT_Device_Read_Sector(pfi->File_CurSec,pexb_buf); 
	  }	   
	 }
	}
    #else
	if(0==(just_file->exb_cursec)) 
	{
	 znFAT_Device_Read_Sector(pfi->File_CurSec,pexb_buf);
	}
    #endif

	Memory_Copy(pexb_buf+pfi->File_CurPos,pbuf,temp); //��������ƴ��

    znFAT_Device_Write_Sector(pfi->File_CurSec,pexb_buf); //��д��������

    #ifndef USE_ALONE_EXB
	sexb_cur_sec=0; //ÿ��EXB�е����������������ݻ�д֮�����Ǳ���Ϊ�����ٱ�ռ����
	sexb_cur_dev=(UINT8)(-1); //EXB�ĵ�ǰ�豸����Ϊ�գ�����ȡ-1�϶���Ϊ�գ�Ϊ������Ч�豸��0������
	psexb_cur_oc=(struct FileInfo *)0;
    #else
	(just_file->exb_cursec)=0; //��ǰ�ļ�����EXBδ��ռ��
    #endif
   }
   else //�����ǰλ����0λ�ã���ֱ��д����
   {
	znFAT_Device_Write_Sector(pfi->File_CurSec,pbuf); //��д��������
   }
   #endif

   len_temp-=temp;
   pbuf+=temp;

   if(!(IS_END_SEC_OF_CLU(pfi->File_CurSec,pfi->File_CurClust))) //�����ǰ�������ǵ�ǰ�ص����һ������
   {
	pfi->File_CurSec++;
	pfi->File_CurPos=0;

    pfi->File_CurOffset+=temp;

    temp=(LAST_SEC_OF_CLU(pfi->File_CurClust)-(pfi->File_CurSec)+1)*(pInit_Args->BytesPerSector);//��ǰ���е�ʣ����������������

    if(len_temp<=temp) //���Ҫд���������С�ڵ��ڵ�ǰ���е�ʣ��������������
	{
	 temp1=len_temp/(pInit_Args->BytesPerSector); //����Ҫд���������������������

	 znFAT_Device_Write_nSector(temp1,pfi->File_CurSec,pbuf);
	 pbuf+=((pInit_Args->BytesPerSector)*temp1);

	 if(len_temp==temp) //�������д����ǰ��
	 {
	  pfi->File_CurSec=SOC(pfi->File_CurClust); //���
	  pfi->File_CurPos=0;

	  pfi->File_CurOffset+=len_temp;
	  pfi->File_Size+=len;

      #ifdef RT_UPDATE_FILESIZE
   	  Update_File_Size(pfi); //���ļ�Ŀ¼���е��ļ���С�ֶ�
      #endif

	  return len;
	 }
	 else
	 {
	  pfi->File_CurSec+=temp1;
	  pfi->File_CurPos=(UINT16)(len_temp%(pInit_Args->BytesPerSector));

	  if(pfi->File_CurPos) //����Ҫд������,���������ֽ�����
	  {
       #ifndef USE_EXCHANGE_BUFFER
	   Memory_Copy(znFAT_Buffer,pbuf,pfi->File_CurPos);
	   znFAT_Device_Write_Sector(pfi->File_CurSec,znFAT_Buffer);
       #else
       #ifndef USE_ALONE_EXB
       if(Dev_No!=sexb_cur_dev) //������ڲ������豸���ǵ�ǰռ��EXB���豸
	   {
	    if(0!=sexb_cur_sec) //���EXB����ռ��
		{
	     Dev_No=sexb_cur_dev;
	     znFAT_Device_Write_Sector(sexb_cur_sec,pexb_buf); //���EXB�л������ݣ����Ƚ���Щ���ݻ�д������Ӧ������

         Dev_No=old_devno;
		}
	   }
       else //������ڲ������豸���ǵ�ǰռ��EXB���豸
	   {
	    if(sexb_cur_sec!=(pfi->File_CurSec)) //ռ��EXB���������ǵ�ǰҪ����������
		{
	     if(0!=sexb_cur_sec) //���EXB����ռ��
		 {
	      znFAT_Device_Write_Sector(sexb_cur_sec,pexb_buf); //���EXB�л������ݣ����Ƚ���Щ���ݻ�д������Ӧ������
		 }	   
		}
	   }
       #endif

	   Memory_Copy(pexb_buf,pbuf,pfi->File_CurPos);

       #ifndef USE_ALONE_EXB
	   sexb_cur_sec=pfi->File_CurSec;
	   sexb_cur_dev=Dev_No;
	   psexb_cur_oc=pfi; //��¼EXB�л�������������ĸ��ļ�
       #else
	   just_file->exb_cursec=pfi->File_CurSec;
       #endif
       #endif
	  }

      pfi->File_CurOffset+=len_temp;
	  pfi->File_Size+=len;

      #ifdef RT_UPDATE_FILESIZE
  	  Update_File_Size(pfi); //���ļ�Ŀ¼���е��ļ���С�ֶ�
      #endif

	  return len;
	 }
	}
	else
	{
	 temp1=temp/(pInit_Args->BytesPerSector);

	 znFAT_Device_Write_nSector(temp1,pfi->File_CurSec,pbuf);
	 pbuf+=temp;

	 len_temp-=temp;

	 pfi->File_CurSec=SOC(pfi->File_CurClust);
	 pfi->File_CurPos=0;

	 pfi->File_CurOffset+=temp;	 
	}
   }
   else //��ǰ�����ǵ�ǰ�����һ������
   {
	pfi->File_CurSec=SOC(pfi->File_CurClust);
	pfi->File_CurPos=0;

	pfi->File_CurOffset+=temp;	   
   }   
  }
 }

 //����ļ��ĵ�ǰƫ�����Ǵش�С������������
 //ֱ�ӽ�����ļ���ʼλ�û�����λ��д���ݵĽ׶�
 WriteData_From_nCluster(pfi,len_temp,pbuf); //�ӿ��ļ���ʼλ�û�����λ��д���ݣ���ʱ����һ�֡������ء������
                                              //��������£���������ֹͣ��ĩ�ص�ĩβ���ǿ��ļ���û�����ݣ�ͨ
                                              //���ļ���Ϣ�����м�¼���ļ�λ����ϢӦ������һ�ص��ʼλ�ã�
                                              //�������ʱ����һ����û�б����䣬���ļ�ĩ�ص�FAT�����¼����
                                              //0XFFFFFF0F������������ǣ�����˴�ʱ���ļ�λ����Ϣ����Ч��
                                              //znFAT������Լ��:�������ء�����£����ļ���ǰ��Ϊ0����ĩ��ĩβ
                                              //ʱ����ǰ��Ϊĩ�ء�����Լ������znFAT������д�빦���ڡ������ء�
                                              //����µ���ȷ��
 pfi->File_Size+=len;
 
 #ifdef RT_UPDATE_FILESIZE
 Update_File_Size(pfi); //���ļ�Ŀ¼���е��ļ���С�ֶ�
 #endif

 return len;
}
#endif

/************************************************************************************
 ���ܣ��ļ����ݽض�
 �βΣ�pfi:ָ���ļ���Ϣ���ϵ�ָ�� offset:Ҫ�������ݽضϵ���ʼλ��
 ���أ����н�� �ɹ���ʧ�� 
 ��⣺�˺������ڽ��ļ������ݴ�offsetλ�ý��нضϣ����Ӵ�λ�ú�������ݾ���ɾ�������
       ָ����offset������ڵ����ļ��Ĵ�С����ֱ�ӷ��ش���
*************************************************************************************/
#ifdef ZNFAT_DUMP_DATA
UINT8 znFAT_Dump_Data(struct FileInfo *pfi,UINT32 offset)
{
 just_file=pfi;

 #ifndef RT_UPDATE_CLUSTER_CHAIN
 #ifdef USE_ALONE_CCCB
 CCCB_To_Alone();
 #endif
 #endif

 if(offset>=(pfi->File_Size)) //Ҫ�ضϵ�λ�ó����ļ���Χ
 {
  return ERR_FAIL;
 }

 znFAT_Seek(pfi,offset); //��λ��Ҫ�ضϵ�λ��

 #ifndef RT_UPDATE_CLUSTER_CHAIN //����֮ǰ���Ȱ�CCCB�еĴ����θ��µ�FAT
 #ifndef USE_ALONE_CCCB
 if(pfi==pcccb_cur_oc) 
 #endif
 {
  CCCB_Update_FAT();
 }
 #endif

 Destroy_FAT_Chain(pfi->File_CurClust); //���ٴ���

 if(offset>0)
 {
  Modify_FAT(pfi->File_CurClust,0X0FFFFFFF); //�������
 }

 pfi->File_Size=offset; //�����ļ���С

 #ifdef RT_UPDATE_FILESIZE
 Update_File_Size(pfi); //���ļ�Ŀ¼���е��ļ���С�ֶ�
 #endif

 if(0==pfi->File_Size) Update_File_sClust(pfi,0); //����ļ���СΪ0�������ļ���ʼ��Ϊ0

 #ifdef RT_UPDATE_FSINFO
 Update_FSINFO();
 #endif

 return ERR_SUCC;
}
#endif

#ifdef ZNFAT_MODIFY_DATA
UINT32 znFAT_Modify_Data(struct FileInfo *pfi,UINT32 offset,UINT32 len,UINT8 *app_Buffer)
{
 UINT32 temp_len=0,temp=0,nsec=0,iClu=0,start_clu=0,end_clu=0,next_clu=0,temp2=0,temp1=0;
 UINT32 Cluster_Size=((pInit_Args->BytesPerSector)*(pInit_Args->SectorsPerClust));

 if(offset>=(pfi->File_Size)) return ERR_MD_POS_OVER_FSIZE; //���Ҫ�޸ĵ�����λ�ó����ļ���С

 if((offset+len)>=(pfi->File_Size))  //��offsetλ��Ҫ��д�����ݳ��ȳ����ļ����ȣ���ȡʵ�ʳ���
 {
  len=(pfi->File_Size)-offset;
  (pfi->File_IsEOF)=1; //�������һ�����޸ĵ��ļ���ĩβ���ļ����ﵽ����λ��
 }

 temp_len=len; 

 znFAT_Seek(pfi,offset); //��λ�ļ�offsetλ�ã���λ֮���ļ���Ϣ����ر������õ�����

 //======================================���濪ʼ�����ݽ��и�д==================================
 just_file=pfi;

 #ifndef RT_UPDATE_CLUSTER_CHAIN //���ʹ����CCCB���壬����Ҫ����CCCB��Ѱ�صı��
 get_next_cluster_in_cccb=1;
 #ifdef USE_ALONE_CCCB //���ʹ���˶���CCCB���壬���CCCB�򵽵�ǰ�������ļ���CCCB
 CCCB_To_Alone();
 #endif
 #endif 
 //==============================================================================================
 if(((pfi->File_CurOffset)%Cluster_Size)!=0) //�����ǰλ�÷Ǵش�С�������������ڴؿ�ʼλ��
 {
  if(((pfi->File_CurOffset)%(pInit_Args->BytesPerSector))!=0) //�����ǰλ�÷���������С��������������������ʼλ��
  {
   if(len<=((pInit_Args->BytesPerSector)-(pfi->File_CurPos))) //���Ҫ�޸ĵ����ݳ���С�ڵ����ļ���ǰ����ʣ��������
   {
	//���������EXB���壬��˲������������ݣ����ܲ��������������У�����EXB������
    #ifdef USE_EXCHANGE_BUFFER //���ʹ����EXB����
    #ifndef USE_ALONE_EXB //���û�����ö���EXB���壬�������˹���EXB����
    if((psexb_cur_oc==pfi)&&(sexb_cur_dev==Dev_No)&&(sexb_cur_sec==(pfi->File_CurSec))) //�����ʱ�ļ���ռ�ù���EXB��ͬʱ��ǰ�ļ����ڵ��豸�������͹���EXB���
	{
	 Memory_Copy(pexb_buf+(pfi->File_CurPos),app_Buffer,len); //�޸��ڹ���EXB�е�����
	}
	else //����Ҫ�޸ĵ����ݲ���EXB�У���������������
	{
	 znFAT_Device_Read_Sector((pfi->File_CurSec),znFAT_Buffer); //���ļ���ǰ�����������ڲ����壬�Ա��޸�
     Memory_Copy(znFAT_Buffer+(pfi->File_CurPos),app_Buffer,len); //�޸����ڲ������е�����
     znFAT_Device_Write_Sector((pfi->File_CurSec),znFAT_Buffer); //�����ݻ�д��������
	}
    #else //�����������EXB����
	if((pfi->exb_cursec)==(pfi->File_CurSec)) //������ļ���EXB��Ӧ�������ļ���ǰ����һ�£���˵��Ҫ�޸ĵ������ڴ��ļ���EXB��
	{
	 Memory_Copy((pfi->exb_buf)+(pfi->File_CurPos),app_Buffer,len); //�޸����ļ�����EXB�е�����
	}
	else //������ļ�EXB��Ӧ�������ļ���ǰ������һ�£�Ҫ�޸ĵ�����������������
	{
	 znFAT_Device_Read_Sector((pfi->File_CurSec),znFAT_Buffer); //���ļ���ǰ�����������ڲ����壬�Ա��޸�
     Memory_Copy(znFAT_Buffer+(pfi->File_CurPos),app_Buffer,len); //�޸����ڲ������е�����
     znFAT_Device_Write_Sector((pfi->File_CurSec),znFAT_Buffer); //�����ݻ�д��������	 
	}
    
    #endif
    #else //���û��ʹ��EXB���壬��ֱ���޸�������������
	znFAT_Device_Read_Sector((pfi->File_CurSec),znFAT_Buffer); //���ļ���ǰ�����������ڲ����壬�Ա��޸�
    Memory_Copy(znFAT_Buffer+(pfi->File_CurPos),app_Buffer,len); //�޸����ڲ������е�����
    znFAT_Device_Write_Sector((pfi->File_CurSec),znFAT_Buffer); //�����ݻ�д��������
    
    #endif

    //���¸����ļ�λ����Ϣ
	if(len<((pInit_Args->BytesPerSector)-(pfi->File_CurPos))) //Ҫ�޸ĵ�������С�ڵ�ǰ����ʣ��������
	{
	 (pfi->File_CurOffset)+=len;
	 (pfi->File_CurPos)+=len;
	}
    else
	if(len==((pInit_Args->BytesPerSector)-(pfi->File_CurPos))) //Ҫ�޸ĵ����������ڵ�ǰ����ʣ��������
	{
     if((len+(pfi->File_CurOffset))==(pfi->File_Size)) //����д���ļ�ĩβ
	 {
	  if(((pfi->File_Size)%Cluster_Size)==0) //����ļ���СΪ�ش�С������������ʱ�����ˡ���ء�
	  {
	   (pfi->File_CurOffset)+=len;
	   (pfi->File_CurPos)=0;
	   (pfi->File_CurSec)=SOC((pfi->File_CurClust));
	  }
	  else //����ļ���С�Ǵش�С������������ʱ����д���˷Ǵ�ĩ����ĩβ
	  {
	   (pfi->File_CurOffset)+=len;
	   (pfi->File_CurPos)=0;
	   (pfi->File_CurSec)++;
	  }
	 }
	 else
	 {
	  if(((len+(pfi->File_CurOffset))%Cluster_Size)==0) //���д����λ�������Ǵش�С������������ʱ��û�е��ļ�ĩβ
	  {
	   (pfi->File_CurOffset)+=len;
	   (pfi->File_CurPos)=0;
	   (pfi->File_CurClust)=Get_Next_Cluster((pfi->File_CurClust));
	   (pfi->File_CurSec)=SOC((pfi->File_CurClust));	   
	  }
	  else //���д����λ�÷Ǵش�С������������ʱ����д���˷Ǵ�ĩ����ĩβ
	  {
	   (pfi->File_CurOffset)+=len;
	   (pfi->File_CurPos)=0;
	   (pfi->File_CurSec)++;
	  }
	 }
	}
	return temp_len;
   }
   else //Ҫ�޸ĵ����ݳ��ȴ��ڵ�ǰ����ʣ��������
   {
	temp=(pInit_Args->BytesPerSector)-(pfi->File_CurPos);
	znFAT_Device_Read_Sector((pfi->File_CurSec),znFAT_Buffer);
    Memory_Copy(znFAT_Buffer+(pfi->File_CurPos),app_Buffer,temp);
	znFAT_Device_Write_Sector((pfi->File_CurSec),znFAT_Buffer);
	len-=temp;app_Buffer+=temp;
    (pfi->File_CurOffset)+=temp;

	(pfi->File_CurPos)=0;

    if(!IS_END_SEC_OF_CLU((pfi->File_CurSec),(pfi->File_CurClust))) //�����ǰ�������ǵ�ǰ�صĽ�������
	{
	 (pfi->File_CurSec)++;
	}
	else
	{
	 (pfi->File_CurClust)=Get_Next_Cluster((pfi->File_CurClust));
	 (pfi->File_CurSec)=SOC((pfi->File_CurClust));
	}
   }
  }

  if(((pfi->File_CurOffset)%(Cluster_Size))!=0) //�����ٴ��жϴ�ʱ��λ���Ƿ������ؿ�ʼ��Ϊ�˺����ͳһ������
  {
   temp=(((SOC(pfi->File_CurClust))+(pInit_Args->SectorsPerClust)-1)-(pfi->File_CurSec)+1)*(pInit_Args->BytesPerSector);

   if(len<=temp) //Ҫ�޸ĵ�ʣ��������С�ڵ��ڵ�ǰ��������ʣ��������
   {
    nsec=len/(pInit_Args->BytesPerSector); //����Ҫ�޸ĵ�ʣ���������е���������
    znFAT_Device_Write_nSector(nsec,(pfi->File_CurSec),app_Buffer); //��ǰ����Ҫ�޸ĵ����������ݲ���д������
    temp=(nsec*(pInit_Args->BytesPerSector));
    len-=temp;app_Buffer+=temp;

    (pfi->File_CurOffset)+=temp;

    if(len==0) //Ҫ�޸ĵ���������д����ǰ��
	{
     if((pfi->File_CurOffset)==(pfi->File_Size)) //�������д�����ļ�ĩβ�����ʱ��������ء�
	 {
	  (pfi->File_CurPos)=0;
	  (pfi->File_CurSec)=SOC((pfi->File_CurClust));
	 }
     else //д���˵�ǰ�أ����������ļ�ĩβ
	 {
	  (pfi->File_CurClust)=Get_Next_Cluster((pfi->File_CurClust));
	  (pfi->File_CurPos)=0;
	  (pfi->File_CurSec)=SOC((pfi->File_CurClust));
	 }
	}
    else //û��д����ǰ��
	{
     (pfi->File_CurPos)=0;
     (pfi->File_CurSec)+=nsec;

     //���������EXB���壬��˲������������ݣ����ܲ��������������У�����EXB������
     #ifdef USE_EXCHANGE_BUFFER //���ʹ����EXB����
     #ifndef USE_ALONE_EXB //���û�����ö���EXB���壬�������˹���EXB����
     if((psexb_cur_oc==pfi)&&(sexb_cur_dev==Dev_No)&&(sexb_cur_sec==(pfi->File_CurSec))) //�����ʱ�ļ���ռ�ù���EXB��ͬʱ��ǰ�ļ����ڵ��豸�������͹���EXB���
	 {
	  Memory_Copy(pexb_buf+(pfi->File_CurPos),app_Buffer,len); //�޸��ڹ���EXB�е�����
	 }
     else //����Ҫ�޸ĵ����ݲ���EXB�У���������������
	 {
	  znFAT_Device_Read_Sector((pfi->File_CurSec),znFAT_Buffer); //���ļ���ǰ�����������ڲ����壬�Ա��޸�
      Memory_Copy(znFAT_Buffer+(pfi->File_CurPos),app_Buffer,len); //�޸����ڲ������е�����
      znFAT_Device_Write_Sector((pfi->File_CurSec),znFAT_Buffer); //�����ݻ�д��������
	 }
     #else //�����������EXB����
     if((pfi->exb_cursec)==(pfi->File_CurSec)) //������ļ���EXB��Ӧ�������ļ���ǰ����һ�£���˵��Ҫ�޸ĵ������ڴ��ļ���EXB��
	 {
	  Memory_Copy((pfi->exb_buf)+(pfi->File_CurPos),app_Buffer,len); //�޸����ļ�����EXB�е�����
	 }
     else //������ļ�EXB��Ӧ�������ļ���ǰ������һ�£�Ҫ�޸ĵ�����������������
	 {
	  znFAT_Device_Read_Sector((pfi->File_CurSec),znFAT_Buffer); //���ļ���ǰ�����������ڲ����壬�Ա��޸�
      Memory_Copy(znFAT_Buffer+(pfi->File_CurPos),app_Buffer,len); //�޸����ڲ������е�����
      znFAT_Device_Write_Sector((pfi->File_CurSec),znFAT_Buffer); //�����ݻ�д��������	 
	 }
   
     #endif
     #else //���û��ʹ��EXB���壬��ֱ���޸�������������
     znFAT_Device_Read_Sector((pfi->File_CurSec),znFAT_Buffer); //���ļ���ǰ�����������ڲ����壬�Ա��޸�
     Memory_Copy(znFAT_Buffer+(pfi->File_CurPos),app_Buffer,len); //�޸����ڲ������е�����
     znFAT_Device_Write_Sector((pfi->File_CurSec),znFAT_Buffer); //�����ݻ�д��������
   
     #endif

     (pfi->File_CurOffset)+=len;
     (pfi->File_CurPos)=len;
	}

	return temp_len;
   }
   else //Ҫ�޸ĵ�ʣ�����������ڵ�ǰ����ʣ��������
   {
    //temp=(pInit_Args->BytesPerSector)-(pfi->File_CurPos);
    //znFAT_Device_Read_Sector((pfi->File_CurSec),znFAT_Buffer);
    //Memory_Copy(znFAT_Buffer,app_Buffer,temp);
    //znFAT_Device_Write_Sector((pfi->File_CurSec),znFAT_Buffer);
    //len-=temp;app_Buffer+=temp;
    //(pfi->File_CurOffset)+=temp;
    //(pfi->File_CurSec)++;

    temp=(((SOC(pfi->File_CurClust))+(pInit_Args->SectorsPerClust)-1)-(pfi->File_CurSec)+1);
    znFAT_Device_Write_nSector(temp,(pfi->File_CurSec),app_Buffer); //��ǰ����Ҫ�޸ĵ����������ݲ���д������
	temp*=(pInit_Args->BytesPerSector);
	len-=temp;app_Buffer+=temp;

	(pfi->File_CurOffset)+=temp;
	(pfi->File_CurClust)=Get_Next_Cluster((pfi->File_CurClust));
	(pfi->File_CurPos)=0;
	(pfi->File_CurSec)=SOC((pfi->File_CurClust));
   }
  }
 }

 //����������ضΣ��Ծ����ܵ�ʹ�ö�����д���������������д���ٶ�
 //start_clu��end_clu���ڼ�¼�����ضε�ʼĩ
 temp=(len/Cluster_Size); 

 if(temp>0) //�������������0�����漰�����д
 {
  start_clu=end_clu=(pfi->File_CurClust);

  for(iClu=1;iClu<temp;iClu++)
  {
   next_clu=Get_Next_Cluster(end_clu);

   if((next_clu-1)==end_clu) //������������٣�������
   {
    end_clu=next_clu;
   }
   else //��������ز����٣������������ϵ�
   {
    znFAT_Device_Write_nSector(((end_clu-start_clu+1)*(pInit_Args->SectorsPerClust)),SOC(start_clu),app_Buffer);
    app_Buffer+=((end_clu-start_clu+1)*Cluster_Size);
    start_clu=next_clu;
    end_clu=next_clu;
   }
  } 
	 
  temp2=Get_Next_Cluster(end_clu);
  temp1=(len-temp*Cluster_Size); //Ҫ�޸ĵ�ʣ��ķ���������
     
  if(temp1==0) //����Ҫ�޸ĵ����ݣ���Ҫ�޸ĵ���������д������
  {
   znFAT_Device_Write_nSector(((end_clu-start_clu+1)*(pInit_Args->SectorsPerClust)),SOC(start_clu),app_Buffer);

   if(!IS_END_CLU(temp2)) //����ѵ��˽����أ����ļ��ѵ�ĩβ��������ء�
   {
    (pfi->File_CurClust)=end_clu;
   }
   else
   {
    (pfi->File_CurClust)=temp2;	   
   }

   (pfi->File_CurOffset)+=(temp*Cluster_Size);
   (pfi->File_CurPos)=0;
   (pfi->File_CurSec)=SOC((pfi->File_CurClust));

   return temp_len;
  }
  else
  {
   if((temp2-1)==end_clu) //��һ�������ǰ���������һ����Ȼ����
   {
    temp=((end_clu-start_clu+1)*(pInit_Args->SectorsPerClust)+(temp1/(pInit_Args->BytesPerSector))); //����������
    znFAT_Device_Write_nSector(temp,SOC(start_clu),app_Buffer);

    (pfi->File_CurOffset)+=(temp*(pInit_Args->BytesPerSector));

    app_Buffer+=(temp*(pInit_Args->BytesPerSector));
    len-=(temp*(pInit_Args->BytesPerSector));
   }
   else //��һ����ǰ��ز�������
   {
    temp=((end_clu-start_clu+1)*(pInit_Args->SectorsPerClust));
    znFAT_Device_Write_nSector(temp,SOC(start_clu),app_Buffer);
    iClu=(temp*(pInit_Args->BytesPerSector)); //������ʱ���������ټ�����
    app_Buffer+=iClu;len-=iClu;
    (pfi->File_CurOffset)+=iClu;

    znFAT_Device_Write_nSector((temp1/(pInit_Args->BytesPerSector)),SOC(temp2),app_Buffer);
    iClu=((temp1/(pInit_Args->BytesPerSector))*(pInit_Args->BytesPerSector)); //������ʱ���������ټ�����
    app_Buffer+=iClu;len-=iClu;
    (pfi->File_CurOffset)+=iClu;
   }

   (pfi->File_CurClust)=temp2;
   (pfi->File_CurPos)=0;
   (pfi->File_CurSec)=SOC(temp2)+(temp1/(pInit_Args->BytesPerSector));
  }
 }
 else
 {
	temp=len/(pInit_Args->BytesPerSector);
	znFAT_Device_Write_nSector(temp,(pfi->File_CurSec),app_Buffer);
	app_Buffer+=(temp*(pInit_Args->BytesPerSector));
	len-=(temp*(pInit_Args->BytesPerSector));
	(pfi->File_CurOffset)+=(temp*(pInit_Args->BytesPerSector));
	 
	(pfi->File_CurSec)+=temp;
 }

 if(len==0) //���Ҫ�޸ĵ��������ޣ�����������д��������ĩβ
 {
  return temp_len;
 }
 else //�����������Ҫ�޸ģ����Ĳ�����������
 {
  //���������EXB���壬��˲������������ݣ����ܲ��������������У�����EXB������
  #ifdef USE_EXCHANGE_BUFFER //���ʹ����EXB����
  #ifndef USE_ALONE_EXB //���û�����ö���EXB���壬�������˹���EXB����
  if((psexb_cur_oc==pfi)&&(sexb_cur_dev==Dev_No)&&(sexb_cur_sec==(pfi->File_CurSec))) //�����ʱ�ļ���ռ�ù���EXB��ͬʱ��ǰ�ļ����ڵ��豸�������͹���EXB���
  {
   Memory_Copy(pexb_buf+(pfi->File_CurPos),app_Buffer,len); //�޸��ڹ���EXB�е�����
  }
  else //����Ҫ�޸ĵ����ݲ���EXB�У���������������
  {
   znFAT_Device_Read_Sector((pfi->File_CurSec),znFAT_Buffer); //���ļ���ǰ�����������ڲ����壬�Ա��޸�
   Memory_Copy(znFAT_Buffer+(pfi->File_CurPos),app_Buffer,len); //�޸����ڲ������е�����
   znFAT_Device_Write_Sector((pfi->File_CurSec),znFAT_Buffer); //�����ݻ�д��������
  }
  #else //�����������EXB����
  if((pfi->exb_cursec)==(pfi->File_CurSec)) //������ļ���EXB��Ӧ�������ļ���ǰ����һ�£���˵��Ҫ�޸ĵ������ڴ��ļ���EXB��
  {
   Memory_Copy((pfi->exb_buf)+(pfi->File_CurPos),app_Buffer,len); //�޸����ļ�����EXB�е�����
  }
  else //������ļ�EXB��Ӧ�������ļ���ǰ������һ�£�Ҫ�޸ĵ�����������������
  {
   znFAT_Device_Read_Sector((pfi->File_CurSec),znFAT_Buffer); //���ļ���ǰ�����������ڲ����壬�Ա��޸�
   Memory_Copy(znFAT_Buffer+(pfi->File_CurPos),app_Buffer,len); //�޸����ڲ������е�����
   znFAT_Device_Write_Sector((pfi->File_CurSec),znFAT_Buffer); //�����ݻ�д��������	 
  }
   
  #endif
  #else //���û��ʹ��EXB���壬��ֱ���޸�������������
	
  znFAT_Device_Read_Sector((pfi->File_CurSec),znFAT_Buffer); //���ļ���ǰ�����������ڲ����壬�Ա��޸�
  Memory_Copy(znFAT_Buffer+(pfi->File_CurPos),app_Buffer,len); //�޸����ڲ������е�����
  znFAT_Device_Write_Sector((pfi->File_CurSec),znFAT_Buffer); //�����ݻ�д��������
   
  #endif

  (pfi->File_CurOffset)+=len;
  (pfi->File_CurPos)=len;	
 
  return temp_len;
 }
}
#endif

/************************************************************************************
 ���ܣ��ر��ļ�
 �βΣ�pfi:ָ���ļ���Ϣ���ϵ�ָ��
 ���أ�0
 ��⣺�˺������ڹر��ļ��������ļ�����Ϣ���Ͻ�����0��ʹ�ļ��������Ϣ�������٣��Ӷ�
       �޷��ٶ�����в������������´򿪴��ļ������ͬʱ���˺���������RT_UPDATE_FILESIZE
       �Ķ�����ļ���С���и��¡����û�п����ļ���С��ʵʱ���¹��ܣ����ڵ��ô˺���ʱ
       ��������и��¡�������û��ʹ��ʵʱ�ļ���С���£���ô���ļ�����������������д��
       ���޸Ĳ���֮��һ��Ҫ����close_file������
*************************************************************************************/
#ifdef ZNFAT_CLOSE_FILE
UINT8 znFAT_Close_File(struct FileInfo *pfi)
{
 #ifndef RT_UPDATE_FILESIZE
 Update_File_Size(pfi); //���ļ�Ŀ¼���е��ļ���С�ֶ�
 #endif

 just_file=pfi;

 #ifndef RT_UPDATE_CLUSTER_CHAIN
 #ifdef USE_ALONE_CCCB
 CCCB_To_Alone();
 #endif
 #endif

 #ifndef RT_UPDATE_CLUSTER_CHAIN
 #ifndef USE_ALONE_CCCB
 if(pfi==pcccb_cur_oc) //������ڲ���������ļ���ռ��CCCB�����ڹر��ļ���ǰ����Ҫ��CCCB���µ�FAT
 #endif
  CCCB_Update_FAT();
 #endif

 #ifndef RT_UPDATE_CLUSTER_CHAIN
 #ifdef USE_ALONE_CCCB
 pcccb_buf=(UINT32 *)0;
 pcccb_curval=(UINT32 *)0;
 pcccb_counter=(UINT8 *)0;
 #endif
 #endif

 #ifdef USE_EXCHANGE_BUFFER
 #ifndef USE_ALONE_EXB
 if(pfi==psexb_cur_oc && 0!=(sexb_cur_sec)) //�����ǰEXB��ռ�ã�����������Ҫ�رյ��ļ�
 {
  znFAT_Device_Write_Sector(sexb_cur_sec,pexb_buf); //EXB�������ݻ�д
  sexb_cur_sec=0;
  psexb_cur_oc=(struct FileInfo *)0;
  sexb_cur_dev=(UINT8)(-1);
 }
 #else
 pexb_buf=pfi->exb_buf;
 if(0!=pfi->exb_cursec) //���Ҫ�ر��ļ���EXB��ռ��
 {
  znFAT_Device_Write_Sector(pfi->File_CurSec,pexb_buf); //EXB�������ݻ�д
 }
 pexb_buf=(UINT8 *)0;
 #endif
 #endif

 Memory_Set((UINT8 *)pfi,(UINT32)sizeof(struct FileInfo),0);

 just_file=(struct FileInfo *)0;

 return ERR_SUCC;
}
#endif

/************************************************************************************
 ���ܣ�ˢ���ļ�ϵͳ
 �βΣ���
 ���أ�0
 ��⣺�����ǽ������ļ�����֮�󣬱��紴���ļ���Ŀ¼�����ļ���д�����ݣ�����ɾ��������
       ����Ӱ�쵽�ļ�ϵͳ�����һЩ������������̵�ʣ�������ȡ�znFAT�п���ͨ����
       RT_UPDATE_FSINFO������������ļ�ϵͳʵʱ���¹��ܣ��κε�����ɾ�Ĳ���znFAT����
       ���������ļ�ϵͳ���������ᱻ����д��Ȳ���Ч�ʽ��ͣ���˿������ε���һʵʱ����
       ���ܣ����Ǳ����������ļ���������֮�󣬵��ô˺�����ˢ���ļ�ϵͳ��
*************************************************************************************/
#ifdef ZNFAT_FLUSH_FS
UINT8 znFAT_Flush_FS(void)
{
 #ifndef RT_UPDATE_FSINFO
 Update_FSINFO(); //�����ļ�ϵͳ��Ϣ
 #endif

 return ERR_SUCC; 
}
#endif
                                       
