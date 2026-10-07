
// Include Standard LIB  files
#include "project.h"
/* IAR Systems\Embedded Workbench 4.0 Evaluation\ARM\INC\stdio.h
 <<< _STDIO Functions >>>
#if defined(_STD_USING) && defined(__cplusplus)
  using _CSTD fpos_t;
  using _CSTD clearerr; using _CSTD fclose; using _CSTD feof;
  using _CSTD ferror; using _CSTD fflush; using _CSTD fgetc;
  using _CSTD fgetpos; using _CSTD fgets; using _CSTD fopen;
  using _CSTD fprintf; using _CSTD fputc; using _CSTD fputs;
  using _CSTD fread; using _CSTD freopen; using _CSTD fscanf;
  using _CSTD fseek; using _CSTD fsetpos; using _CSTD ftell;
  using _CSTD fwrite; using _CSTD getc; using _CSTD getchar;
  using _CSTD gets; using _CSTD perror;
  using _CSTD putc; using _CSTD putchar;
  using _CSTD printf; using _CSTD puts; using _CSTD remove;
  using _CSTD rename; using _CSTD rewind; using _CSTD scanf;
  using _CSTD setbuf; using _CSTD setvbuf; using _CSTD sprintf;
  using _CSTD sscanf; using _CSTD tmpfile; using _CSTD tmpnam;
  using _CSTD ungetc; using _CSTD vfprintf; using _CSTD vprintf;
  using _CSTD vsprintf;
  #if _DLIB_ADD_EXTRA_SYMBOLS
    using _CSTD fdopen; using _CSTD fileno;
  #endif // _DLIB_ADD_EXTRA_SYMBOLS
  #if _DLIB_ADD_C99_SYMBOLS
    using _CSTD snprintf; using _CSTD vsnprintf;
    using _CSTD vscanf; using _CSTD vsscanf;
    using _CSTD vfscanf; 
  #endif // _DLIB_ADD_C99_SYMBOLS 


  #if _DLIB_FILE_DESCRIPTOR
    using _CSTD FILE;
  #endif
#endif //defined(_STD_USING) && defined(__cplusplus)
*/

// printf() Function using _CSTD putchar;
int putchar(int c)
{
  unsigned short code;
  code = c;
  if (DebugMonit & MONOUT) putchar_dp(code);
  //if (DebugMonit & COM2OUT) putchar2(code);
  if (DebugMonit & COM1OUT) putchar1(code);
  if (DebugMonit & COM0OUT) putchar0(code);
  if (DebugMonit & FND_OUT) putchar_fnd(code);
  return c;
}

void print_str(const unsigned char* ptr)
{
  do 
  {
    if (*ptr == '\r') 
    {
      LCD_carrige_return();
    }
    else if (*ptr == '\n') 
    {
      LCD_line_feed();
    }
    else if (*ptr >= ' ') putchar_dp(*ptr);  
  }
    while(*ptr++ != ASC_ETX);           
}

void putstring1(const unsigned char* ptr)
{
  unsigned char c, end;
  end = 1;
 
  do 
  {
    c = *ptr;
    putchar1(c);
    delay_us(200);
    //ifMODEMctrl
    //  if (Com2Mode) printf("%1c", c);
    //  else putchar2(c);
    if ((c == ASC_ETX)|(c == '\r')|(c == '\n')) end = 0;
    ptr++;
  }
    while(end);           
}

char DebugMonit;
void debug_monit(char mode)
{
   DebugMonit = mode; 
}

char ascii2hex(char asc)                  
{                                              
if (asc < 58) asc = asc - 48; else asc = asc - 55;
return asc;
}

char ascii2byte(char high, char low)                  
{                                              
char byte;
if (high < 58) high = high - 48; else high = high - 55;
if (low < 58) low = low - 48; else low = low - 55;
byte = (low & 0x0F) | (high << 4);
return byte;
}

unsigned short ascii2short(char d3, char d2, char d1, char d0)                  
{                                              
unsigned short s;
s = ascii2byte(d3, d2); 
s = s<<8 | ascii2byte(d1, d0); 
return s;
}

