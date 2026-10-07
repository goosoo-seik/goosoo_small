
// Include Standard LIB  files
#include "project.h"
#include "CSLab_SAM7_ADE7758.h"
/********************************/
/*  ADE7758 Control Functions   */
/********************************/
#define ADE7758_REG_SIZE        72
#define	ADE_BUS_DELAY	        1
#define RMS_SAMPLE_COUNT	10
#define	METER_PARAMETER_ADD	0x0013FE00  // power meter parameter area
#define	METER_PARAMETER_PAGE	0x03FE00    // power meter parameter area
#define METER_SAVE_MASK         0xAA55AA55

int iADEdata;
unsigned char AdeVersion;
unsigned char AdeChkSum;
unsigned char ADEadd; 
int iWattSum, iVarSum, iVaSum;
int iPT1stVolt, iPT2stVolt;
int iCT1stAmp, iCT2stAmp;
int iAVrms, iBVrms, iCVrms;
int iVrmsRS, iVrmsST, iVrmsTR;
int iIa, iIb, iIc;
int iIa1, iIb1, iIc1;
int iIa0, iIb0, iIc0;
int iVrs, iVst, iVtr;
int iVrsRate, iVstRate, iVtrRate;
int iAIrms, iBIrms, iCIrms;
int iIaRate, iIbRate, iIcRate;
//int iAIrms1, iBIrms1, iCIrms1;
//int iAIrms0, iBIrms0, iCIrms0;
int iPw, iPvar, iPva;
int iAWatt, iBWatt, iCWatt;
int iWattRate, iVarRate, iVaRate;
int iWattDivider, iVarDivider, iVaDivider;
int iAVrmsDivider, iBVrmsDivider, iCVrmsDivider;
int iAIrmsDivider, iBIrmsDivider, iCIrmsDivider;
int iPowerFactor;
unsigned char AdeOPmode, AdeMmode, AdeWAVmode, AdeCOMPmode, AdeLCYCmode;
unsigned int uiAdeIRQmask;
short usAVrmsOffset, usBVrmsOffset, usCVrmsOffset;
short usAIrmsOffset, usBIrmsOffset, usCIrmsOffset;
short usAWattOffset, usBWattOffset, usCWattOffset;
short usAVarOffset,  usBVarOffset,  usCVarOffset;

unsigned char AdeGain;
unsigned short usAVrmsGain, usBVrmsGain, usCVrmsGain;
unsigned short usAIrmsGain, usBIrmsGain, usCIrmsGain;
unsigned short usAWattGain, usBWattGain, usCWattGain;
unsigned short usAVarGain,  usBVarGain,  usCVarGain;
unsigned short usAVaGain,  usBVaGain,  usCVaGain;
unsigned char APhaseCal, BPhaseCal, CPhaseCal;
unsigned char PowerDivider;
int iADEregistor[ADE7758_REG_SIZE];

void ADE7758_buffer_clear(char add, char size)
{
  char lp;
  for (lp = 0; lp < size; lp++) iADEregistor[add++] = 0;
}

void ADE7758_add_write(unsigned char add)
{
 unsigned char lp, mask;
    mask = 0x80;
    
    for (lp = 0; lp < 8; lp++)
    {
      pio_set( PIOA, PM_CLK );
      delay_nop(ADE_BUS_DELAY);
      if (add & mask) pio_set( PIOA, PM_DIN ); else pio_clear( PIOA, PM_DIN );
      pio_clear( PIOA, PM_CLK );  
      delay_nop(ADE_BUS_DELAY);
      mask = mask >> 1;
    }
}

void ADE7758_data_write(unsigned int data, char length)
{
 unsigned char lp;
 unsigned int mask;

 if (length == 12) length = 16;
   else if (length < 8) length = 8;
   
 mask = 1 << (length - 1);  
 for (lp = 0; lp < length; lp++)
 {
    pio_set( PIOA, PM_CLK );
    delay_nop(ADE_BUS_DELAY);
    if (data & mask) pio_set( PIOA, PM_DIN ); else pio_clear( PIOA, PM_DIN );
    pio_clear( PIOA, PM_CLK );
    delay_nop(ADE_BUS_DELAY);
    mask = mask >> 1;
  }
}

char ADE7758_registor_write(char add, unsigned int data)
{
  char length, enable;
  char error = 0;

  PMclock(OFF);
  length = ADE_REG_LENGTH[add];
  enable = ADE_WRITE_ENABLE[add];
  if ((length != 0)&(enable != 0))
  {
    PMenable(ON);
    add &= 0x7F;
    add |= 0x80;
    ADE7758_add_write(add);
    delay_us(3);
    ADE7758_data_write(data, length);
    PMenable(OFF);
   } 
   else error = 1;
   PMclock(OFF);
  return error;
}

int fit_reg_size(char add, int idata)
{
  char size;
  size = ADE_REG_LENGTH[add];
  if (size == 12)  idata &= 0x00000FFF;
  else if (size == 16) idata &= 0x0000FFFF;
  else if (size == 24) idata &= 0x00FFFFFF;
  else if (size == 7)idata &= 0x0000007F;
  else idata &= 0x000000FF;
  return idata;
}

int ADE7758_read(char length)
{
  char lp;
  int data;
  unsigned int mask;
  if (length == 12) length = 16;
   else if (length < 8) length = 8;
   
  mask = 1 << (length - 1);
  data = 0;
 
    for (lp = 0; lp < length; lp++)
    {
      PMclock(ON);
      delay_nop(ADE_BUS_DELAY);
      if ((pio_read(PIOA) & PM_DOUT) != 0) data |= mask;
      PMclock(OFF);
      delay_nop(ADE_BUS_DELAY);
      mask = mask >> 1;
    }
 return data;
}

int ADE7758_registor_read(char add)
{
  char length;
  int data = 0;
    PMclock(OFF);
    length = ADE_REG_LENGTH[add];
    if (length != 0) 
    {
      PMenable(ON); 
      add &= 0x7F;
      ADE7758_add_write(add);
      delay_us(3);
      data = ADE7758_read(length);
      PMenable(OFF);
    }
    PMclock(OFF);
  return data;
}

char ADE7758_registor_block_read(char add, char size)
{
  char error, lp, length;
  error = 0;
    
    PMclock(OFF);
    for (lp = 0; lp < size; lp++)
    {
      length = ADE_REG_LENGTH[add];
      if (length != 0) 
      {
	PMenable(ON); 
	add &= 0x7F;
        ADE7758_add_write(add);
        delay_us(3);
        //iADEregistor[add] = ADE7758_read(length);
        iADEregistor[add] = fit_reg_size(add, ADE7758_read(length));
	PMenable(OFF);
	add++;
      }
      else 
      {
	error = 1;
	break;
      }
    }
    PMclock(OFF);
    PMenable(OFF);
  return error;
}

char MeterSumErr;
int meter_data_save(void)
{
  unsigned int flashadd, result;
  sBackupAdd = 0;
  uiBackupSum = 0;

  //unlock_flash_page(METER_PARAMETER_PAGE);  
// Flash Saved Mark  
  backup_flash_write(METER_SAVE_MASK);  
//int iVrsRate, iVstRate, iVtrRate;
  backup_flash_write(iVrsRate);
  backup_flash_write(iVstRate);
  backup_flash_write(iVtrRate);
//int iIaRate, iIbRate, iIcRate;
  backup_flash_write(iIaRate);
  backup_flash_write(iIbRate);
  backup_flash_write(iIcRate);
//int iWattRate, iVarRate, iVaRate;
  backup_flash_write(iWattRate);
  backup_flash_write(iVarRate);
  backup_flash_write(iVaRate);
//int iWattDivider, iVarDivider, iVaDivider;
  backup_flash_write(iWattDivider);
  backup_flash_write(iVarDivider);
  backup_flash_write(iVaDivider);
//int iAVrmsDivider, iBVrmsDivider, iCVrmsDivider;
  backup_flash_write(iAVrmsDivider);
  backup_flash_write(iBVrmsDivider);
  backup_flash_write(iCVrmsDivider);
//int iAIrmsDivider, iBIrmsDivider, iCIrmsDivider;
  backup_flash_write(iAIrmsDivider);
  backup_flash_write(iBIrmsDivider);
  backup_flash_write(iCIrmsDivider);
//short usAVrmsOffset, usBVrmsOffset, usCVrmsOffset;
  backup_flash_write(usAVrmsOffset);
  backup_flash_write(usBVrmsOffset);
  backup_flash_write(usCVrmsOffset);
//short usAIrmsOffset, usBIrmsOffset, usCIrmsOffset;
  backup_flash_write(usAIrmsOffset);
  backup_flash_write(usBIrmsOffset);
  backup_flash_write(usCIrmsOffset);
//short usAVarOffset,  usBVarOffset,  usCVarOffset;
  backup_flash_write(usAVarOffset);
  backup_flash_write(usBVarOffset);
  backup_flash_write(usCVarOffset);
//int iPT1stVolt, iCT1stAmp
  backup_flash_write(iPT1stVolt);
  backup_flash_write(iCT1stAmp);
// Flash Backup Check Sum
  backup_flash_write(uiBackupSum);  

// Flash data write
  flashadd = METER_PARAMETER_ADD;
  result = AT91F_Flash_Write( flashadd , 256, &uiBackupData[0], 0);
  //printf("\nG:%8X",result);
  return result;
} 

void meter_data_read(void)
{
  char lp;
  unsigned int *uiptr;
  unsigned int flashadd, ui, sum;
  
  flashadd = METER_PARAMETER_ADD;
  uiptr = (unsigned int*) flashadd;

// Flash data move to buffer
  for (lp = 0; lp < 64; lp++)
  {
    uiBackupData[lp] = *uiptr;
    uiptr++;
  }
  
  sBackupAdd = 0;
  uiBackupSum = 0;
  MeterSumErr = 0;
// Flash Save Mark  
   ui = backup_flash_read(0);
   if (ui != METER_SAVE_MASK) MeterSumErr = 1; 
//int iVrsRate, iVstRate, iVtrRate;
   iVrsRate = backup_flash_read(VRS_RATE_DEFAULT);
   iVstRate = backup_flash_read(VST_RATE_DEFAULT);
   iVtrRate = backup_flash_read(VTR_RATE_DEFAULT);
//int iIaRate, iIbRate, iIcRate;
   iIaRate = backup_flash_read(IA_RATE_DEFAULT);
   iIbRate = backup_flash_read(IB_RATE_DEFAULT);
   iIcRate = backup_flash_read(IC_RATE_DEFAULT);
//int iWattRate, iVarRate, iVaRate;
   iWattRate = backup_flash_read(WATT_RATE_DEFAULT);
   iVarRate = backup_flash_read(VAR_RATE_DEFAULT);
   iVaRate = backup_flash_read(VA_RATE_DEFAULT);
//int iWattDivider, iVarDivider, iVaDivider;
   iWattDivider = backup_flash_read(WATT_DIVIDER_DEFAULT);
   iVarDivider = backup_flash_read(VAR_DIVIDER_DEFAULT);
   iVaDivider = backup_flash_read(VA_DIVIDER_DEFAULT);
//int iAVrmsDivider, iBVrmsDivider, iCVrmsDivider;
   iAVrmsDivider = backup_flash_read(AVRMS_DIVIDER_DEFAULT);
   iBVrmsDivider = backup_flash_read(BVRMS_DIVIDER_DEFAULT);
   iCVrmsDivider = backup_flash_read(CVRMS_DIVIDER_DEFAULT);
//int iAIrmsDivider, iBIrmsDivider, iCIrmsDivider;
   iAIrmsDivider = backup_flash_read(AIRMS_DIVIDER_DEFAULT);
   iBIrmsDivider = backup_flash_read(BIRMS_DIVIDER_DEFAULT);
   iCIrmsDivider = backup_flash_read(CIRMS_DIVIDER_DEFAULT);
//short usAVrmsOffset, usBVrmsOffset, usCVrmsOffset;
   usAVrmsOffset = backup_flash_read(AVRMS_OFFSET_DEFAULT);
   usBVrmsOffset = backup_flash_read(BVRMS_OFFSET_DEFAULT);
   usCVrmsOffset = backup_flash_read(CVRMS_OFFSET_DEFAULT);
//short usAIrmsOffset, usBIrmsOffset, usCIrmsOffset;
   usAIrmsOffset = backup_flash_read(AIRMS_OFFSET_DEFAULT);
   usBIrmsOffset = backup_flash_read(BIRMS_OFFSET_DEFAULT);
   usCIrmsOffset = backup_flash_read(CIRMS_OFFSET_DEFAULT);
//short usAVarOffset,  usBVarOffset,  usCVarOffset;
   usAVarOffset = backup_flash_read(AVAR_OFFSET_DEFAULT);
   usBVarOffset = backup_flash_read(BVAR_OFFSET_DEFAULT);
   usCVarOffset = backup_flash_read(CVAR_OFFSET_DEFAULT);
//int iPT1stVolt, iCT1stAmp
   iPT1stVolt = backup_flash_read(PT1_VOLT_DEFAULT);
   iCT1stAmp = backup_flash_read(CT1_AMP_DEFAULT);
// Flash Backup Check Sum
   sum = uiBackupSum;
   ui = backup_flash_read(0);
   if (ui != sum) MeterSumErr = 1;
   //MeterSumErr = 1;
}

