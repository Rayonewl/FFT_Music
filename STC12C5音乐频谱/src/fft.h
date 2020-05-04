#ifndef _FFT_INCLUDED_
#define _FFT_INCLUDED_

struct compx
{
	float real;
	float imag;
};
struct compx dd[65]; //FFT����   
data struct compx temp;  
 
code float iw[64]=
{
	1.000,0,0.9952,-0.0980,0.9808,-0.1951,0.9569,-0.2903,0.9239,-0.3827,0.8819,-0.4714,0.8315,-0.5556,
	0.7730,-0.6344,0.7071,-0.7071,0.6344,-0.7730,0.5556,-0.8315,0.4714,-0.8819,0.3827,-0.9239,0.2903,-0.9569,
	0.1951,-0.9808,0.0980,-0.9952,0.0,-1.0000,-0.0980,-0.9952,-0.1951,-0.9808,-0.2903,0.9569,-0.3827,-0.9239,
	-0.4714,-0.8819,-0.5556,-0.8315,-0.6344,-0.7730,-0.7071,-0.7071,-0.7730,-0.6344,-0.8315,-0.5556,-0.8819,-0.4714,
	-0.9239,-0.3827,-0.9569,-0.2903,-0.9808,-0.1951,-0.9952,-0.0980
};


//�����˷�
void ee(struct compx b1,uchar data b2)
{ 
	temp.real=b1.real*iw[2*b2]-b1.imag*iw[2*b2+1];
	temp.imag=b1.real*iw[2*b2+1]+b1.imag*iw[2*b2]; 
} 
//�˷�����  
uint mypow(uchar data nbottom,uchar data ntop)
{
    uint data result=1;
    uchar data t;    
    for(t=0;t<ntop;t++)result*=nbottom; 
    return result;
}
//���ٸ���Ҷ�任  
void fft(struct compx *xin,uchar data N)
{
	uchar data  fftnum,i,j,k,l,m,n,disbuff,dispos,dissec;
	data struct compx t;
	fftnum=N;                         //����Ҷ�任����
	for(m=1;(fftnum=fftnum/2)!=1;m++);//���M��ֵ 
	for(k=0;k<=N-1;k++)               //��λ����
	{
		n=k;
		j=0; 
		for(i=m;i>0;i--)             //����
		{
			j=j+((n%2)<<(i-1));
			n=n/2;
		} 
		if(k<j){t=xin[1+j];xin[1+j]=xin[1+k];xin[1+k]=t;}//��������
	}  
	for(l=1;l<=m;l++)                //FFT����
	{
		disbuff=mypow(2,l);          //��õ������
		dispos=disbuff/2;            //��õ�������֮��ľ���
		for(j=1;j<=dispos;j++)
			for(i=j;i<N;i=i+disbuff) //����M�����еĵ���
			{
				dissec=i+dispos;     //��õڶ����λ��
				ee(xin[dissec],(uint)(j-1)*(uint)N/disbuff);//�����˷�
				t=temp;
				xin[dissec].real=xin[i].real-t.real;
				xin[dissec].imag=xin[i].imag-t.imag;
				xin[i].real=xin[i].real+t.real;
				xin[i].imag=xin[i].imag+t.imag;
			}
	}
} 


//�����ʾ���ݵ�ת������,�������������ٶȿ����Լ�������¶���
//��fft���ݽ��д���,�õ�����Ƶ�ʶεĵ�ѹ��ֵ
void processfft(void)
{ 
    uchar data pt=0,tmp; 

    for(pt=1;pt<65;pt++)
    {
        dd[pt].imag=0;                //�����鲿 
    }
    fft(dd,64);                       //�Ե�ǰ���ݽ��и���Ҷ�任
	 
    for(pt=1;pt<65;pt++)
    {                                      
        dd[pt].real=sqrt(dd[pt].real*dd[pt].real+dd[pt].imag*dd[pt].imag);//ȡ������ 
    }

		//�����
		for(pt=2;pt<34;pt+=2)	
	    {       
			tmp=(dd[pt].real/32)+1;
			if(refreshflag[pt]<tmp)		            //ˢ������,ȡ�ϴ�߶�ֵ �洢��ʾ  
			{
				for(LEDBuf[pt]=0xFF;tmp>1;tmp--)	//tmp>1;������  ���λ��һ�г���;{}һ��Ҳ��ִ�У�
				{
				 	LEDBuf[pt]<<=1;	
					
				}
				refreshflag[pt]=(dd[pt].real/32)+1;
			}
			else
			{
			 	if(refreshflag[pt]>1)refreshflag[pt]--;                //���������ٶȿ���  �ı�ֵ���Ըı��½��ٶ�
			 	for(LEDBuf[pt]=0xFF,tmp=refreshflag[pt];tmp>1;tmp--)   //tmp>1;������ ���λ��һ�г���
				{
				 	LEDBuf[pt]<<=1;						
				}
			}
		}	
}  
#endif      















