
// Include Standard LIB  files
#include "project.h"

/********************************/
/*      A/D convert function    */
/********************************/
int iVccVolt;
int iSamAdcRead[8];
int iDacMonVolt;
int iDacMonAmp;
int iVccRead[5];
int iSamAdcStatus;
short sSamAdcDelay;
char VccReadNo;
char SamAdcFail;
char SamAdcStep;
AT91PS_ADC pAdc;
void sam_adc_converting(void)
{  
  //char lp;
  switch (SamAdcStep)
  { 
  case 0:
    sSamAdcDelay = 0;
    SamAdcFail = 0;
    SamAdcStep++;
    //if (ExecMode != SYSTEM_TEST) SamAdcStep++;
    return;
    
  case 1:
    //if (ExecMode != SYSTEM_TEST)
    if (++sSamAdcDelay > SEC_1*2) SamAdcStep++;   
    return;
    
  case 2:
    sSamAdcDelay = 0;
    pAdc = AT91C_BASE_ADC;
    // All interrupt disable
    AT91F_ADC_CfgModeReg (pAdc, 0);
    // ADC clock:500kHz
    // StartUp Time: 256uS
    // Sample Hold Time: 16uS   
    // ADC Mode Register: 0x0F1F2F00
    AT91F_ADC_CfgTimings (pAdc, 96,1,256,16000);
    // Enable Channel: 4,5,6,7
    AT91F_ADC_EnableChannel(pAdc, 0xF0);
    SamAdcStep++;
    return;
    
  case 3:
    if (++sSamAdcDelay > 10) SamAdcStep++;   
    return;

  case 4:
    sSamAdcDelay = 0;
    iSamAdcStatus = AT91F_ADC_GetStatus(pAdc);
    if (iSamAdcStatus == 0x000C0000) 
    {
      AT91F_ADC_StartConversion(pAdc);
      SamAdcStep++; 
    }
    else if (++SamAdcFail > 50) SamAdcStep = 0;
    return;

  case 5:
    iSamAdcStatus = AT91F_ADC_GetStatus(pAdc);
    if ((iSamAdcStatus & 0xF0) == 0xF0)
    {  
      iSamAdcRead[4] = AT91F_ADC_GetConvertedDataCH4(pAdc); 
      iSamAdcRead[5] = AT91F_ADC_GetConvertedDataCH5(pAdc); 
      iSamAdcRead[6] = AT91F_ADC_GetConvertedDataCH6(pAdc);
      iSamAdcRead[7] = AT91F_ADC_GetConvertedDataCH7(pAdc);
      iSamAdcRead[3]++;
      SamAdcFail = 0;
      SamAdcStep++;
    }
    else SamAdcStep = 4;
    return;

  case 6:
  // ADC(Vcc)in = Vref x 33k / (120k + 33k)
  // Vcc(x100) = ADC[x] x 2.5(Vref) x 100 x 153 / (33 * 1024);
    if (++VccReadNo > 3) VccReadNo = 0;
    iVccRead[VccReadNo] = iSamAdcRead[7];
    iVccRead[4] = iVccRead[0] + iVccRead[1] + iVccRead[2] + iVccRead[3];    
    iVccVolt = (iVccRead[4] * 250 * 153) / (33 * 1024 * 4); 
    SamAdcStep++ ;
    return;
    
  case 7:
  // ADC(DAC)in = Vref x 10k / (10k + 3k)
  // DAC(x100) = ADC[x] x 2.5(Vref) x 100 x 13 / (3 * 1024);
    iDacMonVolt = (iSamAdcRead[4] * 250 * 13) / (3 * 1024);
    iDacMonAmp = (iSamAdcRead[5] * 250 * 13) / (3 * 1024);

    sSamAdcDelay = 0;
    SamAdcStep++ ;
    return;

  case 8:
    if (++sSamAdcDelay > 50) SamAdcStep = 4;
    //if (ExecMode == SYSTEM_TEST) SamAdcStep = 0;
    return;
   
  default: SamAdcStep = 0; return;
  }
}

