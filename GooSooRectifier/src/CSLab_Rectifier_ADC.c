

// Include Standard LIB  files
#include "project.h"

#define AD_CH_VOLT          0
#define AD_CH_AMP           1 
#ifdef MONO_POLE
  #define BIT_PER_mV    5827  
  #define BIT_PER_uA    9537  
  #define MAX_AMP_ADC   65536 
  #define MAX_VOLT_ADC  64356
#else
  #define BIT_PER_mV    11654
  #define BIT_PER_uA    19073
  #define MAX_AMP_ADC   32768
  #define MAX_VOLT_ADC  32178
#endif
#define STABLE_COUNT    100
#define PRE_PID_COUNT   100

float fAmpInput;
float fVoltInput;
//float fAmpOffSet;
//float fVoltOffSet;
int iOutAmp;
float fOutAmp;
float fOutVolt;
int iOutVolt;
int iOutAmp0;
int iOutVolt0;
int iRiseTime0;
int iAmpOffset;
int iVoltOffset;
int iMaxReactSpeed;
int iDacOutVolt;
int iDacOutAmp;
int iVoltGain;
int iAmpGain;
char PidStatus;

//************************************
// DAC OUT ERROR Verify
//************************************
#define DA_DIFF     100    // 500 mV
#define DA_ERR_RATE 50     // 5.0 %
char dacout_verify(void)
{
  char result;
  int dv, da;
  //, da, ea;
  result = 0;
  
  dv = iDacOutVolt - iDacMonVolt;
  if (iDacMonVolt > 1000) dv = iDacOutVolt - 1000;
  if (dv < 0) dv = dv * -1;
  da = iDacOutAmp - iDacMonAmp;
  if (iDacMonAmp > 1000) da = iDacOutAmp - 1000;
  if (da < 0) da = da * -1;
  
  //ev = dv * 1000 / iDacMonVolt;
  //ea = da * 1000 / iDacMonAmp;
  
  if (dv > DA_DIFF) result = 1;
  if (da > DA_DIFF) result = 1;
  //printf("\ndv:%d da:%d r:%d  ", dv, da, result);
  return result;
}  

void control_amp_out(float amp, float max)
{
    int da;
    if (amp >= max) amp = 65535;
    else amp = (amp / max) * 65536;
    da = amp;
    DAout_ad5663(0, 1, da);
    iDacOutAmp = da * 1000 / 65536;
    //iDacOutAmp = da;
}

void control_volt_out(float volt, float max)
{
    int da;
    if (volt >= max) volt = 65535;
    else volt = (volt / max) * 65536;
    da = volt;
    DAout_ad5663(0, 0, da);
    iDacOutVolt = da * 1000 / 65535;
}

#define MAX_INTERGAL_NO   50
float fIntergBuffer[MAX_INTERGAL_NO];
int iIntergBuffer[MAX_INTERGAL_NO];
char IntergalNo;
int intergal_calculate(int err)
{
  int sum, interg;
  char lp;
 
  if (++IntergalNo >= MAX_INTERGAL_NO) IntergalNo = 0;
  iIntergBuffer[IntergalNo] = err;
  sum = 0;
  for (lp = 0; lp < IntergalNo; lp++) sum += iIntergBuffer[IntergalNo];
  interg = sum / MAX_INTERGAL_NO;
  return interg;
}

void integal_clear_float(void)
{
  char lp;
  for (lp = 0; lp < MAX_INTERGAL_NO; lp++) fIntergBuffer[lp] = 0;
}

void integal_clear(void)
{
  char lp;
  for (lp = 0; lp < MAX_INTERGAL_NO; lp++) iIntergBuffer[lp] = 0;
}

float intergal_calculate_float(float err)
{
  float interg;
  char lp;
  
  if (++IntergalNo >= MAX_INTERGAL_NO) IntergalNo = 0;
  fIntergBuffer[IntergalNo] = err;
  interg = 0;
  for (lp = 0; lp < MAX_INTERGAL_NO; lp++) interg += fIntergBuffer[IntergalNo];
  interg /= MAX_INTERGAL_NO;
  return interg;
}
  
int irate, ia;
int iTolerance;
int iTargetAmp;
int iTargetVolt;
int iRefAmp;
int iRefVolt;
int iDifferent; 
int iProport;
int iIntegral;
int iOutAmpOld;
int iOutVoltOld;
int iAmpInOld;
int iVoltInOld;
int iToleranceRange;

float fRate;
float fTolerance;
float fTargetAmp;
float fTargetVolt;
float fRefAmp;
float fRefVolt;
float fDifferent; 
float fProport;
float fIntegral;
float fOutAmpOld;
float fOutVoltOld;
float fAmpInOld;
float fVoltInOld;
//float fToleranceRange;

#define TOTERANCE_RANGE 
//#define ADC_TIME      110
#define CONTROL_MAIN  10
#define CONTROL_CC    100
#define CONTROL_CV    200
short sConStep;
short sConDelay;
char ViewUpdate;

//*************************************
// 출력 제어 
// 대상기종: SCR 정류기 
//*************************************
int iResult, iResultMod;
float fResult, fResultMod;

float fResult, fResultMod;
float fAmpInMax, fAmpInMin;
float fAmpDiff, fAmpDiffRate;
float fAmpDiffPlus, fAmpDiffMinus;
float fAmpMaxRatePlus, fAmpMaxRateMinus;
float fAmpErrRate;
float fPreRefAmp;
float fPreRefVolt;

float fVoltInMax, fVoltInMin;
float fVoltDiff, fVoltDiffRate;
float fVoltDiffPlus, fVoltDiffMinus;
float fVoltMaxRatePlus, fVoltMaxRateMinus;
float fVoltErrRate;

int iVoltInMax, iVoltInMin;
int iVoltDiff;

short sStableCount;
short sPrePidCount;
char PidStable;

void calculate_error_clear_cc(void)
{
  fAmpInMin = fRefAmp;
  fAmpInMax = fRefAmp;
}
  
void calculate_error_pid_cc(void)
{
      if (fAmpAvrInput > fAmpInMax) fAmpInMax = fAmpAvrInput;
      if (fAmpAvrInput < fAmpInMin) fAmpInMin = fAmpAvrInput;
      if (fAmpInMax > fTargetAmp) fAmpDiffPlus = fAmpInMax - fTargetAmp;
      if (fAmpInMin < fTargetAmp) fAmpDiffMinus = fTargetAmp - fAmpInMin;
      fAmpErrRate = (fTargetAmp - fAmpAvrInput) * 100 / fTargetAmp;
      fAmpMaxRatePlus = fAmpDiffPlus * 100 / fTargetAmp;
      fAmpMaxRateMinus = fAmpDiffMinus * 100 / fTargetAmp;
}

void calculate_error_clear_cv(void)
{
  fVoltInMax = fRefVolt;
  fVoltInMin = fRefVolt;
}

void calculate_error_pid_cv(void)
{
      if (fVoltInput > fVoltInMax) fVoltInMax = fVoltInput;
      if (fVoltInput < fVoltInMin) fVoltInMin = fVoltInput;
      if (fVoltInMax > fTargetVolt) fVoltDiffPlus = fVoltInMax - fTargetVolt;
      if (fVoltInMin < fTargetVolt) fVoltDiffMinus = fTargetVolt - fVoltInMin;
      fVoltErrRate = (fTargetVolt - fVoltInput) * 100 / fTargetVolt;
      fVoltMaxRatePlus = fVoltDiffPlus * 100 / fTargetVolt;
      fVoltMaxRateMinus = fVoltDiffMinus * 100 / fTargetVolt;
}

void output_level_control_init(void)
{
    iAmpInOld = 0;
    iVoltInOld = 0;
    iOutAmpOld = 0;
    iOutVoltOld = 0;
    
    fAmpInOld = 0;
    fOutAmpOld = 0;
    sConDelay = 0;
    PidStatus = 0;
    sPrePidCount = 0;
}

char DacErrCount;
void dac_error_check(void)
{
    if (dacout_verify() == 0) DacErrCount = 0; else DacErrCount++;
    if (DacErrCount > 10) 
    {
      DacError = 1;
      DacErrCount = 0;
    }
}

void DA_soft_reset(void)
{
  // Power-on-reset
  DAout_ad5663(5, 0, 1);
}

// CC_MODE에서 전압설정이 전류설정보다 적으면 
// 전압설정비 이하로 전류를 설정하고
// CV_MODE에서 전류설정이 전압설정보다 적으면 
// 전류설정비 이하로 전압을 설정한다.
float fLoadOhm;
float cc_cv_limit_decide(float set)
{
  float ra, rv, ref;
  ref = set;
  ra = fRefAmp * 100 / fMaxOperAmp;
  rv = fRefVolt * 100 / fMaxOperVolt;
  
  if (OperMode == CC_MODE)
  {
    if (rv < ra)  ref = fMaxOperAmp * rv / 100;
  }
  else if (ra < rv)  
  {
    if (!PidStable) ref = fMaxOperVolt * ra / 100;
    else ref = fLoadOhm * fRefAmp / 1000;
  }

  return ref;
}

