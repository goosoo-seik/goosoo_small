

// Include Standard LIB  files
#include "project.h"

/********************************************/
/* DEMO1302.C 				    */
/* 2007-08-30: Modefied by Lee Y K    	    */
/********************************************/

/***************************** Defines *****************************/
#define	uchar	unsigned char
#define	HT1381	/* compile directive, modify as required */

/***************************************************************************/
/* Prototypes                                                              */
/***************************************************************************/
uchar	rbyte_3w();
void	reset_3w();
void	wbyte_3w(uchar);
uchar	DS1302_readbyte(uchar ClkAdd);
void	DS1302_writebyte(uchar ClkAdd, uchar ClkData);
void	HT1381_burstramrd();
void	HT1381_burstramwr();

/* global variables */
uchar	Year, Month, Date, Hour, Minute, Sec, Day;
uchar	sda;
char Hour0;
short sRtcTime;
char DSerror;
unsigned char DSadd, DSdata;
#ifdef HT1381
 #define RTC_MEM_SIZE	8
 unsigned char DSregister[RTC_MEM_SIZE];
#endif
#ifdef DS1302
 #define RTC_MEM_SIZE	64
 unsigned char DSregister[RTC_MEM_SIZE];
#endif
 

/****************************/
/* Real Time Clock DS1302   */
/*  Test Functions          */
/****************************/
#define	RTC_UPDATE	SEC_1/4

void date_time_update(void)
{
  if ((ExecMode != RTC_SET)&(ExecMode != RTC_TEST))
  if (++sRtcTime >= RTC_UPDATE)
  {
    sRtcTime = 0;
    rtc_time_read();
  }
}

/**************************************/
/* RTC Test Menu display              */
/**************************************/
void menu_display_rtc_set(char no)
{  
  if (no == 0)      printf("<현재시간설정>");
  else if (no == 1) printf("\n현재시간변경");
  else if (no == 2) printf("\n현재시간조회");
  else if (no == 3) printf("\n내장시계리셋");
  else if (no == 4) printf("\n복귀");
}