void power_factory_setting(void)
{
  iVrsRate = VRS_RATE_DEFAULT;
  iVstRate = VST_RATE_DEFAULT;
  iVtrRate = VTR_RATE_DEFAULT;
  iIaRate = IA_RATE_DEFAULT; 
  iIbRate = IB_RATE_DEFAULT; 
  iIcRate = IC_RATE_DEFAULT;
  iWattRate = WATT_RATE_DEFAULT;
  iVarRate = VAR_RATE_DEFAULT;
  iWattDivider = WATT_DIVIDER_DEFAULT;
  iVarDivider = VAR_DIVIDER_DEFAULT;
  iVaDivider = VA_DIVIDER_DEFAULT;
  
  // CT/PT default value setting
  iPT1stVolt = PT1_VOLT_DEFAULT;
  iPT2stVolt = PT2_VOLT_DEFAULT;
  iCT1stAmp = CT1_AMP_DEFAULT;
  iCT2stAmp = CT2_AMP_DEFAULT;
  iAVrmsDivider = AVRMS_DIVIDER_DEFAULT;
  iBVrmsDivider = BVRMS_DIVIDER_DEFAULT;
  iCVrmsDivider = CVRMS_DIVIDER_DEFAULT;
  iAIrmsDivider = AIRMS_DIVIDER_DEFAULT;
  iBIrmsDivider = BIRMS_DIVIDER_DEFAULT;
  iCIrmsDivider = CIRMS_DIVIDER_DEFAULT;

  // ADE7758 mode setting
  AdeOPmode    = ADE_OPMODE_DEFAULT;
  AdeMmode     = ADE_MMODE_DEFAULT;
  AdeWAVmode   = ADE_WAVMODE_DEFAULT;
  AdeCOMPmode  = ADE_COMPMODE_DEFAULT;
  AdeLCYCmode  = ADE_LCYCMODE_DEFAULT;
  
  // ADE7758 Vrms offset setting
  usAVrmsOffset = AVRMS_OFFSET_DEFAULT;
  usBVrmsOffset = BVRMS_OFFSET_DEFAULT;
  usCVrmsOffset = CVRMS_OFFSET_DEFAULT; 
  
  // ADE7758 Irms offset setting
  usAIrmsOffset = AIRMS_OFFSET_DEFAULT;
  usBIrmsOffset = BIRMS_OFFSET_DEFAULT;
  usCIrmsOffset = CIRMS_OFFSET_DEFAULT;
  
  // ADE7758 Watt offset setting
  usAWattOffset = AWATT_OFFSET_DEFAULT;
  usBWattOffset = BWATT_OFFSET_DEFAULT;
  usCWattOffset = CWATT_OFFSET_DEFAULT;
 
  // ADE7758 VAR offset setting
  usAVarOffset  = AVAR_OFFSET_DEFAULT;
  usBVarOffset  = BVAR_OFFSET_DEFAULT;
  usCVarOffset  = CVAR_OFFSET_DEFAULT;
 
  // ADE7758 Vrms gain setting
  AdeGain = ADE_GAIN_DEFAULT;
  
  // ADE7758 Vrms gain setting
  usAVrmsGain = AVRMS_GAIN_DEFAULT;
  usBVrmsGain = BVRMS_GAIN_DEFAULT;
  usCVrmsGain = CVRMS_GAIN_DEFAULT; 
  
  // ADE7758 Irms gain setting
  usAIrmsGain = AIRMS_GAIN_DEFAULT;
  usBIrmsGain = BIRMS_GAIN_DEFAULT;
  usCIrmsGain = CIRMS_GAIN_DEFAULT;
  
  // ADE7758 Watt gain setting
  usAWattGain = AWATT_GAIN_DEFAULT;
  usBWattGain = BWATT_GAIN_DEFAULT;
  usCWattGain = CWATT_GAIN_DEFAULT;
  
  // ADE7758 VAR gain setting
  usAVarGain = AVAR_GAIN_DEFAULT;
  usBVarGain = BVAR_GAIN_DEFAULT;
  usCVarGain = CVAR_GAIN_DEFAULT;
  
  // ADE7758 Power Divider setting
  PowerDivider = ADE_PW_DIVIDER_DEFAULT;
  
  // ADE7758 VA gain setting
  APhaseCal = ADE_APHCAL_DEFAULT;
  BPhaseCal = ADE_BPHCAL_DEFAULT;
  CPhaseCal = ADE_CPHCAL_DEFAULT;
}

char ADE7758_init(void)
{
  char err;
  PMenable(OFF);
  PMclock(OFF);
  PMdataout(OFF);
  delay_us(1000);
  err = 0;
  
  // CT/PT default value setting
  iPT2stVolt = PT2_VOLT_DEFAULT;
  iCT2stAmp = CT2_AMP_DEFAULT;

  // ADE7758 mode setting
  AdeOPmode    = ADE_OPMODE_DEFAULT;
  AdeMmode     = ADE_MMODE_DEFAULT;
  AdeWAVmode   = ADE_WAVMODE_DEFAULT;
  AdeCOMPmode  = ADE_COMPMODE_DEFAULT;
  AdeLCYCmode  = ADE_LCYCMODE_DEFAULT;
  err += ADE7758_registor_write(ADE_OPMODE, AdeOPmode);
  err += ADE7758_registor_write(ADE_MMODE, AdeMmode);
  err += ADE7758_registor_write(ADE_WAVMODE, AdeWAVmode);
  err += ADE7758_registor_write(ADE_COMPMODE, AdeCOMPmode);
  err += ADE7758_registor_write(ADE_LINECYC, AdeLCYCmode);
  
  // ADE7758 IRQ Mask setting
  uiAdeIRQmask = ADE_IRQ_SIMPLE;
  err += ADE7758_registor_write(ADE_IMASK, uiAdeIRQmask);
  
  // ADE7758 Vrms offset setting
  err += ADE7758_registor_write(ADE_AVRMSOS, usAVrmsOffset);
  err += ADE7758_registor_write(ADE_BVRMSOS, usBVrmsOffset);
  err += ADE7758_registor_write(ADE_CVRMSOS, usCVrmsOffset);
  
  // ADE7758 Irms offset setting
  err += ADE7758_registor_write(ADE_AIRMSOS, usAIrmsOffset);
  err += ADE7758_registor_write(ADE_BIRMSOS, usBIrmsOffset);
  err += ADE7758_registor_write(ADE_CIRMSOS, usCIrmsOffset);
  
  // ADE7758 Watt offset setting
  usAWattOffset = AWATT_OFFSET_DEFAULT;
  usBWattOffset = BWATT_OFFSET_DEFAULT;
  usCWattOffset = CWATT_OFFSET_DEFAULT;
  err += ADE7758_registor_write(ADE_AWATTOS, usAWattOffset);
  err += ADE7758_registor_write(ADE_BWATTOS, usBWattOffset);
  err += ADE7758_registor_write(ADE_CWATTOS, usCWattOffset);
  
  // ADE7758 VAR offset setting
  err += ADE7758_registor_write(ADE_AVAROS,  usAVarOffset);
  err += ADE7758_registor_write(ADE_BVAROS,  usBVarOffset);
  err += ADE7758_registor_write(ADE_CVAROS,  usCVarOffset);
  
  // ADE7758 Vrms gain setting
  AdeGain = ADE_GAIN_DEFAULT;
  err += ADE7758_registor_write(ADE_GAIN, AdeGain);
  
  // ADE7758 Vrms gain setting
  usAVrmsGain = AVRMS_GAIN_DEFAULT;
  usBVrmsGain = BVRMS_GAIN_DEFAULT;
  usCVrmsGain = CVRMS_GAIN_DEFAULT; 
  err += ADE7758_registor_write(ADE_AVRMSGAIN, usAVrmsGain);
  err += ADE7758_registor_write(ADE_BVRMSGAIN, usBVrmsGain);
  err += ADE7758_registor_write(ADE_CVRMSGAIN, usCVrmsGain);
  
  // ADE7758 Irms gain setting
  usAIrmsGain = AIRMS_GAIN_DEFAULT;
  usBIrmsGain = BIRMS_GAIN_DEFAULT;
  usCIrmsGain = CIRMS_GAIN_DEFAULT;
  err += ADE7758_registor_write(ADE_AIGAIN, usAIrmsGain);
  err += ADE7758_registor_write(ADE_BIGAIN, usBIrmsGain);
  err += ADE7758_registor_write(ADE_CIGAIN, usCIrmsGain);
  
  // ADE7758 Watt gain setting
  usAWattGain = AWATT_GAIN_DEFAULT;
  usBWattGain = BWATT_GAIN_DEFAULT;
  usCWattGain = CWATT_GAIN_DEFAULT;
  err += ADE7758_registor_write(ADE_AWGAIN, usAWattGain);
  err += ADE7758_registor_write(ADE_BWGAIN, usBWattGain);
  err += ADE7758_registor_write(ADE_CWGAIN, usCWattGain);
  
  // ADE7758 VAR gain setting
  usAVarGain = AVAR_GAIN_DEFAULT;
  usBVarGain = BVAR_GAIN_DEFAULT;
  usCVarGain = CVAR_GAIN_DEFAULT;
  err += ADE7758_registor_write(ADE_AVARGAIN, usAVarGain);
  err += ADE7758_registor_write(ADE_BVARGAIN, usBVarGain);
  err += ADE7758_registor_write(ADE_CVARGAIN, usCVarGain);
  
  // ADE7758 Power Divider setting
  PowerDivider = ADE_PW_DIVIDER_DEFAULT;
  err += ADE7758_registor_write(ADE_WDIV, PowerDivider);
  err += ADE7758_registor_write(ADE_VARDIV, PowerDivider);
  err += ADE7758_registor_write(ADE_VADIV, PowerDivider);
  
  // ADE7758 VA gain setting
  APhaseCal = ADE_APHCAL_DEFAULT;
  BPhaseCal = ADE_BPHCAL_DEFAULT;
  CPhaseCal = ADE_CPHCAL_DEFAULT;
  err += ADE7758_registor_write(ADE_APHCAL, usAVaGain);
  err += ADE7758_registor_write(ADE_BPHCAL, usBVaGain);
  err += ADE7758_registor_write(ADE_CPHCAL, usCVaGain);

  return err;
}

