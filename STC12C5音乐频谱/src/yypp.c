#include "src\STC12C5A.h"		 

#include <intrins.h>      //51�������㣨����_nop_�պ�����
#include <math.h>

#include "src\Define.h"

///////ȫ�ֱ���///////
uchar data DelayTime=9;	  	
uchar data gain=6,tt=0;

uchar idata refreshflag[40];
uchar  data LEDBuf[35];	  //15�У������ң�����λΪ�棨1����

#include "src\ADC.c"
#include "src\fft.h"

uchar code ax[]={0,0,0,0,0,0,0,0,				   //�����ġ�ͼ������
				 0,0,0,0,0,0,0,0,			
0x00,0x00,0x00,0x70,0xF8,0xFC,0xFE,0x7F,0x7F,0x7F,0xFE,0xFC,0xF8,0x70,0x00,0x00,
				 0,0,0,0,0,0,0,0,
			     0,0,0,0,0,0,0,0,
				};

void Delays(uint t)	  	//��ʱ����
{  
	uint j;
	while(--t)
	{
		for(j=0;j<5;j++); 
	}   
}

void IOINIT()		//��ʼ��IO�ڹ���ģʽ
{
	P0M1 = 0x00;	    P0M0 = 0xff;       P0=0; //�����У�Y����L:OFF�� H��H+:ON,ON+;��
	P1M1 = B(01000011); P1M0 = B(00110000);P1=B(10111111); //(P17@�����裬IN��&P16@�����裬IN�� ... P11@(���裬IN)AD2;P10@(���裬IN)AD1;      
	P2M1 = B(00000000); P2M0 = B(11111111);P2=0; //�����У�X0��Left������L:OFF�� H��H+:ON,ON+;��  
	P3M1 = B(00000000);	P3M0 = B(11111111);P3=0; //�����У�X1��Right������L:OFF�� H��H+:ON,ON+;��  	      
	P1ASF =B(00000011);                          //��P10,P11��IO����Ϊģ�����빦�ܣ�     
}

void BufToLED_Refresh()		//��LEDBuf����ֵ���ݴ���P0,ͨ��LED��ʾ����
{	
	 uchar data i; 		//����ѭ������i		
	 uint temp=0x7fff; 	//����temp	

	 for(i=0;i<32;i+=2)	//ѭ��16�β���
	 {
	 	P0=0x00;  		//P0����͵�ƽ
		P2=temp/256; 	//P2Ϊǰ8λ
		P3=temp%256;	//P3Ϊ��8λ
		P0=~LEDBuf[i];	//����LEDBuf����ȡ��	
		Delays(DelayTime);	//��ʱһ��
		P2=0xff; 		//����	
		P3=0xff;	   	//����
		temp>>=1;		//����һλ	
		temp|=0x8000;	//Ŀ����Ϊ�����������������LED(��)�ṩ����	
	 }
}

void LOVE()         //�����ġ�ͼ����ʾ
{	
	 uchar data i;	   	//����ѭ������i
	 uint temp=0x7fff; 	//����temp

	 for(i=0;i<16;i++)	//ѭ��16�β���
	 {
	 	P0=0x00;		//P0����͵�ƽ
		P2=temp/256;  	//P2Ϊǰ8λ
		P3=temp%256;	//P3Ϊ��8λ
		P0=ax[i+tt];  	//���á����ġ�ͼ������
		Delays(DelayTime); 	//��ʱһ��
		P2=0xff;   		//����
		P3=0xff;		//����
		temp>>=1;	   	//����һλ
		temp|=0x8000;  	//Ŀ����Ϊ�����������������LED(��)�ṩ����
	 }
}

void Timer_INT()	//��ʱ����ʼ��
{
 	TMOD = 0x10; //��4λ����T/C1 
	EA = 1;	     //�����ж�
	TH1 = 0x00;  //16λ�����Ĵ���T1��8λ
	TL1 = 0x00;  //16λ�����Ĵ���T1��8λ
	ET1 = 1;     //T/C1�жϿ�
    TR1 = 1;     //T/C1����
}

