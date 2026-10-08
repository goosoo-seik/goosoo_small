

// Include Standard LIB  files
#include "project.h"

void print_guide_message1(void)
{
#ifdef HANGUL
    printf("구수디지탈정류기");
    printf("KS0902_DCS03");
    printf("\nVersion %01d.%02d", VERSION, RELEASE);
    printf("\n%4d년%2d월%2d일", UPDATE_YEAR,UPDATE_MONTH,UPDATE_DATE); 
  #ifdef MONO_POLE  
    printf("\n정운전(Mono)형");
  #else  
    printf("\n정/역운전(PR)형");
  #endif    
#else
    printf("KS0902_DCS03");
    printf("\nVersion %01d.%02d", VERSION, RELEASE);
    printf("\n%4d/%2d/%2d/", UPDATE_YEAR,UPDATE_MONTH,UPDATE_DATE); 
  #ifdef MONO_POLE  
    printf("\nMono-Pole Type");
  #else  
    printf("\nBi-Pole Type");
  #endif    
#endif
}

void print_guide_message2(void)
{
#ifdef HANGUL
    printf("<A/S 문의>\n");
    printf("구수중전기(주)\n");
    printf("정류기사업부\n");
    printf("82-31-497-3521");
#else
    printf("<A/S Center>\n");
    printf("KooSoo Ecectric\n");
    printf("Rectifier Part\n");
    printf("82-31-497-3521");
#endif
}

short OpDelay;
char OpStep;
char RunComplete;
char RemoteRun;
char SystemRun;
int iRiseTime;
char MaxHour, MaxMinute, MaxSec;
char RunHour, RunMinute, RunSec;
char Sec0;
char OperMode;  // CC/CV mode
char OperUser;  // Local/Remote
char OperPole;  // Forward, Reverse
char OldOperPole; // Old Forward, Reverse
char LocalMode, LocalPole;
char RemotMode, RemotPole;
//char OperStop;
char ViewPage;
char CursorBlockNo;
int iRunTime;
int iMaxRunTime;
int iMaxOperAmp; 
int iMaxOperVolt;
int iOperAmp; 
int iRevOperAmp;
int iOperVolt;
int iRevOperVolt;
int iMaxOverAmp;
int iSoftTime;
int iReactRate;
int iPoleTurnTime;
int iPoleTurnWait;
int resetcount;

float fMaxOverAmp;
float fMaxOperAmp;
float fOperAmp;
float fRevOperAmp;

float fMaxOverVolt;
float fMaxOperVolt;
float fOperVolt;
float fRevOperVolt;


void system_start(void)
{
  SystemRun = ON;
  STARTlamp(ON);
  RunComplete = OFF;
  sConStep = 1;
  iRiseTime = 0;
  iRunTime = 0;
  iMaxRunTime = (MaxHour * 3600) + (MaxMinute * 60) + MaxSec;
  execmode_change(RUN_STATUS);
}

void system_stop(void)
{
  if (SystemRun)
  {
    control_amp_out(0, fMaxOperAmp);
    control_volt_out(0, fMaxOperVolt);
    SystemRun = OFF;
  }
  STARTlamp(OFF);
  iRiseTime = 0;
  //sConStep = 0;

  //DAout_ad5663(0, 0, 0);
  //DAout_ad5663(0, 1, 0);
  fOutAmp = 0;
  fOutAmpOld = 0;
  fOutVolt = 0;
  fOutVoltOld = 0;
  RunHour = 0;
  RunMinute = 0; 
  RunSec = 0;
  iRunTime = 0;
}

// 운전상황에 따라 경보 발생
//int iTotalAcAmp;
int iAcOverAmp;
char OpError;
char TotalError;
char TotalAlarm;
char proTalarm;
char TotalVcsTrip;
char ErrorStop;
char ReadyStop;
char AmpOverErr;
char VoltOverErr;
char AcLowFault;
char AcLowAlarm;
char DcOverErr;
char AdcError;
char DacError;
char BootError;
char PhaseError;
char RemoteError;
char LineEmegErr;
// 2009-02-13 수정
char EmegError;
//char ExtFuseErr;
//char ExtVcsErr;
char ExtManualOp;
//char ExtScrErr;
//char ExtTrErr;
//char ExtTempErr;
//char ExtWaterErr;
//char ExtCoolerErr;
char ExtError1;
char ExtError2;
char ExtError3;
char ExtError4;
char ExtError5;
char ExtError6;
char ExtAlarm1;
char ExtAlarm2;
char ExtAlarm3;
char ExtAlarm4;
char ExtAlarm5;
char ExtAlarm6;
char ExtAlarm7;
char ExtAlarm8;
char PoleTurnError;

// 2009-02-13 수정
void all_error_reset(void)
{ 
  //ExtFuseErr  = 0;
  //ExtVcsErr   = 0;
  ExtManualOp = 0;
  //ExtTrErr    = 0;
  //ExtScrErr   = 0;
  //ExtTempErr  = 0;
  //ExtWaterErr = 0;
  //ExtCoolerErr  = 0;
  ExtError1 = 0;
  ExtError2 = 0;
  ExtError3 = 0;
  ExtError4 = 0;
  ExtError5 = 0;
  ExtError6 = 0;
  ExtAlarm1 = 0;
  ExtAlarm2 = 0;
  ExtAlarm3 = 0;
  ExtAlarm4 = 0;
  ExtAlarm5 = 0;
  ExtAlarm6 = 0;
  ExtAlarm7 = 0;
  ExtAlarm8 = 0;
  
  AmpOverErr  = 0;
  VoltOverErr = 0;
  DcOverErr   = 0;
  AcOverErr   = 0;
  AcLowVoltErr = 0;
  AcLowFault = 0;
  AcLowAlarm = 0;
  AcOverCount = 0;
  AdcError    = 0;
  DacError    = 0;
  PwMeterError = 0;
  OperSumErr = 0;
  MeterSumErr = 0;
  BootError = 0;
  
  PhaseError  = 0;
  RemoteError = 0;
  ProfiTestOut = 0;

  TotalError  = 0;
  TotalAlarm  = 0;
  proTalarm =0;
  TotalVcsTrip = 0;
  ErrorStop = 0;
  PoleTurnError = 0;
  RemoteLiveError = 0; 
  
  // 2009-01-09 펌웨어 오류 발생 대비용
  DacTestMode = 0;
  resetcount=256;
  extout_onoff(EXTOUT_FAULT, OFF);
  extout_onoff(EXTOUT_ALARM, OFF);
  extout_onoff(EXTOUT_MCCB_TRIP, OFF);
}

