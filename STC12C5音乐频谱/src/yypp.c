#include "src\STC12C5A.h"		 

#include <intrins.h>      //51基本运算（包括_nop_空函数）
#include <math.h>

#include "src\Define.h"

///////全局变量///////
uchar data DelayTime=9;	  	
uchar data gain=6,tt=0;

uchar idata refreshflag[40];
uchar  data LEDBuf[35];	  //15列；从左到右；亮的位为真（1）；

#include "src\ADC.c"
#include "src\fft.h"

uchar code ax[]={0,0,0,0,0,0,0,0,				   //“爱心”图标数组
				 0,0,0,0,0,0,0,0,			
0x00,0x00,0x00,0x70,0xF8,0xFC,0xFE,0x7F,0x7F,0x7F,0xFE,0xFC,0xF8,0x70,0x00,0x00,
				 0,0,0,0,0,0,0,0,
			     0,0,0,0,0,0,0,0,
				};

void Delays(uint t)	  	//延时函数
{  
	uint j;
	while(--t)
	{
		for(j=0;j<5;j++); 
	}   
}

void IOINIT()		//初始化IO口工作模式
{
	P0M1 = 0x00;	    P0M0 = 0xff;       P0=0; //点阵列（Y）（L:OFF； H，H+:ON,ON+;）
	P1M1 = B(01000011); P1M0 = B(00110000);P1=B(10111111); //(P17@（高阻，IN）&P16@（高阻，IN） ... P11@(高阻，IN)AD2;P10@(高阻，IN)AD1;      
	P2M1 = B(00000000); P2M0 = B(11111111);P2=0; //点阵行（X0（Left））（L:OFF； H，H+:ON,ON+;）  
	P3M1 = B(00000000);	P3M0 = B(11111111);P3=0; //点阵行（X1（Right））（L:OFF； H，H+:ON,ON+;）  	      
	P1ASF =B(00000011);                          //将P10,P11的IO设置为模拟输入功能；     
}

void BufToLED_Refresh()		//把LEDBuf的数值数据传到P0,通过LED显示出来
{	
	 uchar data i; 		//定义循环变量i		
	 uint temp=0x7fff; 	//定义temp	

	 for(i=0;i<32;i+=2)	//循环16次操作
	 {
	 	P0=0x00;  		//P0输出低电平
		P2=temp/256; 	//P2为前8位
		P3=temp%256;	//P3为后8位
		P0=~LEDBuf[i];	//调用LEDBuf数组取反	
		Delays(DelayTime);	//延时一下
		P2=0xff; 		//消隐	
		P3=0xff;	   	//消隐
		temp>>=1;		//右移一位	
		temp|=0x8000;	//目的是为从左到右依次允许点亮LED(列)提供条件	
	 }
}

void LOVE()         //“爱心”图标显示
{	
	 uchar data i;	   	//定义循环变量i
	 uint temp=0x7fff; 	//定义temp

	 for(i=0;i<16;i++)	//循环16次操作
	 {
	 	P0=0x00;		//P0输出低电平
		P2=temp/256;  	//P2为前8位
		P3=temp%256;	//P3为后8位
		P0=ax[i+tt];  	//调用“爱心”图标数据
		Delays(DelayTime); 	//延时一下
		P2=0xff;   		//消隐
		P3=0xff;		//消隐
		temp>>=1;	   	//右移一位
		temp|=0x8000;  	//目的是为从左到右依次允许点亮LED(列)提供条件
	 }
}

void Timer_INT()	//定时器初始化
{
 	TMOD = 0x10; //高4位控制T/C1 
	EA = 1;	     //开总中断
	TH1 = 0x00;  //16位计数寄存器T1高8位
	TL1 = 0x00;  //16位计数寄存器T1低8位
	ET1 = 1;     //T/C1中断开
    TR1 = 1;     //T/C1启动
}

void ax_xs()	  	//爱心显示
{
	uint pp=4000;	//定义pp=4000
	while(pp--)		//如果PP减1后为真(即pp减1后不为0,直到pp=0才跳出while循环)
	{
		if(pp%250==0)	//如果PP/250,余数为0(即pp能被250整除)
		{
			tt++;		//tt加1
		}
		LOVE();		 	//“爱心”图标显示
	} 
	pp=4000; 			//pp重新赋值0			               
	while(pp--)		//如果PP减1后为真(即pp减1后不为0,直到pp=0才跳出while循环)
	{
		LOVE();	 	//“爱心”图标显示
	}
	pp=4000;   		//pp重新赋值0
	while(pp--)		//如果PP减1后为真(即pp减1后不为0,直到pp=0才跳出while循环)
	{
		if(pp%250==0)	//如果PP/250,余数为0(即pp能被250整除)
		{
			tt++;		//tt加1
		} 
		LOVE();			//“爱心”图标显示
	}
}

void Main()			//主函数入口
{  
	uint data Count=700;   		//定义变量Count
	uchar data i,Cgain,num=0;	//定义变量 i,Cgain,num	

	IOINIT();			//初始化IO口工作模式
	ax_xs();	  		//爱心显示
	InitADC(); 	  		//ADC初始化
	Timer_INT(); 		//定时器初始化

	while(1)		//主循环
	{
		for(i=0;i<65;i++) 		//for循环操作65次(读取64次ADC转换结果)
		{  
			dd[i].real=(GetADCResult(0)+GetADCResult(1))<<gain;	 
			//读取 ad 结果并放大 2 的 gain（变量）次方，然后给 FFT 算法的缓存区赋值
		}
	 	processfft();			//傅立叶变化及处理
		if(dd[2].real<32)  		//如果读取到的AD结果<32(即音频幅度太小)
		{
			if(++Count==300)	//电平值过小，则降低gain，减少噪音的电平显示
			{
				Count=0;		//Count清0		
				gain=6;			//gain=6		
			}
		}
		else 					//否则在AD结果在正常音频范围
		{
			Count=1;			//Count赋值1(为下次的音频幅度太小做准备)
			if(++num==0xAF)		//播放时，Auto gain；
			{
				num=0; 			//num清0		
/*自动增益的作用: 当输入的音频信号的音量变大或变小时，采样的 AD 值幅度也会有相应趋势的变化，
于是根据这个值自动调节 AD 值放大的倍数，维持在一定的范围内，从而使显示的效果最好，
不会出现满屏或者空屏的现象*/
				//自动增益；Automatic gain;
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

void LEDRefresh_INT() interrupt 3    //定时器中断服务函数
{ 
    TH1 = 0x8A;     //定时15ms
    TL1 = 0xD0;     //定时时间越小显示越清楚,但是频谱变化越缓慢
					//反之......大....抖动大.....频谱变化越快
	BufToLED_Refresh();	//把LEDBuf的数值数据传到P0,通过LED显示出来 
						//刷新显示缓存区数据来点亮点阵屏		
} 


