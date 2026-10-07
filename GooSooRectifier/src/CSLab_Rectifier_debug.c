
// Include Standard LIB  files
#include "project.h"
 
#define	SYSTEM_MAIN	  10
#define SYS_TEST_EXE      20
#define SYS_TEST_NOFUNC   110
#define SYS_TEST_ADC      120
#define SYS_TEST_DAC      130
#define SYS_TEST_EIN      140
#define SYS_TEST_EIN1     145
#define SYS_TEST_EOUT     150
#define SYS_TEST_RTC      160
#define SYS_METER_ADJ     170
#define SYS_TEST_PID      180
#define SYS_VCC_MONIT     190
#define SYS_TEST_FND      200
#define SYS_TEST_WHEEL    210  
#define SYS_TEST_BOTTON   220
#define SYS_MAIN_MENU     230
#define SYS_NOFUNC        240
#define SYS_TEST_END      250
#define SYS_TEST_ADE      260 
#define SYS_REACTION_SET  270
#define SYS_PROFI_SET     280
#define SYS_RS485_SET     290
#define SYS_INFO_VIEW     300
#define SYS_CANT_USE      310
#define SYS_SAVE_END      320
#define	WDT_TEST	  330
#define WDT_STATUS	  340
#define	WDT_READ	  350
#define SYS_POLE_TIME     360

#define	WDV		0x0FFF		// 12bit WDT counter
#define	WDFIEN		1<<12		// WDT int enable
#define	WDRSTEN		1<<13		// WDT reset enable
#define	WDRPROC		1<<14		// 1:WDT All reset 0:processor reset
#define	WDDIS		1<<15		// 0: WDT enable 1: disable
#define	WDDBGHLT	1<<28		// When debug state, 0: WDT run 1: stop  
#define	WDIDLEHLT	1<<29		// When idle mode, 0: WDT run 1: stop  