#define EXTIN_NO_ERROR  0x01
// 2009-02-13 수정
void extin_ararm_deside(void)
{
  if (OperUser == LOCAL)
  {
    OperMode = LocalMode;
    OperPole = LocalPole;
  }
  else 
   {
  // Remote 상태에서는 외부 입력에 따라 CC/CV, 정/역을 결정
    OperMode = LocalMode;//RemotMode;
    OperPole = RemotPole;
  }
  // 2010.11.8 fault alram 모두 자동 클리어  
  if (EmegStop   == ON) EmegError = 1; else EmegError = 0; 
  if (ExtIn[EXTIN_MANUAL_OP] == ON) ExtManualOp = 1; else ExtManualOp = 0;
  if (ExtIn[EXTIN_ERROR_1] == ON) ExtError1 = 1; else ExtError1 = 0;
  if (ExtIn[EXTIN_ERROR_2] == ON) ExtError2 = 1; else ExtError2 = 0;
  if (ExtIn[EXTIN_ERROR_3] == ON) ExtError3 = 1; else ExtError3 = 0;
  if (ExtIn[EXTIN_ERROR_4] == ON) ExtError4 = 1; else ExtError4 = 0;
  if (ExtIn[EXTIN_ERROR_5] == ON) ExtError5 = 1; else ExtError5 = 0; 
  if (ExtIn[EXTIN_ERROR_6] == ON) ExtError6 = 1; else  ExtError6 = 0;
  if (ExtIn[EXTIN_ERROR_7] == ON) ExtAlarm1 = 1; else ExtAlarm1 = 0;
  if (ExtIn[EXTIN_ERROR_8] == ON) ExtAlarm2 = 1;  else ExtAlarm2 = 0;
  if (ExtIn[EXTIN_ERROR_9] == ON) ExtAlarm3 = 1;  else ExtAlarm3 = 0;
  if (ExtIn[EXTIN_ERROR_10] == ON) ExtAlarm4 = 1;  else ExtAlarm4 = 0;
  if (ExtIn[EXTIN_ERROR_11] == ON) ExtAlarm5 = 1;  else ExtAlarm5 = 0;
  if (ExtIn[EXTIN_ERROR_12] == ON) ExtAlarm6 = 1;  else ExtAlarm6 = 0;
  if (ExtIn[EXTIN_ERROR_13] == ON) ExtAlarm7 = 1;  else ExtAlarm7 = 0;
  if (ExtIn[EXTIN_ERROR_14] == ON) ExtAlarm8 = 1;  else ExtAlarm8 = 0;
#ifdef MONO_POLE 
  PoleTurnError = 0;
#else 
  if (OldOperPole != OperPole)
  {
    OldOperPole = OperPole;
    iPoleTurnWait = iPoleTurnTime * SEC_1 / 10;
  }
  if (iPoleTurnWait != 0) iPoleTurnWait--;
  if (iPoleTurnWait) PoleTurnError = 1; else PoleTurnError = 0;
#endif 
}

//
// DC 과전압, 과전류 체크
// 과전류가 검출되면 AmpOverErr/VoltOverErr set.
//
#define DC_OVER_WAIT  25
char AmpOverCnt, VoltOverCnt;
void dc_amp_volt_over_check(void)
{
    //2008/05/09 구수 김태곤과장과 협의
    //CC, CV모드와 무관하게 과전압, 과전류 점검
float fa, fv;
    if (fAmpInput  < 0) fa = fAmpInput  * -1; else fa = fAmpInput;
    if (fVoltInput < 0) fv = fVoltInput * -1; else fv = fVoltInput;
    if (fa > fMaxOverAmp ) AmpOverCnt++;  else AmpOverCnt  = 0;       
    if (fv > fMaxOverVolt) VoltOverCnt++; else VoltOverCnt = 0; 

    if (AmpOverCnt  >= DC_OVER_WAIT) AmpOverErr  = 1;
    if (VoltOverCnt >= DC_OVER_WAIT) VoltOverErr = 1;
}

//
// Remote status check
//
void remote_status_check(void)
{
  if (OperUser == LOCAL) RemoteError = 0;
}

//
// Ext Alarm check
// Ext Relay ON/OFF decide
//
// 2009-02-13 수정
void extout_ararm_deside(void)
{
  if (SystemRun == ON)
  {
    extout_onoff(EXTOUT_RUNNING, ON);
    dc_amp_volt_over_check();
  }
  else extout_onoff(EXTOUT_RUNNING, OFF);
  
  if (OperUser == REMOTE) extout_onoff(EXTOUT_REMOTE, ON);
  else extout_onoff(EXTOUT_REMOTE, OFF);
 
  if (ExtManualOp == 0) extout_onoff(EXTOUT_DAOUT, ON);
  else extout_onoff(EXTOUT_DAOUT, OFF);
  
  DcOverErr = AmpOverErr + VoltOverErr;
 
  //remote_status_check(); not use

  if (SystemRun) 
  {
    if (AcLowVoltErr) AcLowFault = 1;
  }
  else 
  {
    if (AcLowVoltErr) AcLowAlarm = 1; else AcLowAlarm = 0;
  }
  
  TotalError = EmegError+  ExtError1 + ExtError2 + ExtError3 + ExtError4 + ExtError5 + ExtError6
             +ExtAlarm1 ;// + DcOverErr ;//+ AcOverErr ;// + AcLowFault; 
  
  if (TotalError) extout_onoff(EXTOUT_FAULT, ON);
  else extout_onoff(EXTOUT_FAULT, OFF);

  TotalVcsTrip = 0 ;//EmegError + DcOverErr +ExtError1 + ExtError2 + AcOverErr;
  
  if (TotalVcsTrip) extout_onoff(EXTOUT_MCCB_TRIP, ON);
  else extout_onoff(EXTOUT_MCCB_TRIP, OFF);
  
  // 2008-11-26 ExtVcsErr 추가, ExtWaterErr, ExtCoolerErr 삭제
  // 2008-12-17 PwMeterError 임시 삭제
  proTalarm =  ExtAlarm2 + ExtAlarm3 + ExtAlarm4 + ExtAlarm5 + ExtAlarm6 + ExtAlarm7 + ExtAlarm8 ;
  TotalAlarm = proTalarm + OperSumErr  + BootError + LineEmegErr   + UnExecuteCMD + ProfiTestOut ; // ExtManualOp +  + MeterSumErr + AcLowAlarm

#ifndef MONO_POLE 
  TotalAlarm += PoleTurnError;
#endif

// 2009/03/26 4 word PLC 통신 추가
//#ifdef PROFI_WORD_4
 // TotalAlarm += RemoteLiveError;
//#endif
  
  if (OperUser == REMOTE) TotalAlarm += RemoteError;
  
  if (TotalAlarm) extout_onoff(EXTOUT_ALARM, ON); 
  else extout_onoff(EXTOUT_ALARM, OFF);
  
  // 2009-02-13
  if ((TotalError != 0)|(LineEmegErr != 0)) 
    ErrorStop = 1; else ErrorStop = 0;

  // 2009-02-13
  //ReadyStop = TotalError + OperSumErr + AdcError + 
  //              DacError + AcLowAlarm + ExtManualOp;
  // 2009-02-13
  ReadyStop = TotalError + TotalAlarm;
  
  
  if(resetcount >2 ) {extout_onoff(EXTOUT_READY, ON); resetcount--; }  // reset 추가
  else { extout_onoff(EXTOUT_READY, OFF); resetcount=0; }
 // if (!ReadyStop) extout_onoff(EXTOUT_READY, ON);
  //else extout_onoff(EXTOUT_READY, OFF);
}

