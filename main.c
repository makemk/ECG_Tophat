/*
 * usart1_1.c
 *
 * Created: 2021/11/25 20:53:43
 * Author : 19025
 */ 
/******************************************/
/*          心率检测程序                */
/*  目标MCU:ATMEGA16   晶振:外部11.0592MHZ  */
/******************************************/

#include "iom16v.h"
#include "usart1.h"
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>

int ADData;


int main(void)
{
  
	Port_Init();//设置通道
	
	uart_init();//设置串口
	
	AD_Init();//设置采集通道ADC6____PA6
	
    timer0_init();//启用定时器timer0,控制数码管刷新率

	
	sei();          //使能全局中断
 
	while(1)
	{
		
		RealTimeProcessing(AD_GetData() );                               //滤波处理
		
		ADData = (int)((long)AD_GetData() * 5010 / 1024);         //将获得的AD值转换为电压值
		
		ADData=(uchar)(ADData/40);                                
		
		chunkouping(ADData*2);                                    
        
		
		_delay_ms(5) ;
		
		

	}
}