void menu_display_test(char no)
{  
  if (no == 0)       printf("<<System Test Menu>>");
  else if (no == 1)  printf("\nAnalog INPUT Test ");
  else if (no == 2)  printf("\nAnalog OUTPUT Test");
  else if (no == 3)  printf("\nExt ERROR IN Test ");
  else if (no == 4)  printf("\nExt ALRAM IN Test ");
  else if (no == 5)  printf("\nExt. OUTPUT Test  ");
  else if (no == 6)  printf("\nPower Meter Adjust");
  else if (no == 7)  printf("\nFeedBack Speed Set");
  else if (no == 8)  printf("\nProfiDevice Config");
#ifdef MONO_POLE
//  else if (no == 8)  printf("\nPID Status View   ");
  else if (no == 9) printf("\nSystem Vin,DAC Monit");
  else if (no == 10) printf("\nFND Display Test  ");
  else if (no == 11) printf("\nPower Meter Test  ");
  else if (no == 12) printf("\nRotary Wheel Test ");
  else if (no == 13) printf("\nWatchDog Reset Test");
  else if (no == 14) printf("\nSystem Infomation ");
  else if (no == 15) printf("\nReturn To Main    ");
#else
//  else if (no == 8)  printf("\nPole Turnover Time"); 
  else if (no == 9)  printf("\nPID Status View   ");
  else if (no == 10) printf("\nSystem Vin,DAC Monit");
  else if (no == 11) printf("\nFND Display Test  ");
  else if (no == 12) printf("\nPower Meter Test  ");
  else if (no == 13) printf("\nRotary Wheel Test ");
  else if (no == 14) printf("\nWatchDog Reset Test");
  else if (no == 15) printf("\nSystem Infomation ");
  else if (no == 16) printf("\nReturn To Main    ");
#endif  
  //else if (no == 8) printf("\nRS485 Com Port Set");
  //else if (no == 9) printf("\nKey/Button Test   ");
  //else if (no == 9) printf("\n2:Flash Memory Test");
}
/********************************/
/*     system_test_function     */
/********************************/
void system_test_function(void)
{  
  short lp;
  char result;

  switch (sExecStep)
 {   
  case 0:
    display_mode(0); 
    CursorUse = 1;
#ifdef MONO_POLE
   menu_range_set(0, 7, 16);
#else
   menu_range_set(0, 7, 17);
#endif    
    //MenuStart = 0;
    //MenuEnd = 7;
    //MenuSize = 17;
    sExecStep = SYSTEM_MAIN;
    return;
    
  case 1:
   DelayStep = 0;
   screen_clear();
   printf("<System Test>");
   
#ifdef DEVELOPE_MODE
   sExecStep = SYSTEM_MAIN;
#else
   printf("\n>Input Secret Code");
   printf("\n>******");
   sExecStep++;
#endif
   return;
   
  case 2:
   keyin_start(6);
   sCurPos -= 6;
   sExecStep++;
   return;
   
  case 3:
   if (DEC_KEY)
   {
     if(key_input_bcd() == 0) sExecStep++;
     printf("*");
   }
   else if (ENTER_KEY) execmode_change(MAIN_MENU);
   else if (MENU_KEY) execmode_change(MAIN_MENU);
   else  if (step_delay(SEC_1<<3)) execmode_change(MAIN_MENU); 
   return;
   
  case 4:
   if (KeyValue == iSecretCode) sExecStep = SYSTEM_MAIN;
   else sExecStep++;
   return;
   
  case 5:
   printf("\n>Invalid code..");
   printf("\n>Please Retry..");
   sExecStep++;
   return;
 
  case 6:
   if (step_delay(SEC_1<<1)) sExecStep = 1; 
   return;
    
  case SYSTEM_MAIN:
    display_mode(0);
    //screen_clear();
    LineBlink = 1;
    DelayStep = 0;
    debug_monit(MONOUT);
    for (lp = MenuStart; lp <= MenuEnd; lp++) menu_display_test(lp);
    cursor_move_home();
    sExecStep++;
    return;
  
  case SYSTEM_MAIN+1:
    if (MENU_UP) popup_menu_update(SYSTEM_TEST,UP); 
    else if (MENU_DN) popup_menu_update(SYSTEM_TEST,DOWN); 
    else if (ENTER_KEY) sExecStep = SYS_TEST_EXE; 
    printf("\r");
    return;

  case SYSTEM_MAIN+2:
    sExecStep--;
    return;
    
  case SYS_TEST_EXE:
    DelayStep = 0;
    LineBlink = 0;
    MenuNo = MenuStart + find_cursor_vpos();
    if (MenuNo == 0) sExecStep = SYS_MAIN_MENU;
    else if (MenuNo == 1) sExecStep = SYS_TEST_ADC;
    else if (MenuNo == 2) sExecStep = SYS_TEST_DAC;
    else if (MenuNo == 3) sExecStep = SYS_TEST_EIN;
    else if (MenuNo == 4) sExecStep = SYS_TEST_EIN1;
    else if (MenuNo == 5) sExecStep = SYS_TEST_EOUT;
    else if (MenuNo == 6) sExecStep = SYS_METER_ADJ;
    else if (MenuNo == 7) sExecStep = SYS_REACTION_SET;
    else if (MenuNo == 8) sExecStep = SYS_PROFI_SET;
#ifdef MONO_POLE
  //  else if (MenuNo == 8) sExecStep = SYS_TEST_PID;
    else if (MenuNo == 9) sExecStep = SYS_VCC_MONIT;    
    else if (MenuNo == 10) sExecStep = SYS_TEST_FND; 
    else if (MenuNo == 11) sExecStep = SYS_TEST_ADE;
    else if (MenuNo == 12) sExecStep = SYS_TEST_WHEEL;
    else if (MenuNo == 13) sExecStep = WDT_TEST;
    else if (MenuNo == 14) sExecStep = SYS_INFO_VIEW;
    else if (MenuNo == 15) sExecStep = SYS_MAIN_MENU;
#else
  //  else if (MenuNo == 8) sExecStep = SYS_POLE_TIME;
    else if (MenuNo == 9) sExecStep = SYS_TEST_PID;
    else if (MenuNo == 10) sExecStep = SYS_VCC_MONIT;    
    else if (MenuNo == 11) sExecStep = SYS_TEST_FND; 
    else if (MenuNo == 12) sExecStep = SYS_TEST_ADE;
    else if (MenuNo == 13) sExecStep = SYS_TEST_WHEEL;
    else if (MenuNo == 14) sExecStep = WDT_TEST;
    else if (MenuNo == 15) sExecStep = SYS_INFO_VIEW;
    else if (MenuNo == 16) sExecStep = SYS_MAIN_MENU;
#endif    
    else sExecStep = SYS_NOFUNC; 
    return;
    //else if (MenuNo == 8) sExecStep = SYS_RS485_SET;
    
  case SYS_TEST_EXE+1:
    sExecStep = SYSTEM_MAIN;
    return;

  case SYS_TEST_NOFUNC:
    sExecStep = SYSTEM_MAIN; 
    return;

//*************************************************
//    Pole Turnover Time Set
//    극성 전환시간 설정
//*************************************************
  case SYS_POLE_TIME:
    screen_clear();
    sExecStep++;
    return;

  case SYS_POLE_TIME+1:
    printf("<Pole Turnover Time>\n");
    printf("\nRange:%1d.%1d - ",MIN_POLE_TURN_TIME/10, MIN_POLE_TURN_TIME%10);
    printf("%1d.%1d[sec]",MAX_POLE_TURN_TIME/10, MAX_POLE_TURN_TIME%10);
    printf("\nOLD:%1d.%1d[sec]",iPoleTurnTime/10, iPoleTurnTime%10);
    iTempSet = iPoleTurnTime;    
    printf("\nNEW:%1d.%1d[sec]",iPoleTurnTime/10, iPoleTurnTime%10);
    sExecStep++;
    return;    

  case SYS_POLE_TIME+2:
    if (wheel_input(MIN_POLE_TURN_TIME, MAX_POLE_TURN_TIME)) 
    {
      if (iPoleTurnTime == iTempSet) sExecStep = 1; 
      else 
      {
        iPoleTurnTime = iTempSet;
        sExecStep = SYS_SAVE_END;
      }
    }
    return; 
    
  case SYS_POLE_TIME+3:
    printf("\rNEW:%1d.%1d[sec]",iTempSet/10, iTempSet%10);
    sExecStep--;
    return;   
    
//*************************************************
//    Reaction Rate Set
//    피드백 반응율 설정
//*************************************************
  case SYS_REACTION_SET:
    if (!SystemRun) sExecStep++; else sExecStep = SYS_CANT_USE;
    return;

  case SYS_REACTION_SET+1:
    //screen_clear();
    display_mode(2);
    printf("<반응감도설정>");
    printf("\n최대:%2d%%", MAX_REACT_RATE);
    printf("\n현재:%2d%%", iReactRate);
    iTempSet = iReactRate;    
    printf("\n변경:%2d%%", iReactRate);
    sExecStep++;
    return;    

  case SYS_REACTION_SET+2:
    if (wheel_input(MIN_REACT_RATE, MAX_REACT_RATE)) 
    {
      if (iSoftTime == iTempSet) sExecStep = 1; 
      else 
      {
        iReactRate = iTempSet;
        sExecStep = SYS_SAVE_END;
      }
    }
    return; 
    
  case SYS_REACTION_SET+3:
    printf("\r변경:%2d%%", iTempSet);
    sExecStep--;
    return;   

//**************************************
// System Infomation Display         
// Version, Release, Update date
//**************************************  
  case SYS_INFO_VIEW:
    screen_clear();
    sExecStep++;
    return;
    
  case SYS_INFO_VIEW+1:
    printf("<System Infomation>"); 
    printf("\nVersion: %01d.%02d", VERSION, RELEASE);
    printf("\nUpdate :%4d/%2d/%2d", UPDATE_YEAR,UPDATE_MONTH,UPDATE_DATE); 
    printf("\nKooSoo HeavyElectric");
    printf("\nDevelopment:C S Lab");
    printf("\nProgramming:Lee Y K");
    printf("\nTEL  :032-670-8014");
    sExecStep++;
    return;

  case SYS_INFO_VIEW+2:
    if (ENTER_KEY) sExecStep = SYS_TEST_END;
    else if (step_delay(SEC_1*2)) sExecStep = SYS_TEST_END;;
    return;
   
//**************************************
// System Main Board DC Source,         
// DAC[0,1] OutPut Monit 
//**************************************  
  case SYS_VCC_MONIT:
    screen_clear();
    sExecStep++;
    return;
    
  case SYS_VCC_MONIT+1:
    printf("<Vin/DAC Out Monit>"); 
    sExecStep++;
    return;

  case SYS_VCC_MONIT+2:
    goto_cursor(0,1);
    printf("\nDAC V Out :%2d.%02d[V]", iDacOutVolt/100, iDacOutVolt%100);
    printf("\nDAC V Mon :%2d.%02d[V]", iDacMonVolt/100, iDacMonVolt%100);
    printf("\nDAC A Out :%2d.%02d[V]", iDacOutAmp/100, iDacOutAmp%100);
    printf("\nDAC A Mon :%2d.%02d[V]", iDacMonAmp/100, iDacMonAmp%100);
    //printf("\nDAC VOLT:%4d[V]", iSamAdcRead[4]);
    //printf("\nDAC AMP :%4d[V]", iSamAdcRead[5]);
    //printf("\nSystem Vin:%4d[V]", iSamAdcRead[7]);
    //printf("\nMonit NO  :%4d", iSamAdcRead[3]&0x3FF);
    DelayStep = 0;
    sExecStep++;
    return;
    
  case SYS_VCC_MONIT+3:
    if (dacout_verify() == 0) printf("\nDAC OUT: GOOD");
    else printf("\nDAC OUT: FAULT");
    dacout_verify();
    //printf("\nSystem Vin:%2d.%02d[V]", iVccVolt/100, iVccVolt%100);
    sExecStep++;
    return;

 case SYS_VCC_MONIT+4:
    if (ANY_KEY) sExecStep = SYSTEM_MAIN;
    else step_delay(SEC_1/4);
    return;
    
  case SYS_VCC_MONIT+5:
    sExecStep = SYS_VCC_MONIT+2;
    return;

 case SYS_RS485_SET:
    execmode_change(REMOTE_SET);
    return;
 
 case SYS_PROFI_SET:
    execmode_change(PROFI_SET);
    return;
 
 case SYS_METER_ADJ:
    execmode_change(METER_ADJUST);
    return;
 
  case SYS_TEST_ADE:
    execmode_change(ADE_TEST);
    return;
 
  case SYS_TEST_DAC:
    if (!SystemRun) execmode_change(DAC_TEST);
    else sExecStep = SYS_CANT_USE;
    return;
    
  case SYS_TEST_ADC:
    execmode_change(ADC_TEST);
    return;
    
  case SYS_TEST_PID:
    ViewPage = 2;
    execmode_change(RUN_STATUS);
    return;

 case SYS_TEST_EIN:  
      
   extin_display(0);
    if (ENTER_KEY) sExecStep = SYSTEM_MAIN;
  // if (MENU_UP) sExecStep = SYSTEM_MAIN;
  // else if (MENU_DN)  sExecStep = SYSTEM_MAIN;
  // else if (ENTER_KEY) sExecStep = SYSTEM_MAIN;
    return;
 case SYS_TEST_EIN1:  
      
   extin_display1(0);
   if (ENTER_KEY) sExecStep = SYSTEM_MAIN;
  // if (MENU_UP) sExecStep = SYSTEM_MAIN;
  // else if (MENU_DN)  sExecStep = SYSTEM_MAIN;
  // else if (ENTER_KEY) sExecStep = SYSTEM_MAIN;
    return;   
    
  case SYS_TEST_EOUT:
#ifdef DEVELOPE_MODE
    execmode_change(EXTOUT_TEST);
#else
    if (!SystemRun) execmode_change(EXTOUT_TEST);
    else sExecStep = SYS_CANT_USE;
#endif

    return;

  case SYS_TEST_RTC:
    execmode_change(RTC_TEST);
    return;

  case SYS_TEST_FND:
    screen_clear();
    DelayStep = 0;
    printf("\nFND Display Test");
    printf("\nPress Any Key");
    FndTestStep = 1;
    sExecStep = SYS_TEST_END;
    return;

  case SYS_TEST_WHEEL:
    screen_clear();
    DelayStep = 0;
    printf("\nRotary Wheel Test ");
    printf("\nPress Any Key");
    sExecStep++; 
    return;
    
   case SYS_TEST_WHEEL+1:
    rotary_key_test();
    if (ENTER_KEY) sExecStep = SYSTEM_MAIN;
    return;

  case SYS_TEST_BOTTON:
    screen_clear();
    DelayStep = 0;
    printf("\nKey/Button Test   ");
    printf("\nPress Any Key");
    sExecStep = SYS_TEST_END;
    return;
    
  case SYS_MAIN_MENU:
    execmode_change(MAIN_MENU);
    return;

  case SYS_NOFUNC:
    screen_clear();
    printf(">Under Contraction..");
    printf("\n>Menu NO:%2d", MenuNo);
    sExecStep = SYS_TEST_END;
    return;

//*************************************************
//    운전중 조작 금지 메시지
//*************************************************
  case SYS_CANT_USE:
    display_mode(2);
    printf("운전중에는 사용할 수 없습니다.");
    DelayStep = 0;
    sExecStep = SYS_TEST_END;
    return;    
//*************************************************
//   변경된 데이터를 저장하고 
//   3초간 지연 또는 키입력으로 메인메뉴로 복귀
//*************************************************
  case SYS_SAVE_END:
    result = backup_data_save();
    if (result == true) printf("\nData Save OK");
    else 
    {
      printf("\nBackup Memory Error!!");
      printf("\nData Save Fail..");
    }
    DelayStep = 0;
    sExecStep = SYS_TEST_END;
    return;

//*************************************************
// 주메뉴로 복귀
//*************************************************
  case SYS_TEST_END:
    if (ENTER_KEY) sExecStep = SYSTEM_MAIN;
    else if (step_delay(SEC_1*2)) sExecStep = SYSTEM_MAIN;
    return;
    
  case WDT_TEST:
   all_flash_lock();
   // 10초간 무한루프
   //for (lp = 0; lp < 10000; lp++) delay_us(1000);
   sExecStep = SYSTEM_MAIN;
   return;    

 default:
    printf("\n?Error:MAIN MENU");
    sExecStep = 0;
    return; 
  }
}