//
// 현재 일자 및 시간 표시
void clock_display(void)
{
  printf("%2X/%02X %2X:%02X:%02X", Month, Date, Hour, Minute, Sec);
}

char ViewStep;
short sViewDelay;
#define VIEW_TIME   SEC_1/2 
#define EXTIN_VIEW  10
/**************************************/
/*   Event Status View                */
/*  정상시: 설정시간 표시             */
/*  에러시: 에러 발생원인 표시        */
/**************************************/
// 2009-02-13 수정
void event_status_view(void)
{
  switch(ViewStep)
  {
  //case 0: remote_status_display();return;
  case 0:
    if (TotalError|TotalAlarm|RemoteLiveError) ViewStep = EXTIN_VIEW;
    else 
    {
      if (OperUser == LOCAL) clock_display();
       else remote_status_display();
      sViewDelay = 0;
      ViewStep++;
    }
    return;
    
  case 1: if (++sViewDelay > VIEW_TIME) ViewStep = 0; return;
           
  case EXTIN_VIEW:    // Emergency Stop Error
    if (EmegError) {printf("Emergency Stop"); sViewDelay = 0;}
    ViewStep++; return;    
  case EXTIN_VIEW+1: if (++sViewDelay > VIEW_TIME) ViewStep++; return;
    
  case EXTIN_VIEW+2:     // Ext IO Error 1
    if (ExtError1) { printf("FT:AC.E.O.C.R "); sViewDelay = 0;}
    ViewStep++; return;
  case EXTIN_VIEW+3: if (++sViewDelay > VIEW_TIME) ViewStep++; return;

  case EXTIN_VIEW+4:    // Ext IO Error 2
    if (ExtError2) { printf("FT:SCRFUSE_CUT"); sViewDelay = 0;}
    ViewStep++;  return;    
  case EXTIN_VIEW+5: if (++sViewDelay > VIEW_TIME) ViewStep++; return;
  
  case EXTIN_VIEW+6:    // Ext IO Error 3
    if (ExtError3) { printf("FT:Water Temp "); sViewDelay = 0;}
    ViewStep++; return;    
  case EXTIN_VIEW+7: if (++sViewDelay > VIEW_TIME) ViewStep++; return;
  
  case EXTIN_VIEW+8:   // Manual Operate
    if (ExtManualOp) { printf("Manual Operate"); sViewDelay = 0;}
    ViewStep++; return;
  case EXTIN_VIEW+9: if (++sViewDelay > VIEW_TIME) ViewStep++; return;

  case EXTIN_VIEW+10:    // Ext IO Error 4
    if (ExtError4) {printf("FT:PHASE_FAULT"); sViewDelay = 0;}
    ViewStep++; return;    
  case EXTIN_VIEW+11: if (++sViewDelay > VIEW_TIME) ViewStep++; return;

  case EXTIN_VIEW+12:    // Ext IO Error 5
    if (ExtError5) {printf("FT:SCR TEMP   "); sViewDelay = 0;}
    ViewStep++; return;    
  case EXTIN_VIEW+13: if (++sViewDelay > VIEW_TIME) ViewStep++; return;
    
  case EXTIN_VIEW+14:    // Ext IO Error 6
    if (ExtError6) {printf("FT:WATER CUT  "); sViewDelay = 0;}
    ViewStep++; return;    
  case EXTIN_VIEW+15: if (++sViewDelay > VIEW_TIME) ViewStep++; return;

  case EXTIN_VIEW+16:   // DC-OCR Error
    if (AmpOverErr)  {printf("DC Amp. Over  "); sViewDelay = 0;}
    ViewStep++; return;    
  case EXTIN_VIEW+17: if (++sViewDelay > VIEW_TIME) ViewStep++; return;

  case EXTIN_VIEW+18:   // DC-OVR Error
    if (VoltOverErr)  {printf("DC Volt Over  "); sViewDelay = 0;}
    ViewStep++; return;    
  case EXTIN_VIEW+19: if (++sViewDelay > VIEW_TIME) ViewStep++; return;

  case EXTIN_VIEW+20:   // AC-OCR Error
    if (AcOverErr) {printf("AC Amp Over   "); sViewDelay = 0;}
    ViewStep++; return;    
  case EXTIN_VIEW+21: if (++sViewDelay > VIEW_TIME) ViewStep++; return;
    
  case EXTIN_VIEW+22:   // DAC Fault
    if (DacError) {printf("DAC Fault     "); sViewDelay = 0;}
    ViewStep++; return;    
  case EXTIN_VIEW+23: if (++sViewDelay > VIEW_TIME) ViewStep++; return;
 
  case EXTIN_VIEW+24:   // ADC Fault
    if (AdcError) {printf("ADC Fault     "); sViewDelay = 0;}
    ViewStep++; return;    
  case EXTIN_VIEW+25: if (++sViewDelay > VIEW_TIME) ViewStep++; return;

  case EXTIN_VIEW+26:   // AC Meter Fault
    if (PwMeterError) {printf("AC Meter Fault"); sViewDelay = 0;}
    ViewStep++; return;    
  case EXTIN_VIEW+27: if (++sViewDelay > VIEW_TIME) ViewStep++; return;

  case EXTIN_VIEW+28:   // Remote Emergency Stop
    if (LineEmegErr) {printf("Emeg Stop-Line"); sViewDelay = 0;}
    ViewStep++; return;    
  case EXTIN_VIEW+29: if (++sViewDelay > VIEW_TIME) ViewStep++; return;

  case EXTIN_VIEW+30:   // AC-OCR Error
    if (AcLowVoltErr) {printf("AC Low Volt   "); sViewDelay = 0;}
    ViewStep++; return;    
  case EXTIN_VIEW+31: if (++sViewDelay > VIEW_TIME) ViewStep++; return;
    
  case EXTIN_VIEW+32:   // Remote Device Error
    if (RemoteError) {printf("Remote Error  "); sViewDelay = 0;}
    ViewStep++; return;     
  case EXTIN_VIEW+33: if (++sViewDelay > VIEW_TIME) ViewStep++; return;

  case EXTIN_VIEW+34:   // Remote UnExecuteCMD Error
    if (UnExecuteCMD) {printf("Unable CMD    "); sViewDelay = 0;}
    ViewStep++; return;     
  case EXTIN_VIEW+35: if (++sViewDelay > VIEW_TIME) ViewStep++; return;

  case EXTIN_VIEW+36:   // Remote Live Error
    if (RemoteLiveError) {printf("Remote: STOP  "); sViewDelay = 0;}
    ViewStep++; return;     
  case EXTIN_VIEW+37: if (++sViewDelay > VIEW_TIME) ViewStep++; return;

  case EXTIN_VIEW+38:   // Pole Turnover Error
    if (PoleTurnError) {printf("Pole Turnover "); sViewDelay = 0;}
    ViewStep++; return;     
  case EXTIN_VIEW+39: if (++sViewDelay > VIEW_TIME) ViewStep++; return;

  case EXTIN_VIEW+40:   // Remote Status display
    if (OperUser == REMOTE) {remote_status_display(); sViewDelay = 0;}
    ViewStep++; return;
  case EXTIN_VIEW+41: if (++sViewDelay > VIEW_TIME) ViewStep++; return;  
  
  case EXTIN_VIEW+42:    // Ext Alarm 1
    if (ExtAlarm1) {printf("FT:TR OIL TEMP"); sViewDelay = 0;}
    ViewStep++; return; 
  case EXTIN_VIEW+43: if (++sViewDelay > VIEW_TIME) ViewStep++; return; 
  
  case EXTIN_VIEW+44:    // Ext Alarm 2
    if (ExtAlarm2) {printf("AL:CH_FAN TRIP"); sViewDelay = 0;}
    ViewStep++; return; 
  case EXTIN_VIEW+45: if (++sViewDelay > VIEW_TIME) ViewStep++; return;
   
  case EXTIN_VIEW+46:    // Ext Alarm 3
    if (ExtAlarm3) {printf("AL:O_PUMP_TRIP"); sViewDelay = 0;}
    ViewStep++; return;
  case EXTIN_VIEW+47: if (++sViewDelay > VIEW_TIME) ViewStep++; return; 
  
  case EXTIN_VIEW+48:    // Ext Alarm 4
    if (ExtAlarm4) {printf("AL:S C R_TEMP "); sViewDelay = 0;}
    ViewStep++; return;
  case EXTIN_VIEW+49: if (++sViewDelay > VIEW_TIME) ViewStep++; return;
  
  case EXTIN_VIEW+50:    // Ext Alarm 5
    if (ExtAlarm5) {printf("AL:TR 0il-Temp"); sViewDelay = 0;}
    ViewStep++; return;
  case EXTIN_VIEW+51: if (++sViewDelay > VIEW_TIME) ViewStep++; return; 
  
  case EXTIN_VIEW+52:    // Ext Alarm 6
    if (ExtAlarm6) {printf("AL:6Phase Fuse"); sViewDelay = 0;}
    ViewStep++; return; 
  case EXTIN_VIEW+53: if (++sViewDelay > VIEW_TIME) ViewStep++; return;
  
  case EXTIN_VIEW+54:    // Ext Alarm 7
    if (ExtAlarm7) {printf("AL:TR_Pressure"); sViewDelay = 0;}
    ViewStep++; return; 
  case EXTIN_VIEW+55: if (++sViewDelay > VIEW_TIME) ViewStep++; return;
  
  case EXTIN_VIEW+56:    // Ext Alarm 8
    if (ExtAlarm8) {printf("AL:WATER_TEMP "); sViewDelay = 0;}
    ViewStep++; return;
  case EXTIN_VIEW+57: if (++sViewDelay > VIEW_TIME) ViewStep++; return;    
   
  //case EXTIN_VIEW+49: if (++sViewDelay > VIEW_TIME) ViewStep++; return;

  case EXTIN_VIEW+58: ViewStep = 0; return;

  default: ViewStep = 0; return;
  }
}