#define ADE_NO_DATA 0x0000FFFF
char PwMeterError;
char ADE7758_check(void)
{
  char err = 0;
  if ((iADEregistor[ADE_AVRMS] & ADE_NO_DATA) == ADE_NO_DATA) err = 1;
  else if ((iADEregistor[ADE_BVRMS] & ADE_NO_DATA) == ADE_NO_DATA) err = 1;
  else if ((iADEregistor[ADE_CVRMS] & ADE_NO_DATA) == ADE_NO_DATA) err = 1;
  else if ((iADEregistor[ADE_AIRMS] & ADE_NO_DATA) == ADE_NO_DATA) err = 1;
  else if ((iADEregistor[ADE_BIRMS] & ADE_NO_DATA) == ADE_NO_DATA) err = 1;
  else if ((iADEregistor[ADE_CIRMS] & ADE_NO_DATA) == ADE_NO_DATA) err = 1;
  return err;
}

void voltage_calculate(void)
{
    if (iADEregistor[ADE_AVRMS] > 0x00800000) iADEregistor[ADE_AVRMS] = 0;
    if (iADEregistor[ADE_BVRMS] > 0x00800000) iADEregistor[ADE_BVRMS] = 0;
    if (iADEregistor[ADE_CVRMS] > 0x00800000) iADEregistor[ADE_CVRMS] = 0;
    iAVrms = iADEregistor[ADE_AVRMS] / iPT2stVolt * iPT1stVolt / iAVrmsDivider;
    iBVrms = iADEregistor[ADE_BVRMS] / iPT2stVolt * iPT1stVolt / iBVrmsDivider;
    iCVrms = iADEregistor[ADE_CVRMS] / iPT2stVolt * iPT1stVolt / iCVrmsDivider;
    iVrmsRS = (iAVrms + iBVrms) * 866 / 1000;
    iVrmsST = (iBVrms + iCVrms) * 866 / 1000;
    iVrmsTR = (iCVrms + iAVrms) * 866 / 1000;
    iVrs = iVrmsRS * iVrsRate / 1000;
    iVst = iVrmsST * iVstRate / 1000;
    iVtr = iVrmsTR * iVtrRate / 1000;
}

void current_calculate(void)
{
    iAIrms = iADEregistor[ADE_AIRMS] / CT2_AMP_DEFAULT * iCT1stAmp / iAIrmsDivider;
    iBIrms = iADEregistor[ADE_BIRMS] / CT2_AMP_DEFAULT * iCT1stAmp / iBIrmsDivider;
    iCIrms = iADEregistor[ADE_CIRMS] / CT2_AMP_DEFAULT * iCT1stAmp / iCIrmsDivider;

    iIa = iAIrms * iIaRate / 1000;
    iIb = iBIrms * iIbRate / 1000;    
    iIc = iCIrms * iIcRate / 1000;
    
    iIa1 = iIa / 10;
    iIa0 = iIa % 10;
    iIb1 = iIb / 10;
    iIb0 = iIb % 10;    
    iIc1 = iIc / 10;
    iIc0 = iIc % 10;
}    

#define AC_OCR_TIME 6
char AcOverCount;
char ac_over_current_check(void)
{
  char err;
  err = 0;
  // iAcOverAmp = 0 이면 AC OCR Check 취소
  if (iAcOverAmp == 0)  AcOverCount = 0;
  else if (PwMeterError == 0)
  {
    if ((iIa1 > iAcOverAmp)|(iIb1 > iAcOverAmp)|(iIc1 > iAcOverAmp))
    AcOverCount++;
    else if (AcOverCount > 0) AcOverCount--;
    if (AcOverCount >= AC_OCR_TIME) 
    {
      err = 1;
      AcOverCount = 0;
    }
  }
  return err;
} 

int iAcLowVolt;
char ac_low_voltage_check(void)
{
  char err;
  err = 0;
  // iAcLowVolt = 0 이면 AC Low Voltage Check 취소
  if (iAcLowVolt != 0)
   if (PwMeterError == 0)
    if ((iAcLowVolt > iVrs)|(iAcLowVolt > iVst)|(iAcLowVolt > iVtr))
     err = 1;
  return err;
}    

void abs_power_register(void)
{
    if (iADEregistor[ADE_AWATTHR] > 0x8000) iADEregistor[ADE_AWATTHR] ^= 0xFFFF;  
    if (iADEregistor[ADE_BWATTHR] > 0x8000) iADEregistor[ADE_BWATTHR] ^= 0xFFFF; 
    if (iADEregistor[ADE_CWATTHR] > 0x8000) iADEregistor[ADE_CWATTHR] ^= 0xFFFF; 
    if (iADEregistor[ADE_AVARHR] > 0x8000) iADEregistor[ADE_AVARHR] ^= 0xFFFF; 
    if (iADEregistor[ADE_BVARHR] > 0x8000) iADEregistor[ADE_BVARHR] ^= 0xFFFF;
    if (iADEregistor[ADE_CVARHR] > 0x8000) iADEregistor[ADE_CVARHR] ^= 0xFFFF;
    if (iADEregistor[ADE_AVAHR] > 0x8000) iADEregistor[ADE_AVAHR] ^= 0xFFFF;
    if (iADEregistor[ADE_BVAHR] > 0x8000) iADEregistor[ADE_BVAHR] ^= 0xFFFF;
    if (iADEregistor[ADE_CVAHR] > 0x8000) iADEregistor[ADE_CVAHR] ^= 0xFFFF; 
}

#define MIN_POWER     100;
void power_factor_calculate(void)
{
    double sum, rate;
    double dsum, wsum, vsum;
    
    abs_power_register();
    rate = iPT1stVolt * iCT1stAmp / iPT2stVolt / iCT2stAmp;

    sum = iADEregistor[ADE_AWATTHR] + iADEregistor[ADE_BWATTHR] + iADEregistor[ADE_CWATTHR];
    if (sum < iWattDivider/100) wsum = 0;
    else wsum = sum * rate * 100 / iWattDivider;

    iWattSum = wsum;                // 순수 측정값
    wsum = wsum * iWattRate / 1000; 
    iPw = wsum;                     // Span을 반영한 교정값    

    sum = iADEregistor[ADE_AVARHR] + iADEregistor[ADE_BVARHR] + iADEregistor[ADE_CVARHR];
    if (sum < iVarDivider/100) vsum = 0;
    else vsum = sum * rate * 100 / iVarDivider;
    
    iVarSum = vsum;                 // 순수 측정값
    vsum = vsum * iVarRate / 1000;  
    iPvar = vsum;                   // Span을 반영한 교정값    
    
    dsum = (wsum * wsum) + (vsum * vsum);
    dsum = sqrt(dsum);
    iPowerFactor = wsum * 1000 / dsum;
    if (iPowerFactor < 0) iPowerFactor = 0;
}

/*****************************************/
/*  ADE7758 Measument caribtate function */
/*****************************************/
// ADE7758 Execute step define
#define	METER_ADJ_MAIN	  10
#define	METER_ADJ_EXE 	  50
#define	METER_ADJ_PT   	  100
#define	METER_ADJ_CT	  200
#define	METER_VOLT_ZERO   300
#define	METER_VOLT_SPAN0  400
#define	METER_VOLT_SPAN1  410
#define	METER_VOLT_SPAN2  420
#define	METER_AMP_ZEROA	  500
#define	METER_AMP_SPAN0   600
#define	METER_AMP_SPAN1   610
#define	METER_AMP_SPAN2   620
#define	METER_WATT_SPAN   700
#define	METER_VAR_SPAN    800
#define	METER_FACTORY_SET 900
#define METER_PAVR_ZERO   1000
#define	METER_ADJ_END     1100
#define	METER_SAVE_END    METER_ADJ_END+1
#define	METER_ZERO_ERR    1200
#define	METER_SPAN_ERR    METER_ZERO_ERR+1

char Phase;
char ZeroExe;
void menu_display_adjust(char no)
{  
  if (no == 0)       printf("<Power Meter Adjust>");
  else if (no == 1)  printf("\nPT 1st Volt Set   ");
  else if (no == 2)  printf("\nCT 1st Ampare Set ");
  else if (no == 3)  printf("\nVolt Zero Clear   ");
  else if (no == 4)  printf("\nVolt[R-S] Adjust  ");
  else if (no == 5)  printf("\nVolt[S-T] Adjust  ");
  else if (no == 6)  printf("\nVolt[T-R] Adjust  ");
  else if (no == 7)  printf("\nCurrent Zero Clear");
  else if (no == 8)  printf("\nCurrent [R] Adjust");
  else if (no == 9)  printf("\nCurrent [S] Adjust");
  else if (no == 10) printf("\nCurrent [T] Adjust");
  else if (no == 11) printf("\nP[Watt] Span Adjust");
  else if (no == 12) printf("\nP[var] Zero Clear ");
  else if (no == 13) printf("\nP[var] Span Adjust ");
  else if (no == 14) printf("\nMeter Factory Set ");
  else if (no == 15) printf("\nReturn To Main    ");
}


int rate_range_limit(int v, int max, int min)
{
  if (v < min ) v = min;
  else if (v > max ) v = max;
  
  return v;
}

