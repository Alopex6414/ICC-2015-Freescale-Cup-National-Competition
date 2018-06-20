#ifndef _DEVICE_IO_H_
#define _DEVICE_IO_H_

/*�������������������������
  �����ϵ�znFAT--Ƕ��ʽFAT32�ļ�ϵͳ�����ʵ�֡�
   һ��[���²�]�Ѿ��ɱ�����������ʽ���淢�С�
   ��������������3���ʱ��Ǳ�ı��������ֽ�������Ψ
   һһ�׽���FAT32�ļ�ϵͳ��SD����Ƕ��ʽ�洢������
   ר�������л������˴����ı�̼��������ϵĿ������顣
   ���ڸ�����������ƽ̨������znFAT�������ɹ���
   ��ȫ���������Ҳ���ۡ�
   ���ϵ�ZN-X�����壬֧��51��AVR��STM32(M0/M3/M4)��
   CPU���˰���������ף����ϸ��־��ʵ�ʵ��ʵ������� 
                 ������վwww.znmcu.cn
  �������������������������*/

struct znFAT_IO_Ctl //�ײ������ӿڵ�IOƵ�ȿ����� 
{
 	UINT32 just_sec;
 	UINT8  just_dev;
};

UINT8 znFAT_Device_Init(void);
UINT8 znFAT_Device_Read_Sector(UINT32 addr,UINT8 *buffer);
UINT8 znFAT_Device_Write_Sector(UINT32 addr,UINT8 *buffer);
UINT8 znFAT_Device_Read_nSector(UINT32 nsec,UINT32 addr,UINT8 *buffer);
UINT8 znFAT_Device_Write_nSector(UINT32 nsec,UINT32 addr,UINT8 *buffer);
UINT8 znFAT_Device_Clear_nSector(UINT32 nsec,UINT32 addr);

#endif