/*****************************************/
/*    정류기 운전                        */
/*****************************************/
#define OP_RUN      100
#define OP_ERROR    200
char BootGuideEnd;
char RemoteReady;
//
// 2026-10-08 추가: 웜 리스타트 운전 상태 복원 (CSLab_Rectifier_reset.c warm_restart_late 에서 호출)
// system_start() 와 같지만 출력 제어를 처음부터 시작(sConStep = 1)하지 않고,
// 부팅 시퀀스(아래 case 0~7: 출력 래치 OFF, DAC 클리어, 부팅 안내 대기)를 건너뛰어 바로 OP_RUN
//   - iRiseTime 을 소프트스타트 완료 값으로 -> 소프트스타트 없음
//   - iRunTime 은 리셋 직전 운전시간에서 이어서 셈
//   - RemotPole 도 복원 (REMOTE 에서는 extin_ararm_deside() 가 OperPole = RemotPole 로 덮어씀)
//   - ExecMode 는 바꾸지 않음: 부팅 안내(GUIDE_MESSAGE) 화면이 PLC 통신(RemoteStep = 1)을 시작함
//
void op_warm_resume(char user, char mode, char pole, int runtime)
{
  SystemRun = ON;
  OperUser = user;
  OperMode = mode;
  OperPole = pole;
  RemotPole = pole;
  STARTlamp(ON);
  RunComplete = OFF;
  iRiseTime = iSoftTime * SEC_1 / 10;   // 소프트스타트 완료 상태
  iRunTime = runtime;
  iMaxRunTime = (MaxHour * 3600) + (MaxMinute * 60) + MaxSec;
  OpStep = OP_RUN;
}

