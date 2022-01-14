/*
 * usart1.h
 *
 * Created: 2021/11/29 21:22:38
 *  Author: 19025
 */ 


#ifndef USART1_H_
#define USART1_H_

#define uchar unsigned char
#define uint unsigned int

#define F_CPU 11059200
#define baud 115200//波特率
#define LEN 40  //取最小值的范围
#define t1   6  // LEN-t1取原址左移八位的数
#define t2   6  // LEN+t2取原址右移八位的数 ***********为了突出峰值
#define interval_max  65  //最大时间间隔
#define interval_min 30    //最小时间间隔
#define T 0.013  //采样周期 单位ms

/*UART初始化*/
void uart_init(void);
/*配置PA,PB*/
void Port_Init(void);   //端口初始化配置

void AD_Init(void);    //AD初始化

unsigned int AD_GetData(void);    //AD转换函数
/*
void Usart_PutChar(char cTXData);  //字节发送函数
void Usart_PutString(char *pcString);  // 字符串发送数据
*/
/*串口发送字符和字符串*/
void put_char(unsigned char c);
void senddate(char*s);

/*串口屏通道选择*/
void chunkouping(uchar c);
void chunkouping1(uchar c);

/*定时器中断配置*/
void timer0_init(void);

/*处理信号*/
void RealTimeProcessing(int _in /*传入即时采得的ADC的值*/); 



#endif /* USART1_H_ */