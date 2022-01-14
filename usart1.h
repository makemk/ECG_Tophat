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
#define baud 115200//������
#define LEN 40  //ȡ��Сֵ�ķ�Χ
#define t1   6  // LEN-t1ȡԭַ���ư�λ����
#define t2   6  // LEN+t2ȡԭַ���ư�λ���� ***********Ϊ��ͻ����ֵ
#define interval_max  65  //���ʱ����
#define interval_min 30    //��Сʱ����
#define T 0.013  //�������� ��λms

/*UART��ʼ��*/
void uart_init(void);
/*����PA,PB*/
void Port_Init(void);   //�˿ڳ�ʼ������

void AD_Init(void);    //AD��ʼ��

unsigned int AD_GetData(void);    //ADת������
/*
void Usart_PutChar(char cTXData);  //�ֽڷ��ͺ���
void Usart_PutString(char *pcString);  // �ַ�����������
*/
/*���ڷ����ַ����ַ���*/
void put_char(unsigned char c);
void senddate(char*s);

/*������ͨ��ѡ��*/
void chunkouping(uchar c);
void chunkouping1(uchar c);

/*��ʱ���ж�����*/
void timer0_init(void);

/*�����ź�*/
void RealTimeProcessing(int _in /*���뼴ʱ�ɵõ�ADC��ֵ*/); 



#endif /* USART1_H_ */