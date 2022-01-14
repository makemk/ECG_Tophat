/*
 * usart1_1.c
 *
 * Created: 2021/11/25 20:53:43
 * Author : 19025
 */ 
/******************************************/
/*          ���ʼ�����                */
/*  Ŀ��MCU:ATMEGA16   ����:�ⲿ11.0592MHZ  */
/******************************************/

#include "iom16v.h"
#include "usart1.h"
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>

int ADData;


int main(void)
{
  
	Port_Init();//����ͨ��
	
	uart_init();//���ô���
	
	AD_Init();//���òɼ�ͨ��ADC6____PA6
	
    timer0_init();//���ö�ʱ��timer0,���������ˢ����

	
	sei();          //ʹ��ȫ���ж�
 
	while(1)
	{
		
		RealTimeProcessing(AD_GetData() );                               //�˲�����
		
		ADData = (int)((long)AD_GetData() * 5010 / 1024);         //����õ�ADֵת��Ϊ��ѹֵ
		
		ADData=(uchar)(ADData/40);                                
		
		chunkouping(ADData*2);                                    
        
		
		_delay_ms(5) ;
		
		

	}
}
