#ifndef  _MYFUN_H_
#define  _MYFUN_H_

/*******************************************************

        +------------------------------------+
                  |  ������������  |
        +------------------------------------+
	  
********************************************************/
void delay_ms(unsigned int count);
void delay(unsigned int time); //��ʱ����
void u32tostr(unsigned long dat,char *str);	//��һ��32λ�ı���datתΪ�ַ���
unsigned long strtou32(char *str); //��һ���ַ���תΪ32λ�ı���

#endif
