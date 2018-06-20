#include "mytype.h"
#include "config.h"
#include "deviceio.h"

#include "SD.h" //�洢�豸����ͷ�ļ�

struct znFAT_IO_Ctl ioctl; //����������д��IO���ƣ��������������������������Ч��
extern UINT8 Dev_No; //�豸��
extern UINT8 *znFAT_Buffer;

/*******************************************************************
 ���ܣ��洢�豸��ʼ��
 �βΣ����β�
 ���أ��洢�豸��ʼ��������Ϣ
 ��⣺znFAT֧�ֶ��豸����ͬʱ�ҽӶ��ִ洢�豸�����д洢�豸�ĳ�ʼ
       ������������ɡ�����ֵ��ĳһλΪ1����˵����Ӧ�Ĵ洢�豸��ʼ
       ��ʧ�ܡ����磺�豸0ʧ�ܣ��������豸���ɹ����򷵻�ֵΪ0X01��
       �豸1���豸2ʧ�ܣ��������豸�ɹ����򷵻�ֵΪ0X06��Ϊ��ʹ����
       ֵ��ʵ�ķ�ӳ��Ӧ�豸�ĳ�ʼ��״̬����ע��洢�豸��ʼ������
       ���õ�˳��
*******************************************************************/
UINT8 znFAT_Device_Init(void) 
{
	UINT8 res=0,err=0;
	ioctl.just_dev=0;
	ioctl.just_sec=0;
	//����Ϊ���洢�豸�ĳ�ʼ���������ã�����Ϯ���¸�ʽ
 	SPI_Init();
 	//SD_Rest();
 	res=SD_Init(); 

 	if(res) err|=0X01;
 	//res=Device1_Init();
 	//if(res) err|=0X02;
 	return err; 		//���ش����룬���ĳһ�豸��ʼ��ʧ�ܣ���err��ӦλΪ1
}
/*****************************************************************************
 ���ܣ�znFAT�Ĵ洢�豸����������ȡ�����ӿ�
 �βΣ�addr:����������ַ buffer:���ݻ�����ָ��
 ���أ�0
 ��⣺���洢�豸������������ȡ���������ŵ�case�ĸ�����֧�У���֧��ž��Ǵ���
       �����豸�š� 
*****************************************************************************/
UINT8 znFAT_Device_Read_Sector(UINT32 addr,UINT8 *buffer)
{
 	if(buffer==znFAT_Buffer) //��������znFAT�ڲ��������Ĳ���
 	{                        
  		if(ioctl.just_dev==Dev_No  //�������Ҫ��ȡ���������ڲ���������Ӧ�������������һ�β�������������ͬһ����
		&& (ioctl.just_sec==addr && 0!=ioctl.just_sec)) //���ٽ��ж�ȡ��ֱ�ӷ���
  		{                                           
   			return 0;      
  		}
  		else //���򣬾ͽ����һ�β������������Ϊ��ǰ����
  		{
   			ioctl.just_dev=Dev_No; 
   			ioctl.just_sec=addr; 
  		}
 	}

 	switch(Dev_No) //�ж��ٸ��洢�豸�����ж��ٸ�case��֧
 	{
  		case 0:
	       while(SD_Read_Sector(addr,buffer));
		     {
		     	break; 
		     } 
 	}
 	return 0;
}
/*****************************************************************************
 ���ܣ�znFAT�Ĵ洢�豸��������д�������ӿ�
 �βΣ�addr:����������ַ buffer:���ݻ�����ָ��
 ���أ�0
 ��⣺���洢�豸����������д�����������ŵ�case�ĸ�����֧�У���֧��ž��Ǵ���
       �����豸�š� 
*****************************************************************************/
UINT8 znFAT_Device_Write_Sector(UINT32 addr,UINT8 *buffer) 
{
 	if(buffer==znFAT_Buffer) //������ݻ��������ڲ�����
 	{
  		ioctl.just_dev=Dev_No; //����Ϊ��ǰ�豸��
  		ioctl.just_sec=addr; //����Ϊ��ǰ������������ַ	
 	}
 	switch(Dev_No)
 	{
  		case 0:
	       while(SD_Write_Sector(addr,buffer));
		     break;
	} 
 	return 0;
}
/***********************************************************************************
 ���ܣ�znFAT�Ĵ洢�豸��������������ȡ�����ӿ�
 �βΣ�nsec:Ҫ��ȡ�������� addr:����������ȡʱ�Ŀ�ʼ������ַ buffer:���ݻ�����ָ��
 ���أ�0
 ��⣺�˺����ӿ���znFAT������������ɸ�����������һ���Զ�ȡ���˺����ӿڵ�ʵ��������
       ģʽ 1����������ȡ����+ѭ�� 2���洢�豸Ӳ���ϵ�����������ȡ ʹ��2��1��Ч��Ҫ
       �ö࣬�ڸ������������Ƚϴ��Ӧ�ó��ϣ�����ʹ�����ṩӲ����������������ȡ����
***********************************************************************************/
UINT8 znFAT_Device_Read_nSector(UINT32 nsec,UINT32 addr,UINT8 *buffer)
{
 	UINT32 i=0;
 	if(0==nsec) return 0;
 	#ifndef USE_MULTISEC_R //�˺�����Ƿ�ʹ��Ӳ��������������ȡ����
  	switch(Dev_No)
  	{
   		case 0:
          	for(i=0;i<nsec;i++) //�����ʹ��Ӳ��������������ȡ����ʹ�õ�������ȡ+ѭ���ķ�ʽ
          	{
           		while(SD_Read_Sector(addr+i,buffer));
           		buffer+=512;
          	}
          	break;
  	}
 	#else
  	switch(Dev_No)
  	{
   		case 0:
          while(SD_Read_nSector(nsec,addr,buffer));
   		//case 1:
        //while(Device1_Read_nSector(nsec,addr,buffer));
  	}
 	#endif
 	return 0;
}
/***********************************************************************************
 ���ܣ�znFAT�Ĵ洢�豸������������д�������ӿ�
 �βΣ�nsec:Ҫд��������� addr:��������д��ʱ�Ŀ�ʼ������ַ buffer:���ݻ�����ָ��
 ���أ�0
 ��⣺�˺����ӿ��������������ȡ�����ӿ�ͬ��
***********************************************************************************/
UINT8 znFAT_Device_Write_nSector(UINT32 nsec,UINT32 addr,UINT8 *buffer)
{
 	UINT32 i=0; 
 	if(0==nsec) return 0;
 	#ifndef USE_MULTISEC_W //�˺�����Ƿ�ʹ��Ӳ������������д�뺯��
	switch(Dev_No)
  	{
   		case 0:
          for(i=0;i<nsec;i++)
          {
           while(SD_Write_Sector(addr+i,buffer));
           buffer+=512;
          }
		  break;
   		//case 1:
          //for(i=0;i<nsec;i++)
          //{
           //while(SD2_Write_Sector(addr+i,buffer));
           //buffer+=512;
          //}
		  //break;
  }
 	#else
  	switch(Dev_No)
  	{
   		case 0:
          while(SD_Write_nSector(nsec,addr,buffer));
		  break;
   		//case 1:
          //while(Device1_Write_nSector(nsec,addr,buffer));
  	}
 	#endif
 	return 0; 
}
/***********************************************************************************
 ���ܣ�znFAT�Ĵ洢�豸��������������0�����ӿ�
 �βΣ�nsec:Ҫ��0�������� addr:����������0�Ŀ�ʼ������ַ
 ���أ�0
 ��⣺�ڸ�ʽ�������У����ʱ�ľ��Ƕ�FAT�����������������0��ʹ��Ӳ����������������0
       �������������Ժܴ�̶��ϼ�����һ���̡�
***********************************************************************************/
UINT8 znFAT_Device_Clear_nSector(UINT32 nsec,UINT32 addr)
{
 	#ifndef USE_MULTISEC_CLEAR  //�˺�����Ƿ�ʹ��Ӳ��������������0����������Ҫ���ڸ�ʽ��������FAT�����0
  	UINT32 i=0;
  	for(i=0;i<512;i++) //����ڲ�����������������������0
  	{
   		znFAT_Buffer[i]=0;
  	}
  	switch(Dev_No)
  	{
   		case 0:
          for(i=0;i<nsec;i++)
          {
           while(SD_Write_Sector(addr+i,znFAT_Buffer));
          }
          break;
   		//case 1:
   		//       for(i=0;i<nsec;i++)
   		//       {
    	//       while(SD2_Write_Sector(addr+i,znFAT_Buffer));
   		//       }
   		//       break;
  }
 	#else
  	switch(Dev_No)
  	{
   		case 0:
          return SD_Erase_nSector(nsec,addr); //��ʹ��Ӳ����������������0��ʱ���뽫����������0����д������
   		//case 1:
          //return Device1_Clear_nSector(nsec,addr);
  	}
 	#endif
 	ioctl.just_dev=Dev_No; //����Ϊ��ǰ�豸��
 	ioctl.just_sec=(addr+nsec-1); //����Ϊ��ǰ������������ַ	 
	return 0;  
}
//==============================���������豸�����㡿========================================================================

