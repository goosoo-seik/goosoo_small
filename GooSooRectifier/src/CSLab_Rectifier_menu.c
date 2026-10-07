

// Include Standard LIB  files
#include "project.h"

/************************************/
/*      Main Munu Display           */
/************************************/
void menu_display_main(char no)
{
#ifdef MONO_POLE 
  // 정운전 정류기용 메뉴
  if (no == 0)       printf("<<메인메뉴>>");
  //else if (no == 1)  printf("\n가동시간 설정");
  else if (no == 1) 
  {
    if (OperMode == CC_MODE) 
                     printf("\n제어모드[C/C]");
                else printf("\n제어모드[C/V]");
  }
  else if (no == 2)  printf("\n현재시간 설정");
  else if (no == 3)  printf("\n운전상태 표시");
  else if (no == 4)  printf("\n운전전류(FW)");
  else if (no == 5)  printf("\n운전전압(FW)");
  else if (no == 6)  printf("\n정격최대전류 ");
  else if (no == 7)  printf("\n정격최대전압 ");
  else if (no == 8)  printf("\n시동시간:%3d.%1dS", iSoftTime/10, iSoftTime%10);
  else if (no == 9)  printf("\n과전류 설정");
  else if (no == 10) printf("\n과전압 설정");
  else if (no == 11) printf("\nAC OCR 설정");
  else if (no == 12) printf("\nAC저전압 설정");
  //else if (no == 15)  printf("\n반응감도:%2d%%", iReactRate); 
  else if (no == 13)  
  {
    if (ViewPage == 0) printf("\n전력상황 보기");
    else printf("\n운전상황 보기");
  }
  else if (no == 14)  printf("\n시스템 진단");

  OperPole = PLUS;
#else 
  // 정/역 가능 정류기용 메뉴
  if (no == 0)       printf("<<메인메뉴>>");
  //else if (no == 1)  printf("\n가동시간 설정");
  else if (no == 1) 
  {
    if (OperMode == CC_MODE) 
                     printf("\n제어모드[C/C]");
                else printf("\n제어모드[C/V]");
  }
  else if (no == 2) 
  {
    if (OperPole == PLUS) 
                     printf("\n출력극성[P:정]");
                else printf("\n출력극성[N:역]");
  }
  else if (no == 3)   printf("\n현재시간 설정");
  else if (no == 4)   printf("\n운전상태 표시");
  else if (no == 5)   printf("\n운전전류(FW)");
  else if (no == 6)   printf("\n운전전압(FW)");
  else if (no == 7)   printf("\n운전전류(REV)");
  else if (no == 8)   printf("\n운전전압(REV)");
  else if (no == 9)  printf("\n정격최대전류 ");
  else if (no == 10)  printf("\n정격최대전압 ");
  else if (no == 11)  printf("\n시동시간:%3d.%1dS", iSoftTime/10, iSoftTime%10);
  else if (no == 12)  printf("\n과전류 설정");
  else if (no == 13)  printf("\n과전압 설정");
  else if (no == 14)  printf("\nAC OCR 설정");
  else if (no == 15) printf("\nAC저전압 설정");
  //else if (no == 15)  printf("\n반응감도:%2d%%", iReactRate); 
  else if (no == 16)  
  {
    if (ViewPage == 0) printf("\n전력상황 보기");
    else printf("\n운전상황 보기");
  }
  else if (no == 17)  printf("\n시스템 진단");
#endif
}  

void menu_range_set(char start, char end, char size)
{
  MenuStart = start;
  MenuEnd = end;
  MenuSize = size;
}
  