// RTC Set Execute step define
#define	RTC_SET_EXEC            10
#define	RTC_TIME_SET		100
#define	RTC_TIME_READ		200
#define	RTC_INIT		300
#define	RTC_WAIT		400
void RTC_set_function(void)
{
  char lp;
 switch (sExecStep)
 {   
 case 0:
   display_mode(2);
   DelayStep = 0;
   CursorUse = 1;
   MenuStart = 0;
   MenuEnd = 3;
   MenuSize = 5;
   sExecStep++;                    
   return;
    
 case 1:
   screen_clear();
   LineBlink = 1;
   DelayStep = 0;
   debug_monit(MONOUT);
   for (lp = MenuStart; lp <= MenuEnd; lp++) menu_display_rtc_set(lp);
   goto_cursor(0,0);
   sExecStep++;
   return;
  
 case 2:
   if (MENU_UP) popup_menu_update(RTC_SET,UP); 
   else if (MENU_DN) popup_menu_update(RTC_SET,DOWN); 
   else if (ENTER_KEY) sExecStep = RTC_SET_EXEC; 
   else if (step_delay(SEC_1*10)) execmode_change(MAIN_MENU);
   return;
   
  case 3:
   sExecStep--;
   return;

  case RTC_SET_EXEC:
   DelayStep = 0;
   LineBlink = 0;
   MenuNo = MenuStart + find_cursor_vpos();
   if (MenuNo == 0) execmode_change(MAIN_MENU); 
   else if (MenuNo == 1) sExecStep = RTC_TIME_SET;
   else if (MenuNo == 2) sExecStep = RTC_TIME_READ;
   else if (MenuNo == 3) sExecStep = RTC_INIT;
   else if (MenuNo == 4) execmode_change(MAIN_MENU); 
   //else if (step_delay(SEC_1*5)) execmode_change(MAIN_MENU); 
   return;
   
 /**********************/
 /*  RTC Time Set      */
 /**********************/
 case RTC_TIME_SET:
   screen_clear();
   rtc_time_read();
   printf(">오늘날자설정");
   printf("\n%02X년%02X월%02X일", Year, Month, Date);
   sExecStep++;
   return;

 case RTC_TIME_SET+1:
   Year = bcd2hex(Year);
   Month = bcd2hex(Month);
   Date = bcd2hex(Date);
   if (Year > 99) Year = 0;
   sCurPos -= 11;
   sExecStep++;
   return;  
   
 case RTC_TIME_SET+2:
   if (COUNT_UP) {if (Year < 99) Year++;}
   else if (COUNT_DN) {if (Year > 0) Year--;}
   else if (ENTER_KEY) sExecStep++;
   if (ANY_KEY)
   {
     printf("\r%02d년%02d월%02d일", Year, Month, Date);
     sCurPos -= 11;
   }
   return;
   
 case RTC_TIME_SET+3:
   if (Month > 12) Month = 1;
   else if (Month < 1) Month = 1;
   sCurPos += 4;
   sExecStep++;
   return;  
   
 case RTC_TIME_SET+4:
   if (COUNT_UP) {if (Month < 12) Month++;}
   else if (COUNT_DN) {if (Month > 1) Month--;}
   else if (ENTER_KEY) sExecStep++;
   if (ANY_KEY)
   {
     printf("\r%02d년%02d월%02d일", Year, Month, Date);
     sCurPos -= 7;
   }
   return;

 case RTC_TIME_SET+5:
   if (Date > 31) Date = 1;
   else if (Date < 1) Date = 1;
   sCurPos += 4;
   sExecStep++;
   return;  
   
 case RTC_TIME_SET+6:
   if (COUNT_UP) {if (Date < 31) Date++;}
   else if (COUNT_DN) {if (Date > 1) Date--;}
   else if (ENTER_KEY) sExecStep++;
   if (ANY_KEY)
   {
     printf("\r%02d년%02d월%02d일", Year, Month, Date);
     sCurPos -= 3;
   }
   return;   

 case RTC_TIME_SET+7:
   Year = hex2bcd(Year);
   Month = hex2bcd(Month);
   Date = hex2bcd(Date);
   printf("\r%02X년%02X월%02X일", Year, Month, Date);
   sExecStep++;
   return;
 
  case RTC_TIME_SET+8:
    rtc_time_set();
    sExecStep++;
    return; 

 case RTC_TIME_SET+9: sExecStep++; return;
 
 case RTC_TIME_SET+10:
   rtc_time_read();
   printf("\n>현재시간설정");
   printf("\n%02X시%02X분%02X초", Hour, Minute, Sec);
   sExecStep++;
   return;

 case RTC_TIME_SET+11:
   Hour = bcd2hex(Hour);
   Minute = bcd2hex(Minute);
   Sec = bcd2hex(Sec);
   if (Hour > 23) Hour = 0;
   sCurPos -= 11;
   sExecStep++;
   return;  
   
 case RTC_TIME_SET+12:
   if (COUNT_UP) {if (Hour < 23) Hour++;}
   else if (COUNT_DN) {if (Hour > 0) Hour--;}
   else if (ENTER_KEY) sExecStep++;
   if (ANY_KEY)
   {
     printf("\r%02d시%02d분%02d초", Hour, Minute, Sec);
     sCurPos -= 11;
   }
   return;
   
 case RTC_TIME_SET+13:
   if (Minute > 59) Minute = 0;
   sCurPos += 4;
   sExecStep++;
   return;  
   
 case RTC_TIME_SET+14:
   if (COUNT_UP) {if (Minute < 59) Minute++;}
   else if (COUNT_DN) {if (Minute > 0) Minute--;}
   else if (ENTER_KEY) sExecStep++;
   if (ANY_KEY)
   {
     printf("\r%02d시%02d분%02d초", Hour, Minute, Sec);
     sCurPos -= 7;
   }
   return;

 case RTC_TIME_SET+15:
   if (Sec > 0x59) Sec = 0;
   sCurPos += 4;
   sExecStep++;
   return;  
   
 case RTC_TIME_SET+16:
   if (COUNT_UP) {if (Sec < 59) Sec++;}
   else if (COUNT_DN) {if (Sec > 0) Sec--;}
   else if (ENTER_KEY) sExecStep++;
   if (ANY_KEY)
   {
     printf("\r%02d시%02d분%02d초", Hour, Minute, Sec);
     sCurPos -= 3;
   }
   return;   

  case RTC_TIME_SET+17:
   Hour = hex2bcd(Hour);
   Minute = hex2bcd(Minute);
   Sec = hex2bcd(Sec);
   printf("\r%02X시%02X분%02X초", Hour, Minute, Sec);
   sExecStep++;
   return;
 
  case RTC_TIME_SET+18:
   rtc_time_set();
   sExecStep++;
   return; 
    
  case RTC_TIME_SET+19:
    sExecStep = RTC_TIME_READ;
    return;   
   
 /**************************/
 /*  RTC date & time read  */
 /**************************/  
 case RTC_TIME_READ:
   screen_clear();
   DelayStep = 0;
   printf(">현재시각보기");
   sExecStep++;
   return;
   
 case RTC_TIME_READ+1:
   goto_cursor(1,0);
   rtc_time_read();
   printf("\n%02X년%02X월%02X일", Year, Month, Date);
   printf("\n%02X시%02X분%02X초", Hour, Minute, Sec);
   sExecStep++;
   return;
   
 case RTC_TIME_READ+2:    
   if (ENTER_KEY) sExecStep = 1;
    else  if (step_delay(SEC_1/8)) sExecStep = RTC_TIME_READ+1;
    return;
 
 /******************************/
 /* RTC initialize & start */
 /******************************/    
 case RTC_INIT:
   screen_clear();
   rtc_initialize();
   printf(">RTC Wake UP..");
   DelayStep = 0;
   sExecStep++;
   return;
   
 case RTC_INIT+1:
   if (step_delay(SEC_1)) sExecStep = 1;
   return;
   
 case RTC_WAIT:
   DelayStep = 0;
   sExecStep++;
   return;
   
 case RTC_WAIT+1:
   if (step_delay(SEC_1*2)) sExecStep = 1;
   return;
   
 };
}