int setA, setB, setC;
int iRateMin, iRateMax;
char err;
#define MAX_GAIN  2000
#define MIN_GAIN  500
void meter_adjust_function(void)
{
 unsigned char lp, result;
 //int sum;

 switch (sExecStep)
 {   
  case 0:
    display_mode(0); 
    CursorUse = 1;
    MenuStart = 0;
    MenuEnd = 7;
    MenuSize = 16;
    sExecStep = METER_ADJ_MAIN;
    return;
    
 case METER_ADJ_MAIN:
    screen_clear();
    LineBlink = 1;
    DelayStep = 0;
    debug_monit(MONOUT);
    for (lp = MenuStart; lp <= MenuEnd; lp++) menu_display_adjust(lp);
    cursor_move_home();
    sExecStep++;
    return;
   
  case METER_ADJ_MAIN+1:
    if (MENU_UP) popup_menu_update(METER_ADJUST,UP); 
    else if (MENU_DN) popup_menu_update(METER_ADJUST,DOWN); 
    else if (ENTER_KEY) sExecStep = METER_ADJ_EXE; 
    printf("\r");
    return;
    
  case METER_ADJ_MAIN+2:
    sExecStep--;
    return;
    
  case METER_ADJ_EXE:
    DelayStep = 0;
    LineBlink = 0;
    MenuNo = MenuStart + find_cursor_vpos();
    if (MenuNo == 0) execmode_change(SYSTEM_TEST);//sExecStep = METER_ADJ_MAIN;
    else if (MenuNo == 1) sExecStep = METER_ADJ_PT;
    else if (MenuNo == 2) sExecStep = METER_ADJ_CT;
    else if (MenuNo == 3) sExecStep = METER_VOLT_ZERO;
    else if (MenuNo == 4) sExecStep = METER_VOLT_SPAN0;
    else if (MenuNo == 5) sExecStep = METER_VOLT_SPAN1;
    else if (MenuNo == 6) sExecStep = METER_VOLT_SPAN2;
    else if (MenuNo == 7) sExecStep = METER_AMP_ZEROA;
    else if (MenuNo == 8) sExecStep = METER_AMP_SPAN0;
    else if (MenuNo == 9) sExecStep = METER_AMP_SPAN1;
    else if (MenuNo == 10) sExecStep = METER_AMP_SPAN2;
    else if (MenuNo == 11) sExecStep = METER_WATT_SPAN;
    else if (MenuNo == 12) sExecStep = METER_PAVR_ZERO;
    else if (MenuNo == 13) sExecStep = METER_VAR_SPAN;
    else if (MenuNo == 14) sExecStep = METER_FACTORY_SET;
    else if (MenuNo == 15) execmode_change(SYSTEM_TEST);
    else sExecStep = METER_ADJ_MAIN; 
    return;
   
//*************************
//  PT 1st Volt Set       
//*************************
 case METER_ADJ_PT:
   DelayStep = 0;
   screen_clear();
   printf(">PT 1st Volt Set ");
   printf("\n>NOW:[%04d]V", iPT1stVolt); 
   iTempSet = iPT1stVolt; 
   printf("\n>ADJ:[%04d]V", iTempSet);  
   sExecStep++;
   return;

 case METER_ADJ_PT+1:
    if (wheel_input(0, 9999)) 
    {
      if (iPT1stVolt == iTempSet) sExecStep = METER_ADJ_MAIN;
      else
      {
        iPT1stVolt = iTempSet;
        printf("\n>NEW:[%04d]V", iPT1stVolt); 
        sExecStep = METER_SAVE_END;
      }
    }
    return; 
 
  case METER_ADJ_PT+2:
    printf("\r>ADJ:[%04d]V", iTempSet); 
    sExecStep--;
    return;

//*************************
//  CT 1st Current Set       
//*************************
 case METER_ADJ_CT:
   DelayStep = 0;
   screen_clear();
   printf(">CT 1st Amp Set ");
   //iCT1stAmp /= 10;
   printf("\n>NOW:[%05d]A", iCT1stAmp); 
   iTempSet = iCT1stAmp / 10; 
   printf("\n>ADJ:[%04d0]A", iTempSet);  
   sExecStep++;
   return;

  case METER_ADJ_CT+1:
    if (wheel_input(0, 9999)) 
    {
      if (iCT1stAmp == iTempSet*10) sExecStep = METER_ADJ_MAIN;
      else
      {
        iCT1stAmp = iTempSet * 10;
        printf("\n>NEW:[%05d]A", iCT1stAmp); 
        sExecStep = METER_SAVE_END;
      }
    }
    return; 
 
  case METER_ADJ_CT+2:
    printf("\r>NEW:[%04d0]A", iTempSet); 
    sExecStep--;
    return;

//******************************
//  AIrms Zero offset calibrate 
//******************************
 case METER_VOLT_ZERO:
   iADEregistor[ADE_AVRMS] = ADE7758_registor_read(ADE_AVRMS);
   iADEregistor[ADE_BVRMS] = ADE7758_registor_read(ADE_BVRMS);
   iADEregistor[ADE_CVRMS] = ADE7758_registor_read(ADE_CVRMS);
   sExecStep++;
   return;
   
 case METER_VOLT_ZERO+1:
   screen_clear();
   DelayStep = 0;
   printf("<Voltage Set to [0]>");
   voltage_calculate();
   printf("\n>NOW:V[RS]-%4d[V]", iVrmsRS);
   printf("\n>NOW:V[ST]-%4d[V]", iVrmsST);
   printf("\n>NOW:V[TR]-%4d[V]", iVrmsTR);
   if ((iADEregistor[ADE_AVRMS] < MAX_ZERO_VRMS)
      &(iADEregistor[ADE_BVRMS] < MAX_ZERO_VRMS)
      &(iADEregistor[ADE_CVRMS] < MAX_ZERO_VRMS)) sExecStep++;
   else sExecStep = METER_VOLT_ZERO+10;
   return;
   
 case METER_VOLT_ZERO+2:
   ZeroExe = 0;
   printf("\n>V[R/S/T] Set to [0]");
   printf("\n>Execute? [NO]");
   sExecStep++;
   return;

 case METER_VOLT_ZERO+3:
   if (ENTER_KEY) 
   {
     if (ZeroExe) sExecStep++; 
     else 
     {
       printf("\n>Canceled..");
       sExecStep = METER_ADJ_END;
     }
   }
   else if (ANY_KEY) 
   {
     ZeroExe ^= 1;
     goto_cursor(9, 5);
     if (!ZeroExe) printf("[NO] "); else printf("[YES]");
   } 
   return;
   
 case METER_VOLT_ZERO+4:  
   printf("\n>V[R]Source:%6d", iADEregistor[ADE_AVRMS]);   
   printf("\n>V[S]Source:%6d", iADEregistor[ADE_BVRMS]);   
   printf("\n>V[T]Source:%6d", iADEregistor[ADE_CVRMS]);
   sExecStep++;
   return;

 case METER_VOLT_ZERO+5:
   usAVrmsOffset -= iADEregistor[ADE_AVRMS] / VOLT_OFFSET_RATE;  
   usBVrmsOffset -= iADEregistor[ADE_BVRMS] / VOLT_OFFSET_RATE;  
   usCVrmsOffset -= iADEregistor[ADE_CVRMS] / VOLT_OFFSET_RATE;  
   sExecStep++;
   return;
   
 case METER_VOLT_ZERO+6:
   err = ADE7758_registor_write(ADE_AVRMSOS, usAVrmsOffset);
   err += ADE7758_registor_write(ADE_BVRMSOS, usBVrmsOffset);
   err += ADE7758_registor_write(ADE_CVRMSOS, usCVrmsOffset);
   sExecStep++;
   return;
   
 case METER_VOLT_ZERO+7:
   setA = ADE7758_registor_read(ADE_AVRMSOS);
   setB = ADE7758_registor_read(ADE_BVRMSOS);
   setC = ADE7758_registor_read(ADE_CVRMSOS);
   sExecStep++;
   return;
   
  case METER_VOLT_ZERO+8:
   printf("\n>Offset R:%4d", setA);
   printf("\n>Offset S:%4d", setB);
   printf("\n>Offset T:%4d", setC);
   if (!err) printf("\n>Zero Set Success");
    else printf("\n>Zero Set Fail..!");
   DelayStep = 0;
   sExecStep++;
   return;
   
 case METER_VOLT_ZERO+9:
   if (ANY_KEY) sExecStep = METER_SAVE_END;
    else if (step_delay(SEC_1*3)) sExecStep = METER_SAVE_END;
   return;

 case METER_VOLT_ZERO+10:
   if (ANY_KEY) sExecStep = METER_ZERO_ERR;
   else if (step_delay(SEC_1*3)) sExecStep = METER_ZERO_ERR;
   return;
   
//******************************
//  AIrms Zero offset calibrate 
//******************************
 case METER_AMP_ZEROA:
   iADEregistor[ADE_AIRMS] = ADE7758_registor_read(ADE_AIRMS);
   iADEregistor[ADE_BIRMS] = ADE7758_registor_read(ADE_BIRMS);
   iADEregistor[ADE_CIRMS] = ADE7758_registor_read(ADE_CIRMS);
   sExecStep++;
   return;
   
 case METER_AMP_ZEROA+1:
   screen_clear();
   DelayStep = 0;
   printf("<Current Set to [0]>");
   current_calculate();
   printf("\n>NOW:I[R]-%4d.%1d[A]", iIa1, iIa0);
   printf("\n>NOW:I[S]-%4d.%1d[A]", iIb1, iIb0);
   printf("\n>NOW:I[T]-%4d.%1d[A]", iIc1, iIc0);
   if ((iADEregistor[ADE_AIRMS] < MAX_ZERO_IRMS)
      &(iADEregistor[ADE_BIRMS] < MAX_ZERO_IRMS)
      &(iADEregistor[ADE_CIRMS] < MAX_ZERO_IRMS)) sExecStep++;
   else sExecStep = METER_AMP_ZEROA+10;
   return;
   
 case METER_AMP_ZEROA+2:
   ZeroExe = 0;
   printf("\n>I[R/S/T] Set to [0]");
   printf("\n>Execute? [NO]");
   sExecStep++;
   return;

 case METER_AMP_ZEROA+3:
   if (ENTER_KEY) 
   {
     if (ZeroExe) sExecStep++; 
     else 
     {
       printf("\n>Canceled..");
       sExecStep = METER_ADJ_END;
     }
   }
   else if (ANY_KEY) 
   {
     ZeroExe ^= 1;
     goto_cursor(9, 5);
     if (!ZeroExe) printf("[NO] "); else printf("[YES]");
   } 
   return;
   
 case METER_AMP_ZEROA+4:  
   printf("\n>I[R]Source:%6d", iADEregistor[ADE_AIRMS]);   
   printf("\n>I[S]Source:%6d", iADEregistor[ADE_BIRMS]);   
   printf("\n>I[T]Source:%6d", iADEregistor[ADE_CIRMS]);
   sExecStep++;
   return;

 case METER_AMP_ZEROA+5:
   usAIrmsOffset -= iADEregistor[ADE_AIRMS] / AMP_OFFSET_RATE;  
   usBIrmsOffset -= iADEregistor[ADE_BIRMS] / AMP_OFFSET_RATE;  
   usCIrmsOffset -= iADEregistor[ADE_CIRMS] / AMP_OFFSET_RATE;  
   sExecStep++;
   return;
   
 case METER_AMP_ZEROA+6:
   err = ADE7758_registor_write(ADE_AIRMSOS, usAIrmsOffset);
   err += ADE7758_registor_write(ADE_BIRMSOS, usBIrmsOffset);
   err += ADE7758_registor_write(ADE_CIRMSOS, usCIrmsOffset);
   sExecStep++;
   return;
   
 case METER_AMP_ZEROA+7:
   setA = ADE7758_registor_read(ADE_AIRMSOS);
   setB = ADE7758_registor_read(ADE_BIRMSOS);
   setC = ADE7758_registor_read(ADE_CIRMSOS);
   sExecStep++;
   return;
   
  case METER_AMP_ZEROA+8:
   printf("\n>Offset R:%4d", setA);
   printf("\n>Offset S:%4d", setB);
   printf("\n>Offset T:%4d", setC);
   if (!err) printf("\n>Zero Set Success");
    else printf("\n>Zero Set Fail..!");
   DelayStep = 0;
   sExecStep++;
   return;
   
 case METER_AMP_ZEROA+9:
   if (ANY_KEY) sExecStep = METER_SAVE_END;
    else if (step_delay(SEC_1*3)) sExecStep = METER_SAVE_END;
   return;

 case METER_AMP_ZEROA+10:
   if (ANY_KEY) sExecStep = METER_ZERO_ERR;
   else if (step_delay(SEC_1*3)) sExecStep = METER_ZERO_ERR;
   return;

//******************************
//  Pvar Zero offset calibrate 
//******************************
 case METER_PAVR_ZERO:
   if (iADEregistor[ADE_AVARHR] > 0x8000) iADEregistor[ADE_AVARHR] -= 0x10000;
   if (iADEregistor[ADE_BVARHR] > 0x8000) iADEregistor[ADE_BVARHR] -= 0x10000;
   if (iADEregistor[ADE_CVARHR] > 0x8000) iADEregistor[ADE_CVARHR] -= 0x10000;
   sExecStep++;
   return;
   
 case METER_PAVR_ZERO+1:
   screen_clear();
   DelayStep = 0;
   printf("<Pvar Set to [0]>");
   //printf("\n>NOW:Pa[R]-%5d", iADEregistor[ADE_AVARHR]);
   //printf("\n>NOW:Pa[S]-%5d", iADEregistor[ADE_BVARHR]);
   //printf("\n>NOW:Pa[T]-%5d", iADEregistor[ADE_CVARHR]);
   if ((iADEregistor[ADE_AVARHR] > MIN_ADJUST_VAR)
      |(iADEregistor[ADE_BVARHR] > MIN_ADJUST_VAR)
      |(iADEregistor[ADE_CVARHR] > MIN_ADJUST_VAR)
      |(iADEregistor[ADE_AVARHR] < -MIN_ADJUST_VAR)
      |(iADEregistor[ADE_BVARHR] < -MIN_ADJUST_VAR)
      |(iADEregistor[ADE_CVARHR] < -MIN_ADJUST_VAR))
     sExecStep = METER_AMP_ZEROA+10;
   else sExecStep++;
   return;
   
 case METER_PAVR_ZERO+2:
   ZeroExe = 0;
   printf("\n>Pvar[R/S/T] Clear");
   printf("\n>Execute? [NO] ");
   sExecStep++;
   return;

 case METER_PAVR_ZERO+3:
   if (ENTER_KEY) 
   {
     if (ZeroExe) sExecStep++; 
     else 
     {
       printf("\n>Canceled..");
       sExecStep = METER_ADJ_END;
     }
   }
   else if (ANY_KEY) 
   {
     ZeroExe ^= 1;
     goto_cursor(10, 2);
     if (!ZeroExe) printf("[NO] "); else printf("[YES]");
   } 
   return;
   
 case METER_PAVR_ZERO+4:
   printf("\n>Pa[R]Source:%6d", iADEregistor[ADE_AVARHR]);   
   printf("\n>Pa[S]Source:%6d", iADEregistor[ADE_BVARHR]);   
   printf("\n>Pa[T]Source:%6d", iADEregistor[ADE_CVARHR]);
   sExecStep++;
   return;

 case METER_PAVR_ZERO+5:
   usAVarOffset -= iADEregistor[ADE_AVARHR] / VAR_OFFSET_RATE;  
   usBVarOffset -= iADEregistor[ADE_BVARHR] / VAR_OFFSET_RATE;  
   usCVarOffset -= iADEregistor[ADE_CVARHR] / VAR_OFFSET_RATE;  
   sExecStep++;
   return;
   
 case METER_PAVR_ZERO+6:
   err = ADE7758_registor_write(ADE_AVAROS, usAVarOffset);
   err += ADE7758_registor_write(ADE_BVAROS, usBVarOffset);
   err += ADE7758_registor_write(ADE_CVAROS, usCVarOffset);
   sExecStep++;
   return;
   
 case METER_PAVR_ZERO+7:
   setA = ADE7758_registor_read(ADE_AVAROS);
   setB = ADE7758_registor_read(ADE_BVAROS);
   setC = ADE7758_registor_read(ADE_CVAROS);
   sExecStep++;
   return;
   
  case METER_PAVR_ZERO+8:
   printf("\n>Offset R:%5d", setA);
   printf("\n>Offset S:%5d", setB);
   printf("\n>Offset T:%5d", setC);
   if (!err) printf("\n>Zero Set Success");
    else printf("\n>Zero Set Fail..!");
   DelayStep = 0;
   sExecStep++;
   return;
   
 case METER_PAVR_ZERO+9:
   if (ANY_KEY) sExecStep = METER_SAVE_END;
    else if (step_delay(SEC_1*3)) sExecStep = METER_SAVE_END;
   return;

 case METER_PAVR_ZERO+10:
   if (ANY_KEY) sExecStep = METER_ZERO_ERR;
   else if (step_delay(SEC_1*3)) sExecStep = METER_ZERO_ERR;
   return;
   
//*************************
//   Vrs Span Calibrate          
//*************************
 case METER_VOLT_SPAN0:
   if (iADEregistor[ADE_AVRMS] > MIN_ADJUST_VRMS) sExecStep++;
    else sExecStep = METER_SPAN_ERR;
   return;

 case METER_VOLT_SPAN0+1:
   DelayStep = 0;
   screen_clear();
   printf(">Volt[RS] PAN Adjust");
   printf("\n>NOW:V[RS]-%04dV", iVrs); 
   iTempSet = iVrs; 
   printf("\n>ADJ:V[RS]-%04dV", iVrs);   
   iVrsRate = rate_range_limit(iVrsRate, MAX_GAIN, MIN_GAIN);
   iRateMin = iVrmsRS * MIN_GAIN / 1000;
   iRateMax = iVrmsRS * MAX_GAIN / 1000;
   sExecStep++;
   return;

  case METER_VOLT_SPAN0+2:
    if (wheel_input(iRateMin, iRateMax)) 
    {
      if (iVrs == iTempSet) sExecStep = METER_ADJ_MAIN;
      else
      {
        iVrsRate = iTempSet * 1000 / iVrmsRS;
        iVrsRate = rate_range_limit(iVrsRate, MAX_GAIN, MIN_GAIN);
        printf("\n>Gain:[R]-%3d.%1d%%", iVrsRate/10, iVrsRate%10 ); 
        iTempSet = iVrmsRS * iVrsRate / 1000;
        printf("\n>NEW:V[RS]-%04dV", iTempSet);
        sExecStep = METER_SAVE_END;
      }
    }
    return; 
 
  case METER_VOLT_SPAN0+3:
    printf("\r>ADJ:V[RS]-%04dV", iTempSet);    
    sExecStep--;
    return;
    
//*************************
//   Vst Span Calibrate          
//*************************
 case METER_VOLT_SPAN1:
   if (iADEregistor[ADE_BVRMS] > MIN_ADJUST_VRMS) sExecStep++;
    else sExecStep = METER_SPAN_ERR;
   return;

 case METER_VOLT_SPAN1+1:
   DelayStep = 0;
   screen_clear();
   printf(">Volt[ST] PAN Adjust");
   printf("\n>NOW:V[ST]-%04dV", iVst); 
   iTempSet = iVst; 
   printf("\n>ADJ:V[ST]-%04dV", iVst);   
   iVstRate = rate_range_limit(iVstRate, MAX_GAIN, MIN_GAIN);
   iRateMin = iVrmsST * MIN_GAIN / 1000;
   iRateMax = iVrmsST * MAX_GAIN / 1000;
   sExecStep++;
   return;

  case METER_VOLT_SPAN1+2:
    if (wheel_input(iRateMin, iRateMax)) 
    {
      if (iVst == iTempSet) sExecStep = METER_ADJ_MAIN;
      else
      {
        iVstRate = iTempSet * 1000 / iVrmsST;
        iVstRate = rate_range_limit(iVstRate, MAX_GAIN, MIN_GAIN);
        printf("\n>Gain:[R]-%3d.%1d%%", iVstRate/10, iVstRate%10 ); 
        iTempSet = iVrmsST * iVstRate / 1000;
        printf("\n>NEW:V[ST]-%04dV", iTempSet);
        sExecStep = METER_SAVE_END;
      }
    }
    return; 
 
  case METER_VOLT_SPAN1+3:
    printf("\r>ADJ:V[ST]-%04dV", iTempSet);    
    sExecStep--;
    return;

//*************************
//   Vrs Span Calibrate          
//*************************
 case METER_VOLT_SPAN2:
   if (iADEregistor[ADE_CVRMS] > MIN_ADJUST_VRMS) sExecStep++;
    else sExecStep = METER_SPAN_ERR;
   return;

 case METER_VOLT_SPAN2+1:
   DelayStep = 0;
   screen_clear();
   printf(">Volt[TR] PAN Adjust");
   printf("\n>NOW:V[TR]-%04dV", iVtr); 
   iTempSet = iVtr; 
   printf("\n>ADJ:V[TR]-%04dV", iVtr);   
   iVtrRate = rate_range_limit(iVtrRate, MAX_GAIN, MIN_GAIN);
   iRateMin = iVrmsTR * MIN_GAIN / 1000;
   iRateMax = iVrmsTR * MAX_GAIN / 1000;
   sExecStep++;
   return;

  case METER_VOLT_SPAN2+2:
    if (wheel_input(iRateMin, iRateMax)) 
    {
      if (iVrs == iTempSet) sExecStep = METER_ADJ_MAIN;
      else
      {
        iVtrRate = iTempSet * 1000 / iVrmsTR;
        iVtrRate = rate_range_limit(iVtrRate, MAX_GAIN, MIN_GAIN);
        printf("\n>Gain:[R]-%3d.%1d%%", iVtrRate/10, iVtrRate%10 ); 
        iTempSet = iVrmsTR * iVtrRate / 1000;
        printf("\n>NEW:V[TR]-%04dV", iTempSet);
        sExecStep = METER_SAVE_END;
      }
    }
    return; 
 
  case METER_VOLT_SPAN2+3:
    printf("\r>ADJ:V[TR]-%04dV", iTempSet);    
    sExecStep--;
    return;

//*************************
//   I[R] Span Calibrate          
//*************************
 case METER_AMP_SPAN0:
   //if (iADEregistor[ADE_AIRMS] > MIN_ADJUST_IRMS) sExecStep++;
   // else sExecStep = METER_SPAN_ERR;
   sExecStep++;
   return;
   
 case METER_AMP_SPAN0+1:
   DelayStep = 0;
   screen_clear();
   printf(">Current[R] Span Adjust");
   printf("\n>NOW:I[R]-%4d.%01d[A]", iIa1, iIa0); 
   iTempSet = iIa; 
   printf("\n>ADJ:I[R]-%4d.%01d[A]", iIa1, iIa0); 
   iIaRate = rate_range_limit(iIaRate, MAX_GAIN, MIN_GAIN);
   iRateMin = iAIrms * MIN_GAIN / 1000;
   iRateMax = iAIrms * MAX_GAIN / 1000;
   sExecStep++;
   return;

  case METER_AMP_SPAN0+2:
    if (wheel_input(iRateMin, iRateMax)) 
    {
      if (iAIrms == iTempSet) sExecStep = METER_ADJ_MAIN;
      else
      {
        iIaRate = iTempSet * 1000 / iAIrms;
        iIaRate = rate_range_limit(iIaRate, MAX_GAIN, MIN_GAIN);
        printf("\n>Gain:[R]-%3d.%1d%%", iIaRate/10,iIaRate%10 ); 
        iTempSet = iAIrms * iIaRate / 1000;
        printf("\n>NEW:I[R]-%4d.%01d[A]", iTempSet/10, iTempSet%10);   
        sExecStep = METER_SAVE_END;
      }
    }
    return; 
 
  case METER_AMP_SPAN0+3:
    printf("\r>ADJ:I[R]-%4d.%01d[A]", iTempSet/10, iTempSet%10);    
    sExecStep--;
    return; 

//*************************
//   I[S] Span Calibrate          
//*************************
 case METER_AMP_SPAN1:
   //if (iADEregistor[ADE_BIRMS] > MIN_ADJUST_IRMS) sExecStep++;
   // else sExecStep = METER_SPAN_ERR;
   sExecStep++;
   return;
   
 case METER_AMP_SPAN1+1:
   DelayStep = 0;
   screen_clear();
   printf(">Current[S] Span Adjust");
   printf("\n>NOW:I[S]-%4d.%01d[A]", iIb1, iIb0); 
   iTempSet = iIb; 
   printf("\n>ADJ:I[S]-%4d.%01d[A]", iIb1, iIb0); 
   iIbRate = rate_range_limit(iIbRate, MAX_GAIN, MIN_GAIN);
   iRateMin = iBIrms * MIN_GAIN / 1000;
   iRateMax = iBIrms * MAX_GAIN / 1000;
   sExecStep++;
   return;

  case METER_AMP_SPAN1+2:
    if (wheel_input(iRateMin, iRateMax)) 
    {
      if (iBIrms == iTempSet) sExecStep = METER_ADJ_MAIN;
      else
      {
        iIbRate = iTempSet * 1000 / iBIrms;
        iIbRate = rate_range_limit(iIbRate, MAX_GAIN, MIN_GAIN);
        printf("\n>Gain:[R]-%3d.%1d%%", iIbRate/10,iIbRate%10 ); 
        iTempSet = iBIrms * iIbRate / 1000;
        printf("\n>NEW:I[S]-%4d.%01d[A]", iTempSet/10, iTempSet%10);   
        sExecStep = METER_SAVE_END;
      }
    }
    return; 
 
  case METER_AMP_SPAN1+3:
    printf("\r>ADJ:I[S]-%4d.%01d[A]", iTempSet/10, iTempSet%10);    
    sExecStep--;
    return;     

//*************************
//   I[T] Span Calibrate          
//*************************
 case METER_AMP_SPAN2:
   //if (iADEregistor[ADE_CIRMS] > MIN_ADJUST_IRMS) sExecStep++;
   // else sExecStep = METER_SPAN_ERR;
   sExecStep++;
   return;
   
 case METER_AMP_SPAN2+1:
   DelayStep = 0;
   screen_clear();
   printf(">Current[T] Span Adjust");
   printf("\n>NOW:I[T]-%4d.%01d[A]", iIc1, iIc0); 
   iTempSet = iIc; 
   printf("\n>ADJ:I[T]-%4d.%01d[A]", iIc1, iIc0); 
   iIcRate = rate_range_limit(iIcRate, MAX_GAIN, MIN_GAIN);
   iRateMin = iCIrms * MIN_GAIN / 1000;
   iRateMax = iCIrms * MAX_GAIN / 1000;
   sExecStep++;
   return;

  case METER_AMP_SPAN2+2:
    if (wheel_input(iRateMin, iRateMax)) 
    {
      if (iCIrms == iTempSet) sExecStep = METER_ADJ_MAIN;
      else
      {
        iIcRate = iTempSet * 1000 / iCIrms;
        iIcRate = rate_range_limit(iIcRate, MAX_GAIN, MIN_GAIN);
        printf("\n>Gain:[R]-%3d.%1d%%", iIcRate/10,iIcRate%10 ); 
        iTempSet = iCIrms * iIcRate / 1000;
        printf("\n>NEW:I[T]-%4d.%01d[A]", iTempSet/10, iTempSet%10);   
        sExecStep = METER_SAVE_END;
      }
    }
    return; 
 
  case METER_AMP_SPAN2+3:
    printf("\r>ADJ:I[T]-%4d.%01d[A]", iTempSet/10, iTempSet%10);    
    sExecStep--;
    return;         

/******************************/
/* P[Watt] Span Calibrate     */
/******************************/
 case METER_WATT_SPAN:
   //abs_power_register();
   //sum = iADEregistor[ADE_AWATTHR] + iADEregistor[ADE_BWATTHR] + iADEregistor[ADE_CWATTHR];
   //if (sum > MIN_ADJUST_WATT) sExecStep++;
   // else sExecStep = METER_SPAN_ERR;
   sExecStep++;
   return;
   
 case METER_WATT_SPAN+1:
   DelayStep = 0;
   screen_clear();
   printf(">P[Watt] Span Adjust");
   printf("\n>NOW:P[W]-%06dW", iPw); 
   iTempSet = iPw/10; 
   printf("\n>ADJ:P[W]-%06dW", iPw);  
   iWattRate = rate_range_limit(iWattRate, MAX_GAIN, MIN_GAIN);
   iRateMin = iWattSum * MIN_GAIN / 10000;
   iRateMax = iWattSum * MAX_GAIN / 10000;
   sExecStep++;
   return;

  case METER_WATT_SPAN+2:
    if (wheel_input(iRateMin, iRateMax))
    {
      if (iPw == iTempSet) sExecStep = METER_ADJ_MAIN;
      else
      {
        iWattRate = iTempSet * 10000 / iWattSum;
        iWattRate = rate_range_limit(iWattRate, MAX_GAIN, MIN_GAIN);
        printf("\n>Gain:[R]-%3d.%1d%%", iWattRate/10, iWattRate%10 ); 
        iTempSet = iWattSum * iWattRate / 1000;
        printf("\n>NEW:P[W]-%06dW", iTempSet); 
        sExecStep = METER_SAVE_END;
      }
    }
    return; 
 
  case METER_WATT_SPAN+3:
    printf("\r>ADJ:P[W]-%06dW", iTempSet*10);  
    sExecStep--;
    return;         

/*****************************/
/* P[var] Span Calibrate     */
/*****************************/
 case METER_VAR_SPAN:
   //abs_power_register();
   //sum = iADEregistor[ADE_AVARHR] + iADEregistor[ADE_BVARHR] + iADEregistor[ADE_CVARHR];
   //if (sum > MIN_ADJUST_VAR) sExecStep++;
   //else sExecStep = METER_SPAN_ERR;
   sExecStep++;
   return;
   
 case METER_VAR_SPAN+1:
   DelayStep = 0;
   screen_clear();
   printf(">P[var] Span Adjust");
   printf("\n>NOW:P[var]-%06dr", iPvar); 
   iTempSet = iPvar/10; 
   printf("\n>ADJ:P[var]-%06dr", iPvar);  
   iVarRate = rate_range_limit(iVarRate, MAX_GAIN, MIN_GAIN);
   iRateMin = iVarSum * MIN_GAIN / 10000;
   iRateMax = iVarSum * MAX_GAIN / 10000;
   sExecStep++;
   return;

  case METER_VAR_SPAN+2:
    if (wheel_input(iRateMin, iRateMax)) 
    {
      if (iPvar == iTempSet) sExecStep = METER_ADJ_MAIN;
      else
      {
        iVarRate = iTempSet * 10000 / iVarSum;
        iVarRate = rate_range_limit(iVarRate, MAX_GAIN, MIN_GAIN);
        printf("\n>Gain:[R]-%3d.%1d%%", iVarRate/10, iVarRate%10 ); 
        iTempSet = iVarSum * iVarRate / 1000;
        printf("\n>NEW:P[var]-%06dr", iTempSet); 
        sExecStep = METER_SAVE_END;
      }
    }
    return; 
 
  case METER_VAR_SPAN+3:
    printf("\r>ADJ:P[var]-%06dr", iTempSet*10);  
    sExecStep--;
    return;  
    
/***************************************/
/* return to Factory Default setting   */
/***************************************/
 case METER_FACTORY_SET:
   display_mode(2);
   printf("<파워메타초기화>");
   printf("주의:전력측정계수가 모두 초기화 됩니다.");
   sExecStep++;
   return;

 case METER_FACTORY_SET+1:
   ZeroExe = 0;
   printf("\n>Execute? [NO]");
   sExecStep++;
   return;

 case METER_FACTORY_SET+2:
   if (ENTER_KEY) 
   {
     if (ZeroExe) sExecStep++; 
     else 
     {
       printf("\n>Canceled..");
       sExecStep = METER_FACTORY_SET+4;
     }
   }
   else if (ANY_KEY) 
   {
     ZeroExe ^= 1;
     goto_cursor(9, 3);
     if (!ZeroExe) printf("[NO] "); else printf("[YES]");
   } 
   return; 
   
 case METER_FACTORY_SET+3:
   power_factory_setting();
   ADE7758_init();
   screen_clear();
   printf("전력측정계수가 모두 초기화 되었습니다..");
   sExecStep++;
   return;
   
 case METER_FACTORY_SET+4:
   if (ANY_KEY) sExecStep++;
    else step_delay(SEC_1*3);
   return;
   
 case METER_FACTORY_SET+5:
   display_mode(0);
   sExecStep = METER_SAVE_END;
   return;

//*************************************************
//   교정이 불가능한 값인 경우 안내문 표시
//*************************************************
  case METER_ZERO_ERR:
   DelayStep = 0;
   display_mode(2);
   printf("영점조정가능값 이상이므로 교정할 수 없습니다.");
   sExecStep = METER_SPAN_ERR+1;
   return;
   
  case METER_SPAN_ERR:
   DelayStep = 0;
   display_mode(2);
   printf("최소교정가능값 이하이므로 교정할 수 없습니다.");
   sExecStep++;
   return;

 case METER_SPAN_ERR+1:
    if (ANY_KEY) sExecStep++;
    else step_delay(SEC_1*3);
   return;
   
 case METER_SPAN_ERR+2:
   display_mode(0);
   sExecStep = METER_ADJ_MAIN;
   return;
   
//*************************************************
//   변경된 데이터를 저장하고 
//   3초간 지연 또는 키입력으로 메인메뉴로 복귀
//*************************************************
  case METER_SAVE_END:
    result = meter_data_save();
    if (result == true) printf("\nAdjust Data Save OK");
    else 
    {
      printf("\nBackup Memory Error!!");
      printf("\nData Save Fail..");
    }
    DelayStep = 0;
    sExecStep++;
    return;
    
  case METER_SAVE_END+1:
    if (ANY_KEY) sExecStep = METER_ADJ_MAIN;
    else if (step_delay(SEC_1*3)) sExecStep = METER_ADJ_MAIN;
    return;    
//
//   1Sec 지연후 메인메뉴로 복귀
//
  case METER_ADJ_END:
    DelayStep = 0;
    sExecStep = METER_SAVE_END+1;
    return;
    
  default: sExecStep = METER_ADJ_MAIN; return;
 }
}
/***************************/
/*  ADE7758 Test functions */
/***************************/
// ADE7758 Execute step define
#define	ADETEST_MAIN	10
#define	ADETEST_EXE     20
#define	ADE_RMS_MONIT	100
#define	ADE_ALL_VIEW	200
#define	ADE_REG_VIEW	300
#define	ADE_REG_SET	400
#define	ADE_REG_INIT    500
#define	ADE_VER_VIEW	600
#define	ADE_OUT_TEST	700
#define	ADE_IN_MONIT	800
#define ADETEST_END    1000