//
// 2026-10-08 추가: RUN/STOP 키 판정과 운전 시작/정지를 UART 로 보고 (CSLab_Rectifier_Main.h DBG_KEYLOG)
//   [KEY] t=.. key=RUN user=LOCAL run=0 emeg=0 errstop=0 toterr=0 line=0 -> start | ignored:이유
//   [KEY] t=.. lost key=RUN now=0x..     패널에서 눌렸는데 운전 판단 전에 다른 키 값으로 바뀜/지워짐
//   [RUN] t=.. start|stop by=..          운전 상태가 바뀜 (원인)
//   패널 키는 한 스캔(2ms)짜리 이벤트라, 그 순간 조건이 안 맞으면 그대로 무시됨
//
#ifdef DBG_KEYLOG
char DbgPanelKey;                       // 이번 스캔 panel_key_scan() 직후의 PushKey (main.c)
static char DbgRun0 = 0x55;             // 지난번 확인한 SystemRun (0x55 = 아직 모름)
static char DbgKey0;                    // 판단 전 PushKey (system_start 의 execmode_change 가 PushKey 를 지우므로)

static void dbg_op_check(char after)
{
  const char *why;
  char key;

  if (!after) DbgKey0 = PushKey;
  key = DbgKey0;
  if (!after)
  {
    // 판단 전 : OP_RUN 밖(PLC 정지 비트 등)에서 운전 상태가 바뀌었으면 먼저 보고
    if ((DbgRun0 != 0x55) & (DbgRun0 != SystemRun))
      dbg_printf("[RUN] t=%u %s by=%s\r\n", uiRstScan * 2, SystemRun ? "start" : "stop", SystemRun ? "other" : "PLC_stop_or_other");
    DbgRun0 = SystemRun;
    // 패널에서 RUN/STOP 이 눌렸는데 여기까지 오는 동안 PushKey 가 바뀐 경우
    if (((DbgPanelKey == 'r') | (DbgPanelKey == 's')) & (key != DbgPanelKey))
      dbg_printf("[KEY] t=%u lost key=%s now=0x%02X\r\n", uiRstScan * 2, (DbgPanelKey == 'r') ? "RUN" : "STOP", (unsigned int)(unsigned char)key);
    if (key == 'r')
    {
      if (OperUser != LOCAL) why = "ignored:REMOTE";
      else if (SystemRun) why = "ignored:already_running";
      else if (EmegStop) why = "ignored:emergency";
      else if (ErrorStop) why = "ignored:fault";
      else why = "start";
    }
    else if (key == 's') why = (OperUser == LOCAL) ? "stop+error_reset" : "error_reset_only(REMOTE)";
    else return;
    dbg_printf("[KEY] t=%u key=%s user=%s run=%d emeg=%d errstop=%d toterr=%d line=%d -> %s\r\n",
               uiRstScan * 2, (key == 'r') ? "RUN" : "STOP", (OperUser == LOCAL) ? "LOCAL" : "REMOTE",
               SystemRun ? 1 : 0, EmegStop ? 1 : 0, ErrorStop ? 1 : 0, (int)TotalError, LineEmegErr ? 1 : 0, why);
    return;
  }
  // 판단 후 : 이번 OP_RUN 에서 운전 상태가 바뀌었으면 원인
  if (DbgRun0 == SystemRun) return;
  if (SystemRun) why = (key == 'R') ? "PLC_run" : "RUN_key";
  else if (ErrorStop) why = "fault";
  else if (key == 'x') why = "emergency";           // EMEG_ON (PushKey 'x')
  else if (key == 's') why = "STOP_key";
  else why = "other";
  dbg_printf("[RUN] t=%u %s by=%s\r\n", uiRstScan * 2, SystemRun ? "start" : "stop", why);
  DbgRun0 = SystemRun;
}
#endif

void system_operate(void)
{
  int irun;
  switch(OpStep)
  {
  case 0: 
    DC24power(OFF);
    OpDelay = 0;
    OpStep++; 
    return;

// 2009-01-09 step 1-4 추가
// 전원투입시 DAC 잔류전압출력 제거용    
  case 1:
    DAclear(OFF);
    OpStep++; 
    return;
    
  case 2:
    if (++OpDelay > 2) OpStep++;
    return;
    
  case 3:
    control_volt_out(0, fMaxOperVolt);
    control_amp_out(0, fMaxOperAmp); 
    OpStep++;
    return;

  case 4:
    DAclear(ON);
    OpStep++;
    return;

  case 5:
    if (BootGuideEnd) OpStep++;
    return;
    
  case 6:
    if (++OpDelay > SEC_1) OpStep++;
    return;
    
  case 7:
    DC24power(ON);
    OpStep = OP_RUN;
    return;

  case OP_RUN: 
    //if ((OperUser == REMOTE)&(EXT_RESET)) all_error_reset();
    //if ((OperUser == LOCAL)&(STOP_KEY)) all_error_reset();
#ifdef DBG_KEYLOG
    dbg_op_check(0);                  // 2026-10-08 추가: RUN/STOP 키 판정 이유 [KEY] (판단 전 상태로)
#endif
    if (STOP_KEY) all_error_reset();
    
    if ((SystemRun == OFF)&(EmegStop == OFF))
    //if ((SystemRun == OFF))
    {
      if (ErrorStop == 0)
      {
        if ((OperUser == LOCAL)&(RUN_KEY)) system_start(); 
        if ((OperUser == REMOTE)&(EXT_RUN)) system_start();
      }
    }
    else
    {
      if (iRiseTime < (iSoftTime*SEC_1/10)) iRiseTime++;
      //if ((ErrorStop != 0)|(STOP_KEY)|(EMEG_ON)) system_stop();
#ifdef REMOTE_START_REARM
      // 2026-10-08 추가: 정지 후에는 PLC 시작 비트 0 -> 1 재입력이 있어야 리모트 기동 (CSLab_Rectifier_profi.c)
      if ((ErrorStop != 0)|(EMEG_ON))
      {
        system_stop();
#ifdef REARM_FAULT_STOP
        remote_rearm_set();
#endif
      }
      if ((OperUser == LOCAL)&(STOP_KEY)) { system_stop(); remote_rearm_set(); }
#else
      if ((ErrorStop != 0)|(EMEG_ON)) system_stop();
      if ((OperUser == LOCAL)&(STOP_KEY)) system_stop();
#endif
    
    }
    /*
    if ((SystemRun == OFF)&(EmegStop == OFF))
    //if ((SystemRun == OFF))
    {
      //if (ErrorStop == 0)
      if (ReadyStop == 0)
      {
        if ((OperUser == LOCAL)&(RUN_KEY)) system_start(); 
        if ((OperUser == REMOTE)&(EXT_RUN)) system_start();
      }
    }
    else
    {
      if (iRiseTime < (iSoftTime*SEC_1/10)) iRiseTime++;
      if ((ErrorStop != 0)|(STOP_KEY)|(EMEG_ON)) system_stop();
    }
    
   */
#ifdef DBG_KEYLOG
    dbg_op_check(1);                  // 2026-10-08 추가: 운전 시작/정지가 일어났으면 원인 [RUN]
#endif
    // 운전시간 카운트
    if (Sec0 != Sec)
    {
      Sec0 = Sec;
      if (SystemRun) 
      {
        iRunTime++;
        irun = iRunTime;
        RunHour =irun / 3600;
        irun = irun % 3600;
        RunMinute = irun / 60;
        RunSec = irun % 60;
       // if (iMaxRunTime)
       //  if (iRunTime >= iMaxRunTime) 
       //   {
       //     RunComplete = ON;
       //     system_stop();
       //   }
      }
    }
    return;
    
  case OP_ERROR:
    OpError = 1;
    return;
    
  default:
    OpStep = 0;
    return;
  }
}    