//
// PID control에 사용되는 기준 전압/전류와
// 측정 전압/전류를 결정
// 역운전모드이면 측정전압/전류를 반전
//
float fAmpInAbs;
float fVoltInAbs;
void refference_set(void)
{
#ifdef MONO_POLE
  fRefAmp = fOperAmp;
  fRefVolt = fOperVolt;
  fAmpInAbs = fAmpInput;
  fVoltInAbs = fVoltInput;
#else
  if (OperPole != MINUS) 
  {
    fRefAmp = fOperAmp;
    fRefVolt = fOperVolt;
    fAmpInAbs = fAmpInput;
    fVoltInAbs = fVoltInput;
  }
  else
  {
    fRefAmp = fRevOperAmp;
    fRefVolt = fRevOperVolt;
    fAmpInAbs = fAmpInput * -1;
    fVoltInAbs = fVoltInput * -1;
  }
#endif
}

float fRefAmp0;
float fRefVolt0;
char OperPole0;
char OperMode0;
char PidOver;
int iSoft;

void output_level_control_SCR(void)
{
  float ftol, ftemp;
  
  switch(sConStep)
  {
  case 0: 
    PidStable = 0;
    PidStatus = 0;
    DacErrCount = 0;
    sConStep++;
    return;

  case 1:
    // 2008-12-27 구수 하과장 요청으로 신속 적응 기능 삭제
    PidStable = 0;
    PidStatus = 0;    
    output_level_control_init();
    sConStep++;
    return;
    
  case 2: 
    if (SystemRun) 
    {
      DAclear(ON);
      sConStep++;
    }
    return;
    
  case 3:
    if (++sConDelay > 10) sConStep++;
    return;   

  case 4:
    if (SystemRun) sConStep++;
    else sConStep = 1;
    return;   
    
  case 5:
    DAclear(OFF);
    fRefAmp0 = fRefAmp;
    fRefVolt0 = fRefVolt;
    OperPole0 = OperPole;
    OperMode0 = OperMode;
    sConStep++;
    return;  
    
  case 6:
    //DA_soft_reset();
    control_volt_out(0, fMaxOperVolt);
    control_amp_out(0, fMaxOperAmp);  
    sConDelay = 0;
    PidOver = 0;
    iSoft = iSoftTime * SEC_1 / 10;
    sConStep = CONTROL_MAIN;
    return;  

  case CONTROL_MAIN:
    if (AdcReady[0] == 1) 
    {
      AdcReady[0] = 0;
      if ((SystemRun)&(OperMode == CC_MODE)) sConStep = CONTROL_CC;
    }
    else sConStep++;
    return;
    
  case CONTROL_MAIN+1:
    if (AdcReady[1] == 1) 
    {
      AdcReady[1] = 0;
      if ((SystemRun)&(OperMode == CV_MODE)) sConStep = CONTROL_CV;
    }
    else sConStep = CONTROL_MAIN;
    return;

  case CONTROL_CC:
    dac_error_check();
    refference_set();
    //fRefAmp = cc_cv_limit_decide(fRefAmp);
    sConStep++;
    return;
    
  case CONTROL_CC+1:
    if ((fRefAmp0 != fRefAmp)|(fRefVolt0 != fRefVolt)) 
    {
      PidStatus = 0;
      PidStable = 0;
    }
    fRefAmp0 = fRefAmp;
    fRefVolt0 = fRefVolt;
    
    if(fRefAmp == 0)
    {
      output_level_control_init();
      control_amp_out(0, fMaxOperAmp);
      control_volt_out(0, fMaxOperVolt);
      sConStep = CONTROL_MAIN;
    }
    else sConStep++;
  
    return;

  case CONTROL_CC+2:
    if (PidStatus == 0) 
      if (iRiseTime >= iSoft) sPrePidCount++; 
    if (PidStatus == 1) sConStep = CONTROL_CC + 10;
    else if (sPrePidCount > PRE_PID_COUNT) sConStep = CONTROL_CC + 10; 
    else if (fAmpInAbs > (fRefAmp * 0.9)) sConStep = CONTROL_CC + 10;
    else if (fVoltInAbs > (fRefVolt * 0.9)) sConStep = CONTROL_CC + 10;
    else sConStep++;
    return;

  case CONTROL_CC+3:
    if (PidStable == 0) fPreRefAmp = fRefAmp; 
    if (iSoftTime == 0) fTargetAmp = fPreRefAmp;
    else if (iRiseTime >= iSoft) fTargetAmp = fPreRefAmp;
    else fTargetAmp = (fPreRefAmp * iRiseTime) / iSoft;
    
    fOutAmp = fTargetAmp;

    // 2008.5.19 CV제한 초과로 수정
    //if (fVoltInAbs < fRefVolt) fOutAmp = fTargetAmp;
    //else fOutAmp = fOutAmpOld * 0.98;
    
    //ftemp1 = fRefAmp / fMaxOperAmp;
    //ftemp2 = fRefVolt / fMaxOperVolt;
    //if (ftemp2 >= ftemp1) fOutAmp = fTargetAmp;
    //else fOutAmp = fMaxOperAmp * ftemp2;
    
    sConStep = CONTROL_CC + 20;
    return;
        
  case CONTROL_CC+10:
    PidStatus = 1;
    
    fTargetAmp = fRefAmp;
    fTolerance = fTargetAmp - fAmpInAbs;
    if (fTolerance > 0) ftol = fTolerance; else ftol = -1 * fTolerance;

    fRate = (1 - (ftol/fTargetAmp)) * iReactRate;
    if (fRate < (iReactRate / 5))fRate = iReactRate / 5;
    else if (fRate > iReactRate) fRate = iReactRate;
    fRate /= DEFAULT_REACT_RANGE;    
    sConStep++;
    return;

  case CONTROL_CC+11:
    fProport = fTolerance / 10;
    fDifferent = fAmpInAbs - fAmpInOld;
    fIntegral = intergal_calculate_float(fTolerance);
    fResult = (fProport + fDifferent + fIntegral) * fRate;
 
    fOutAmp = fOutAmpOld + fResult;
    fAmpInOld = fAmpInAbs;
    sConStep = CONTROL_CC+20;
    return;

  case CONTROL_CC+20:
    // 운전전압 제한기능 추가 2007/10/27
    //if (fVoltInAbs > fRefVolt) fOutAmp = fOutAmpOld * 0.98;
    if (fVoltInAbs > (fRefVolt * 1.03)) PidOver = 1;
    else if(fVoltInAbs < (fRefVolt * 0.97)) PidOver = 0;
    if (PidOver)
    {
      ftemp = 100.0 - (fRefVolt * 100 / fVoltInAbs);
      ftemp = ftemp * iReactRate * 2 / DEFAULT_REACT_RANGE;
      if (fVoltInAbs > fRefVolt) fOutAmp = fOutAmpOld * (100 - ftemp) / 100;
    }
    
    if (fOutAmp > fMaxOperAmp) fOutAmp = fMaxOperAmp;
    if (fOutAmp < 0) fOutAmp = 0;
    fOutAmpOld = fOutAmp;
    sConStep++;
    return;
    
  case CONTROL_CC+21:
    if (SystemRun) 
    {
      control_amp_out(fOutAmp, fMaxOperAmp); 
      control_volt_out(fMaxOperVolt, fMaxOperVolt);
    }
    else 
    {
      control_amp_out(0, fMaxOperAmp);
      control_volt_out(0, fMaxOperVolt);
    }
    
    ViewUpdate = 1;
    sConDelay = 0;
    sConStep++;
    return;
  
  case CONTROL_CC+22:
    if (!SystemRun) calculate_error_clear_cc();
    calculate_error_pid_cc();
    sConStep++;
    return;

  case CONTROL_CC+23:
    if (PidStatus == 1)
    {
      if (fAmpErrRate >= 0) ftemp = fAmpErrRate; else ftemp = -1 * fAmpErrRate;
      if (ftemp > 0.5) sStableCount = 0;
      else if (++sStableCount > STABLE_COUNT) 
      {
        if (fResult >= 0) ftemp = fResult; else ftemp = -1 * fResult;
        if (ftemp < 1) 
        {
          fPreRefAmp = fOutAmp;
          if (fPreRefAmp > fRefAmp * 1.1) fPreRefAmp = fRefAmp * 1.1;
          PidStable = 1;
          //fLoadOhm = fVoltInAbs * 1000 / fAmpInAbs; 
          sStableCount = 0;          
        }
      }
    }
    
    sConStep++;
    return;
    
  case CONTROL_CC+24:
    if (SystemRun) sConStep = CONTROL_MAIN;
    else sConStep = 1;
    return;

  case CONTROL_CV:
    dac_error_check();
    refference_set();
    //fRefVolt = cc_cv_limit_decide(fRefVolt);
    sConStep++;
    return;
    
  case CONTROL_CV+1:
    if ((fRefAmp0 != fRefAmp)|(fRefVolt0 != fRefVolt)) 
    {
      PidStatus = 0;
      PidStable = 0;
    }
    fRefAmp0 = fRefAmp;
    fRefVolt0 = fRefVolt;
    sConStep++;
    return;

  case CONTROL_CV+2:
    if (PidStatus == 0) 
      if (iRiseTime >= iSoft) sPrePidCount++; 
    if (PidStatus == 1) sConStep = CONTROL_CV+10;
    else if (sPrePidCount > PRE_PID_COUNT) sConStep = CONTROL_CV + 10; 
    else if (fVoltInAbs > (fRefVolt * 0.9)) sConStep = CONTROL_CV + 10;
    else if (fAmpInAbs > (fRefAmp * 0.9)) sConStep = CONTROL_CV + 10;
    else sConStep++;
    return;

  case CONTROL_CV+3:
    if (PidStable == 0) fPreRefVolt = fRefVolt; 
    if (iSoftTime == 0) fTargetVolt = fPreRefVolt;
    else if (iRiseTime >= iSoft) fTargetVolt = fPreRefVolt;
    else fTargetVolt = (fPreRefVolt * iRiseTime) / iSoft;
    
    fOutVolt = fTargetVolt;
    // 2008.5.19 CC제한 초과로 수정
    //if (fAmpInAbs < fRefAmp) fOutVolt = fTargetVolt;
    //else fOutVolt = fTargetVolt * ftemp;
    //ftemp1 = fRefAmp / fMaxOperAmp;
    //ftemp2 = fRefVolt / fMaxOperVolt;
    //if (ftemp1 >= ftemp2) fOutVolt = fTargetVolt;
    //else fOutVolt = fMaxOperVolt * ftemp1 * 0.9;
    
    sConStep = CONTROL_CV + 20;
    return;

  case CONTROL_CV+10:
    PidStatus = 1;
    
    fTargetVolt = fRefVolt;
    fTolerance = fTargetVolt - fVoltInAbs;
    if (fTolerance > 0) ftol = fTolerance; else ftol = -1 * fTolerance;

    fRate = (1 - (ftol / fTargetVolt)) * iReactRate;
    if (fRate < (iReactRate / 5)) fRate = iReactRate / 5;
    else if (fRate > iReactRate) fRate = iReactRate;
    fRate /= DEFAULT_REACT_RANGE;    
    sConStep++;
    return;

  case CONTROL_CV+11:
    fProport = fTolerance;
    fDifferent = fVoltInAbs - fVoltInOld;
    fIntegral = intergal_calculate_float(fTolerance);
    fResult = (fProport + fDifferent + fIntegral) * fRate;
 
    fOutVolt = fOutVoltOld + fResult;
    fVoltInOld = fVoltInAbs;
    sConStep = CONTROL_CV+20;
    return;
 
  case CONTROL_CV+20:
    // 운전전류 제한기능 추가 2007/10/27
    //if (fAmpInAbs > fRefAmp) fOutVolt = fOutVoltOld * 0.98;
    if (fAmpInAbs > (fRefAmp * 1.03)) PidOver = 1;
    else if (fAmpInAbs < (fRefAmp * 0.97)) PidOver = 0;
    if (PidOver)
    {
      ftemp = 100.0 - (fRefAmp * 100 / fAmpInAbs);
      ftemp = ftemp * iReactRate * 2 / DEFAULT_REACT_RANGE;
      fOutVolt = fOutVoltOld * (100 - ftemp) / 100;
    }
    if (fOutVolt > fMaxOperVolt) fOutVolt = fMaxOperVolt;
    if (fOutVolt < 0) fOutVolt = 0;
    fOutVoltOld = fOutVolt;
    sConStep++;
    return;
    
  case CONTROL_CV+21:
    if (SystemRun) 
    {
      control_volt_out(fOutVolt, fMaxOperVolt); 
      control_amp_out(fMaxOperAmp, fMaxOperAmp);
    }
    else 
    {
      control_volt_out(0, fMaxOperVolt);
      control_amp_out(0, fMaxOperAmp);      
    }
    
    ViewUpdate = 1;
    sConDelay = 0;
    sConStep++;
    return;

  case CONTROL_CV+22:
    if (!SystemRun) calculate_error_clear_cv();
    calculate_error_pid_cv();
    sConStep++;
    return;

  case CONTROL_CV+23:
    if (PidStatus == 1)
    {
      if (fVoltErrRate >= 0) ftemp = fVoltErrRate; 
      else ftemp = -1 * fVoltErrRate;
      if (ftemp > 1.0) sStableCount = 0;
      else if (++sStableCount > STABLE_COUNT) 
      {
        if (fResult >= 0) ftemp = fResult; else ftemp = -1 * fResult;
        if (fResult < 1) 
        {
          fPreRefVolt = fOutVolt;
          if (fPreRefVolt > fRefVolt * 1.1) fPreRefVolt = fRefVolt * 1.1;
          PidStable = 1;
          sStableCount = 0;          
        }
      }
    }
    sConStep++;
    return;
    
  case CONTROL_CV+24:
    if (SystemRun) sConStep = CONTROL_MAIN;
    else sConStep = 1;
    return;

  default: 
    DacError = 2;
    sConStep = 0; 
    return;
  }
}

