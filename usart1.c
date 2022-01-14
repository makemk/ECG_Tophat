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
int        data_org[LEN]    = {0};  //ԭʼ����
int        data_min[LEN]    = {0};  //��ʴ���
int        data_delta[LEN]  = {0};  //��� org-min
int        data_window[LEN] = {0};  //��������
unsigned long _index          = 0;
char  heart;

int* ElemP(int* arr, unsigned long n) { return &arr[n % LEN]; }  //ȡԪ�ص�ַ Pointer
int  ElemV(int* arr, unsigned long n) { return arr[n % LEN]; }   //ȡԪ��ֵ Value

char ch1 = 1,ch2 = 0,ch3 = 0; //������־λ

char R_flag; //Ѱ��R���־λ

unsigned long R_old,R_new;

                 //  0   1    2    3     4    5    6    7    8    9
char number[10] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};    //��������ֱ���

/*			UART��ʼ��				*/
void uart_init(void)
{
	UCSRA|=(1<<U2X);
	UCSRB=(1<<RXEN)|(1<<TXEN);//�����ͺͽ���
	UBRRL=(F_CPU/8/(baud+1))%256;
	UBRRH=(F_CPU/8/(baud+1))/256;
	UCSRC=(1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);//8λ����+1λSTOPλ

}

//�˿�״̬��ʼ�����ú���
void Port_Init()
{

	DDRA=0x0f;
	DDRB = 0xFF;
}

//ADת����ʼ������
void AD_Init()
{
	ADMUX |= (1 << REFS0)|(1 <<MUX2)|(1 <<MUX1);     //ADC�ο���ѹΪAVcc��ADC����Ҷ���,�����|(1 << ADLAR))��ѡ��ͨ��ADC6
	ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1); //ʹ��ADת����ADCʱ��64��Ƶ
}

//ADת������
unsigned int AD_GetData()
{
	ADCSRA |= (1 << ADSC);     //��ʼADת��
	while(!(ADCSRA & (1 << ADIF)));    //�ȴ�ת�����
	ADCSRA |= (1 << ADIF);              //����ADC�жϱ�־λ
    
	return ADC;        //����ADCֵ
}

/*  		�ַ�������� 		 */
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
	TIMSK = 0x45;  //0b 0100 0101 ����ж�ʹ��
    TCNT0 = 50;
    }


void RealTimeProcessing(int _in /*���뼴ʱ�ɵõ�ADC��ֵ*/) 
{
	int n;  //ѭ���еļƴα���
	int min = 300;
	//��¼����
	*ElemP(data_org, _index) = _in;

	if (_index >= LEN+t2) 
	{
		_index -= t2;
		//��ʴ
		for (n = 0; n < LEN; n++)
		if (min > ElemV(data_org, _index - n))
		min = ElemV(data_org, _index - n);
       *ElemP(data_min, _index) = min;
       //���
       *ElemP(data_delta, _index) = ElemV(data_org, _index) - ElemV(data_min, _index);
       //�Ӵ�
       *ElemP(data_window, _index) = (ElemV(data_delta, _index) - ElemV(data_delta, _index - t1)) * (ElemV(data_delta, _index) - ElemV(data_delta, _index + t2));
       //Ԥ����
       *ElemP(data_window, _index) = ElemV(data_window, _index) > 0 ?( ElemV(data_window, _index) / 1024/9*90):0;//ȡ������Ľ����ȡ�߰�λ(ȡ���Ľ��Ϊ unsigned char
       
	   
   
        //��������coding   2021��12��6��
          if( ElemV(data_window, _index) < 10 )  R_flag = 1;  //С����ֵ,��־λΪ1 Ѱ��R��λ�ù��ܿ���
          else {  //������ֵ

	          if(R_flag) 
			  {  //��־λ,�Ƿ�Ѱ��R���±�
		          R_new = _index; //�ҵ�R��λ��
		          if( ((R_new - R_old) < interval_max) && ((R_new - R_old) > interval_min)   )  //�жϼ���Ƿ�С�������,������
		          heart = (char)(  60/((R_new - R_old)*T));
				  
		          R_old = R_new;
	          }

	          R_flag = 0;   //�ڴ�����ֵʱ����ס ��R���±�Ѱ�ҵĹ���
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