/*
void print_ade7758_register(void)
{
  char size;
  size = ADE_REG_LENGTH[ADEadd];
  if (size == 12) printf("%02X:   %02X ", ADEadd, iADEregistor[ADEadd]*0xFF);
  else if (size == 16) printf("%02X:  %02X ", ADEadd, iADEregistor[ADEadd]*0xFFFF);
  else if (size == 24) printf("%02X:%06X ", ADEadd, iADEregistor[ADEadd]*0xFFFF);
  else printf("%02X:    %02X ", ADEadd, iADEregistor[ADEadd]*0xFFFF);
}

void ade7758_register_display(void)
{
  char lp;
   for (lp = 0; lp <16 ; lp++)
    {
      if ((lp & 1) == 0) printf("\n");
      if (ADEadd <= ADE_REG_LENGTH[ADEadd]) print_ade7758_register();
      ADEadd++;
    }
}
*/

void menu_display_adetest(char no)
{  
  if (no == 0)       printf("<<AD7758 Test>>");
  else if (no == 1)  printf("\nRMS Power Monit");
  else if (no == 2)  printf("\nAll Register View");
  else if (no == 3)  printf("\nOne Register View");
  else if (no == 4)  printf("\nOne Register Set");
  else if (no == 5)  printf("\nADE7758 Initialize");
  else if (no == 6)  printf("\nVersion & CheckSum");
  else if (no == 7)  printf("\nADE7758 I/O Test");  
  else if (no == 8)  printf("\nReturn To Menu");
}