//*********************************
// PID Control status view
//*********************************

void print_control_status_cc(void)
{
    goto_cursor(0,0);
    printf("In/Tag :%5.0f/%5.0f", fAmpInAbs, fTargetAmp);
    printf("\nTol/Pro:%5.0f/%5.0f", fTolerance, fProport);
    printf("\nDif/Itg:%5.0f/%5.0f", fDifferent, fIntegral);
    printf("\nAdd/rat:%5.2f/%5.2f", fResult, fRate*100);
    //printf("\nOldOutA:%6.1f ", fOutAmpOld);
    //printf("\nOutAmp :%6.1f[%d]", fOutAmp, PidStatus);
    printf("\nOutAmp :%5.0f/%5.0f", fOutAmp, fMaxOperAmp);
    if (PidStable) printf("S"); else printf("N");
    printf("\nPreRefA:%6.1f[%3d]", fPreRefAmp, sPrePidCount);
    //printf("\nMax/Min:%5.0f/%5.0f", fAmpInMax, fAmpInMin);
    printf("\nErrRate:%2.2f%%  ", fAmpErrRate);
    if (fAmpMaxRatePlus > 99) fAmpMaxRatePlus = 99;
    if (fAmpMaxRateMinus > 99) fAmpMaxRateMinus = 99;
    printf("\nMaxRate:");
    if (fAmpMaxRatePlus > 10) printf("%2.0f~", fAmpMaxRatePlus);
    else printf("%2.2f~",fAmpMaxRatePlus);
    if (fAmpMaxRateMinus > 10) printf("%2.0f%% ", fAmpMaxRateMinus);
    else printf("%2.2f~%%",fAmpMaxRateMinus);
}

void print_control_status_cv(void)
{
    goto_cursor(0,0);
    printf("In/Tag :%4.2f/%4.2f", fVoltInAbs, fTargetVolt);
    printf("\nIn/Tag :%5.0f/%5.0f", fAmpInAbs, fRefAmp);
    printf("\nTol/Pro:%4.2f/%4.2f", fTolerance, fProport);
    printf("\nDif/Itg:%4.2f/%4.2f", fDifferent, fIntegral);
    printf("\nResult :%7.3f ", fResult);
    //printf("\nOldOutV:%5.3f ", fOutVoltOld);
    //printf("\nOutVolt:%5.3f[%d]  ", fOutVolt, PidStatus);
    printf("\nOutVolt :%4.2f/%4.1f", fOutVolt, fMaxOperVolt);
    if (PidStable) printf("S"); else printf("N");
    //printf("\nPreRefV:%5.3f[%3d]", fPreRefVolt, sPrePidCount);
    //printf("\nMax/Min:%4.2f/%4.2f", fVoltInMax, fVoltInMin);
    printf("\nErrRate:%2.2f%%  ", fVoltErrRate);
    if (fVoltMaxRatePlus > 99) fVoltMaxRatePlus = 99;
    if (fVoltMaxRateMinus > 99) fVoltMaxRateMinus = 99;
    printf("\nMaxRate:");
    if (fVoltMaxRatePlus > 10) printf("%2.0f~", fVoltMaxRatePlus);
    else printf("%2.2f~",fVoltMaxRatePlus);
    if (fVoltMaxRateMinus > 10) printf("%2.0f%% ", fVoltMaxRateMinus);
    else printf("%2.2f%%",fVoltMaxRateMinus);
}

void print_control_status_cvi(void)
{
    printf("\nIn/Tag :%5d/%5d", iVoltInput, iTargetVolt);
    //printf("\nTargetVolt:%5d ", iTargetVolt);
    printf("\nTol/Pro:%5d/%5d", iTolerance, iProport);
    //printf("\nProport   :%5d ", iProport);
    printf("\nDif/Itg:%5d/%5d", iDifferent, iIntegral);
    //printf("\nIntegral  :%5d ", iIntegral);
    printf("\nResult-Mod:%5d/%2d", iResult, iResultMod);
    printf("\nOutVolt   :%5d ", iOutVolt);
    printf("\nMax/Min:%5d/%5d", iVoltInMax, iVoltInMin);
    printf("\nErr/%%E:%5d/%4.2f", iVoltDiff, fVoltDiffRate);           
}

