#ifndef  _MYFUN_H_
#define  _MYFUN_H_

/*******************************************************

        +------------------------------------+
                  |  公共函数部分  |
        +------------------------------------+
	  
********************************************************/
void delay_ms(unsigned int count);
void delay(unsigned int time); //延时函数
void u32tostr(unsigned long dat,char *str);	//将一个32位的变量dat转为字符串
unsigned long strtou32(char *str); //将一个字符串转为32位的变量

#endif
