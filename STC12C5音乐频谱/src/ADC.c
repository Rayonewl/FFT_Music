/*------------------------------------------------------------------*/
/* --- STC MCU International Limited -------------------------------*/
/* --- STC 1T Series MCU A/D Conversion Demo -----------------------*/
/*------------------------------------------------------------------*/

//#include "reg51.h"
#include "intrins.h"

#define FOSC    18432000L
#define BAUD    9600

typedef unsigned char BYTE;
typedef unsigned int WORD;

/*Declare SFR associated with the ADC */
//sfr ADC_CONTR   =   0xBC;           //ADC control register
//sfr ADC_RES     =   0xBD;           //ADC high 8-bit result register
sfr ADC_LOW2    =   0xBE;           //ADC low 2-bit result register
//sfr P1ASF       =   0x9D;           //P1 secondary function control register

/*Define ADC operation const for ADC_CONTR*/
#define ADC_POWER   0x80            //ADC power control bit
#define ADC_FLAG    0x10            //ADC complete flag
#define ADC_START   0x08            //ADC start control bit
#define ADC_SPEEDLL 0x00            //420 clocks
#define ADC_SPEEDL  0x20            //280 clocks
#define ADC_SPEEDH  0x40            //140 clocks
#define ADC_SPEEDHH 0x60            //70 clocks

void InitUart();
void InitADC();
void SendData(BYTE dat);
BYTE GetADCResult(BYTE ch);
void Delay(WORD n);
//void ShowResult(BYTE ch);
//
//
///*----------------------------
//Send ADC result to UART
//----------------------------*/
//void ShowResult(BYTE ch)
//{
//    SendData(ch);                   //Show Channel NO.
//    SendData(GetADCResult(ch));     //Show ADC high 8-bit result
//
////if you want show 10-bit result, uncomment next line
////    SendData(ADC_LOW2);             //Show ADC low 2-bit result
//}

/*----------------------------
Get ADC result
----------------------------*/
BYTE GetADCResult(BYTE ch)
{
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ch | ADC_START;
    _nop_();                        //Must wait before inquiry
    _nop_();
    _nop_();
    _nop_();
    while (!(ADC_CONTR & ADC_FLAG));//Wait complete flag
    ADC_CONTR &= ~ADC_FLAG;         //Close ADC

    return ADC_RES;                 //Return ADC result
}
//
///*----------------------------
//Initial UART
//----------------------------*/
//void InitUart()
//{
//    SCON = 0x5a;                    //8 bit data ,no parity bit
//    TMOD = 0x20;                    //T1 as 8-bit auto reload
//    TH1 = TL1 = -(FOSC/12/32/BAUD); //Set Uart baudrate
//    TR1 = 1;                        //T1 start running
//}

/*----------------------------
Initial ADC sfr
----------------------------*/
void InitADC()
{
    //P1ASF = 0xff;                   //Open 8 channels ADC function
    ADC_RES = 0;                    //Clear previous result
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL;
    Delay(2);                       //ADC power-on and delay
}

/*----------------------------
Send one byte data to PC
Input: dat (UART data)
Output:-
----------------------------*/
//void SendData(BYTE dat)
//{
//    while (!TI);                    //Wait for the previous data is sent
//    TI = 0;                         //Clear TI flag
//    SBUF = dat;                     //Send current data
//}

/*----------------------------
Software delay function
----------------------------*/
void Delay(WORD n)
{
    WORD x;

    while (n--)
    {
        x = 5000;
        while (x--);
    }
}