void printf_control_status(void)
{
  goto_cursor(0,0);
  if (OperMode == CC_MODE) print_control_status_cc();
  else print_control_status_cv();
}

// 제어상태 표시 화면(ViewPage = 1)
void control_status_view(void)
{
  switch (sExecStep)
  {
  case 0:
    display_mode(0);
    debug_monit(MONOUT);
    CursorUse = 1;
    LineBlink = 0;
    DelayStep = 0;
    sExecStep++;
    return;
    
  case 1:
    sExecStep++;
    return;

  case 2:
    printf_control_status();
    sExecStep++;
    return;

  case 3:
    if (ViewUpdate)
    {
      ViewUpdate = 0;
      printf_control_status();
    }
    if (ENTER_KEY) execmode_change(MAIN_MENU);
    else if (ANY_KEY) sExecStep++;
    return;

  case 4:
    if (ENTER_KEY) execmode_change(MAIN_MENU);
    else if (ViewUpdate) 
    {
      DelayStep = 0;
      screen_clear();
      fAmpInMax = fAmpInAbs;
      fAmpInMin = fAmpInAbs;
      fAmpDiffPlus = 0;
      fAmpDiffMinus = 0;
      fAmpDiff = 0;
      fAmpDiffRate = 0;
      
      fVoltInMax = fVoltInAbs;
      fVoltInMin = fVoltInAbs;
      fVoltDiffPlus = 0;
      fVoltDiffMinus = 0;
      fVoltDiff = 0;
      fVoltDiffRate = 0;
      sExecStep = 3;
    }
    return;
    
  case 5:
    if (ENTER_KEY) execmode_change(MAIN_MENU);
    else if (step_delay(SEC_1>>3)) sExecStep = 3;
    return;
    
  default:
    sExecStep = 0;
    return;
  }  
}

//*************************************
// DA converter AD5663에 data를 write
//*************************************
#define DA_DELAY  5
void serieal_out_ad5663(int data)
{
  char lp;
  int mask;
  mask = 0x00800000;
  pio_clear(PIOA, DA_CK|DA_SYNC);
  pio_set(PIOA, DA_LDAC);
  for (lp = 0; lp < 24; lp++)
  {
    if (data & mask) pio_set(PIOA, DA_DOUT); 
     else pio_clear(PIOA, DA_DOUT);
    pio_set(PIOA, DA_CK);
    delay_nop(DA_DELAY);
    pio_clear(PIOA, DA_CK);
    delay_nop(DA_DELAY);
    mask >>= 1;
  }
  pio_set(PIOA, DA_CK);
  pio_clear(PIOA, DA_LDAC);
  delay_nop(DA_DELAY);
  pio_set(PIOA, DA_LDAC);
  pio_set(PIOA, DA_SYNC);
  //pio_set(PIOA, DA_CK|DA_SYNC);
}

void DAout_ad5663(char cmd, char ch, int val)
{  
  cmd &= 7;
  ch &= 7;
  val &= 0x0000FFFF;
  val = (cmd<<19)|(ch<<16)|val;
  serieal_out_ad5663(val);
}
  
void menu_display_dactest(char no)
{  
  if (no == 0)       printf("<<D/A Test Menu>>");
  else if (no == 1)  printf("\nDAC 0(DC Volt) Test");
  else if (no == 2)  printf("\nDAC 1(DC Amp) Test");
  else if (no == 3)  printf("\nDAC reset");
  else if (no == 4)  printf("\nReturn To Menu");
}

#define DACTEST_MAIN      10
#define DACTEST_EXE       100
#define DACTEST_RST       200
#define DACTEST_END       300
/********************************/
/*  D/A converter test_function */
/********************************/
int DacVolt[2];
int DacReg[2];
int dg0, dg1;
char DacCh;
char DacTestMode;
void dac_test_function(void)
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
    DAclear(OFF);   // 2009-01-09
    sExecStep++;
    return;
    
  case 1:
    DacVolt[0] = 0;
    DacVolt[1] = 0;
    DAout_ad5663(0, 0, 0);
    DAout_ad5663(0, 1, 0);
    DacTestMode = 1;
    sExecStep = DACTEST_MAIN;
    return;
    
 case DACTEST_MAIN:
    screen_clear();
    LineBlink = 1;
    DelayStep = 0;
    for (lp = MenuStart; lp <= MenuEnd; lp++) menu_display_dactest(lp);
    cursor_move_home();
    sExecStep++;
    return;
  
  case DACTEST_MAIN+1:
    if (MENU_UP) popup_menu_update(DAC_TEST,UP); 
    else if (MENU_DN) popup_menu_update(DAC_TEST,DOWN); 
    else if (ENTER_KEY) sExecStep = DACTEST_MAIN+3;
    return;

  case DACTEST_MAIN+2:
    sExecStep--;
    return;
    
  case DACTEST_MAIN+3:
   DelayStep = 0;
   LineBlink = 0;
   MenuNo = MenuStart + find_cursor_vpos();
   if (MenuNo == 0)  sExecStep = DACTEST_END;
   else if (MenuNo == 1) sExecStep = DACTEST_EXE+2;
   else if (MenuNo == 2) sExecStep = DACTEST_EXE+3;
   else if (MenuNo == 3) sExecStep = DACTEST_RST;
   else if (MenuNo == 4) sExecStep = DACTEST_END;
   return;

  // 2009-01-09 DACTEST_END 추가
  case DACTEST_END:
    DAout_ad5663(0, 0, 0);
    DAout_ad5663(0, 1, 0);
    sExecStep++;
    return;

  case DACTEST_END+1:
    DacTestMode = 0;
    DAclear(ON);
    execmode_change(SYSTEM_TEST);
    return;

  case DACTEST_EXE:
    screen_clear();
    printf("<<Analog out Test >>");
    printf("\nSelect Channel: %1d", DacCh);
    sExecStep++;
    return;
    
  case DACTEST_EXE+1:
    if (MENU_UP) DacCh ^= 1; 
    else if (MENU_DN) DacCh ^= 1; 
    else if (ENTER_KEY) sExecStep = DACTEST_EXE+4;
    if (ANY_KEY) printf("\rSelect Channel: %1d", DacCh);
    return;
    
  case DACTEST_EXE+2:
    screen_clear();
    printf("<<DC Volt Out Test>>");
    printf("\nPush Wheel:[END]");
    DacCh = 0;
    sExecStep = DACTEST_EXE+4;
    return;
    
  case DACTEST_EXE+3:
    screen_clear();
    printf("<<DC Amp Out Test>>");
    printf("\nPush Wheel:[END]");
    DacCh = 1;
    sExecStep = DACTEST_EXE+4;
    return;

  case DACTEST_EXE+4:
    dg1 = DacVolt[DacCh] / 1000;
    dg0 = DacVolt[DacCh] % 1000;
    if (DacCh == 0)
    {
      printf("\nSet DAC[VOLT](Max:10V)");
      printf("\rDAC V Out :%2d.%03d[V]", dg1, dg0);
      printf("\nDAC V Mon :%2d.%02d [V]", iDacMonVolt/100, iDacMonVolt%100);
    }
    else
    {
      printf("\nSet DAC[AMP](Max:10V)");
      printf("\rDAC A Out :%2d.%03d[V]", dg1, dg0);
      printf("\nDAC A Mon :%2d.%02d [V]", iDacMonAmp/100, iDacMonAmp%100);
    }
    DelayStep = 0;
    sExecStep++;
    return;
    
  case DACTEST_EXE+5:
    if (COUNT_UP) 
    {
      if (JogSpeed < 4) JogSpeed = 1; 
      if (DacVolt[DacCh] < 9999) DacVolt[DacCh]+= JogSpeed;
      if (DacVolt[DacCh] > 9999) DacVolt[DacCh] = 9999;
      sExecStep++;
    }
    else if (COUNT_DN) 
    {
      if (JogSpeed < 4) JogSpeed = 1; 
      if (DacVolt[DacCh] > 0) DacVolt[DacCh] -= JogSpeed;
      if (DacVolt[DacCh] < 0) DacVolt[DacCh] = 0;
      sExecStep++;
    }
    else if (ENTER_KEY) sExecStep = DACTEST_EXE+7;
    else step_delay(SEC_1/4);
    return;

  case DACTEST_EXE+6:
    DacReg[DacCh] = DacVolt[DacCh] * 65536 / 10000; 
    DAout_ad5663(0, DacCh, DacReg[DacCh]);
    dg1 = DacVolt[DacCh] / 1000;
    dg0 = DacVolt[DacCh] % 1000;
    goto_cursor(0,3);
    if (DacCh == 0)
    {
      printf("\rDAC V Out :%2d.%03d[V]", dg1, dg0);
      printf("\nDAC V Mon :%2d.%02d [V]", iDacMonVolt/100, iDacMonVolt%100);
    }
    else
    {
      printf("\rDAC A Out :%2d.%03d[V]", dg1, dg0);
      printf("\nDAC A Mon :%2d.%02d [V]", iDacMonAmp/100, iDacMonAmp%100);
    }
    DelayStep = 0;
    sExecStep = DACTEST_EXE+5;
    return;
    
  case DACTEST_EXE+7:
    DAout_ad5663(0, DacCh, 0);
    sExecStep = DACTEST_MAIN;
    return;

  case DACTEST_RST:
    screen_clear();
    printf("DAC Reset..");
    DAclear(ON);
    DelayStep = 0;
    sExecStep++;
    return;

  case DACTEST_RST+1:
    if (step_delay(100)) DAclear(OFF);
    return;

  case DACTEST_RST+2:
    DA_soft_reset();
    sExecStep++;
    return;
    
  case DACTEST_RST+3:    
    if (ANY_KEY) sExecStep = DACTEST_MAIN;
    else if (step_delay(SEC_1)) sExecStep = DACTEST_MAIN;
    return;
    
  default:
    sExecStep = DACTEST_MAIN;
    return;
  }
}