/********************************************/
/* Sysyem Running Status Display            */
/********************************************/
#define MAX_CURSOR_BLOCK  3
#define BLOCK_MOVE_INTERVAL 100
void running_status_view(void);

void operate_status_view(void)
{
  if (ViewPage == 0) running_status_view();
  else if (ViewPage == 1) power_status_view();
  else if (ViewPage == 2) control_status_view();
  else ViewPage = 0;
}

//************************************
// 운전상태 표시
// 기본표시 화면(ViewPage = 0)
//************************************
#define RUN_STATUS_SET    100
#define RUN_OPVOLT_SET    200
#define RUN_OPAMP_SET     300
#define RUN_REVVOLT_SET   400
#define RUN_REVAMP_SET    500
#define RUN_RUNTIME_SET   600
#define RUNNING_SET_END   700
#define RUNNING_SAVE_END  710

short BlockMoveDelay;
void cursor_move_running(void)
{
  if (BlockMoveDelay != 0) BlockMoveDelay--;
  //if (ENTER_KEY) execmode_change(MAIN_MENU);
  if (ENTER_KEY) sExecStep = RUN_STATUS_SET;
  else if ((OperUser == LOCAL)&(BlockMoveDelay== 0 ))
  {
    if (MENU_DN) 
    {
      if (++CursorBlockNo >= MAX_CURSOR_BLOCK) CursorBlockNo = 0;
      cursor_block(CursorBlockNo);
      if (CursorBlockNo == 2) dp_type_assign(BLINK, 62, 63);
      BlockMoveDelay = BLOCK_MOVE_INTERVAL;
    }
    else if (MENU_UP)
    {
      if (CursorBlockNo == 0) CursorBlockNo = MAX_CURSOR_BLOCK - 1; else CursorBlockNo--;
      cursor_block(CursorBlockNo);
      if (CursorBlockNo == 2) dp_type_assign(BLINK, 62, 63); 
      BlockMoveDelay = BLOCK_MOVE_INTERVAL;
    }
  }
}