char TempAdd;
void ade_test_function(void)
{
 unsigned char lp;
 //int sum, rate;
 short dg0, dg1;
 //double dsum;

 switch (sExecStep)
 {   
  case 0:
    display_mode(0); 
    debug_monit(MONOUT);
    CursorUse = 1;
    MenuStart = 0;
    MenuEnd = 7;
    MenuSize = 9;
    sExecStep = ADETEST_MAIN;
    return;
    
  case ADETEST_MAIN:
    screen_clear();
    LineBlink = 1;
    DelayStep = 0;
    for (lp = MenuStart; lp <= MenuEnd; lp++) menu_display_adetest(lp);
    cursor_move_home();
    sExecStep++;
    return;
    
  case ADETEST_MAIN+1:
    if (MENU_UP) popup_menu_update(ADE_TEST,UP); 
    else if (MENU_DN) popup_menu_update(ADE_TEST,DOWN); 
    else if (ENTER_KEY) sExecStep = ADETEST_EXE;
    return;
   
  case ADETEST_MAIN+2:
    sExecStep--;
    return;
  
  case ADETEST_EXE:
    DelayStep = 0;
    LineBlink = 0;
    MenuNo = MenuStart + find_cursor_vpos();
    if (MenuNo == 0) execmode_change(SYSTEM_TEST);
    else if (MenuNo == 1) sExecStep = ADE_RMS_MONIT;
    else if (MenuNo == 2) sExecStep = ADE_ALL_VIEW;
    else if (MenuNo == 3) sExecStep = ADE_REG_VIEW;
    else if (MenuNo == 4) sExecStep = ADE_REG_SET;
    else if (MenuNo == 5) sExecStep = ADE_REG_INIT;
    else if (MenuNo == 6) sExecStep = ADE_VER_VIEW;
    else if (MenuNo == 7) sExecStep = ADE_OUT_TEST;
    else if (MenuNo == 8) execmode_change(SYSTEM_TEST);
    else sExecStep = ADETEST_MAIN;
    return;
   
  case ADE_OUT_TEST:
    execmode_change(ADEIO_TEST);
    return;
    
  case ADE_REG_VIEW:
    screen_clear();
    printf(">ADE7758 Reg. READ");
    sExecStep++;
    return;
    
  case ADE_REG_VIEW+1:
    printf("\nADEreg[%02X]:", ADEadd);
    TempAdd = ADEadd; 
    sExecStep++;
    return;
                
  case ADE_REG_VIEW+2:
    if (COUNT_UP) 
    {
      if (TempAdd < ADE7758_REG_SIZE) TempAdd++;
      if (TempAdd > ADE7758_REG_SIZE) TempAdd = 1;
      printf("\rADEreg[%02X]:", TempAdd);
    }
    else if (COUNT_DN) 
    {
      if (TempAdd > 1) TempAdd--;
      if (TempAdd < 1) TempAdd = ADE7758_REG_SIZE;
      printf("\rADEreg[%02X]:", TempAdd);
    }
    else if (ENTER_KEY) 
    {
      ADEadd = TempAdd;
      printf("\rADEreg[%02X]:", ADEadd);
      sExecStep++;
    }
    return;
 
  case ADE_REG_VIEW+3:
     iADEregistor[ADEadd] = ADE7758_registor_read(ADEadd);
     printf("-%6X", iADEregistor[ADEadd]);
     //print_ade7758_register();
     sExecStep++;
    return;
    
  case ADE_REG_VIEW+4:
    if (ENTER_KEY) sExecStep = ADETEST_MAIN;
    else if (ANY_KEY) sExecStep = ADE_REG_VIEW+1;
    return;
  
  case ADE_REG_SET:
    screen_clear();
    printf(">ADE7758 Reg. Write");
    sExecStep++;
    return;
    
  case ADE_REG_SET+1:
    printf("\n[%02X]", ADEadd);
    TempAdd = ADEadd; 
    sExecStep++;
    return;
                   
  case ADE_REG_SET+2:  
    if (COUNT_UP) 
    {
      if (TempAdd < ADE7758_REG_SIZE) TempAdd++;
      if (TempAdd > ADE7758_REG_SIZE) TempAdd = 1;
      printf("\r[%02X]", TempAdd);
    }
    else if (COUNT_DN) 
    {
      if (TempAdd > 1) TempAdd--;
      if (TempAdd < 1) TempAdd = ADE7758_REG_SIZE;
      printf("\r[%02X]", TempAdd);
    }
    else if (ENTER_KEY) 
    {
      ADEadd = TempAdd;
      printf("\r[%02X]", ADEadd);
      if (ADE_WRITE_ENABLE[ADEadd] == 1) sExecStep++;
      else sExecStep = ADE_REG_SET + 20;
    }
    return;

  case ADE_REG_SET+3:
     iADEregistor[ADEadd] = fit_reg_size(ADEadd, ADE7758_registor_read(ADEadd));
     sExecStep++;
    return;

  case ADE_REG_SET+4:
    iADEdata = fit_reg_size(ADEadd, iADEregistor[ADEadd]);
    printf("\r[%02X]%6X->%6X", ADEadd, iADEregistor[ADEadd], iADEdata );
    sExecStep++;
    return;
    
  case ADE_REG_SET+5:
    if (COUNT_UP) 
    {
      iADEdata++;
      iADEdata = fit_reg_size(ADEadd, iADEdata);
      printf("\r[%02X]%6X->%6X", ADEadd, iADEregistor[ADEadd], iADEdata );
    }
    else if (COUNT_DN) 
    {
      iADEdata--;
      iADEdata = fit_reg_size(ADEadd, iADEdata);
      printf("\r[%02X]%6X->%6X", ADEadd, iADEregistor[ADEadd], iADEdata );
    }
    else if (ENTER_KEY) sExecStep++;
    return;
    
  case ADE_REG_SET+6:
    ADE7758_registor_write(ADEadd, iADEdata);
    sExecStep++;
    return;
  
  case ADE_REG_SET+7:
    iADEregistor[ADEadd] = fit_reg_size(ADEadd, ADE7758_registor_read(ADEadd));
    if (iADEregistor[ADEadd] == iADEdata)
    {
       printf("\r[%02X]%6X-> OK   ", ADEadd, iADEdata);
       iADEregistor[ADEadd] = iADEdata;
    }
     else printf("\r[%02X]%6X->%6X", ADEadd, iADEregistor[ADEadd], iADEdata);
    sExecStep++;
    return;
    
  case ADE_REG_SET+8:
    if (ENTER_KEY) sExecStep = ADETEST_MAIN;
    else if (ANY_KEY) sExecStep = ADE_REG_SET+1;
    return;

  case ADE_REG_SET+20:
    printf(">!Read Only");
    DelayStep = 0;
    sExecStep++;
    return;
    
  case ADE_REG_SET+21:
    if (ANY_KEY) sExecStep = ADE_REG_SET+1;
    else if (step_delay(SEC_1)) sExecStep = ADE_REG_SET+1;
    return;    
 
 
 case ADE_ALL_VIEW:
    ADE7758_buffer_clear(0, ADE7758_REG_SIZE);
    sExecStep++;
    return;
    
  case ADE_ALL_VIEW+1:
    ADE7758_registor_block_read(1, ADE7758_REG_SIZE);
    sExecStep++;
    return;
      
  case ADE_ALL_VIEW+2:
    sExecStep++;
    return;
  
  case ADE_ALL_VIEW+3:
    screen_clear();
    ADEadd = 1;
    sExecStep++;
    return;
    
  case ADE_ALL_VIEW+4:  
    for (lp = 0; lp <16 ; lp++)
    {
      if ((lp & 1) == 0) printf("\n");
      if (ADEadd <= ADE7758_REG_SIZE)
      {
        printf("%02X:%6X ", ADEadd, iADEregistor[ADEadd]);
      }
      ADEadd++;
    }
    sExecStep++;
    return;
  
  case ADE_ALL_VIEW+5:
    if (ROLL_DN) 
    {
      if (ADEadd >= ADE7758_REG_SIZE) sExecStep = ADE_ALL_VIEW;
      else sExecStep = ADE_ALL_VIEW + 4;
    }
    else if (ROLL_UP) 
    {
      if (ADEadd <= 0x11) ADEadd = 0x41;
      else ADEadd -= 0x20;
      sExecStep = ADE_ALL_VIEW + 4;
    }
    else if (ENTER_KEY) sExecStep = ADETEST_MAIN; 
    return;
    
 //
 // ADE7758 RMS Value monit 
 //
  case ADE_RMS_MONIT:
    screen_clear();
    printf("<Power Data Monit>");
    sExecStep++;
    return;
   
  case ADE_RMS_MONIT+1: ADE7758_registor_block_read(1, 4); sExecStep++; return;
  case ADE_RMS_MONIT+2: ADE7758_registor_block_read(5, 4); sExecStep++; return;
  case ADE_RMS_MONIT+3: ADE7758_registor_block_read(9, 4); sExecStep++; return;
  case ADE_RMS_MONIT+4: ADE7758_registor_block_read(13,4); sExecStep++; return;
   
  case ADE_RMS_MONIT+5:
    voltage_calculate();
    sExecStep++;
    return;

  case ADE_RMS_MONIT+6:
    current_calculate();
    //printf("Vt:%6X %6X %6X", iADEregistor[ADE_AVRMS],iADEregistor[ADE_BVRMS],iADEregistor[ADE_CVRMS] );
    //printf("\nCt:%5d %5d %5d", iADEregistor[ADE_AIRMS],iADEregistor[ADE_BIRMS],iADEregistor[ADE_CIRMS] );
    //unsigned int uiPT1Volt, uiPT2Volt;
    //unsigned int uiCT1Amp, uiCT2Amp;
    //unsigned int uiAVrms, usBVrms, usCVrms;
    //unsigned int uAIrms, usBIrms, usCIrms;
    //unsigned short usAVrmsDivider, usBVrmsDivider, usCVrmsDivider;
    //unsigned short usAIrmsDivider, usBIrmsDivider, usCIrmsDivider;
    sExecStep++;
    return;
 
  case ADE_RMS_MONIT+7:
    goto_cursor(0,0);
    //printf("\nV[RS]%4d IA:%4d.%01d", iVrs, iAIrms1, iAIrms0 );
    //printf("\nV[ST]%4d IB:%4d.%01d", iVst, iBIrms1, iBIrms0 );
    //printf("\nV[TR]%4d IC:%4d.%01d", iVtr, iCIrms1, iCIrms0 );
    printf("\nVA:%6d IA:%6d", iADEregistor[ADE_AVRMS],iADEregistor[ADE_AIRMS] );
    printf("\nVB:%6d IB:%6d", iADEregistor[ADE_BVRMS],iADEregistor[ADE_BIRMS] );
    printf("\nVC:%6d IC:%6d", iADEregistor[ADE_CVRMS],iADEregistor[ADE_CIRMS] );
    //printf("\nVA:%6d IA:%4d.%01d", iAVrms, iAIrms1, iAIrms0 );
    //printf("\nVB:%6d IB:%4d.%01d", iBVrms, iBIrms1, iBIrms0 );
    //printf("\nVC:%6d IC:%4d.%01d", iCVrms, iCIrms1, iCIrms0 );
    sExecStep++;
    return;
 
  case ADE_RMS_MONIT+8:
    //rate = iPT1stVolt * iCT1stAmp * iWattRate / 1000 / iPT2stVolt / iCT2stAmp; 
    printf("\nPw:%5d %5d %5d", iADEregistor[ADE_AWATTHR],iADEregistor[ADE_BWATTHR],iADEregistor[ADE_CWATTHR] );
    printf("\nPr:%5d %5d %5d", iADEregistor[ADE_AVARHR],iADEregistor[ADE_BVARHR],iADEregistor[ADE_CVARHR] );
    sExecStep++;
    return;
    
  case ADE_RMS_MONIT+9:
    power_factor_calculate();
    sExecStep++;
    return;
 
  case ADE_RMS_MONIT+10: 
    dg1 = iPowerFactor / 10;
    dg0 = iPowerFactor % 10;
    printf("\n%5dW %5dvr %2d.%01d%%", iWattSum, iVarSum, dg1, dg0 );
    DelayStep = 0;
    sExecStep++; 
    return;
 
  case ADE_RMS_MONIT+11:
    if (ENTER_KEY) sExecStep = ADETEST_MAIN;
    else if (step_delay(SEC_1-10)) sExecStep = ADE_RMS_MONIT+1; 
    return;   
//
//   ADE7758 Register Init..
//
  case ADE_REG_INIT:
    ADE7758_init();
    screen_clear();
    printf("ADE7758 Register Init..");
    sExecStep = ADETEST_END;
    return;  
//
//   ADE7758 Version & Register Check Sum View
//
  case ADE_VER_VIEW:
    screen_clear();
    AdeVersion = ADE7758_registor_read(ADE_VERSION);
    AdeChkSum = ADE7758_registor_read(ADE_CHKSUM);
    sExecStep++;
    return;
    
   case ADE_VER_VIEW+1:
    printf(">ADE7758 Version:%02X", AdeVersion);
    printf("\n>Register ChkSum:%02X", AdeChkSum);
    DelayStep = 0;
    sExecStep++;
    return;
    
  case ADE_VER_VIEW+2:
    if (ANY_KEY) sExecStep = ADETEST_MAIN;
    else if (step_delay(SEC_1*4)) sExecStep = ADETEST_MAIN;
    return; 
    
//
//   1Sec 지연후 메인메뉴로 복귀
//
  case ADETEST_END:
    DelayStep = 0;
    sExecStep++;
    return;
    
  case ADETEST_END+1:
    if (ANY_KEY) sExecStep = ADETEST_MAIN;
    else if (step_delay(SEC_1)) sExecStep = ADETEST_MAIN;
    return;    
    
  default:
    printf("\n?Error:ADE_TEST");
    sExecStep = 1;
    return;     
 };
}