/**************************************/
/* RTC Test Menu display              */
/**************************************/
/*
void menu_display_rtc_test(char no)
{  
  if (no == 0)      printf("<< RTC Test >>   ");
  else if (no == 1) printf("\nDate Time Set  ");
  else if (no == 2) printf("\nDate Time Read ");
  else if (no == 3) printf("\nRTC Wake UP    ");
  //else if (no == 4) printf("\nRegister READ  ");
  //else if (no == 5) printf("\nAll Reg. READ  ");
  //else if (no == 6) printf("\nRegister WRITE ");
  else if (no == 4) printf("\nReturn To Main ");
}

short  shex2bcd(short hex)
{                                             
short bcd, bcd1, bcd0;
bcd1 = hex / 10;
bcd0 = hex % 10;
bcd = (bcd1 << 4) + bcd0;
return bcd;                           
} 

// Execute step define
#define	RTC_TEST_EXEC           10
#define	RTC_TEST_NOFUNC         50
#define	DS1302_ALL_READ		100
#define	DS1302_READ		200
#define	DS1302_WRITE		300
#define	DS1302_TIME_READ	400
#define	DS1302_INIT		500
#define	DS1302_TIME_SET		600
#define	DS1302_WAIT		700
*/
/**************************************/
/* RTC DS1302 or HT1381 test_function */
/**************************************/
//unsigned char sYear, sMonth, sDate;
/*
void RTC_test_function(void)
{
  char lp;
 switch (sExecStep)
 {   
 case 0:
   display_mode(0);
   DelayStep = 0;
   CursorUse = 1;
   MenuStart = 0;
   MenuEnd = 4;
   MenuSize = 5;
   sExecStep++;
   return;
    
 case 1:
   screen_clear();
   LineBlink = 1;
   DelayStep = 0;
   debug_monit(MONOUT);
   for (lp = MenuStart; lp <= MenuEnd; lp++) menu_display_rtc_test(lp);
   goto_cursor(0,0);
   sExecStep++;
   return;

 case 2:
   if (MENU_UP) popup_menu_update(RTC_TEST,UP); 
   else if (MENU_DN) popup_menu_update(RTC_TEST,DOWN); 
   else if (ENTER_KEY) sExecStep = RTC_TEST_EXEC; 
   return;
   
 case 3:
   sExecStep--;
   return;
   
 case RTC_TEST_EXEC:
   DelayStep = 0;
   LineBlink = 0;
   MenuNo = MenuStart + find_cursor_vpos();
   if (MenuNo == 0) sExecStep = RTC_TEST_NOFUNC;
   else if (MenuNo == 1) sExecStep = DS1302_TIME_SET;
   else if (MenuNo == 2) sExecStep = DS1302_TIME_READ;
   else if (MenuNo == 3) sExecStep = DS1302_INIT;
   //else if (MenuNo == 4) sExecStep = DS1302_READ;
   //else if (MenuNo == 5) sExecStep = DS1302_ALL_READ;
   //else if (MenuNo == 6) sExecStep = DS1302_WRITE;
   else if (MenuNo == 4) execmode_change(SYSTEM_TEST); 
   //else if (step_delay(SEC_1*5)) execmode_change(MAIN_MENU); 
   return;
 
 //
 //  RTC Time Set      
 //
 case RTC_TEST_NOFUNC:
   screen_clear();
   DelayStep = 0;
   sExecStep = 1; 
   return;
 
 case DS1302_TIME_SET:
   screen_clear();
   rtc_time_read();
   printf("<Date & Time Set>");
   printf("\n>YY/MM/DD:%02X/%02X/%02X", Year, Month, Date);
   sExecStep++;
   return;

 case DS1302_TIME_SET+1:
   Year = bcd2hex(Year);
   Month = bcd2hex(Month);
   Date = bcd2hex(Date);
   if (Year > 99) Year = 0;
   sCurPos -= 8;
   sExecStep++;
   return;  
   
 case DS1302_TIME_SET+2:
   if (COUNT_UP) {if (Year < 99) Year++;}
   else if (COUNT_DN) {if (Year > 0) Year--;}
   else if (ENTER_KEY) sExecStep++;
   if (ANY_KEY)
   {
     printf("\r>YY/MM/DD:%02d/%02d/%02d", Year, Month, Date);
     sCurPos -= 8;
   }
   return;
   
 case DS1302_TIME_SET+3:
   if (Month > 12) Month = 1;
   else if (Month < 1) Month = 1;
   sCurPos += 3;
   sExecStep++;
   return;  
   
 case DS1302_TIME_SET+4:
   if (COUNT_UP) {if (Month < 12) Month++;}
   else if (COUNT_DN) {if (Month > 1) Month--;}
   else if (ENTER_KEY) sExecStep++;
   if (ANY_KEY)
   {
     printf("\r>YY/MM/DD:%02d/%02d/%02d", Year, Month, Date);
     sCurPos -= 5;
   }
   return;

 case DS1302_TIME_SET+5:
   if (Date > 12) Date = 1;
   else if (Date < 1) Date = 1;
   sCurPos += 3;
   sExecStep++;
   return;  
   
 case DS1302_TIME_SET+6:
   if (COUNT_UP) {if (Date < 31) Date++;}
   else if (COUNT_DN) {if (Date > 1) Date--;}
   else if (ENTER_KEY) sExecStep++;
   if (ANY_KEY)
   {
     printf("\r>YY/MM/DD:%02d/%02d/%02d", Year, Month, Date);
     sCurPos -= 2;
   }
   return;   

 case DS1302_TIME_SET+7:
   Year = hex2bcd(Year);
   Month = hex2bcd(Month);
   Date = hex2bcd(Date);
   printf("\n>YY/MM/DD:%02X/%02X/%02X", Year, Month, Date);
   sExecStep++;
   return;
   
 case DS1302_TIME_SET+8: sExecStep++; return;
 case DS1302_TIME_SET+9: sExecStep++; return;
 

 case DS1302_TIME_SET+10:
   //rtc_time_read();
   printf("\n>HH/MM/SS:%02X/%02X/%02X", Hour, Minute, Sec);
   sExecStep++;
   return;
   
 case DS1302_TIME_SET+11:
   Hour = bcd2hex(Hour);
   Minute = bcd2hex(Minute);
   Sec = bcd2hex(Sec);
   if (Hour > 23) Hour = 0;
   sCurPos -= 8;
   sExecStep++;
   return;  
   
 case DS1302_TIME_SET+12:
   if (COUNT_UP) {if (Hour < 23) Hour++;}
   else if (COUNT_DN) {if (Hour > 0) Hour--;}
   else if (ENTER_KEY) sExecStep++;
   if (ANY_KEY)
   {
     printf("\r>HH/MM/SS:%02d/%02d/%02d", Hour, Minute, Sec);
     sCurPos -= 8;
   }
   return;
   
 case DS1302_TIME_SET+13:  
   if (Minute > 59) Minute = 0;
   sCurPos += 3;
   sExecStep++;
   return;  
   
 case DS1302_TIME_SET+14:
   if (COUNT_UP) {if (Minute < 59) Minute++;}
   else if (COUNT_DN) {if (Minute > 0) Minute--;}
   else if (ENTER_KEY) sExecStep++;
   if (ANY_KEY)
   {
     printf("\r>HH/MM/SS:%02d/%02d/%02d", Hour, Minute, Sec);
     sCurPos -= 5;
   }
   return;
   
 case DS1302_TIME_SET+15:
   if (Sec > 59) Sec = 0;
   sCurPos += 3;
   sExecStep++;
   return;  
   
 case DS1302_TIME_SET+16:
   if (COUNT_UP) {if (Sec < 59) Sec++;}
   else if (COUNT_DN) {if (Sec > 0) Sec--;}
   else if (ENTER_KEY) sExecStep++;
   if (ANY_KEY)
   {
     printf("\r>HH/MM/SS:%02d/%02d/%02d", Hour, Minute, Sec);
     sCurPos -= 2;
   }
   return;

 case DS1302_TIME_SET+17:
   Hour = hex2bcd(Hour);
   Minute = hex2bcd(Minute);
   Sec = hex2bcd(Sec);
   printf("\n>HH/MM/SS:%02X/%02X/%02X", Hour, Minute, Sec);
   sExecStep++;
   return; 
 
  case DS1302_TIME_SET+18:
    if (ENTER_KEY)
    {
      rtc_time_set();
      //rtc_init_set();
      sExecStep++;
    }
   return; 
    
  case DS1302_TIME_SET+19:
    // 데이터저장실행을 초기화
    sStoreStep = 1;
    sExecStep = DS1302_TIME_READ;
    return;   

 //
 //  RTC date & time read  
 //
 case DS1302_TIME_READ:
   screen_clear();
   printf(">RTC time read\n");
   sExecStep++;
   return;
 
 case DS1302_TIME_READ+1:
   DelayStep = 0;
   rtc_time_read();
   printf("\r%02X/%02X/%02X  ", Year, Month, Date);
   printf("%02X:%02X:%02X", Hour, Minute, Sec);
   sExecStep++;
   return;
   
 case DS1302_TIME_READ+2:    
   if (ENTER_KEY) sExecStep = 1;
    else if (ANY_KEY) sExecStep = DS1302_TIME_READ+1; 
    else  if (step_delay(SEC_1)) sExecStep = DS1302_TIME_READ+1;
    return;
 
 //
 //  DS1302 initialize & start 
 // 
 case DS1302_INIT:
   screen_clear();
   rtc_initialize();
   printf(">DS1302 Wake UP..");
   sExecStep++;
   return;
   
 case DS1302_INIT+1:
   if (step_delay(SEC_1)) sExecStep = 1;
   return;
   
 case DS1302_WAIT:
   DelayStep = 0;
   sExecStep++;
   return;
   
 case DS1302_WAIT+1:
   if (step_delay(SEC_1*20)) sExecStep = 1;
   return;
   
 //
 //  DS1302 all register read  
 //
 case DS1302_ALL_READ:   
   screen_clear();
   printf(">DS1302 All Reg. READ");
   sExecStep++;
   return;

 case DS1302_ALL_READ+1:
   HT1381_burstramrd();
   sExecStep++;
   return;
 
  case DS1302_ALL_READ+2:
   if (MENU_KEY) sExecStep = 1;
   else if (ENTER_KEY) sExecStep = DS1302_ALL_READ+1;
   return;   
    
 //
 //  DS1302 register read  
 //
 case DS1302_READ:   
   screen_clear();
   printf(">DS1302 Reg. READ");
   sExecStep++;
   return;

 case DS1302_READ+1:
   keyin_start(2);
   printf("\nDSaddress:%02X", KeyValue);
   sCurPos--;
   sExecStep++;
   return;
 
 case DS1302_READ+2:  
   if (PushKey)
   {
     if(key_input_hex() == 0) 
     {
       DSadd = KeyValue;
       sExecStep++;
     }
     printf("\rDSaddress:%02X", KeyValue);
     sCurPos--;
   }
   return;
 
 case DS1302_READ+3:
   if (KeyValue > 7)
   {
     printf("\nRange Over");
     sExecStep = DS1302_WAIT;     
    }
   else
   {
     DSadd = KeyValue;
     DSregister[DSadd] = DS1302_readbyte(DSadd);
     sExecStep++;
   }
   return;
  
 case DS1302_READ+4:
   printf("\rDS1302[%02X]:%02X", DSadd, DSregister[DSadd]);
   if (DSerror) printf (" E:%1d", DSerror);
   sExecStep++;
   return;
  
 case DS1302_READ+5: sExecStep++; return;
    
 case DS1302_READ+6:
   if (PushKey == 'a') sExecStep = 1;
     else if (PushKey != 0) sExecStep = DS1302_READ+1;
     else if (step_delay(SEC_1)) sExecStep = DS1302_READ+1; 
   return;


 //
 // DS1302 regiter monit & write 
 //
 case DS1302_WRITE:
   screen_clear();
   printf(">DS1302 Reg. Write");
   sExecStep++;
   return;
   
 case DS1302_WRITE+1:
   keyin_start(2);
   printf("\nDS1302[%02X]", KeyValue);
   sCurPos -= 2;
   sExecStep++;
   return;
 
 case DS1302_WRITE+2:  
   if (PushKey)
   {
     if(key_input_hex() == 0) sExecStep++;
     printf("\rDS1302[%02X]", KeyValue);
     sCurPos -= 2;
   }
   return;

 case DS1302_WRITE+3:
   if (KeyValue > 7) 
   {
     printf("\nRange Over");
     sExecStep = DS1302_WAIT;   
   }
   else
   {
     DSadd = KeyValue;
     DSregister[DSadd] = DS1302_readbyte(DSadd);
     printf("\rDS1302[%02X]%02X", DSadd, DSregister[DSadd]);
     keyin_start(2);
     printf("\nDS1302[%02X]%02X", DSadd, KeyValue);
     sCurPos--;
     sExecStep++;
   }
   return;
   
 case DS1302_WRITE+4:  
   if (PushKey)
   {
     if(key_input_hex() == 0) sExecStep++;
     printf("\rDS1302[%02X]%02X", DSadd, KeyValue);
     sCurPos--;
   }
   return;
    
 case DS1302_WRITE+5:
   DSdata = KeyValue;
   DS1302_writebyte(DSadd, DSdata);
   sExecStep++;
   return;
   
 case DS1302_WRITE+6:  
   DSregister[DSadd] = DS1302_readbyte(DSadd);
   if (DSerror) printf("\n>Write Error..");
    else if (DSregister[DSadd] != DSdata) 
      printf("\rDS1302[%02X]%02X-%02X", DSadd, DSregister[DSadd], DSdata);
    else printf("\rDS1302[%02X]%02X-OK", DSadd, DSregister[DSadd]);
   sExecStep++;
    return;

 case DS1302_WRITE+7: sExecStep++; return;  
 
 case DS1302_WRITE+8:  
   if (PushKey == 'a') sExecStep = 1;
    else if (PushKey != 0) sExecStep = DS1302_WRITE+1; 
    else  if (step_delay(SEC_1)) sExecStep = DS1302_WRITE+1; 
   return;

 default: sExecStep = 0; return;   
 };
}
*/
void reset_3w()	/* ----- reset and enable the 3-wire interface ------ */
{
   pio_clear(PIOB, RTC_CK|RTC_RST);
   delay_us(10);
   pio_set(PIOB, RTC_RST);   
}