/*****************************************/
/* AD Converting 결과를 기반으로         */
/*    전류입력값과 전압입력값을 산출     */
/*****************************************/
#define AVERAGE_NO   24 // 12
int iAmpInput;
int iVoltInput;
//int iVoltAverage;
//int iAmpAverage;
short sVoltDiv;
int iAdcAvrAmp;
int iAdcAvrVolt;
int iAdcRealAmp;
int iAdcRealVolt;
int iAdcArrayVolt[AVERAGE_NO];
int iAdcArrayAmp[AVERAGE_NO];
char AvrageNo;

void input_signal_average(void)
{
  int ivsum, itp, iasum;
  char lp;
  
  
  if (++AvrageNo >= AVERAGE_NO) AvrageNo = 0;
  
#ifdef MONO_POLE
  iAdcArrayVolt[AvrageNo] = (iAdcRead[AD_CH_VOLT] & 0xFFFF); 
  iAdcArrayAmp[AvrageNo] = (iAdcRead[AD_CH_AMP] & 0xFFFF);
#else 
  iAdcArrayVolt[AvrageNo] = (iAdcRead[AD_CH_VOLT] & 0xFFFF) - 0x8000; 
  iAdcArrayAmp[AvrageNo] = (iAdcRead[AD_CH_AMP] & 0xFFFF) - 0x8000; 
#endif
  
    if (iVoltGain > MAX_DC_GAIN) iVoltGain = DEFAULT_DC_GAIN;
    else if(iVoltGain < MIN_DC_GAIN) iVoltGain = DEFAULT_DC_GAIN;

    if (iAmpGain > MAX_DC_GAIN) iAmpGain = DEFAULT_DC_GAIN;
    else if(iAmpGain < MIN_DC_GAIN) iAmpGain = DEFAULT_DC_GAIN;

// 평균 전압/전류 계산
    iasum = 0;
    ivsum = 0;
    for (lp = 0; lp < AVERAGE_NO; lp++)
    {
      iasum += iAdcArrayAmp[lp];
      ivsum += iAdcArrayVolt[lp];
    } 
    itp = (iasum / AVERAGE_NO) - iAmpOffset;
    iAdcAvrAmp = itp * iAmpGain / 1000;
    itp = (ivsum / AVERAGE_NO) - iVoltOffset;
    iAdcAvrVolt = itp * iVoltGain / 1000;
    
// 최근 전압/전류값으로 계산
    if (PidStable == 0)
    {
      itp = iAdcArrayAmp[AvrageNo] - iAmpOffset;
      iAdcRealAmp = itp * iAmpGain / 1000;
      itp = iAdcArrayVolt[AvrageNo] - iVoltOffset;
      iAdcRealVolt = itp * iVoltGain / 1000;
    }
    else 
    {
      iAdcRealAmp = iAdcAvrAmp;
      iAdcRealVolt = iAdcAvrVolt;
    }
}

/*     
void input_signal_calc(void)
{
  int ia, ib, mod;
  ia = (iAdcRead[AD_CH_AMP]*iMaxOperAmp) >> 16;
  ia *= 5;
  ia /= 4;
  iAmpInput = ia;
  mod = ia % 4;
  if (mod >= 2) iAmpInput++;

  //ib = fMaxOperVolt * 125;
  //ia = (iAdcRead[1] * ib) >> 16;
  ib = fMaxOperVolt * 100;
  ia = (iAdcRead[AD_CH_VOLT] * ib) >> 16;
  ia *= 5;
  ia /= 4;
  if ( ia >= 10000) sVoltDiv = 100;
    else if (ia >= 1000) sVoltDiv = 10; 
    else sVoltDiv = 1; 
  iVoltInput = ia;
}
*/

float fAmpAvrInput, fVoltAvrInput;
void average_signal_calc(void)
{
  int ia, ib, mod;
  
  input_signal_average();

  // Float type DC Volt, Amp calculate  
  fAmpAvrInput = (iAdcAvrAmp * fMaxOperAmp * 1.25) / MAX_AMP_ADC;
  fVoltAvrInput = (iAdcAvrVolt * fMaxOperVolt * 1.25) / MAX_VOLT_ADC;

  fAmpInput = (iAdcRealAmp * fMaxOperAmp * 1.25) / MAX_AMP_ADC;
  fVoltInput = (iAdcRealVolt * fMaxOperVolt * 1.25) / MAX_VOLT_ADC;
 
  // integer type DC Volt, Amp calculate  
#ifdef MONO_POLE
  ia = (iAdcAvrAmp * (iMaxOperAmp/2)) >> 15; //50000A넘을때
  ia *= 5;
  ia /= 4;
  iAmpInput = ia;
  mod = ia % 4;
  if (mod >= 2) iAmpInput++;

  ib = fMaxOperVolt * 125;
  ia = (iAdcAvrVolt * ib) >> 16;

  if ( ia >= 10000) sVoltDiv = 100;
    else if (ia >= 1000) sVoltDiv = 10; 
    else sVoltDiv = 1; 
  iVoltInput = ia;
#else 
  ia = (iAdcAvrAmp * iMaxOperAmp) >> 15;
  ia *= 5;
  ia /= 4;
  iAmpInput = ia;
  mod = ia % 4;
  if (mod >= 2) iAmpInput++;

  ib = fMaxOperVolt * 125;
  ia = (iAdcAvrVolt * ib) >> 15;

  if ( ia >= 10000) sVoltDiv = 100;
    else if (ia >= 1000) sVoltDiv = 10; 
    else sVoltDiv = 1; 
  iVoltInput = ia;
#endif
}

// AD7705 Communacation Register
// b7: DRDY_  0:Ready 1: No Ready
// b654: RS2-0 Register Selection                    length
#define ADREG_COM     0<<4   // Communication Register  8bit
#define ADREG_SET     1<<4   // Setup Register          8bit
#define ADREG_CLK     2<<4   // Clock Register          8bit
#define ADREG_DATA    3<<4   // Data Register           16bit
#define ADEG_TEST     4<<4   // Test Register           8bit
#define ADEG_NOP      5<<4   // No Operation
#define ADEG_OFFSET   6<<4   // Offset Register         24bit
#define ADEG_GAIN     7<<4   // Gain Register           24bit
// b3 : Operation is Write(0) / Read(1)
#define AD_READ       1<<3
// b2 : Power down Standby(1)
#define AD_STANDBY    1<<2
// b10: Channel Select 00: ch0, 01: ch1(single input)
#define AD_AIN1        0
#define AD_AIN2        1
#define AD_AIN3        2
#define AD_AIN4        3

// AD7705 Clock Register
#define AD_CLKDIS           1<<4  //Master clock disable
#define AD_CLKDIV           1<<3  //Clock divide by 2
#define AD_CLK_HIGH         1<<2  //Clock is 4.9152 MHz
#define AD_FILTER_50        0     //Rate: 20(2.4576MHz) or 50(4.9152MHz)
#define AD_FILTER_60        1     //Rate: 25(2.4576MHz) or 60(4.9152MHz)
#define AD_FILTER_250       2     //Rate: 100(2.4576MHz) or 250(4.9152MHz)
#define AD_FILTER_500       3     //Rate: 200(2.4576MHz) or 500(4.9152MHz)