/************************************/
/*   ADE7758 IO PIN test functions  */
/************************************/
#define ADE_IOTEST_MAIN   10
#define ADE_IOTEST_EXE    20
    
void adeout_togle(char no)
{
  if (no == 1) PMenable(TOGLE);
  else if (no == 2) PMclock(TOGLE);
  else if (no == 3) PMdataout(TOGLE);
}

void menu_display_adeiotest(char no)
{
  short pos;   
  if (no == 0)      printf("<<ADE7758 OUT Test>>");
  else if (no == 1)  {printf("\nChip Enable "); if (PMenableIs)  printf("[H] "); else printf("[L]");}
  else if (no == 2)  {printf("\nCLOCK       "); if (PMclockIs)   printf("[H] "); else printf("[L]");}
  else if (no == 3)  {printf("\nData OUT    "); if (PMdataoutIs) printf("[H] "); else printf("[L]");}
  else if (no == 4)  printf("\nReturn To Menu");
  pos = sCurPos; 
  goto_cursor(0, 5);
  printf("<<IN Status>>");
  printf("\nPM_DIN      "); if (pio_read(PIOA) & PM_DOUT) printf("[H] "); else printf("[L]");
  printf("\nPM_IRQ      "); if (pio_read(PIOA) & PM_IRQ) printf("[H] "); else printf("[L]");
  sCurPos = pos; 
}

