/*
 * usart1.c
 *
 * Created: 2021/11/29 21:23:01
 *  Author: 19025
 */
#include "iom16v.h"
#include "usart1.h" 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>


char a[20];
int        data_org[LEN]    = {0};  //原始数据
int        data_min[LEN]    = {0};  //腐蚀结果
int        data_delta[LEN]  = {0};  //相减 org-min
int        data_window[LEN] = {0};  //窗运算结果
unsigned long _index          = 0;
char  heart;

int* ElemP(int* arr, unsigned long n) { return &arr[n % LEN]; }  //取元素地址 Pointer
int  ElemV(int* arr, unsigned long n) { return arr[n % LEN]; }   //取元素值 Value

char ch1 = 1,ch2 = 0,ch3 = 0; //互锁标志位

char R_flag; //寻找R峰标志位

unsigned long R_old,R_new;

                 //  0   1    2    3     4    5    6    7    8    9
char number[10] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};    //数码管数字编码

/*			UART初始化				*/
void uart_init(void)
{
	UCSRA|=(1<<U2X);
	UCSRB=(1<<RXEN)|(1<<TXEN);//允许发送和接收
	UBRRL=(F_CPU/8/(baud+1))%256;
	UBRRH=(F_CPU/8/(baud+1))/256;
	UCSRC=(1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);//8位数据+1位STOP位

}

//端口状态初始化设置函数
void Port_Init()
{

	DDRA=0x0f;
	DDRB = 0xFF;
}

//AD转换初始化函数
void AD_Init()
{
	ADMUX |= (1 << REFS0)|(1 <<MUX2)|(1 <<MUX1);     //ADC参考电压为AVcc，ADC结果右对齐,左对齐|(1 << ADLAR))，选择通道ADC6
	ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1); //使能AD转换，ADC时钟64分频
}

//AD转换函数
unsigned int AD_GetData()
{
	ADCSRA |= (1 << ADSC);     //开始AD转换
	while(!(ADCSRA & (1 << ADIF)));    //等待转换完成
	ADCSRA |= (1 << ADIF);              //清零ADC中断标志位
    
	return ADC;        //返回ADC值
}

/*  		字符输出函数 		 */
void put_char(unsigned char c)
{
	while (!(UCSRA&(1<<UDRE))) {;}
	UDR=c;
	
}

void senddate(char*s)
{
	int i;
	for (i=0;s[i]!=0;i++)
	{
		put_char(s[i]);
	}
}

void chunkouping(uchar c)
{
	sprintf(a,"add 2,0,%d",c);
	senddate(a);
	put_char(0xff);
	put_char(0xff);
	put_char(0xff);
	//PORTD=~PORTD;
}

void chunkouping1(uchar c)
{
	sprintf(a,"add 2,1,%d",c);
	senddate(a);
	put_char(0xff);
	put_char(0xff);
	put_char(0xff);
	//PORTD=~PORTD;
}

void timer0_init(void)
{

	TCCR0 =0x04;       //0b 0000 0011
	TIMSK = 0x45;  //0b 0100 0101 溢出中断使能
    TCNT0 = 50;
    }


void RealTimeProcessing(int _in /*传入即时采得的ADC的值*/) 
{
	int n;  //循环中的计次变量
	int min = 300;
	//记录数据
	*ElemP(data_org, _index) = _in;

	if (_index >= LEN+t2) 
	{
		_index -= t2;
		//腐蚀
		for (n = 0; n < LEN; n++)
		if (min > ElemV(data_org, _index - n))
		min = ElemV(data_org, _index - n);
       *ElemP(data_min, _index) = min;
       //相减
       *ElemP(data_delta, _index) = ElemV(data_org, _index) - ElemV(data_min, _index);
       //加窗
       *ElemP(data_window, _index) = (ElemV(data_delta, _index) - ElemV(data_delta, _index - t1)) * (ElemV(data_delta, _index) - ElemV(data_delta, _index + t2));
       //预处理
       *ElemP(data_window, _index) = ElemV(data_window, _index) > 0 ?( ElemV(data_window, _index) / 1024/9*90):0;//取大于零的结果并取高八位(取出的结果为 unsigned char
       
	   
   
        //计算心率coding   2021年12月6日
          if( ElemV(data_window, _index) < 10 )  R_flag = 1;  //小于阈值,标志位为1 寻找R峰位置功能开放
          else {  //大于阈值

	          if(R_flag) 
			  {  //标志位,是否寻找R峰下标
		          R_new = _index; //找到R峰位置
		          if( ((R_new - R_old) < interval_max) && ((R_new - R_old) > interval_min)   )  //判断间隔是否小于最大间隔,否则丢弃
		          heart = (char)(  60/((R_new - R_old)*T));
				  
		          R_old = R_new;
	          }

	          R_flag = 0;   //在大于阈值时候锁住 对R峰下标寻找的功能
                }
		//
		
		chunkouping1(ElemV(data_window, _index));
       _index += t2;  
        
	}

	_index++;
	
}

ISR(TIMER0_OVF_vect)
{


if(ch1){
	PORTA= 0x02;
	PORTB = number[heart / 100 % 10];
	_delay_us(800);
	ch1 = 0;
	ch2 = 1;
	ch3 = 0;
}

if(ch2){
	PORTA= 0x04;
	PORTB= number[heart / 10 % 10];
	_delay_us(800);
	ch1 = 0;
	ch2 = 0;
	ch3 = 1;
}

if(ch3){
	PORTA= 0x08;
	PORTB= number[heart/ 1 % 10];
	_delay_us(80);
	ch1 = 1;
	ch2 = 0;
	ch3 = 0;
}
    TCNT0 = 50;
}