void running_status_view(void)
{
  short word;
  //char result;
  switch (sExecStep)
  {
  case 0:
    display_mode(2);
    sExecStep++;
    return;
    
  case 1:
    screen_clear();
    DelayStep = 0;
    debug_monit(MONOUT);
    CursorBlockNo = 2;
    cursor_block(CursorBlockNo);
    dp_type_assign(BLINK, 62, 63); 
    sExecStep++;
    return;
    
  case 2:
    CursorUse = 0;
    LineBlink = 2;
    goto_cursor(0,0);
    if (ExtManualOp) printf("MANUAL");
     else if (OperUser == REMOTE) printf("REMOT "); else printf("LOCAL ");
    if (SystemRun == ON) printf("RUN "); else printf("STOP");
    if (OperMode == CC_MODE) printf("[CC:"); else printf("[CV:");
    if (OperPole == PLUS) printf("P]"); else printf("N]");
   // if(OperUser == LOCAL)cursor_move_running();           //CursorBlockNo = 2;
    cursor_move_running();
    if (NO_KEY) sExecStep++;
    return;

  case 3:
    goto_cursor(0,1);
    printf("SET ");
    if (OperPole == PLUS)
    {
      printf_volt(fOperVolt);
      printf("V ");
      goto_cursor(10, 1);
      printf_volt(fOperAmp);
      printf("A");
    }
    else 
    {
      printf_volt(fRevOperVolt);
      printf("V ");
      goto_cursor(10, 1);
      printf_volt(fRevOperAmp);
      printf("A");
    }    
    cursor_move_running();
    if (NO_KEY) sExecStep++;
    return;

  case 4:
    goto_cursor(0,2);
  
// 2009/03/26 4 word PLC 통신 추가
#ifdef PROFI_WORD_4
    printf("RUN %02d:%02d:%02d ", RunHour, RunMinute, RunSec);
    word = 0;   // Warning 없애기 코드
    word = word;
#else
    if (sLineSpeed < 9999) word = sLineSpeed; else word = 9999;
    printf("SPEED:%4dmpm", word);
#endif

    if (CursorBlockNo == 2) 
    {
      printf(" ME"); 
      putchar_pos(62, 'N');
      putchar_pos(63, 'U');
    }
    else 
    {
      printf("   ");
      putchar_pos(62, ' ');
      putchar_pos(63, ' ');
    }
    
    cursor_move_running();
    if (NO_KEY) sExecStep++;
    return;

  case 5:
    DelayStep = 0;
    cursor_move_running();
    if (NO_KEY) sExecStep++;
    return;
    
  case 6:
    goto_cursor(0,3);       
    event_status_view();
    goto_cursor(15,3);   
 
    cursor_move_running();
    if (NO_KEY)
      if (step_delay(SEC_1>>3)) sExecStep = 2;
    return;
    
  case RUN_STATUS_SET:
     if(OperUser == REMOTE)CursorBlockNo = 2;
     
    if((OperUser == LOCAL)&(CursorBlockNo == 0)) //
    { 
      if (OperPole == PLUS) sExecStep = RUN_OPVOLT_SET;
      else sExecStep = RUN_REVVOLT_SET;
      //else if (OperPole == MINUS) sExecStep = RUN_REVVOLT_SET;
      //else sExecStep = RUNNING_SET_END;
    }
    else if((OperUser == LOCAL)&(CursorBlockNo == 1)) //
    { 
      if (OperPole == PLUS) sExecStep = RUN_OPAMP_SET;
      else if (OperPole == MINUS) sExecStep = RUN_REVAMP_SET;
      else sExecStep = RUNNING_SET_END;
    }
    //else if (CursorBlockNo == 2) sExecStep = RUN_RUNTIME_SET;
    else if (CursorBlockNo == 2) execmode_change(MAIN_MENU);
    else sExecStep = RUNNING_SET_END;
    return;

//*************************************************
//    운전전압 변경(FW)
//    운전전압값을 입력받고 플레시메모리에 저장
//*************************************************
  case RUN_OPVOLT_SET:
    DelayStep = 0;
    fTempSet = fOperVolt; 
    sExecStep++;
    return;    
  
  case RUN_OPVOLT_SET+1:
    if (COUNT_UP) 
    {
      float_jog_plus(fMaxOperVolt); 
      DelayStep = 0;
      sExecStep++;
    }
    else if (COUNT_DN) 
    {
      float_jog_minus(0);      
      DelayStep = 0;
      sExecStep++;
    }
    else if (ENTER_KEY) 
    {
      //PidStable = 0;  // 예비 Ref 값 다시 설정
      fOperVolt = fTempSet;
      sExecStep = RUNNING_SAVE_END;
    }
    else if (step_delay(SEC_1*10)) sExecStep = RUNNING_SAVE_END;
    return; 
    
  case RUN_OPVOLT_SET+2:
    fOperVolt = fTempSet;
    goto_cursor(4, 1);
    printf_volt(fOperVolt);
    sExecStep--;
    return;

//*************************************************
//    운전전압 변경(REV)
//    운전전압값을 입력받고 플레시메모리에 저장
//*************************************************
  case RUN_REVVOLT_SET:
    DelayStep = 0;
    fTempSet = fRevOperVolt; 
    sExecStep++;
    return;    
  
  case RUN_REVVOLT_SET+1:
    if (COUNT_UP) 
    {
      float_jog_plus(fMaxOperVolt); 
      DelayStep = 0;
      sExecStep++;
    }
    else if (COUNT_DN) 
    {
      float_jog_minus(0);      
      DelayStep = 0;
      sExecStep++;
    }
    else if (ENTER_KEY) 
    {
      //PidStable = 0;  // 예비 Ref 값 다시 설정
      fRevOperVolt = fTempSet;
      sExecStep = RUNNING_SAVE_END;
    }
    else if (step_delay(SEC_1*10)) sExecStep = RUNNING_SAVE_END;
    return; 
    
  case RUN_REVVOLT_SET+2:
    fRevOperVolt = fTempSet;
    goto_cursor(4, 1);
    printf_volt(fRevOperVolt);
    sExecStep--;
    return;
    
//*************************************************
//    운전전류 변경(FW)
//    운전전류값을 입력받고 플레시메모리에 저장
//*************************************************    
  case RUN_OPAMP_SET:
    DelayStep = 0;
    goto_cursor(10, 1);
    printf_volt(fOperAmp);
    fTempSet = fOperAmp; 
    sExecStep++;
    return;    
  
  case RUN_OPAMP_SET+1:
    if (COUNT_UP) 
    {
      float_jog_plus(fMaxOperAmp); 
      DelayStep = 0;
      sExecStep++;
    }
    else if (COUNT_DN) 
    {
      float_jog_minus(0);      
      DelayStep = 0;
      sExecStep++;
    }
    else if (ENTER_KEY) 
    {
      //PidStable = 0;  // 예비 Ref 값 다시 설정
      fOperAmp = fTempSet;
      sExecStep = RUNNING_SAVE_END;
    }
    else if (step_delay(SEC_1*10)) sExecStep = RUNNING_SAVE_END;
    return; 
        
  case RUN_OPAMP_SET+2:
    fOperAmp = fTempSet;
    goto_cursor(10, 1);
    printf_volt(fOperAmp);
    sExecStep--;
    return;    
    
//*************************************************
//    운전전류 변경(REV)
//    운전전류값을 입력받고 플레시메모리에 저장
//************************************************* 
  case RUN_REVAMP_SET:
    DelayStep = 0;
    goto_cursor(10, 1);
    printf_volt(fRevOperAmp);
    fTempSet = fRevOperAmp; 
    sExecStep++;
    return;    
  
  case RUN_REVAMP_SET+1:
    if (COUNT_UP) 
    {
      float_jog_plus(fMaxOperAmp); 
      DelayStep = 0;
      sExecStep++;
    }
    else if (COUNT_DN) 
    {
      float_jog_minus(0);      
      DelayStep = 0;
      sExecStep++;
    }
    else if (ENTER_KEY) 
    {
      //PidStable = 0;  // 예비 Ref 값 다시 설정
      fRevOperAmp = fTempSet;
      sExecStep = RUNNING_SAVE_END;
    }
    else if (step_delay(SEC_1*10)) sExecStep = RUNNING_SAVE_END;
    return; 
        
  case RUN_REVAMP_SET+2:
    fRevOperAmp = fTempSet;
    goto_cursor(10, 1);
    printf_volt(fRevOperAmp);
    sExecStep--;
    return;    
    
//*************************************************
//    운전시간 변정
//*************************************************
  case RUN_RUNTIME_SET:
    if (MaxHour > 23) MaxHour = 0;
    if (MaxMinute > 59) MaxMinute = 0;
    if (MaxSec > 59) MaxSec = 0;
    iTempSet = (MaxHour * 3600) + (MaxMinute * 60) + MaxSec;
    DPtype_clear();
    LineBlink = 2;
    dp_type_assign(BLINK, 52, 53);
    sExecStep++;
    return;

  case RUN_RUNTIME_SET+1:
    sExecStep++;
    return;

  case RUN_RUNTIME_SET+2:
    if (COUNT_UP) {if (MaxHour < 23) MaxHour++;}
    else if (COUNT_DN) {if (MaxHour > 0) MaxHour--;}
    else if (ENTER_KEY) sExecStep++;
    if (ANY_KEY)
    {
      goto_cursor(4, 3);
      printf("%02d", MaxHour);
    }
    return; 

  case RUN_RUNTIME_SET+3:
    DPtype_clear();
    LineBlink = 2;
    dp_type_assign(BLINK, 55, 56);
    sExecStep++;
    return;  
   
  case RUN_RUNTIME_SET+4:
    if (COUNT_UP) {if (MaxMinute < 59) MaxMinute++;}
    else if (COUNT_DN) {if (MaxMinute > 0) MaxMinute--;}
    else if (ENTER_KEY) sExecStep++;
    if (ANY_KEY)
    {
      goto_cursor(7, 3);
      printf("%02d", MaxMinute);
    }
    return;
   
  case RUN_RUNTIME_SET+5:
    DPtype_clear();
    LineBlink = 2;
    dp_type_assign(BLINK, 58, 59);
    sExecStep++;
    return;  
   
  case RUN_RUNTIME_SET+6:
    if (COUNT_UP) {if (MaxSec < 59) MaxSec++;}
    else if (COUNT_DN) {if (MaxSec > 0) MaxSec--;}
    else if (ENTER_KEY) sExecStep++;
    if (ANY_KEY)
    {
      goto_cursor(10, 3);
      printf("%02d", MaxSec);
    }
    return;
   
  case RUN_RUNTIME_SET+7:
    goto_cursor(4, 3);
    printf("%02d:%02d:%02d", MaxHour, MaxMinute, MaxSec);
    iMaxRunTime = (MaxHour * 3600) + (MaxMinute * 60) + MaxSec;
    DelayStep = 0;
    sExecStep++;
    return;
    
  case RUN_RUNTIME_SET+8:
    if (iTempSet == iMaxRunTime) sExecStep = RUNNING_SET_END;
    else sExecStep = RUNNING_SAVE_END;
    return;    

  case RUNNING_SAVE_END:
    //result = backup_data_save();
    backup_data_save();
    //if (result == false) 
    DelayStep = 0;
    sExecStep = RUNNING_SET_END;
    return;
    
  case RUNNING_SET_END:
    sExecStep = 1;
    return;
    
  default:
    printf("\nError:%4d", sExecStep);
    sExecStep = 0;
    return;
  }  
}