float fTempSet;
char TempSet;
char Answer;
int iTempSet;
short sMainTime;
#define MAIN_EXECUTE        10
#define MAIN_SYSTEM_TEST    20
#define MAIN_RTC_SET        30
#define MAIN_RUN_STATUS     40
#define MAIN_RUNTIME_SET    100
#define MAIN_OPMODE_CHANGE  200
#define MAIN_POLE_CHANGE    300
#define MAIN_MAXAMP_SET     400
#define MAIN_MAXVOLT_SET    500  
#define MAIN_OPAMP_SET      600
#define MAIN_OPVOLT_SET     700
#define MAIN_REVAMP_SET     800
#define MAIN_REVVOLT_SET    900
#define MAIN_RISE_TIME_SET  1000
#define MAIN_OVERAMP_SET    1100
#define MAIN_OVERVOLT_SET   1200
#define MAIN_AC_OCR_SET     1300
#define MAIN_AC_LOW_SET     1400
#define MAIN_REACTION_SET   1500
#define MAIN_MONIT_CHANGE   1600
#define MAIN_NOFUNC         2000
#define MAIN_SAVE_END       2100 
#define MAIN_END            2200
#define MAIN_CANT_USE       2300
/**************************/
/*  MAIN MENU Functions   */
/**************************/
void MAIN_MENU_function(void)
{
  unsigned char lp, result;
  switch (sExecStep)
  {
  case 0:
    display_mode(2);
    LineBlink = 1;
    CursorUse = 1;
#ifdef MONO_POLE
   menu_range_set(0, 3, 15);
#else
   menu_range_set(0, 3, 18);
#endif
    sExecStep++;
    return;
    
  case 1:
    screen_clear();
    //display_mode(2);
    sMainTime = 0;
    LineBlink = 1;
    DelayStep = 0;
    debug_monit(MONOUT);
    for (lp = MenuStart; lp <= MenuEnd; lp++) menu_display_main(lp);
    goto_cursor(0,0);
    sExecStep++;
    return;
    
  case 2:
    if (MENU_UP) popup_menu_update(MAIN_MENU,UP); 
    else if (MENU_DN) popup_menu_update(MAIN_MENU,DOWN);
    else if (ENTER_KEY) sExecStep = MAIN_EXECUTE; 
    //else if (RUN_KEY) system_start();
    //else if (STOP_KEY) system_stop();
    //else if (MENU_KEY) sExecStep = 0;
    //else if (CLEAR_KEY) sExecStep = 0;
    else if (++sMainTime > SEC_1*30) execmode_change(RUN_STATUS);
    if (ANY_KEY) sMainTime = 0;
    return;

  case 3:
    sExecStep--;
    return;
   
  case MAIN_EXECUTE:
    LineBlink = 0;
    MenuNo = MenuStart + find_cursor_vpos();
#ifdef MONO_POLE
    if (MenuNo == 0) sExecStep = MAIN_RUN_STATUS;
    //else if (MenuNo == 1) sExecStep = MAIN_RUNTIME_SET;
    else if (MenuNo == 1) sExecStep = MAIN_OPMODE_CHANGE;
    else if (MenuNo == 2) sExecStep = MAIN_RTC_SET;
    else if (MenuNo == 3) sExecStep = MAIN_RUN_STATUS;
    else if (MenuNo == 4) sExecStep = MAIN_OPAMP_SET;
    else if (MenuNo == 5) sExecStep = MAIN_OPVOLT_SET;
    else if (MenuNo == 6) sExecStep = MAIN_MAXAMP_SET;
    else if (MenuNo == 7) sExecStep = MAIN_MAXVOLT_SET;
    else if (MenuNo == 8) sExecStep = MAIN_RISE_TIME_SET;
    else if (MenuNo == 9) sExecStep = MAIN_OVERAMP_SET;
    else if (MenuNo == 10) sExecStep = MAIN_OVERVOLT_SET;
    else if (MenuNo == 11) sExecStep = MAIN_AC_OCR_SET;
    else if (MenuNo == 12) sExecStep = MAIN_AC_LOW_SET;
    //else if (MenuNo == 15) sExecStep = MAIN_REACTION_SET;
    else if (MenuNo == 13) sExecStep = MAIN_MONIT_CHANGE;
    else if (MenuNo == 14) sExecStep = MAIN_SYSTEM_TEST;
    else sExecStep = MAIN_NOFUNC; 
#else
    if (MenuNo == 0) sExecStep = MAIN_RUN_STATUS;
    //else if (MenuNo == 1) sExecStep = MAIN_RUNTIME_SET;
    else if (MenuNo == 1) sExecStep = MAIN_OPMODE_CHANGE;
    else if (MenuNo == 2) sExecStep = MAIN_POLE_CHANGE;
    else if (MenuNo == 3) sExecStep = MAIN_RTC_SET;
    else if (MenuNo == 4) sExecStep = MAIN_RUN_STATUS;
    else if (MenuNo == 5) sExecStep = MAIN_OPAMP_SET;
    else if (MenuNo == 6) sExecStep = MAIN_OPVOLT_SET;
    else if (MenuNo == 7) sExecStep = MAIN_REVAMP_SET;
    else if (MenuNo == 8) sExecStep = MAIN_REVVOLT_SET;
    else if (MenuNo == 9) sExecStep = MAIN_MAXAMP_SET;
    else if (MenuNo == 10) sExecStep = MAIN_MAXVOLT_SET;
    else if (MenuNo == 11) sExecStep = MAIN_RISE_TIME_SET;
    else if (MenuNo == 12) sExecStep = MAIN_OVERAMP_SET;
    else if (MenuNo == 13) sExecStep = MAIN_OVERVOLT_SET;
    else if (MenuNo == 14) sExecStep = MAIN_AC_OCR_SET;
    else if (MenuNo == 15) sExecStep = MAIN_AC_LOW_SET;
    //else if (MenuNo == 15) sExecStep = MAIN_REACTION_SET;
    else if (MenuNo == 16) sExecStep = MAIN_MONIT_CHANGE;
    else if (MenuNo == 17) sExecStep = MAIN_SYSTEM_TEST;
    else sExecStep = MAIN_NOFUNC; 
#endif
    return;
  
  case MAIN_EXECUTE+1:
    sExecStep = MAIN_END;
    return;

//*************************************************
//    운전상태 표시화면 전환
//*************************************************
  case MAIN_MONIT_CHANGE:
    if (ViewPage == 0) ViewPage = 1;
    else ViewPage = 0;
    execmode_change(RUN_STATUS);
    return;

//*************************************************
//    시스템 진단모드로 이동
//*************************************************
  case MAIN_SYSTEM_TEST:
#ifdef DEVELOPE_MODE
    sSecretPassTime = 3600;
    execmode_change(SYSTEM_TEST);
#else    
    if (sSecretPassTime != 0) sExecStep =  MAIN_SYSTEM_TEST+4;
    else 
    {
      screen_clear();
      sExecStep++;
    }
#endif
    return;    
    
  case MAIN_SYSTEM_TEST+1:
    printf("비밀번호 입력");
    iTempSet = 9492;
    printf("\nCODE:%04d", iTempSet);
    sExecStep++;
    return;
    
  case MAIN_SYSTEM_TEST+2:
    if (wheel_input(0, 9999)) 
    {
      if (iTempSet == SECRET_CODE) sExecStep = MAIN_SYSTEM_TEST+4; 
      else
      {
        printf("\n비밀번호 오류..");
        sExecStep = MAIN_END;
      }        
    }
    return; 
    
  case MAIN_SYSTEM_TEST+3:
    printf("\rCODE:%04d", iTempSet);
    sExecStep--;
    return;   

  case MAIN_SYSTEM_TEST+4:
    sSecretPassTime = 3600;
    execmode_change(SYSTEM_TEST);
    return;
    
//*************************************************
//    현재시각 변경모드로 이동
//*************************************************
  case MAIN_RTC_SET:
    execmode_change(RTC_SET);
    return;
    
//*************************************************
//    운전상태보기로 전환
//*************************************************
  case MAIN_RUN_STATUS:
    execmode_change(RUN_STATUS);
    return;        
  
//*************************************************
//    Reaction Rate Set
//    피드백 반응율 설정
//*************************************************
/* 
  case MAIN_REACTION_SET:
    if (!SystemRun) sExecStep++; else sExecStep = MAIN_CANT_USE;
    return;

  case MAIN_REACTION_SET+1:
    screen_clear();
    printf("<반응감도설정>");
    printf("\n최대:%2d%%", MAX_REACT_RATE);
    printf("\n현재:%2d%%", iReactRate);
    iTempSet = iReactRate;    
    printf("\n변경:%2d%%", iReactRate);
    sExecStep++;
    return;    

  case MAIN_REACTION_SET+2:
     if (COUNT_UP) 
    {
      //if (JogSpeed < 4) JogSpeed = 1; 
      if (iTempSet < MAX_REACT_RATE) iTempSet++;
      if (iTempSet > MAX_REACT_RATE) iTempSet = 99;
      sExecStep++;
    }
    else if (COUNT_DN) 
    {
      //if (JogSpeed < 4) JogSpeed = 1; 
      if (iTempSet > MIN_REACT_RATE) iTempSet--;
      if (iTempSet < MIN_REACT_RATE) iTempSet = MIN_REACT_RATE;
      sExecStep++;
    }
    else if (ENTER_KEY) 
    {
      if (iReactRate == iTempSet) sExecStep = 1;      
      else 
      {
        iReactRate = iTempSet;
        printf("\n변경:%2d%%", iReactRate);
        sExecStep = MAIN_SAVE_END;
      }
    }
    return; 
    
  case MAIN_REACTION_SET+3:
    printf("\r변경:%2d%%", iTempSet);
    sExecStep--;
    return;   
*/    
//*************************************************
//    Soft Start Time Set
//    기동 시간 설정
//*************************************************
  case MAIN_RISE_TIME_SET:
    if (!SystemRun) sExecStep++; else sExecStep = MAIN_CANT_USE;
    return;

  case MAIN_RISE_TIME_SET+1:
    screen_clear();
    printf("Soft Start Time");
    printf("\n현재:%3d.%01d[초]", iSoftTime/10, iSoftTime%10);
    iTempSet = iSoftTime;    
    printf("\n변경:%3d.%01d[초]", iTempSet/10, iTempSet%10);
    sExecStep++;
    return;    

  case MAIN_RISE_TIME_SET+2:
    if (wheel_input(1, 9999)) 
    {
      if (iSoftTime == iTempSet) sExecStep = 1; 
      else 
      {
        iSoftTime = iTempSet;
        printf("\r변경:%3d.%01d[초]", iSoftTime/10, iSoftTime%10);
        sExecStep = MAIN_SAVE_END;
      }
    }
    return; 
    
  case MAIN_RISE_TIME_SET+3:
    printf("\r변경:%3d.%01d[초]", iTempSet/10, iTempSet%10);
    sExecStep--;
    return;   
    
//*************************************************
//    운전전류 설정
//    운전전류값을 입력받고 플레시메모리에 저장
//*************************************************
  case MAIN_OPAMP_SET:
    if (!SystemRun) sExecStep = MAIN_OPAMP_SET+10; 
    else sExecStep++;
    return; 

  case MAIN_OPAMP_SET+1:
    screen_clear();
    Answer = 0;
    printf("<주의>");
    printf("지금운전중입니다");
    printf(" 설정값을 변경하시겠습니까?");
    printf("[NO] ");
    sExecStep++;
    return;
    
  case MAIN_OPAMP_SET+2:
    if ((MENU_UP)|(MENU_DN)) 
    {
      Answer ^= 1;
      sCurPos -=5; 
      if (Answer) printf("[YES]"); else printf("[NO] ");      
    }
    else if (ENTER_KEY) sExecStep++;
    return;

  case MAIN_OPAMP_SET+3:
    if (!Answer) sExecStep = 1;
    else sExecStep = MAIN_OPAMP_SET+10;
    return;       
 
  case MAIN_OPAMP_SET+10:
    screen_clear();
    printf("정운전전류설정");
    printf("\n현재:"); printf_volt(fOperAmp); printf("[A] ");
    fTempSet = fOperAmp; 
    printf("\n변경:"); printf_volt(fOperAmp); printf("[A] ");
    sExecStep++;
    return;    

  case MAIN_OPAMP_SET+11:
    if (COUNT_UP) 
    {
      float_jog_plus(fMaxOperAmp);      
      sExecStep++;
    }
    else if (COUNT_DN) 
    {
      float_jog_minus(0);
      sExecStep++;
    }
    else if (ENTER_KEY) 
    {
      if (fOperAmp == fTempSet) sExecStep = 1;
      else 
      {
        //PidStable = 0;  // 예비 Ref 값 다시 설정
        fOperAmp = fTempSet;
        printf("\r변경:"); 
        printf_volt(fOperAmp); 
        printf("[A] ");
        sExecStep = MAIN_SAVE_END;
      }
    }
    return; 
    
  case MAIN_OPAMP_SET+12:
    printf("\r변경:"); printf_volt(fTempSet); printf("[A] ");
    sExecStep--;
    return;
      
//*************************************************
//    운전전압 설정
//    운전전압값을 입력받고 플레시메모리에 저장
//*************************************************
  case MAIN_OPVOLT_SET:
    if (!SystemRun) sExecStep = MAIN_OPVOLT_SET+10; 
    else sExecStep++;
    return; 

  case MAIN_OPVOLT_SET+1:
    screen_clear();
    Answer = 0;
    printf("<주의>");
    printf("지금운전중입니다");
    printf(" 설정값을 변경하시겠습니까?");
    printf("[NO] ");
    sExecStep++;
    return;
    
  case MAIN_OPVOLT_SET+2:
    if ((MENU_UP)|(MENU_DN)) 
    {
      Answer ^= 1;
      sCurPos -=5;
      if (Answer) printf("[YES]"); else printf("[NO] ");       
    }
    else if (ENTER_KEY) sExecStep++;
    return;

  case MAIN_OPVOLT_SET+3:
    if (!Answer) sExecStep = 1;
    else sExecStep = MAIN_OPVOLT_SET+10;
    return;       

  case MAIN_OPVOLT_SET+10:
    screen_clear();
    printf("정운전전압설정");
    printf("\n현재:"); printf_volt(fOperVolt); printf("[V] ");
    fTempSet = fOperVolt; 
    printf("\n변경:"); printf_volt(fTempSet); printf("[V] ");
    sExecStep++;
    return;    

  case MAIN_OPVOLT_SET+11:
    if (COUNT_UP) 
    {
      float_jog_plus(fMaxOperVolt);      
      sExecStep++;
    }
    else if (COUNT_DN) 
    {
      float_jog_minus(0);
      sExecStep++;
    }
    else if (ENTER_KEY) 
    {
      if (fOperVolt == fTempSet) sExecStep = 1;
      else 
      {
        //PidStable = 0;  // 예비 Ref 값 다시 설정
        fOperVolt = fTempSet;
        printf("\r변경:"); 
        printf_volt(fOperVolt); 
        printf("[V] ");
        sExecStep = MAIN_SAVE_END;
      }
    }
    return; 
    
  case MAIN_OPVOLT_SET+12:
    printf("\r변경:"); printf_volt(fTempSet); printf("[V] ");
    sExecStep--;
    return;
       
//*************************************************
//    AC OCR(Over Current Relay) 설정
//    AC 고장전류값을 입력받고 플레시메모리에 저장
//*************************************************
  case MAIN_AC_OCR_SET:
    if (!SystemRun) sExecStep++; else sExecStep = MAIN_CANT_USE;
    return; 

  case MAIN_AC_OCR_SET+1:
    screen_clear();
    sExecStep++;
    return; 

  case MAIN_AC_OCR_SET+2:    
    printf("AC 과전류 설정");
    printf("\n0[A]:AC OCR OFF");
    printf("\n현재:%3d[A]", iAcOverAmp);
    iTempSet = iAcOverAmp;    
    printf("\n변경:%3d[A]", iTempSet);
    sExecStep++;
    return;    

  case MAIN_AC_OCR_SET+3:
    if (wheel_input(0, 999)) 
    {
      if (iAcOverAmp == iTempSet) sExecStep = 1; 
      else 
      {
        iAcOverAmp = iTempSet;
        printf("\r변경:%3d[A]", iAcOverAmp);
        sExecStep = MAIN_SAVE_END;
      }
    }
    return; 
    
  case MAIN_AC_OCR_SET+4:
    printf("\r변경:%3d[A]", iTempSet);
    sExecStep--;
    return;  

//*************************************************
//    AC 저전압(Low Voltage Relay) 설정
//    AC 고장저전압값을 입력받고 플레시메모리에 저장
//*************************************************
  case MAIN_AC_LOW_SET:
    if (!SystemRun) sExecStep++; else sExecStep = MAIN_CANT_USE;
    return; 

  case MAIN_AC_LOW_SET+1:
    screen_clear();
    sExecStep++;
    return; 

  case MAIN_AC_LOW_SET+2:    
    printf("AC 저전압 설정");
    printf("\n0[V]:AC LOW OFF");
    printf("\n현재:%4d[V]", iAcLowVolt);
    iTempSet = iAcLowVolt;    
    printf("\n변경:%4d[V]", iTempSet);
    sExecStep++;
    return;    

  case MAIN_AC_LOW_SET+3:
    if (wheel_input(0, 9999)) 
    {
      if (iAcLowVolt == iTempSet) sExecStep = 1; 
      else 
      {
        iAcLowVolt = iTempSet;
        printf("\r변경:%4d[V]", iAcLowVolt);
        sExecStep = MAIN_SAVE_END;
      }
    }
    return; 
    
  case MAIN_AC_LOW_SET+4:
    printf("\r변경:%4d[V]", iTempSet);
    sExecStep--;
    return;  
    
 //*************************************************
//    운전전류 설정(REV)
//    운전전류값을 입력받고 플레시메모리에 저장
//*************************************************
  case MAIN_REVAMP_SET:
    if (!SystemRun) sExecStep = MAIN_REVAMP_SET+10; 
    else sExecStep++;
    return; 

  case MAIN_REVAMP_SET+1:
    screen_clear();
    Answer = 0;
    printf("<주의>");
    printf("지금운전중입니다");
    printf(" 설정값을 변경하시겠습니까?");
    printf("[NO] ");
    sExecStep++;
    return;
    
  case MAIN_REVAMP_SET+2:
    if ((MENU_UP)|(MENU_DN)) 
    {
      Answer ^= 1;
      sCurPos -=5; 
      if (Answer) printf("[YES]"); else printf("[NO] ");      
    }
    else if (ENTER_KEY) sExecStep++;
    return;

  case MAIN_REVAMP_SET+3:
    if (!Answer) sExecStep = 1;
    else sExecStep = MAIN_REVAMP_SET+10;
    return;       
 
  case MAIN_REVAMP_SET+10:
    screen_clear();
    printf("역운전전류설정");
    printf("\n현재:"); printf_volt(fRevOperAmp); printf("[A] ");
    fTempSet = fRevOperAmp; 
    printf("\n변경:"); printf_volt(fRevOperAmp); printf("[A] ");
    sExecStep++;
    return;    

  case MAIN_REVAMP_SET+11:
    if (COUNT_UP) 
    {
      float_jog_plus(fMaxOperAmp);      
      sExecStep++;
    }
    else if (COUNT_DN) 
    {
      float_jog_minus(0);
      sExecStep++;
    }
    else if (ENTER_KEY) 
    {
      if (fRevOperAmp == fTempSet) sExecStep = 1;
      else 
      {
        //PidStable = 0;  // 예비 Ref 값 다시 설정
        fRevOperAmp = fTempSet;
        printf("\r변경:"); 
        printf_volt(fRevOperAmp); 
        printf("[A] ");
        sExecStep = MAIN_SAVE_END;
      }
    }
    return; 
    
  case MAIN_REVAMP_SET+12:
    printf("\r변경:"); printf_volt(fTempSet); printf("[A] ");
    sExecStep--;
    return;
    
//*************************************************
//    운전전압 설정(REV)
//    운전전압값을 입력받고 플레시메모리에 저장
//*************************************************
  case MAIN_REVVOLT_SET:
    if (!SystemRun) sExecStep = MAIN_REVVOLT_SET+10; 
    else sExecStep++;
    return; 

  case MAIN_REVVOLT_SET+1:
    screen_clear();
    Answer = 0;
    printf("<주의>");
    printf("지금운전중입니다");
    printf(" 설정값을 변경하시겠습니까?");
    printf("[NO] ");
    sExecStep++;
    return;
    
  case MAIN_REVVOLT_SET+2:
    if ((MENU_UP)|(MENU_DN)) 
    {
      Answer ^= 1;
      sCurPos -=5;
      if (Answer) printf("[YES]"); else printf("[NO] ");       
    }
    else if (ENTER_KEY) sExecStep++;
    return;

  case MAIN_REVVOLT_SET+3:
    if (!Answer) sExecStep = 1;
    else sExecStep = MAIN_REVVOLT_SET+10;
    return;       

  case MAIN_REVVOLT_SET+10:
    screen_clear();
    printf("역운전전압설정");
    printf("\n현재:"); printf_volt(fRevOperVolt); printf("[V] ");
    fTempSet = fRevOperVolt; 
    printf("\n변경:"); printf_volt(fTempSet); printf("[V] ");
    sExecStep++;
    return;    

  case MAIN_REVVOLT_SET+11:
     if (COUNT_UP) 
    {
      float_jog_plus(fMaxOperVolt);      
      sExecStep++;
    }
    else if (COUNT_DN) 
    {
      float_jog_minus(0);
      sExecStep++;
    }
    else if (ENTER_KEY) 
    {
      if (fRevOperVolt == fTempSet) sExecStep = 1; 
      else 
      {
        //PidStable = 0;  // 예비 Ref 값 다시 설정
        fRevOperVolt = fTempSet;
        printf("\r변경:"); 
        printf_volt(fRevOperVolt); 
        printf("[V] ");
        sExecStep = MAIN_SAVE_END; 
      }
    }
    return; 
    
  case MAIN_REVVOLT_SET+12:
    printf("\r변경:"); printf_volt(fTempSet); printf("[V] ");
    sExecStep--;
    return;
    
//*************************************************
//    정격최대출력전류 설정
//    전류제어출력신호 10.0 V 출력에 대응되는 
//    전류값을 입력받고 플레시메모리에 저장
//*************************************************
  case MAIN_MAXAMP_SET:
    if (!SystemRun) sExecStep++; else sExecStep = MAIN_CANT_USE;
    return;

  case MAIN_MAXAMP_SET+1:
    screen_clear();
    printf("정류기 정격최대출력전류설정");
    if (fMaxOperAmp < fOperAmp) fMaxOperAmp = fOperAmp;
    printf("\n현재:"); printf_volt(fMaxOperAmp); printf("[A] ");
    fTempSet = fMaxOperAmp; 
    printf("\n변경:"); printf_volt(fTempSet); printf("[A] ");
    sExecStep++;
    return;    

  case MAIN_MAXAMP_SET+2:
    if (COUNT_UP) 
    {
      float_jog_plus(70000);
      sExecStep++;
    }
    else if (COUNT_DN) 
    {
      float_jog_minus(fOperAmp);
      sExecStep++;
    }
    else if (ENTER_KEY) 
    {
      if (fMaxOperAmp == fTempSet) sExecStep = 1;
      else
      {
        //PidStable = 0;  // 예비 Ref 값 다시 설정
        fMaxOperAmp = fTempSet;
        iMaxOperAmp = fMaxOperAmp;
        printf("\r변경:"); 
        printf_volt(fMaxOperAmp); 
        printf("[A] ");
        //printf("-%6d", iMaxOperAmp);
        sExecStep = MAIN_SAVE_END;
      }
    }
    return; 
    
  case MAIN_MAXAMP_SET+3:
    printf("\r변경:"); 
    printf_volt(fTempSet); 
    printf("[A] ");
    sExecStep--;
    return;
   
//*************************************************
//    정격최대출력전압 설정
//    전압제어출력신호 10.0 V 출력에 대응되는 
//    전압값을 입력받고 플레시메모리에 저장
//*************************************************
  case MAIN_MAXVOLT_SET:
    if (!SystemRun) sExecStep++; else sExecStep = MAIN_CANT_USE;
    return;

  case MAIN_MAXVOLT_SET+1:
    screen_clear();
    printf("정류기 정격최대출력전압설정");
    if (fMaxOperVolt < fOperVolt) fMaxOperVolt = fOperVolt;
    printf("\n현재:"); printf_volt(fMaxOperVolt); printf("[V] ");
    fTempSet = fMaxOperVolt; 
    printf("\n변경:"); printf_volt(fTempSet); printf("[V] ");
    sExecStep++;
    return;    

  case MAIN_MAXVOLT_SET+2:
     if (COUNT_UP) 
    {
      float_jog_plus(999);
      sExecStep++;
    }
    else if (COUNT_DN) 
    {
      float_jog_minus(fOperVolt);
      sExecStep++;
    }
    else if (ENTER_KEY) 
    {
      if (fMaxOperVolt == fTempSet) sExecStep = 1;
      else
      {
        //PidStable = 0;  // 예비 Ref 값 다시 설정
        fMaxOperVolt = fTempSet;
        iMaxOperVolt = fMaxOperVolt * 100;
        printf("\r변경:"); 
        printf_volt(fMaxOperVolt); 
        printf("[V] ");
        sExecStep = MAIN_SAVE_END;
      }
    }
    return; 
    
  case MAIN_MAXVOLT_SET+3:
    printf("\r변경:"); 
    printf_volt(fTempSet); 
    printf("[V] ");
    sExecStep--;
    return;

//*************************************************
//    고장전류(Over Current) 설정
//    시스템 이상으로 판단하는 
//    고장과전류값을 입력받고 플레시메모리에 저장
//*************************************************
  case MAIN_OVERAMP_SET:
    if (!SystemRun) sExecStep++; else sExecStep = MAIN_CANT_USE;
    return;

  case MAIN_OVERAMP_SET+1:
    screen_clear();
    printf("과전류기준설정");
    if (fMaxOverAmp < fOperVolt) fMaxOverAmp = fOperAmp * 1.1;
    printf("\n현재:"); printf_volt(fMaxOverAmp); printf("[A] ");
    fTempSet = fMaxOverAmp; 
    printf("\n변경:"); printf_volt(fTempSet); printf("[A] ");
    sExecStep++;
    return;    

  case MAIN_OVERAMP_SET+2:
    if (COUNT_UP) 
    {
      float_jog_plus(79999);
      sExecStep++;
    }
    else if (COUNT_DN) 
    {
      float_jog_minus(fOperAmp);
      sExecStep++;
    }
    else if (ENTER_KEY) 
    {
      if (fMaxOverAmp == fTempSet) sExecStep = 1;
      else
      {
        fMaxOverAmp = fTempSet;
        printf("\r변경:"); 
        printf_volt(fMaxOverAmp); 
        printf("[A] ");
        sExecStep = MAIN_SAVE_END;
      }
    }
    return; 
    
  case MAIN_OVERAMP_SET+3:
    printf("\r변경:"); printf_volt(fTempSet); printf("[A] ");
    sExecStep--;
    return;
   
//*************************************************
//    고장전압(Over Voltage) 설정
//    시스템 이상으로 판단하는 
//    고장과전압값을 입력받고 플레시메모리에 저장
//*************************************************
  case MAIN_OVERVOLT_SET:
    if (!SystemRun) sExecStep++; else sExecStep = MAIN_CANT_USE;
    return;

  case MAIN_OVERVOLT_SET+1:
    screen_clear();
    printf("과전압기준설정");
    if (fMaxOverVolt < fOperVolt) fMaxOverVolt = fOperVolt * 1.1;
    printf("\n현재:"); printf_volt(fMaxOverVolt); printf("[V] ");
    fTempSet = fMaxOverVolt; 
    printf("\n변경:"); printf_volt(fTempSet); printf("[V] ");
    sExecStep++;
    return;    

  case MAIN_OVERVOLT_SET+2:
    if (COUNT_UP) 
    {
      float_jog_plus(999);
      sExecStep++;
    }
    else if (COUNT_DN) 
    {
      float_jog_minus(fOperVolt);
      sExecStep++;
    }
    else if (ENTER_KEY) 
    {
      if (fMaxOverVolt == fTempSet) sExecStep = 1;
      else
      {
        fMaxOverVolt = fTempSet;
        printf("\r변경:"); 
        printf_volt(fMaxOverVolt); 
        printf("[V] ");
        sExecStep = MAIN_SAVE_END;
      }
    }
    return; 
    
  case MAIN_OVERVOLT_SET+3:
    printf("\r변경:"); printf_volt(fTempSet); printf("[V] ");
    sExecStep--;
    return;
        
//*************************************************
//    운전모드 전환
//    C/C(전류기준) <-> C/V(전압기준)
//*************************************************
  case MAIN_OPMODE_CHANGE:
    if (!SystemRun) sExecStep++; else sExecStep = MAIN_CANT_USE;
    return;
    
  case MAIN_OPMODE_CHANGE+1:
    screen_clear();
    printf("<운전모드 전환>");
    if (LocalMode == CC_MODE) printf("\nC/C(전류기준)");
    else printf("\nC/V(전압기준)");
    TempSet = LocalMode;
    sExecStep++;
    return;    

  case MAIN_OPMODE_CHANGE+2:
    if ((MENU_UP)|(MENU_DN)) 
    {
      if (TempSet == CC_MODE) TempSet = CV_MODE; else TempSet = CC_MODE;
      if (TempSet == CC_MODE) printf("\rC/C(전류기준)");
      else printf("\rC/V(전압기준)");
    }
    else if (ENTER_KEY) sExecStep++;
    return;    

  case MAIN_OPMODE_CHANGE+3:
    if (LocalMode == TempSet) sExecStep = 1;
    else 
    {
      LocalMode = TempSet;     
      screen_clear();
      sExecStep++;
    }
    return; 
    
  case MAIN_OPMODE_CHANGE+4:
    //PidStable = 0;  // 예비 Ref 값 다시 설정
    if (LocalMode == CC_MODE)
    {
      printf("LOCAL운전모드가");
      printf("\nC/C(전류기준)로 변경되었습니다");
    }
    else
    {
      printf("LOCAL운전모드가");
      printf("\nC/V(전압기준)로 변경되었습니다");
    }
    DelayStep = 0;
    sExecStep++;
    return; 

  case MAIN_OPMODE_CHANGE+5:
    if (ANY_KEY) sExecStep = MAIN_SAVE_END;
    else if (step_delay(SEC_1*3)) sExecStep = MAIN_SAVE_END;
    return;

//*************************************************
//    출력극성 전환
//    P:정출력 <-> N:역출력
//*************************************************
  case MAIN_POLE_CHANGE:
    if (!SystemRun) sExecStep++; else sExecStep = MAIN_CANT_USE;
    return;
    
  case MAIN_POLE_CHANGE+1:
    screen_clear();
    printf("<출력극성 전환>");
    if (LocalPole == PLUS) printf("\n정출력[PLUS]");
    else printf("\n역출력[MINUS]");
    TempSet = LocalPole;
    sExecStep++;
    return;    

  case MAIN_POLE_CHANGE+2:
    if ((MENU_UP)|(MENU_DN)) 
    {
      if (TempSet == PLUS) TempSet = MINUS; else TempSet = PLUS;
      if (TempSet == PLUS) printf("\r정출력[PLUS] "); else printf("\r역출력[MINUS]");
    }
    else if (ENTER_KEY) sExecStep++;
    return;    

  case MAIN_POLE_CHANGE+3:
    if (LocalPole == TempSet) sExecStep = 1;
    else 
    { 
      LocalPole = TempSet;
      screen_clear();
      sExecStep++;
    }
    return; 
    
  case MAIN_POLE_CHANGE+4:
    // 예비 Ref 값 다시 설정
    PidStatus = 0;
    PidStable = 0;
    if (LocalPole == PLUS)
    {
      printf("LOCAL운전극성이");
      printf("\n정[PLUS]으로변경되었습니다");
    }
    else
    {
      printf("LOCAL운전극성이");
      printf("\n역[MINUS]으로변경되었습니다");
    }
    DelayStep = 0;
    sExecStep++;
    return; 

  case MAIN_POLE_CHANGE+5:
    if (ANY_KEY) sExecStep = MAIN_SAVE_END;
    else if (step_delay(SEC_1*3)) sExecStep = MAIN_SAVE_END;
    return;
//*************************************************
//    운전시간 설정
//*************************************************
  case MAIN_RUNTIME_SET:
    //if (!SystemRun) sExecStep++; else sExecStep = MAIN_CANT_USE;
    sExecStep++;
    return;

  case MAIN_RUNTIME_SET+1:
    screen_clear();
    if (MaxHour > 23) MaxHour = 0;
    if (MaxMinute > 59) MaxMinute = 0;
    if (MaxSec > 59) MaxSec = 0;
    iTempSet = (MaxHour * 3600) + (MaxMinute * 60) + MaxSec;
    printf("운전시간 설정");
    printf("\n현재-%02d:%02d:%02d", MaxHour, MaxMinute, MaxSec);
    printf("\n변경-%02d:%02d:%02d", MaxHour, MaxMinute, MaxSec);
    sCurPos -= 7;
    sExecStep++;
    return;

  case MAIN_RUNTIME_SET+2:
    if (COUNT_UP) {if (MaxHour < 23) MaxHour++;}
    else if (COUNT_DN) {if (MaxHour > 0) MaxHour--;}
    else if (ENTER_KEY) sExecStep++;
    if (ANY_KEY)
    {
      printf("\r변경-%02d:%02d:%02d", MaxHour, MaxMinute, MaxSec);
      sCurPos -= 7;
    }
    return; 

  case MAIN_RUNTIME_SET+3:
    sCurPos += 3;
    sExecStep++;
    return;  
   
  case MAIN_RUNTIME_SET+4:
    if (COUNT_UP) {if (MaxMinute < 59) MaxMinute++;}
    else if (COUNT_DN) {if (MaxMinute > 0) MaxMinute--;}
    else if (ENTER_KEY) sExecStep++;
    if (ANY_KEY)
    {
      printf("\r변경-%02d:%02d:%02d", MaxHour, MaxMinute, MaxSec);
      sCurPos -= 4;
    }
    return;
   
  case MAIN_RUNTIME_SET+5:
    sCurPos += 3;
    sExecStep++;
    return;  
   
  case MAIN_RUNTIME_SET+6:
    if (COUNT_UP) {if (MaxSec < 59) MaxSec++;}
    else if (COUNT_DN) {if (MaxSec > 0) MaxSec--;}
    else if (ENTER_KEY) sExecStep++;
    if (ANY_KEY)
    {
      printf("\r변경-%02d:%02d:%02d", MaxHour, MaxMinute, MaxSec);
      sCurPos--;
    }
    return;
   
  case MAIN_RUNTIME_SET+7:
    printf("\n확인-%02d:%02d:%02d", MaxHour, MaxMinute, MaxSec);
    iMaxRunTime = (MaxHour * 3600) + (MaxMinute * 60) + MaxSec;
    DelayStep = 0;
    sExecStep++;
    return;
    
  case MAIN_RUNTIME_SET+8:
    if (iTempSet == iMaxRunTime) sExecStep = 1;
    else sExecStep = MAIN_SAVE_END;
    return; 
   
//*************************************************
//   변경된 데이터를 저장하고 
//   3초간 지연 또는 키입력으로 메인메뉴로 복귀
//*************************************************
  case MAIN_SAVE_END:
    result = backup_data_save();
    if (result == true) printf("\n변경된 데이터가 저장되었습니다");
    else                printf("\n메모리에러발생! 데이터저장에러");
    DelayStep = 0;
    sExecStep++;
    return;
    
  case MAIN_SAVE_END+1:
    if (ANY_KEY) sExecStep = 1;
    else if (step_delay(SEC_1*3)) sExecStep = 1;
    return;
   
//*************************************************
//   작업중 안내
//*************************************************  
  case MAIN_NOFUNC:
    screen_clear();
    printf("준비중입니다.");
    printf("\n>Not Ready:%2d ", MenuNo);
    sExecStep = MAIN_END;
    return;
  
//*************************************************
//    운전중 조작 금지 메시지
//*************************************************
  case MAIN_CANT_USE:
    screen_clear();
    printf("운전중에는 사용할 수 없습니다.");
    sExecStep = MAIN_END;
    return;    
   
//*************************************************
//   1Sec 지연후 메인메뉴로 복귀
//*************************************************  
  case MAIN_END:
    DelayStep = 0;
    sExecStep++;
    return;
    
  case MAIN_END+1:
    if (ANY_KEY) sExecStep = 1;
    else if (step_delay(SEC_1)) sExecStep = 1;
    return;

  default:
    printf("\n?Error:MAIN MENU");
    sExecStep = 0;
    return;
  }
 }

char ErrStatus;
char RestHour, RestMinute, RestSec;
//*************************************************
//   장애 발생 원인 표시
//*************************************************
void error_display(void)
{
  printf("\nError NO:%2d", ErrStatus);
}