void wbyte_3w(uchar W_Byte)	/* ------ write one byte to the device ------- */
{
uchar i;
	
	pio_is_out( PIOB, RTC_DT ) ;  

	for(i = 0; i < 8; ++i)
	{
		pio_clear(PIOB, RTC_DT);	//IO = 0;
		
		if(W_Byte & 0x01)
		{
		  pio_set(PIOB, RTC_DT);	//IO = 1; set port pin high to read data
		}
		pio_clear(PIOB, RTC_CK);	//SCLK = 0;
		pio_set(PIOB, RTC_CK); 		//SCLK = 1;
		W_Byte >>= 1;
      }
}

uchar	rbyte_3w()	/* ------- read one byte from the device -------- */
{
uchar i;
uchar R_Byte;
uchar TmpByte;

	pio_is_in( PIOB, RTC_DT );
	
	R_Byte = 0x00;
	pio_set(PIOB, RTC_DT);		//IO = 1;
	for(i = 0; i < 8; i++)
	{
		pio_set(PIOB, RTC_CK); 		//SCLK = 1;
		pio_clear(PIOB, RTC_CK);	//SCLK = 0;
		//TmpByte = (uchar)IO;
		if ((pio_read(PIOB) & RTC_DT ) == RTC_DT) TmpByte = 1; else TmpByte = 0;
		TmpByte <<= 7;
		R_Byte >>= 1;
		R_Byte |= TmpByte; 
	}
	return R_Byte;
}