// AD7705 Setup Register
// b76- MD1, MD0: ADC Mode bit
#define AD_NORMAL_OPERATE   0<<6 //Normal Conversion Mode
#define AD_CALBRATE_SELF    1<<6 //Self Calibration(Auto Zero & Full scale cabration execute)
#define AD_CALBRATE_ZERO    2<<6 //Zero-Scale System Calibration(Manual Zero scale cabration execute)
#define AD_CALBRATE_FULL    3<<6 //Full-Scale System Calibration(Manual Full scale cabration execute)     
// b543- Gain select
#define AD_GAIN_1           0<<3
#define AD_GAIN_2           1<<3
#define AD_GAIN_4           2<<3
#define AD_GAIN_8           3<<3
#define AD_GAIN_16          4<<3
#define AD_GAIN_32          5<<3
#define AD_GAIN_64          6<<3 
#define AD_GAIN_128         7<<3
// b2- 0:Bipolar operate, 1:Unipolar operate
#define AD_UNIPOLE          1<<2 //unipolar operation
#define AD_BIPOLE           0    //bipolar operation
// b1- Buffer control 0:short(reduce current) 1:on(High inpedance buffer)
#define AD_BUFFER_ON        1<<1 //AD input buffer ON
// b0- Filter Synchronization 1:Reset state, 0:Filter start
#define AD_FSYNC_RESET      1    //AD Filter sync. is reset dtate

unsigned char AdcComm;
#define AD_DELAY  1
void serieal_write_ad7705(int data, char length)
{
  char lp;
  int mask;
  mask = 1 << (length-1);
  pio_set(PIOA, AD_CS|AD_CLK|AD_DIN);
  pio_clear(PIOA, AD_CS);
  
  for (lp = 0; lp < length; lp++)
  {
    pio_clear(PIOA, AD_CLK);
    if (data & mask) pio_set(PIOA, AD_DIN); 
     else pio_clear(PIOA, AD_DIN);
    delay_nop(AD_DELAY);
    pio_set(PIOA, AD_CLK);
    
    delay_nop(AD_DELAY);
    mask >>= 1;
  }
  pio_set(PIOA, AD_CS);
}

int serieal_read_ad7705(char length)
{
  int data = 0;
  char lp;
  int mask;
  
  mask = 1<<(length-1);
  mask = 0x8000;
  pio_set(PIOA, AD_CS|AD_CLK);
  pio_clear(PIOA, AD_CS);
  
  for (lp = 0; lp < length; lp++)
  {
    pio_clear(PIOA, AD_CLK);    
    delay_nop(AD_DELAY);
    if ((pio_read(PIOA) & AD_DOUT) != 0) data = data|mask;
    pio_set(PIOA, AD_CLK);  
    delay_nop(AD_DELAY);
    mask >>= 1;
  }
  pio_set(PIOA, AD_CS);
  return data;
}

/***********************************/
/*  A/D converter AD7705 Reading   */
/***********************************/
int iAdcVolt[2];
int iAdcRead[2];
char AdcStep;
short sAdcDelay;
short sAdcTime[2];
char AdcReady[2];
char AdcNo;
char AdcDebug;

#define AD_FILTER       AD_FILTER_60
//#define AD_SYNC         AD_FSYNC_RESET
#define AD_SYNC             0
#define AD_STEP_WAIT        100
#define READY_WAIT          9999
#define ADC_READY_HIGH (pio_read(PIOA) & AD_RDY) != 0
#define ADC_READY_LOW (pio_read(PIOA) & AD_RDY) == 0
#define ADCREAD_RESET       10
#define ADCREAD_INIT        20 
#define ADCREAD_READ        30
#define ADCREAD_SOFTREAD    40
#define ADCREAD_ERR         50
char AdcModeAmp, AdcModeVolt;
char AdcClock;

void adc_setup_volt(void)
{
    // clock div, 4.9152 MHz, Convert rate
    serieal_write_ad7705(ADREG_CLK|AD_CH_VOLT,8);
    serieal_write_ad7705(AdcClock,8);
    // ADC mode set
    serieal_write_ad7705(ADREG_SET|AD_CH_VOLT,8);
    serieal_write_ad7705(AdcModeVolt,8);
}    

void adc_setup_amp(void)
{
    // clock div, 4.9152 MHz, Convert rate
    serieal_write_ad7705(ADREG_CLK|AD_CH_AMP,8);
    serieal_write_ad7705(AdcClock,8);
    // ADC mode set
    serieal_write_ad7705(ADREG_SET|AD_CH_AMP,8);
    serieal_write_ad7705(AdcModeAmp,8);
}    

void adc_read_ad7705(void)
{   
  switch (AdcStep)
 {   
  case 0:
    //display_mode(0); 
    //debug_monit(MONOUT);
    //AdcDebug = 1;
    AdcReady[AD_CH_AMP] = 0;
    AdcReady[AD_CH_VOLT] = 0;
    
#ifdef MONO_POLE
    AdcModeAmp  = AD_CALBRATE_SELF|AD_GAIN_8|AD_UNIPOLE;
    AdcModeVolt = AD_CALBRATE_SELF|AD_GAIN_8|AD_UNIPOLE;
#else
    AdcModeAmp  = AD_CALBRATE_SELF|AD_GAIN_8|AD_BIPOLE;
    AdcModeVolt = AD_CALBRATE_SELF|AD_GAIN_8|AD_BIPOLE;
#endif

    //AdcClock = AD_CLKDIV|AD_CLK_HIGH|AD_FILTER_60;
    AdcClock = AD_CLKDIV|AD_CLK_HIGH|AD_FILTER_60;
    AdcStep = ADCREAD_RESET;    
    return;
    
  case ADCREAD_RESET:
    pio_clear(PIOA, AD_RST);
    sAdcDelay = 0;
    AdcNo = 0;
    AdcStep++;
    return;
    
  case ADCREAD_RESET+1:
    if (++sAdcDelay > SEC_1/8) AdcStep++;
    return;
    
  case ADCREAD_RESET+2:
    pio_set(PIOA, AD_RST);
    AdcNo = 0;
    sAdcDelay = 0;
    AdcStep++;
    return;
  
  case ADCREAD_RESET+3:
    //if (ADC_READY_LOW) AdcStep = ADCREAD_INIT;
    if (++sAdcDelay > SEC_1/8) AdcStep = ADCREAD_INIT;
    return;
    
  case ADCREAD_INIT:
    // Volt ADC SETUP 
    adc_setup_volt();
    sAdcDelay = 0;
    AdcStep++;
    return;
    
  case ADCREAD_INIT+1:
    if (ADC_READY_LOW) AdcStep++; 
    else if (++sAdcDelay > SEC_1) AdcStep++;  
    return;

  case ADCREAD_INIT+2:
    // Current ADC SETUP 
    adc_setup_amp();
    sAdcDelay = 0;
    AdcStep++;
    return;

 case ADCREAD_INIT+3:
    if (ADC_READY_LOW) AdcStep++; 
    else if (++sAdcDelay > SEC_1) AdcStep++;  
    return;
    
  case ADCREAD_INIT+4:
   AdcStep++;
    return;
    
  case ADCREAD_INIT+5:
    AdcStep++;
    return;

  case ADCREAD_INIT+6:
    AdcStep++;
    return;
 
  case ADCREAD_INIT+7:
    sAdcDelay = 0;
    AdcStep = ADCREAD_READ;
    return;
    
  case ADCREAD_READ:
    // Volt ADC start
    serieal_write_ad7705(ADREG_DATA|AD_READ|AD_CH_VOLT,8);
    sAdcDelay = 0;
    AdcStep++;
    return;
    
  case ADCREAD_READ+1:    
    // wait ADC ready
    if (ADC_READY_LOW)
    {
      iAdcRead[AD_CH_VOLT] = serieal_read_ad7705(16);
      if (AdcDebug) printf("\n0:%04X-%02d ", iAdcRead[AD_CH_VOLT], sAdcDelay);
      sAdcTime[AD_CH_VOLT] = sAdcDelay<<1;
      AdcReady[AD_CH_VOLT] = 1;
      // Current ADC start
      serieal_write_ad7705(ADREG_DATA|AD_READ|AD_CH_AMP,8);
      AdcStep++;
    }
    else if ( ++sAdcDelay > AD_STEP_WAIT) AdcStep = ADCREAD_ERR;
    return;

  case ADCREAD_READ+2:
    sAdcDelay = 0;
    AdcStep++;
    return;
    
  case ADCREAD_READ+3:    
    // wait ADC ready
    if (ADC_READY_LOW)
    {
      iAdcRead[AD_CH_AMP] = serieal_read_ad7705(16);
      if (AdcDebug) printf("1:%04X-%02d ", iAdcRead[AD_CH_AMP], sAdcDelay);
      sAdcTime[AD_CH_AMP] = sAdcDelay<<1;
      AdcReady[AD_CH_AMP] = 1;
      // Volt ADC start
      serieal_write_ad7705(ADREG_DATA|AD_READ|AD_CH_VOLT,8);
      AdcStep++;
    }
    else if ( ++sAdcDelay > AD_STEP_WAIT) AdcStep = ADCREAD_ERR;
    return;

  case ADCREAD_READ+4:      
    //input_signal_calc();
    average_signal_calc();
    sAdcDelay = 0;
    AdcStep = ADCREAD_READ+1;
    return;

  case ADCREAD_ERR:
    if (AdcDebug) printf("\nADC Time Out:%04d", sAdcDelay);
    AdcError = 1;
    sAdcDelay = 0;
    AdcStep++;
    return;
    
  case ADCREAD_ERR+1:
    if (++sAdcDelay > SEC_1/8) AdcStep = ADCREAD_RESET;
    return;
    
  default: 
    AdcError = 2;
    AdcStep = 0; 
    return;
 }
}


