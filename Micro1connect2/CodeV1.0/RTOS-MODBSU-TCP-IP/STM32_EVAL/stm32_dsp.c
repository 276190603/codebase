

#include "stm32f10x.h"
#include <math.h>
#include "stm32_dsp.h"

#define NPT 64               /* NPT = Number of FFT point*/

u16     BUFMAG[64];	     //

long    lBUFIN[NPT];         /* Complex input vector */
long    lBUFOUT[NPT];        /* Complex output vector */
long    lBUFMAG[NPT];        /* Magnitude vector */

extern  u16   usRegHoldingBuf[];

u16 real[NPT]=
{
  878,603,193,289,961,951,956,935,
  704,350,266,252,210,334,595,934,
  959,956,952,923,676,621,245,249,
  213,327,558,925,960,955,953,927,
  673,332,251,249,208,324,574,933,
  959,954,952,918,663,317,244,237,
  206,327,582,939,959,953,951,897,
  632,307,246,240,211,330,574,931
}; /* real and imaginary arrays */

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------

void powerMag(long nfill)
{
  s32 lX,lY;
  u32 i;

  for (i=0; i < nfill; i++)
  {
    lX= (lBUFOUT[i]<<16)>>16; /* sine_cosine --> cos */
    lY= (lBUFOUT[i] >> 16);   /* sine_cosine --> sin */    
    {
      float X=  64*((float)lX)/32768;
      float Y = 64*((float)lY)/32768;
      float Mag = sqrt(X*X+ Y*Y)/nfill;		 // 先平方和,再开方
      lBUFMAG[i] = (u32)(Mag*65536);		 //
    }    
  }
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------- 
void DSP_main(void)
{
  u8 i,j;

  for(i=0; i<NPT; i+=1)
    lBUFIN[i]=((u32)real[i]*10)<<16;               //把数放大10倍，计算结果可以更准确一些。 
						   //实数放在32位数组的高16位。
  cr4_fft_64_stm32(lBUFOUT, lBUFIN, NPT);	   //FFT计算
   
  powerMag(NPT);				   //取模
  
  j=10;
  for(i=0; i<NPT; i+=1)			           //数据输出
  {
      usRegHoldingBuf[j++]=lBUFMAG[i];
  } 
}