uchar	DS1302_readbyte(uchar add)	/* --- write one byte using values entered by user --- */
{
	uchar data;
	if (add >= RTC_MEM_SIZE) add = 0;
	add = add << 1;
	add &= 0x0F;
	add |= 0x81;

	reset_3w();
	wbyte_3w(add);
	data = rbyte_3w();
	reset_3w();
	return data;
}

void	DS1302_writebyte(uchar add, uchar data)	/* --- write one byte using values entered by user --- */
{
	if (add >= RTC_MEM_SIZE) add = 0;
	add = add << 1;
	add &= 0x0E;
	add |= 0x80;
	
	reset_3w();
	wbyte_3w(add);
	wbyte_3w(data);
	reset_3w();
}

void rtc_time_read()	/* ---- loop read & display clock registers ---- */
{
		reset_3w();
		wbyte_3w(0xBF);	/* clock burst */
		Sec = rbyte_3w();
		Minute = rbyte_3w();
		Hour = rbyte_3w();
		Date = rbyte_3w();
		Month = rbyte_3w();
		Day = rbyte_3w();
		Year  = rbyte_3w();
		reset_3w();
}

void HT1381_burstramrd()	/* ----------- read RAM in burst mode --------------- */
{
uchar i;

	printf("\nHT1381:");

	reset_3w();
	wbyte_3w(0xBF);	/* RAM burst read */
	for (i = 0; i < RTC_MEM_SIZE; i++)
	{
		//if(!(i % 8)) printf("\n");
		DSregister[i] = rbyte_3w();
		printf("%02X ", DSregister[i] );
	}
	reset_3w();
}