//*----------------------------------------------------------------------------
//* dbg_status_log : 운전 상태를 COM1 으로 주기 출력 (2026-10 추가)
//*   main 루프(2ms)에서 매번 호출, DBG_LOG_PERIOD 스캔마다 CSV 한 줄 출력
//*   38400bps = 약 3.8 글자/ms. 한 줄(약 100자) ≒ 26ms → 0.5초 주기면 여유 충분
//*   열 순서: 전류 (원시값, 기준, 측정, 지령) / 전압 (원시값, 기준, 측정, 지령)
//*     amp_out  : CV 모드에서는 갱신 안 됨 (전류 DAC 는 최대로 고정)
//*     volt_out : CC 모드에서는 갱신 안 됨 (전압 DAC 는 최대로 고정)
//*----------------------------------------------------------------------------
#define DBG_LOG_PERIOD   250        // 250 x 2ms = 0.5초마다 한 줄 (0 이면 출력 안 함)

extern unsigned short DbgDropCount;

void dbg_status_log(void)
{
  static unsigned short sDbgScan;
  static unsigned int   uiDbgTick;      // 2ms 단위 경과 시간
  static char sDbgHeader = 0;

  uiDbgTick++;
  if (DBG_LOG_PERIOD == 0) return;
  if (++sDbgScan < DBG_LOG_PERIOD) return;
  sDbgScan = 0;

  if (!sDbgHeader)
  {
    if (dbg_printf("\r\nt_ms,step,mode,stable,"
                   "adc_amp,amp_ref[A],amp_in[A],amp_out[A],"
                   "adc_volt,volt_ref[V],volt_in[V],volt_out[V],"
                   "adc_err,alarm,drop\r\n"))
      sDbgHeader = 1;
    return;
  }

  dbg_printf("%u,%d,%d,%d,%d,%d,%d,%d,%d,%.2f,%.2f,%.2f,%d,%d,%u\r\n",
             uiDbgTick * 2, (int)sConStep, (int)OperMode, (int)PidStable,
             iAdcRead[1], (int)fRefAmp, (int)fAmpInput, (int)fOutAmp,
             iAdcRead[0], fRefVolt, fVoltInput, fOutVolt,
             (int)AdcError, (int)TotalAlarm, (unsigned int)DbgDropCount);
}