char high2ascii(char byte)
{
byte = byte>>4;
if (byte < 10) byte = byte + 48; else byte = byte + 55;
return byte;
}

char low2ascii(char byte)
{
byte = byte & 0x0F;
if (byte < 10) byte = byte + 48; else byte = byte + 55;
return byte;
}

int bcd2int(char high, char low)
{                                             
int	int_hex;
int int1000, int100, int10;
int1000 = (int)(high >> 4) * 1000;
int100 = (int)(high & 0x0F) * 100;
int10 = (int)(low >> 4) * 10;   
int_hex = int1000 + int100 + int10;
return int_hex;                           
}                                                          

unsigned char bcd2hex(unsigned char bcd)
{                                             
unsigned char hex;
hex = (bcd >> 4) * 10;
hex += bcd & 0x0F;
return hex;                           
}

unsigned char hex2bcd(unsigned char hex)
{                                             
unsigned char bcd, bcd1, bcd0;
bcd1 = hex / 10;
bcd0 = hex % 10;
bcd = (bcd1 << 4) + bcd0;
return bcd;                           
} 

unsigned short hex2bcd3(short hex)
{                                             
unsigned short bcd, bcd2, bcd1, bcd0;
bcd2 = hex / 100;
hex = hex % 100;
bcd1 = hex / 10;
hex = hex % 10;
bcd0 = hex;
bcd = bcd2<<8|bcd1<<4|bcd0;
return bcd;                           
} 

int hex2bcd5(int hex)
{                                             
int bcd, bcd4, bcd3, bcd2, bcd1, bcd0;
bcd4 = hex / 10000;
hex = hex % 10000;
bcd3 = hex / 1000;
hex = hex % 1000;
bcd2 = hex / 100;
hex = hex % 100;
bcd1 = hex / 10;
hex = hex % 10;
bcd0 = hex;
bcd = bcd4<<16|bcd3<<12|bcd2<<8|bcd1<<4|bcd0;
return bcd;                           
} 

int bcd2hex_int(int bcd)
{                                             
int hex;
hex = ((bcd >> 12)& 0x0F) * 1000;
hex += ((bcd >> 8)& 0x0F) * 100;
hex += ((bcd >> 4)& 0x0F) * 10;
hex += bcd & 0x0F;
return hex;                           
}  

char hour24to12(char hour)                            
{
if (hour > 23) hour = 23;
if (hour > 11) hour = hour - 12;
return hour;
}

/********************************************************************
*  MakeCrc16() : make to cyclic redendancy check 16 
* *ptr로 지정된 번지부터 n byte 의 CRC를 계산해 16bit CRC를 리턴
********************************************************************/
unsigned short	MakeCrc16(unsigned char	*ptr, unsigned short	n)
{
	unsigned short	crc = 0xffff;
	unsigned short	uitmp;
	unsigned char	uctmp;

	for(uitmp = 0;uitmp < n;uitmp++)
	{
		crc = (crc & 0xff00) | (crc ^ (unsigned short)(ptr[uitmp] & 0xff));
		for(uctmp = 0;uctmp < 8;uctmp++)
		{
			if(crc & 0x0001)
				crc = (crc >> 1) ^ 0xa001;
			else
				crc >>= 1;
		}
	}
	return(crc);
}

//*----------------------------------------------------------------------------
//* Function Name       : delay
//* Object              : Wait
//* Input Parameters    : none
//* Output Parameters   : none
//* Functions calAT91B_LED    : none
//*----------------------------------------------------------------------------
void delay ( void )
{
//* Set in Volatile for Optimisation
    volatile unsigned int    i ;
//* loop delay
    for ( i = 0 ;(i < WAIT_TIME/100 );i++ ) ;
    //for ( i = 0 ;(i < 30000 );i++ ) ;
}

__ramfunc void delay_us (int time )
{
//* Set in Volatile for Optimisation
    volatile unsigned int    i ;
//* loop delay
    time = time << 3;//* 48 / 5;
    for ( i = 0 ;(i < time );i++ ) ;
}

void delay_nop (short time )
{
    short    i ;
    for ( i = 0 ;(i < time );i++ );
}