void HT1381_burstramwr()	/* ---- write one value entire RAM in burst mode ---- */
{
uchar	i;

	reset_3w();
	wbyte_3w(0xBE);	/* RAM burst write */
	for (i=0; i<RTC_MEM_SIZE; ++i)
	{
		wbyte_3w(DSregister[i]);
	}
	reset_3w();
}

void	rtc_time_set()	/* --- initialize time & date from user entries --- */
/* Note: NO error checking is done on the user entries! */
{
	reset_3w();
	wbyte_3w(0xbe);		/* clock burst write (eight registers) */
	wbyte_3w(Sec);
	wbyte_3w(Minute);
	wbyte_3w(Hour);
	wbyte_3w(Date);
	wbyte_3w(Month);
	wbyte_3w(Day);
	wbyte_3w(Year);
	wbyte_3w(0);		/* must write control register in burst mode */
	reset_3w();
}

void rtc_initialize()
{
  	reset_3w();
	wbyte_3w(0x8e);		/* control register */
	wbyte_3w(0);		/* disable write protect */
	reset_3w();
	wbyte_3w(0x90);		/* trickle charger register */
	wbyte_3w(0xab);		/* enable, 2 diodes, 8K resistor */
	reset_3w();
}
  
void rtc_init_set()
{
  	reset_3w();
	wbyte_3w(0x8e);		/* control register */
	wbyte_3w(0);		/* disable write protect */
	reset_3w();
	wbyte_3w(0x90);		/* trickle charger register */
	wbyte_3w(0xab);		/* enable, 2 diodes, 8K resistor */
	reset_3w();
	wbyte_3w(0xbe);		/* clock burst write (eight registers) */
	wbyte_3w(Sec);
	wbyte_3w(Minute);
	wbyte_3w(Hour);
	wbyte_3w(Date);
	wbyte_3w(Month);
	wbyte_3w(Day);
	wbyte_3w(Year);
	wbyte_3w(0);		/* must write control register in burst mode */
	reset_3w();
}