/********************************/
/*  A/D converter test_function */
/********************************/
#define ADCTEST_MAIN      10
#define ADCTEST_EXE       100
#define ADCTEST_READ      200
#define ADCTEST_ZERO1SET  300
#define ADCTEST_ZERO2SET  400
#define ADCTEST_GAIN1SET  500
#define ADCTEST_GAIN2SET  600
#define ADCTEST_PARA_SET  700
#define ADCTEST_CANCEL    800
#define ADCTEST_NOT_READY 1000
#define	ADCTEST_END       1100
#define	ADCTEST_SAVE_END    ADCTEST_END+1
#define	ADCTEST_ZERO_ERR    1200
#define	ADCTEST_SPAN_ERR    ADCTEST_ZERO_ERR+1


void menu_display_adctest(char no)
{  
  if (no == 0)       printf("<<ADC Test Menu>>");
  else if (no == 1)  printf("\nAnalog Input Read");
  else if (no == 2)  printf("\nCH[1:AMP ] ZERO Adj");
  else if (no == 3)  printf("\nCH[0:Volt] ZERO Adj");
  else if (no == 4)  printf("\nCH[1:AMP ] Gain Adj");
  else if (no == 5)  printf("\nCH[0:Volt] Gain Adj");
  //else if (no == 6)  printf("\nAD Parameter Set");
  else if (no == 6)  printf("\nReturn To Menu");
}
  
void print_amp_sense(int v)
{

    v = v * BIT_PER_uA / 100000; 
    dg0 =  v/100; dg1 = v%100;
    if (dg1 < 0) dg1 *= -1;
    printf("%2d.%02d[mV]", dg0, dg1);
}

void print_volt_sense(int v)
{
    v = v * BIT_PER_mV / 100000;
    dg0 =  v/100; dg1 = v%100;
    if (dg1 < 0) dg1 *= -1;
    printf("%2d.%02d[V]", dg0, dg1);
}