char DelayStep;
int iStepCount;
char step_delay(int time)
{
  char end;
  end = 0;
  if (DelayStep == 0) 
  {
    DelayStep = 1;
    iStepCount = time;
  }
  else if (--iStepCount < 1)
  {
    DelayStep = 0;
    sExecStep++;
    end = 1;
  }
  return end;
}
  
/*****************************************/
/*        Lamp ON/OFF Functions          */
/*              0: OFF                   */
/*              1: ON                    */
/*              2: TOGGLE                */
/*              3: Blink                 */
/*****************************************/
unsigned char RunLampIs;
void RUNlamp(BYTE onoff)
{
 if (!onoff)  pio_set( PIOB, LP_RUN );
  else if (onoff == 1) pio_clear( PIOB, LP_RUN );
  else if (onoff == 2) 
   //* Read the output state
    if ((pio_read(PIOB) & LP_RUN ) == LP_RUN ) pio_clear( PIOB, LP_RUN );
     else pio_set( PIOB, LP_RUN);
 RunLampIs = onoff;
} 

unsigned char PlusLampIs;
void PLUSlamp(BYTE onoff)
{
 if (!onoff)  pio_set( PIOB, LP_PLUS );
  else if (onoff == 1) pio_clear( PIOB, LP_PLUS );
  else if (onoff == 2) 
   //* Read the output state
    if ((pio_read(PIOB) & LP_PLUS ) == LP_PLUS ) pio_clear( PIOB, LP_PLUS );
     else pio_set( PIOB, LP_PLUS);
 PlusLampIs = onoff;
} 

unsigned char StartLampIs;
void STARTlamp(BYTE onoff)
{
 if (!onoff)  pio_set( PIOB, LP_START );
  else if (onoff == 1) pio_clear( PIOB, LP_START );
  else if (onoff == 2) 
   //* Read the output state
    if ((pio_read(PIOB) & LP_START ) == LP_START ) pio_clear( PIOB, LP_START );
     else pio_set( PIOB, LP_START);
 StartLampIs = onoff;
} 

unsigned char LcdLampIs;
void LCDlamp(BYTE onoff)
{
 if (!onoff)  pio_clear( PIOB, LCD_LP );
  else if (onoff == 1) pio_set( PIOB, LCD_LP );
  else if (onoff == 2) 
   //* output togle
    if ((pio_read(PIOB) & LP_RUN ) == LCD_LP ) pio_clear( PIOB, LCD_LP);
     else pio_set( PIOB, LCD_LP);
 LcdLampIs = onoff;
} 

unsigned char Dc24PowerIs;
void DC24power(BYTE onoff)
{
 if (!onoff)  pio_set( PIOA, DC24_RDY );
  else if (onoff == 1) pio_clear( PIOA, DC24_RDY );
  else if (onoff == 2) 
   //* output togle
    if ((pio_read(PIOA) & DC24_RDY) != 0 ) pio_clear( PIOA, DC24_RDY);
     else pio_set( PIOA, DC24_RDY);
 Dc24PowerIs = onoff;
} 

unsigned char BuzzerIs;
void BUZZERonoff(BYTE onoff)
{
 if (!onoff)  pio_clear( PIOA, BUZZER );
  else if (onoff == 1) pio_set( PIOA, BUZZER );
  else if (onoff == 2) 
   //* output togle
    if ((pio_read(PIOA) & BUZZER) != 0 ) pio_set( PIOA, BUZZER);
     else pio_clear( PIOA, BUZZER);
 BuzzerIs = onoff;
} 

unsigned char PMenableIs;
void PMenable(BYTE onoff)
{
  if (!onoff)  { pio_set( PIOA, PM_CE ); PMenableIs = 0; }
  else if (onoff == 1) {pio_clear( PIOA, PM_CE ); PMenableIs = 1;}
  //* output togle
  if (onoff == 2)
    if (PMenableIs) {pio_set( PIOA, PM_CE); PMenableIs = 0;}
    else { pio_clear( PIOA, PM_CE); PMenableIs = 1;}
}

