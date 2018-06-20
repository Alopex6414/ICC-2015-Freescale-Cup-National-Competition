#ifndef _MYTYPE_H_
#define _MYTYPE_H_

/*============================================================================================
  ���ļ����ڽ����������͵����¶��壬�밴��ʵ��ʹ�õ�Ӳ��ƽ̨�ϵ��������ͶԺ�������¶���
=============================================================================================*/

//�����������ͷ�ļ� ����AVR GCC�е� ROM������<AVR/pgmspace.h>�ж���

#define UINT8   unsigned char
#define UINT16  unsigned int
#define UINT32  unsigned long 

#define INT8    char 
#define INT16   int
#define INT32   long

#define ROM_TYPE_UINT8  const unsigned char  
#define ROM_TYPE_UINT16 const unsigned int  
#define ROM_TYPE_UINT32 const unsigned long  

#endif