int iTempSet0;
void adc_test_function(void)
{
  char result;
  short lp;
  int iadc, dg0, dg1;
  
  switch (sExecStep)
 {   
  case 0:
    display_mode(0); 
    debug_monit(MONOUT);
    CursorUse = 1;
    MenuStart = 0;
    MenuEnd = 6;
    MenuSize = 7;
    sExecStep = ADCTEST_MAIN;
    return;
    
  case ADCTEST_MAIN:
    screen_clear();
    LineBlink = 1;
    DelayStep = 0;
    for (lp = MenuStart; lp <= MenuEnd; lp++) menu_display_adctest(lp);
    cursor_move_home();
    sExecStep++;
    return;
  
  case ADCTEST_MAIN+1:
    if (MENU_UP) popup_menu_update(ADC_TEST,UP); 
    else if (MENU_DN) popup_menu_update(ADC_TEST,DOWN); 
    else if (ENTER_KEY) sExecStep = ADCTEST_EXE;
    return;
   
 case ADCTEST_MAIN+2:
    sExecStep--;
    return;
      
 case ADCTEST_EXE:
   DelayStep = 0;
   LineBlink = 0;
   MenuNo = MenuStart + find_cursor_vpos();
   if (MenuNo == 0) execmode_change(SYSTEM_TEST);//sExecStep = ADCTEST_MAIN;
   else if (MenuNo == 1) sExecStep = ADCTEST_READ;
   else if (MenuNo == 2) sExecStep = ADCTEST_ZERO1SET;
   else if (MenuNo == 3) sExecStep = ADCTEST_ZERO2SET;
   else if (MenuNo == 4) sExecStep = ADCTEST_GAIN1SET;
   else if (MenuNo == 5) sExecStep = ADCTEST_GAIN2SET;
   //else if (MenuNo == 6) sExecStep = ADCTEST_PARA_SET;
   else if (MenuNo == 6) execmode_change(SYSTEM_TEST);
   else sExecStep = ADCTEST_MAIN;
   return;

//*************************************************
//    ADC Amp Gain Adjust
//   ADC 전압 측정 게인 조정
//   조정범위 : 85.0% - 115.0%
//*************************************************
  case ADCTEST_GAIN1SET:
    screen_clear();
    sExecStep++;
    return;
    
  case ADCTEST_GAIN1SET+1:
    printf("<Amp Gain Adjust>");
    if (iAmpGain > MAX_DC_GAIN) iAmpGain = DEFAULT_DC_GAIN;
    else if(iAmpGain < MIN_DC_GAIN) iAmpGain = DEFAULT_DC_GAIN;
    printf("\nNow:%3d.%01d", iAmpGain/10, iAmpGain%10);
    iTempSet = iAmpGain;
    iTempSet0 = iAmpGain;    
    printf("\nAdj:%3d.%01d", iAmpGain/10, iAmpGain%10);
    sExecStep++;
    return;    

  case ADCTEST_GAIN1SET+2:
    if (wheel_input(MIN_DC_GAIN, MAX_DC_GAIN)) 
    {
      if (iTempSet0 == iTempSet) sExecStep = ADCTEST_END; 
      else 
      {
        iAmpGain = iTempSet;
        printf("\rAdj:%3d.%01d", iAmpGain/10, iAmpGain%10);
        sExecStep = ADCTEST_SAVE_END;
      }
    }
    return; 
    
  case ADCTEST_GAIN1SET+3:
    printf("\rAdj:%3d.%01d", iTempSet/10, iTempSet%10);
    iAmpGain = iTempSet;
    sExecStep--;
    return;   

//*************************************************
//    ADC Volt Gain Adjust
//   ADC 전압 측정 게인 조정
//   조정범위 : 85.0% - 115.0%
//*************************************************
  case ADCTEST_GAIN2SET:
    screen_clear();
    sExecStep++;
    return;
    
  case ADCTEST_GAIN2SET+1:
    printf("<Volt Gain Adjust>");
    if (iVoltGain > MAX_DC_GAIN) iVoltGain = DEFAULT_DC_GAIN;
    else if(iVoltGain < MIN_DC_GAIN) iVoltGain = DEFAULT_DC_GAIN;
    printf("\nNow:%3d.%01d", iVoltGain/10, iVoltGain%10);
    iTempSet = iVoltGain;
    iTempSet0 = iVoltGain;
    printf("\nAdj:%3d.%01d", iVoltGain/10, iVoltGain%10);
    sExecStep++;
    return;    

  case ADCTEST_GAIN2SET+2:
    if (wheel_input(MIN_DC_GAIN, MAX_DC_GAIN)) 
    {
      if (iTempSet0 == iTempSet) sExecStep = ADCTEST_END; 
      else 
      {
        iVoltGain = iTempSet;
        printf("\rAdj:%3d.%01d", iVoltGain/10, iVoltGain%10);
        sExecStep = ADCTEST_SAVE_END;
      }
    }
    return; 
    
  case ADCTEST_GAIN2SET+3:
    printf("\rAdj:%3d.%01d", iTempSet/10, iTempSet%10);
    iVoltGain = iTempSet;
    sExecStep--;
    return;   
    
//
// AD7705 Mode Register, Adc Clock Register Set
//
  case ADCTEST_PARA_SET:
    screen_clear();
    printf(">ADC[VOLT] Mode Reg.");
    printf("\n>NOW: %02XH", AdcModeVolt); 
    iTempSet = AdcModeVolt;
    printf("\n>NEW: %02XH", iTempSet); 
    sExecStep++;
    return;
 
  case ADCTEST_PARA_SET+1:
    if (wheel_input(0, 255)) 
    {
      AdcModeVolt = iTempSet & 0xFF;
      printf("\r>NEW: %02XH", AdcModeVolt); 
      sExecStep = ADCTEST_PARA_SET+3;
    }
    return; 
 
  case ADCTEST_PARA_SET+2:
    printf("\r>NEW: %02XH", iTempSet);  
    sExecStep--;
    return;
 
  case ADCTEST_PARA_SET+3:
    printf("\n>ADC[AMP] Mode Reg.");
    printf("\n>NOW: %02XH", AdcModeAmp); 
    iTempSet = AdcModeAmp;
    printf("\n>NEW: %02XH", iTempSet); 
    sExecStep++;
    return;
 
  case ADCTEST_PARA_SET+4:
    if (wheel_input(0, 255)) 
    {
      AdcModeAmp = iTempSet & 0xFF;
      printf("\r>NEW: %02XH", AdcModeAmp); 
      sExecStep = ADCTEST_PARA_SET+6;
    }
    return; 
 
  case ADCTEST_PARA_SET+5:
    printf("\r>NEW: %02XH", iTempSet);  
    sExecStep--;
    return;  
 
 
  case ADCTEST_PARA_SET+6:
    printf("\n>ADC Clock Reg. Set");
    printf("\n>NOW: %02XH", AdcClock); 
    iTempSet = AdcClock;
    printf("\n>NEW: %02XH", iTempSet); 
    sExecStep++;
    return;
 
  case ADCTEST_PARA_SET+7:
    if (wheel_input(0, 255)) 
    {
      AdcClock = iTempSet & 0xFF;
      printf("\r>NEW: %02XH", AdcClock); 
      AdcStep = ADCREAD_RESET;
      DelayStep = 0;
      sExecStep = ADCTEST_PARA_SET+9;
    }
    return; 
 
  case ADCTEST_PARA_SET+8:
    printf("\r>NEW: %02XH", iTempSet);  
    sExecStep--;
    return;  

  case ADCTEST_PARA_SET+9:
    if (ANY_KEY) sExecStep = ADCTEST_MAIN;
    else if (step_delay(SEC_1*2)) sExecStep = ADCTEST_MAIN;
    return;

//
// AD7705 Adc Value Read
//

 case ADCTEST_READ:
    screen_clear();
    printf("<Analog Input Read>");
    sExecStep++;
    return;
   
  case ADCTEST_READ+1:
    if (AdcReady[AD_CH_VOLT])
    {
      AdcReady[AD_CH_VOLT] = 0;
#ifdef MONO_POLE
      iadc = (iAdcRead[AD_CH_VOLT] & 0xFFFF);
      iAdcVolt[AD_CH_VOLT] = iadc * BIT_PER_mV / 100000;
#else
      iadc = (iAdcRead[AD_CH_VOLT] & 0xFFFF) - 0x8000;
      iAdcVolt[AD_CH_VOLT] = iadc * BIT_PER_mV / 100000;
#endif

      //iAdcVolt[0] = (iadc*12500) >> 16;
      //printf("\nCH[VOLT]:0x%04X", iAdcRead[AD_CH_VOLT]&0xFFFF);
      dg0 =  iAdcVolt[AD_CH_VOLT]/100; dg1 = iAdcVolt[AD_CH_VOLT]%100;
      if (dg1 < 0) dg1 *= -1;
      goto_cursor(0,0);
      printf("\nCH[VOLT]:%6d", iadc);
      printf("\nDC Volt :%3d.%02d[V]", dg0, dg1);
      //printf("\nfVolt/iV:%4.2f/%6d", fVoltInput, iVoltInput);
      printf("\nADC Time  :%4d[mS]", sAdcTime[AD_CH_VOLT]);
    }
    if (ENTER_KEY) sExecStep = ADCTEST_MAIN;
    else sExecStep++;
    return;

  case ADCTEST_READ+2:
    if (AdcReady[AD_CH_AMP])
    {
      AdcReady[AD_CH_AMP] = 0;

      iadc = (iAdcRead[AD_CH_AMP] & 0xFFFF);
      iAdcVolt[AD_CH_AMP] = iadc * BIT_PER_uA / 100000;

      dg0 =  iAdcVolt[AD_CH_AMP]/100; dg1 = iAdcVolt[AD_CH_AMP]%100;
      if (dg1 < 0) dg1 *= -1;
      goto_cursor(0,3);
      printf("\nCH[AMP] :%6d", iadc);
      printf("\nAmpSense: %3d.%02d[mV]", dg0, dg1);
      //printf("\nfAmpiAmp:%5.0f/%6d", fAmpInput, iAmpInput);
      printf("\nADC Time  :%4d[mS]", sAdcTime[AD_CH_AMP]);
    }
    if (ENTER_KEY) sExecStep = ADCTEST_MAIN;
    else sExecStep++;
    return;
    
  case ADCTEST_READ+3:
    if (ENTER_KEY) sExecStep = ADCTEST_MAIN;
    else sExecStep = ADCTEST_READ+1;
    return; 

//
// AD7705 Zero Offset Set(Current)
//
 case ADCTEST_ZERO1SET:
    iTempSet = iAdcAvrAmp + iAmpOffset;
    if (iTempSet < 0) iTempSet *= -1;
    if (iTempSet < MAX_OFFSET_RANGE) sExecStep++;
    else sExecStep = ADCTEST_ZERO_ERR;
    return; 

 case ADCTEST_ZERO1SET+1:
    screen_clear();
    printf("<<Analog CH[1:AMP]>>");
    printf("\n>ZERO Offset Adjust");
    Answer = 0;
    sExecStep++;
    return; 
    
  case ADCTEST_ZERO1SET+2:
    goto_cursor(0,1);
    printf("\nAmpSense  :"); print_amp_sense(iAdcAvrAmp);
    iTempSet = iAdcAvrAmp + iAmpOffset;
    printf("\nNow Offset:"); print_amp_sense(iTempSet); 

    if (ENTER_KEY) sExecStep++;
      else if ((MENU_UP)|(MENU_DN)) Answer ^= 1;
    
    if (Answer) printf("\nZero Clear? [YES]");
      else printf("\nZero Clear? [NO] ");
    return;
    
  case ADCTEST_ZERO1SET+3:
    if (Answer == 1) 
    {
      iAmpOffset = iTempSet;
      input_signal_average();
      printf("\nAmpSense  :"); print_amp_sense(iAdcAvrAmp);
      printf("\nZero Adjust Complete");
      sExecStep = ADCTEST_SAVE_END;
    }
    else 
    {
      printf("\nZero Adjust Cancel..");
      sExecStep = ADCTEST_END;
    }
    return;
//
// AD7705 Zero Offset Set(Voltage)
//
 case ADCTEST_ZERO2SET:
    iTempSet = iAdcAvrVolt + iVoltOffset;;
    if (iTempSet < 0) iTempSet *= -1;
    if (iTempSet < MAX_OFFSET_RANGE) sExecStep++;
    else sExecStep = ADCTEST_ZERO_ERR;
    return; 

 case ADCTEST_ZERO2SET+1:
    screen_clear();
    printf("<<Analog CH[2:Volt]>>");
    printf("\n>ZERO Offset Adjust");
    Answer = 0;
    sExecStep++;
    return; 
    
  case ADCTEST_ZERO2SET+2:
    goto_cursor(0,1);
    printf("\nVolt Sense:"); print_volt_sense(iAdcAvrVolt);
    iTempSet = iAdcAvrVolt + iVoltOffset;
    printf("\nNow Offset:"); print_volt_sense(iTempSet); 

    if (ENTER_KEY) sExecStep++;
      else if ((MENU_UP)|(MENU_DN)) Answer ^= 1;
    
    if (Answer) printf("\nZero Clear? [YES]");
      else printf("\nZero Clear? [NO] ");
    return;
    
  case ADCTEST_ZERO2SET+3:
    if (Answer == 1) 
    {
      iVoltOffset = iTempSet;
      input_signal_average();
      printf("\nVolt Sense :"); print_volt_sense(iAdcAvrVolt);
      printf("\nZero Adjust Complete");
      sExecStep = ADCTEST_SAVE_END;
    }
    else 
    {
      printf("\nZero Adjust Cancel..");
      sExecStep = ADCTEST_END;
    }
    return;
 
 case ADCTEST_NOT_READY:
    screen_clear();
    printf("Under Construction..");
    DelayStep = 0;
    sExecStep++;
    return;
    
  case ADCTEST_NOT_READY+1:
    if (ENTER_KEY) sExecStep = ADCTEST_MAIN;
    else if (step_delay(SEC_1*2)) sExecStep = ADCTEST_MAIN;
    return;  
  
//*************************************************
//   교정이 불가능한 값인 경우 안내문 표시
//*************************************************
  case ADCTEST_ZERO_ERR:
   DelayStep = 0;
   display_mode(2);
   printf("영점조정가능값 이상이므로 교정할 수 없습니다.");
   sExecStep = ADCTEST_SPAN_ERR+1;
   return;
   
  case ADCTEST_SPAN_ERR:
   DelayStep = 0;
   display_mode(2);
   printf("최소교정가능값 이하이므로 교정할 수 없습니다.");
   sExecStep++;
   return;

 case ADCTEST_SPAN_ERR+1:
    if (ANY_KEY) sExecStep++;
    else step_delay(SEC_1*3);
   return;
   
 case ADCTEST_SPAN_ERR+2:
   display_mode(0);
   sExecStep = ADCTEST_MAIN;
   return;
   
//*************************************************
//   변경된 데이터를 저장하고 
//   3초간 지연 또는 키입력으로 메인메뉴로 복귀
//*************************************************
  case ADCTEST_SAVE_END:
    result = backup_data_save();
    if (result == true) printf("\nAdjust Data Save OK");
    else 
    {
      printf("\nBackup Memory Error!!");
      printf("\nData Save Fail..");
    }
    DelayStep = 0;
    sExecStep++;
    return;
    
  case ADCTEST_SAVE_END+1:
    if (ANY_KEY) sExecStep = ADCTEST_MAIN;
    else if (step_delay(SEC_1*3)) sExecStep = ADCTEST_MAIN;
    return;    

//
//   1Sec 지연후 메인메뉴로 복귀
//
  case ADCTEST_END:
    DelayStep = 0;
    sExecStep = ADCTEST_SAVE_END+1;
    return;
 
  default:
    sExecStep = ADCTEST_MAIN;
    return;
  }
}