void adeio_test_function(void)
{ 
  short lp;
  
  switch (sExecStep)
 {   
  case 0:
    display_mode(0); 
    debug_monit(MONOUT);
    CursorUse = 1;
    MenuStart = 0;
    MenuEnd = 4;
    MenuSize = 5;
    sExecStep = ADE_IOTEST_MAIN;
    return;
    
  case ADE_IOTEST_MAIN:
    screen_clear();
    LineBlink = 1;
    DelayStep = 0;
    for (lp = MenuStart; lp <= MenuEnd; lp++) menu_display_adeiotest(lp);
    cursor_move_home();
    sExecStep++;
    return;
  
  case ADE_IOTEST_MAIN+1:
    if (ROLL_UP) popup_menu_update(ADEIO_TEST,UP); 
    else if (ROLL_DN) popup_menu_update(ADEIO_TEST,DOWN); 
    else if (ENTER_KEY) sExecStep = ADE_IOTEST_EXE; 
    //printf("\r");
    return;

  case ADE_IOTEST_MAIN+2:
    sExecStep--;
    return;
    
  case ADE_IOTEST_EXE:
    DelayStep = 0;
    MenuNo = MenuStart + find_cursor_vpos();
    if (MenuNo == 0) sExecStep = ADE_IOTEST_MAIN;
    else if (MenuNo == 4) execmode_change(ADE_TEST);
    else 
    {
      adeout_togle(MenuNo);
      cursor_move_up();
      menu_display_adeiotest(MenuNo);
      sExecStep = ADE_IOTEST_MAIN+1; 
    }
    return;

  default:
    sExecStep = ADE_IOTEST_MAIN;
    return;
  }
}

short sMeasureDelay;
char MeasureStep;
char ADEregNo;
char AcOverErr;
char AcLowVoltErr;
int iAcLowVolt;
// 전압 및 전력 계측
void AV_measure_ADE7758(void)
{
  switch(MeasureStep)
  {
  case 0:
    if (ExecMode != ADE_TEST) MeasureStep++;
    return;
    
  case 1:
    ADE7758_init();
    MeasureStep++;
    return;
  
  case 2: 
    sMeasureDelay = 0;
    ADEregNo = 1;
    MeasureStep++;
    return;


  case 3:
    iADEregistor[ADEregNo] = ADE7758_registor_read(ADEregNo);
    MeasureStep++;
    return;
    
  case 4:
    if (++ADEregNo > 16) MeasureStep++; else MeasureStep--;
    return;

  case 5: 
    PwMeterError = ADE7758_check();
    if (PwMeterError == 0) MeasureStep++; 
    else MeasureStep = 20;    
    return;    
  
  case 6:
    voltage_calculate();
    MeasureStep++;
    return;

  case 7:
    current_calculate();
    MeasureStep++;
    return;
 
  case 8:
    power_factor_calculate();
    MeasureStep++; 
    return;

  case 9:
    if (!AcOverErr) AcOverErr = ac_over_current_check();
    MeasureStep++; 
    return;
    
  case 10:
    AcLowVoltErr = ac_low_voltage_check();
    MeasureStep++; 
    return;

  case 11:
    if (++sMeasureDelay > SEC_1-40) MeasureStep = 2;
    return;

  case 20:
    if (++sMeasureDelay > SEC_1) MeasureStep = 2;
    return;
    
  default: MeasureStep = 0; return;
  }
}

void print_power_korean(void)
{
  int dg0, dg1, kw0, kw1;
  sCurPos = 0;  
  printf(" <전력사용현황>");
  goto_cursor(0,1); 
  //printf("%02X월%02X일%2X:%02X:%02X", Month, Date, Hour, Minute, Sec);
  printf("[V]%4d%4d%4d", iVrs, iVst, iVtr);
  goto_cursor(0,2);
  printf("[A]%4d%4d%4d", iIa1, iIb1, iIc1);
  dg1 = iPowerFactor / 10;
  dg0 = iPowerFactor % 10;
  kw1 = iWattSum / 1000;
  kw0 = iWattSum % 1000;
  kw0 = kw0 / 100;
  goto_cursor(0,3);
  if (kw1 < 100) printf("[P]%2d.%01dkW %2d.%01d%%", kw1, kw0, dg1, dg0 );
  else if (kw1 < 10000) printf("[P]%4dkW %2d.%01d%%", kw1, dg1, dg0 ); 
  else printf("[P]%5dkW %2d%%", kw1, dg1); 
  //printf("%8x%7x", iWattSum, iPowerFactor );
}

void print_power_line1(void)
{
  sCurPos = 0;  
  printf(" <전력사용현황>");
  //printf("%02X월%02X일%2X:%02X:%02X", Month, Date, Hour, Minute, Sec);
}

void print_power_line2(void)
{
  goto_cursor(0,1); 
  printf("%4d %4d %4dV", iVrs, iVst, iVtr);
}

void print_power_line3(void)
{
  goto_cursor(0,2);
  printf("%4d %4d %4dA", iIa1, iIb1, iIc1);
}

void print_power_line4(void)
{
  int dg0, dg1, kw0, kw1;
  dg1 = iPowerFactor / 10;
  dg0 = iPowerFactor % 10;
  kw1 = iPw / 1000;
  kw0 = iPw % 1000;
  kw0 = kw0 / 100;
  goto_cursor(0,3);
  
  if (kw1 < 100) 
  {
    printf("%2d.%01dkW", kw1, kw0 );
    if (dg1 > 99) printf(" Pf:100%%");
    else printf(" pf:%2d.%01d%%", dg1, dg0 );
  }
  else if (kw1 < 10000) 
  {
    printf("%4dkW", kw1); 
    if (dg1 > 99) printf(" Pf:100%%");
    else printf(" pf:%2d.%01d%%", dg1, dg0 );
  }
  else 
  {
    printf("%5dkW", kw1);
    if (dg1 > 99) printf(" Pf:100%%");
    else printf(" pf:%2d%%", dg1);
  }
}

/*************************/
/*  POWER Meter Display  */
/*************************/         
void power_status_view(void)
{
 switch (sExecStep)
 {   
  case 0:
    display_mode(2);
    debug_monit(MONOUT);
    LineBlink = 0;
    sExecStep++;
    return;

  case 1:
    screen_clear();
    DelayStep = 0;   
    sExecStep++;
    return;
    
  case 2:
    CursorUse = 0;
    print_power_line1();
    if (ENTER_KEY) execmode_change(MAIN_MENU);
    else sExecStep++;
    return;

  case 3:
    CursorUse = 0;
    print_power_line2();
    if (ENTER_KEY) execmode_change(MAIN_MENU);
    else sExecStep++;
    return;
    
  case 4:
    CursorUse = 0;
    print_power_line3();
    if (ENTER_KEY) execmode_change(MAIN_MENU);
    else sExecStep++;
    return;
    
  case 5:
    CursorUse = 0;
    print_power_line4();
    if (ENTER_KEY) execmode_change(MAIN_MENU);
    else sExecStep++;
    return;

  case 6:   
    CursorUse = 1;
    if (ENTER_KEY) execmode_change(MAIN_MENU);
    else if (step_delay(SEC_1-6)) sExecStep = 1;
    return;
    
  default:
    sExecStep = 0;
    return;
  }  
}  