unsigned char PMclockIs;
void PMclock(BYTE onoff)
{
  if (!onoff)  {pio_clear( PIOA, PM_CLK ); PMclockIs = 0;}
  else if (onoff == 1) {pio_set( PIOA, PM_CLK ); PMclockIs = 1;}
  //* output togle
  if (onoff == 2) 
    if (PMclockIs ) {pio_clear( PIOA, PM_CLK); PMclockIs = 0;}
    else { pio_set( PIOA, PM_CLK); PMclockIs = 1; }
}

unsigned char PMdataoutIs;
void PMdataout(BYTE onoff)
{
  if (!onoff)  {pio_set( PIOA, PM_DIN ); PMdataoutIs = 0;}
  else if (onoff == 1) {pio_clear( PIOA, PM_DIN ); PMdataoutIs = 1;}
  //* output togle
  if (onoff == 2) 
    if (PMdataoutIs ) {pio_set( PIOA, PM_DIN); PMdataoutIs = 0;}
    else { pio_clear( PIOA, PM_DIN); PMdataoutIs = 1; }
}

char DAclearIs;
void DAclear(BYTE onoff)
{
  if (!onoff)  { pio_set( PIOA, DA_CLR ); DAclearIs = 0; }
  else if (onoff == 1) {pio_clear( PIOA, DA_CLR ); DAclearIs = 1;}
  //* output togle
  if (onoff == 2)
    if (DAclearIs) {pio_set( PIOA, DA_CLR); DAclearIs = 0;}
    else { pio_clear( PIOA, DA_CLR); DAclearIs = 1;}
}

char ProfiResetIs;
void profi_reset(BYTE onoff)
{
  if (!onoff)  { pio_set( PIOA, PROFI_RST ); ProfiResetIs = 0; }
  else if (onoff == 1) {pio_clear( PIOA, PROFI_RST ); ProfiResetIs = 1;}
  //* output togle
  if (onoff == 2)
    if (DAclearIs) {pio_set( PIOA, PROFI_RST); ProfiResetIs = 0;}
    else { pio_clear( PIOA, PROFI_RST); ProfiResetIs = 1;}
}

char RS485DirIs;
void rs485_direction(BYTE dir)
{
 // if (dir == READ)  {pio_clear(PIOA, REMOTE_DR); RS485DirIs = 0;}
 // else if (dir == WRITE) {pio_set(PIOA, REMOTE_DR); RS485DirIs = 1;}
}

void running_pole_indicate(char pole)
{
  if (pole == 0) 
  {
    PLUSlamp(ON);
    extout_onoff(EXTOUT_REVERSE, OFF);
  }
  else
  {
    PLUSlamp(OFF);
    extout_onoff(EXTOUT_REVERSE, ON);
  }
}

void pio_init(void)
//configure the PIO Lines
{
// First, enable the clock of the PIO
  AT91F_PMC_EnablePeriphClock ( AT91C_BASE_PMC, 1 << AT91C_ID_PIOA ) ;
  AT91F_PMC_EnablePeriphClock ( AT91C_BASE_PMC, 1 << AT91C_ID_PIOB ) ;

  pio_is_in( PIOA, AT91A_IN_MASK ) ;
  pio_is_out( PIOA, AT91A_OUT_MASK ) ;
  //pio_open( PIOA, PROFI_Tx|PROFI_RST|DC24_RDY|PM_CE|PM_DIN|PM_CLK); 
  pio_open( PIOA, PROFI_RST|DC24_RDY|PM_CE|PM_DIN|PM_CLK); 
 //pio_open( PIOA, DC24_RDY);   	
  pio_pullup(PIOA, PM_CE|PM_DIN|PM_CLK|FND_CK|FND_DT|FND_LD|EXT_E3|AD_RST|AD_CS|DA_LDAC|DA_DOUT|DA_CK|DA_CLR); 
  pio_set( PIOA, FND_CK|FND_DT|FND_LD|AD_CS|DA_LDAC|EXT_E3|DA_DOUT|DA_CK|DA_SYNC|DC24_RDY);
  pio_clear( PIOA, BUZZER|AD_RST|PROFI_RST|PM_CLK );

//AT91B_OUT_MASK    (LCD_DATA|LCD_LP|LCD_RST|LCD_A0|LCD_CS1|LCD_CS2|LCD_RW|LCD_EN|EXT_E1|EXT_E2|EXT_E3|EXT_E4|LP_RUN)
  pio_is_in( PIOB, AT91B_IN_MASK ) ;
  pio_is_out( PIOB, AT91B_OUT_MASK ) ;
  pio_is_direct( PIOB, EXT_DATA );
  
  pio_open( PIOB, LCD_RST|LP_PLUS|LP_START|LP_RUN ); 
  //pio_open( PIOB, LP_PLUS|LP_START|LP_RUN );  
  //pio_pullup( PIOB, LCD_DATA|LCD_A0|LCD_CS1|LCD_CS2|LCD_RW|LCD_EN|LCD_RST|LP_RUN );   	
  pio_pullup( PIOB, EXT_DATA||EXT_E1|EXT_E2|RTC_CK|RTC_DT|RTC_RST ); 
  pio_set( PIOB, LCD_CS1|LCD_CS2|LCD_RW|LCD_EN|EXT_E1|EXT_E2|RTC_CK|RTC_RST|LP_RUN);
  pio_clear( PIOB, LCD_DATA|LCD_RST|LCD_A0|LCD_LP ); 
}        