/**********************************/
/*   시스템 부트 초기 안내화면    */
/**********************************/
short sBootDelay;

void wait_next_message(void)
{
  if (STOP_KEY) execmode_change(RUN_STATUS);
  else if (ENTER_KEY) sExecStep++;
  //else if (MENU_UP) sExecStep -= 3;
  else step_delay(SEC_1<<2);
}

void guide_message_display(void)
{
  char type;
  switch (sExecStep)
  {
  case 0:
    BootGuideEnd = 0;
    RemoteStep = 1;
    sExecStep++;
    return;

  case 1:
    display_mode(2);
    LCDlamp(ON);
    sLcdOnTime = SEC_1 * 60;
    sBootDelay = SEC_1;
    sExecStep++;
    return;
    
  case 2:
    // WatchDog Register View
    printf("<시스템진단>");
    type = reset_status_check();
    if ((type == 0)|(type == 4)) BootError = 0; else BootError = type; 
    if (type == 0) printf("\n시스템시동:정상");
    else if (type == 2) printf("\n시스템이 알수없는 장애로 재시동되었습니다.");
    else if (type == 3) printf("\n시스템이 펌웨어오류로  재시동되었습니다.");
    else if (type == 4) printf("\n시스템시동:정상");
    else if (type == 5) printf("\n시스템이 전압강하로 재시동되었습니다.");
    else sExecStep++;
    // 백업메모리가 정상이면 시스템 작동 가능
    if ((!OperSumErr)&(!MeterSumErr)) BootGuideEnd = 1;
    DelayStep = 0;
    sExecStep++;
    return;
    
  case 3:       
    if (ENTER_KEY) sExecStep++; 
    else step_delay(sBootDelay);
    return;

  case 4:
    if (OperSumErr == 0) printf("\n운전설정값:정상"); 
    else 
    {
      sBuzzOnTime = SEC_1<<4;
      sBootDelay = SEC_1<<4;
      DC24power(ON);
      BUZZERonoff(ON);
      printf("\n");
      printf("\n운전설정이 손상되었습니다.");
      printf("\n운전설정을 점검하십시요.");
    }
    DelayStep = 0;
    sExecStep++;
    return;
    
  case 5:     
    if (ANY_KEY) sExecStep++; 
    else if (OperSumErr == 0) step_delay(sBootDelay);
    return;
    
  case 6:
    sBootDelay = SEC_1;
    if (OperSumErr != 0) screen_clear();
    if (MeterSumErr == 0) printf("\n전력설정값:정상"); 
    else 
    {
      sBuzzOnTime = SEC_1<<4;
      sBootDelay = SEC_1<<4;
      DC24power(ON);
      BUZZERonoff(ON);
      printf("\n전력설정이 손상되었습니다.");
      printf("\n파워메타설정을 점검하십시요.");
    }
    DelayStep = 0;
    sExecStep++;
    return;
    
  case 7:     
    if (ANY_KEY) sExecStep++; 
    else if (MeterSumErr == 0) step_delay(sBootDelay);
    return;
    
  case 8:
    display_mode(2);
    print_guide_message1();
    DelayStep = 0;
    sExecStep++;
    return;
    
  case 9: wait_next_message(); return;
    
  case 10:
    screen_clear();
    print_guide_message2();
    DelayStep = 0;
    sExecStep++;
    return;
    
  case 11: 
    sExecStep++;
    //step_delay(SEC_1*3);  
    return;

  case 12:
    if (ProfiFind == TRUE) printf("\nProfi Dev. Find");
    sExecStep++;
    return;   

  case 13:
    BootGuideEnd = 1;
    BootError = 0;
    OperSumErr = 0;
    MeterSumErr = 0;
    sExecStep++;
    return;

  case 14: 
    if (ENTER_KEY) execmode_change(MAIN_MENU);
    else step_delay(SEC_1<<2);   
    return;

  case 15:
    execmode_change(RUN_STATUS);
    return;
    
  default:
    sExecStep = 0; 
    return;
  }
}