void ax_xs()	  	//������ʾ
{
	uint pp=4000;	//����pp=4000
	while(pp--)		//���PP��1��Ϊ��(��pp��1��Ϊ0,ֱ��pp=0������whileѭ��)
	{
		if(pp%250==0)	//���PP/250,����Ϊ0(��pp�ܱ�250����)
		{
			tt++;		//tt��1
		}
		LOVE();		 	//�����ġ�ͼ����ʾ
	} 
	pp=4000; 			//pp���¸�ֵ0			               
	while(pp--)		//���PP��1��Ϊ��(��pp��1��Ϊ0,ֱ��pp=0������whileѭ��)
	{
		LOVE();	 	//�����ġ�ͼ����ʾ
	}
	pp=4000;   		//pp���¸�ֵ0
	while(pp--)		//���PP��1��Ϊ��(��pp��1��Ϊ0,ֱ��pp=0������whileѭ��)
	{
		if(pp%250==0)	//���PP/250,����Ϊ0(��pp�ܱ�250����)
		{
			tt++;		//tt��1
		} 
		LOVE();			//�����ġ�ͼ����ʾ
	}
}

void Main()			//���������
{  
	uint data Count=700;   		//�������Count
	uchar data i,Cgain,num=0;	//������� i,Cgain,num	

	IOINIT();			//��ʼ��IO�ڹ���ģʽ
	ax_xs();	  		//������ʾ
	InitADC(); 	  		//ADC��ʼ��
	Timer_INT(); 		//��ʱ����ʼ��

	while(1)		//��ѭ��
	{
		for(i=0;i<65;i++) 		//forѭ������65��(��ȡ64��ADCת�����)
		{  
			dd[i].real=(GetADCResult(0)+GetADCResult(1))<<gain;	 
			//��ȡ ad ������Ŵ� 2 �� gain���������η���Ȼ��� FFT �㷨�Ļ�������ֵ
		}
	 	processfft();			//����Ҷ�仯������
		if(dd[2].real<32)  		//�����ȡ����AD���<32(����Ƶ����̫С)
		{
			if(++Count==300)	//��ƽֵ��С���򽵵�gain�����������ĵ�ƽ��ʾ
			{
				Count=0;		//Count��0		
				gain=6;			//gain=6		
			}
		}
		else 					//������AD�����������Ƶ��Χ
		{
			Count=1;			//Count��ֵ1(Ϊ�´ε���Ƶ����̫С��׼��)
			if(++num==0xAF)		//����ʱ��Auto gain��
			{
				num=0; 			//num��0		
/*�Զ����������: ���������Ƶ�źŵ����������Сʱ�������� AD ֵ����Ҳ������Ӧ���Ƶı仯��
���Ǹ������ֵ�Զ����� AD ֵ�Ŵ�ı�����ά����һ���ķ�Χ�ڣ��Ӷ�ʹ��ʾ��Ч����ã�
��������������߿���������*/
				//�Զ����棻Automatic gain;
				Cgain=dd[2].real/32;					
				if(7<Cgain<=8) 		
				{
					gain=4;			//4
				}
				else if(4<Cgain<=6) 
				{
					gain=5;			//5
				}
				else if(2<Cgain<=4) 
				{
					gain=6;			//6
				}
				else 
				{
					gain=7;	  		//7
				} 
			}
		}		 
	}
}

void LEDRefresh_INT() interrupt 3    //��ʱ���жϷ�����
{ 
    TH1 = 0x8A;     //��ʱ15ms
    TL1 = 0xD0;     //��ʱʱ��ԽС��ʾԽ���,����Ƶ�ױ仯Խ����
					//��֮......��....������.....Ƶ�ױ仯Խ��
	BufToLED_Refresh();	//��LEDBuf����ֵ���ݴ���P0,ͨ��LED��ʾ���� 
						//ˢ����ʾ����������������������		
} 