/******************************************/
/*    EVENT 발생시 램프 점등으로 표시     */
/*    LampOnTime 동안 점등을 유지         */
/* This function must locate in main loop */
/******************************************/
//#define EXT_BUZZ_TIME   SEC_1 * 3
//short sExtBuzzOnTime;
unsigned short sBuzzOnTime;
unsigned short sLcdOnTime;
unsigned short sLampOnTime;
unsigned short usScanCount;
char PidStable0;
void event_indicating(void)
{
 //if (PushKey|bPullKey)
  if (PushKey)
  { 
    sLampOnTime = SEC_1 >> 2;
    sLcdOnTime = LCD_LIGHT_ON_TIME;
    LCDlamp(ON);
    
    if (!(COUNT_UP)&!(COUNT_DN)) sBuzzOnTime = SEC_1 >> 4 ;
    BUZZERonoff(ON);
  }

// 제어가 안정화 되면 알림  
  if ((PidStable)&(!PidStable0))
  {
    sBuzzOnTime = SEC_1 >> 2;
    BUZZERonoff(ON);
  }
  PidStable0 = PidStable;   
 
  if (sLampOnTime) sLampOnTime--; else RUNlamp(OFF);
  if (sBuzzOnTime) sBuzzOnTime--; else BUZZERonoff(OFF);
  
  // 2009-01-09 구수 하과장과 협의
  // 리모트에서 시스템 작동을 확인하기 위해 매 Scan마다 카운터값 inc
  usScanCount++;
}

/*****************************************/
/*    EVENT 발생 변수를 클리어           */
/*****************************************/
void event_clear(void)
{
  PushKey = 0;
  PullKey = 0;
  //ModemCmd = 0;
  //MsgPC = 0;
}

/**********************************************/
/* Calculate execute time current main loop   */
/* If ExecTime is biggest, update MaxExecTime */
/* This function must locate in main loop     */ 
/**********************************************/
int iCounterT0;
int iExecTime;
int iMaxExecTime;
void exec_time_check(void)
{
  //int time;
  AT91PS_TC TC_pt = AT91C_BASE_TC0; 
  iCounterT0 = TC_pt->TC_CV;
  iCounterT0 = iCounterT0 + ((Scan2ms - 1) * CLOCK_2MS);
  Scan2ms--;// = 0;
  iExecTime = iCounterT0 * 1000;
  iExecTime = iExecTime / CLOCK_1MS;  // us
  //time = iCounterT0 * 1000;
  //time = time / CLOCK_1MS;  // us
  //if (time > iExecTime) iExecTime = time;
  //else if (iExecTime > time) iExecTime--;
  if (iExecTime > iMaxExecTime) iMaxExecTime = iExecTime;
  else iMaxExecTime--;  
}

void print_ExecTime(void)
{
   printf("\nExecTime:%4duS", iExecTime);
}

void print_MaxExecTime(void)
{
   printf("\nMaxExecTime:%4duS", iMaxExecTime);
   iMaxExecTime = iExecTime+1;
}

void exec_time_display(void)
{
  iExecTime = iCounterT0 * 1000;
  iExecTime = iExecTime / CLOCK_1MS;  // us
  if (iExecTime > iMaxExecTime) print_MaxExecTime();
}

void execmode_change(char mode)
{
   sExecStep = 0;
   PushKey = 0;
   ExecMode = mode;
}

void printf_volt(float val)
{
  if (val > 9999) printf("%5.0f", val);
  else if (val > 999) printf("%4.0f ", val);
  else if (val > 99.9) printf("%3.0f ", val);
  else if (val > 9.99) printf("%2.1f", val);
  else  printf("%1.2f", val);
}

void float_jog_plus(float ref)
{
  float finc;
  if (JogSpeed > 20) JogSpeed = 10;
  else if (JogSpeed > 10) JogSpeed -= 8;
  else JogSpeed = 1;
  
  if (fTempSet < 10) finc = JogSpeed * 0.01;
  else if (fTempSet < 100) finc = JogSpeed * 0.1;
  else finc = JogSpeed;
  if (fTempSet < ref) fTempSet+= finc;
  if (fTempSet > ref) fTempSet = ref;
}

void float_jog_minus(float ref)
{
  float finc;
  if (JogSpeed > 20) JogSpeed = 10;
  else if (JogSpeed > 10) JogSpeed -= 8;
  else JogSpeed = 1;
  
  if (fTempSet < 10) finc = JogSpeed * 0.01;
  else if (fTempSet < 100) finc = JogSpeed * 0.1;
  else finc = JogSpeed;
  if (fTempSet > ref) fTempSet -= finc;
  if (fTempSet < ref) fTempSet = ref;
}

unsigned char ExecMode, ExecMode0;
unsigned short sExecStep;
unsigned short sOldStep;
short sStoreStep0;
unsigned short sExecDelay;
short sSVrxStep0;
/*******************************************/
/*              Execute monit              */
/* 실행모드와 실행스텝의 변화를 검출       */
/* This function must locate in main loop  */ 
/*******************************************/

void exec_step_monit(void)
{
  if ((sOldStep != sExecStep)|(ExecMode0 != ExecMode)) 
  {
    printf("\nEx%02d S:%4d", ExecMode, sExecStep);
    sOldStep =  sExecStep;
    ExecMode0 = ExecMode;
  }
}

void store_step_monit(void)
{
  if (sStoreStep != sStoreStep0)
  {
    DataMonit = 1;
    sStoreStep0 = sStoreStep;
    printf("Fs:%d ", sStoreStep);
  }
}

void execstep_monit(void)
{
  char mode;
    mode = DebugMonit;
    //if (Com2Mode) debug_monit(MONOUT);
    //else debug_monit(COM2OUT);
    debug_monit(COM2OUT);
    //sever_step_monit();
    exec_step_monit();
    //store_step_monit();
    debug_monit(mode);
}

// 매 1초마다 실행되는 함수
short sOneSecEventTime;
short sSecretPassTime;
short sSystemLiveTime;
void execute_per_sec(void)
{
  if (++sOneSecEventTime > SEC_1)
  {
    // 운영자 메뉴 사용시간 제한
    sOneSecEventTime = 0;
    if (sSecretPassTime != 0)  sSecretPassTime--;
    // LCD Back Lignt control
    if (sLcdOnTime) sLcdOnTime--; else LCDlamp(OFF);
    RemoteScanSpeed = RemoteScanNo;
    RemoteScanNo = 0;
   }

  // 2009-01-30 구수 하영상과장 요청으로 추가
  if (++sSystemLiveTime > SYSTEM_LIVE_TIME)
  {
    sSystemLiveTime = 0;
    if ( SystemLive ) SystemLive = 0; else SystemLive = 1;
  }    
}

//
// USART speed가 지정된 설정값이 아닌경우 default 값으로 정정
//
int usart_baudrate_check(int rate, int base)
{
  if ((rate != 9600)& (rate != 19200)&
      (rate != 38400)&(rate != 57600)& (rate != 115200))
       rate = base;
  return rate;
}
