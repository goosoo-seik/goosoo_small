
// Include Standard LIB  files

// Prototypes in main.c
extern void printf_PowerOnMsg(void);
extern void AT91F_LowLevelInit(void);

// Prototypes in ILMAC_Rectifier_menu.c
extern char Answer;
extern char ViewPage;
extern short sSysTestTime;
extern void menu_display_main(char no);
extern void MAIN_MENU_function(void);
extern void menu_range_set(char start, char end, char size);

// Prototypes in ILMAC_Rectifier_main.c
extern char LocalMode, LocalPole;
extern char RemotMode, RemotPole;
extern char RemoteReady;
extern char RemoteRun;
extern char ReadyStop;
extern char SystemRun;
//extern char TotalAlarm;
extern char proTalarm;
extern char TotalError;
extern char TotalVcsTrip;
extern char ExtManualOp;
extern char AcLowFault;
extern char AcLowAlarm;
extern int iRiseTime;
extern char MaxHour, MaxMinute, MaxSec;
extern char RunHour, RunMinute, RunSec;
extern char Sec0;
extern int iRunTime;
extern int iMaxRunTime;
extern int iSoftTime;
extern int iReactRate;
extern int iMaxOperAmp;
extern int iMaxOperVolt;
extern int iAcOverAmp;
extern int iOperAmp;
extern int iRevOperAmp;
extern int iOperVolt;
extern int iRevOperVolt;
extern int iMaxOverAmp;
extern int iMaxOverVolt;
extern int iPoleTurnTime;
extern int iPoleTurnWait;
extern int resetcount;
extern float frate, fa;
extern float fTolerance;
extern float fTargetAmp;
extern float fTargetVolt;
extern float fRefAmp;
extern float fRefVolt;
extern float fDifferent; 
extern float fProport;
extern float fIntegral;
extern float fOutAmpOld;
extern float fOutVoltOld;
extern float fAmpInOld;
extern float fVoltInOld;
extern float fToleranceRange;

extern float fMaxOverAmp;
extern float fMaxOperAmp;
extern float fOperAmp;
extern float fRevOperAmp;
extern float fMaxOverVolt;
extern float fMaxOperVolt;
extern float fOperVolt;
extern float fRevOperVolt;
extern float fTempSet;
extern char SystemRun;
extern char OperMode;
extern char OperPole;
extern char OperUser;
extern char OperStop;
extern char OpStep;
// Error Events
extern char ExtError1;
extern char ExtError2;
extern char ExtError3;
extern char ExtError4;
extern char ExtError5;
extern char ExtError6;
extern char ExtAlarm1;
extern char ExtAlarm2;
extern char ExtAlarm3;
extern char ExtAlarm4;
extern char ExtAlarm5;
extern char ExtAlarm6;
extern char ExtAlarm7;
extern char ExtAlarm8;
extern char  OpError;
extern char  EmegError;
extern char  TotalError;
extern char  TotalAlarm;
extern char  TotalVcsTrip;
extern char  ErrorStop;
extern char  ReadyStop;
extern char  AmpOverErr;
extern char  VoltOverErr;
extern char  AcLowFault;
extern char  AcLowAlarm;
extern char  DcOverErr;
extern char  AdcError;
extern char  DacError;
extern char  BootError;
extern char  PhaseError;
extern char  RemoteError;
extern char  LineEmegErr;
//extern char  ExtFuseErr;
//extern char  ExtVcsErr;
extern char  ExtManualOp;
//extern char  ExtScrErr;
//extern char  ExtTrErr;
//extern char  ExtTempErr;
//extern char  ExtWaterErr;
extern char  ExtRemoteRun;
//extern char  ExtCoolerErr;
extern char RunComplete;
extern char DebugMode;
extern void system_start(void);
extern void system_stop(void);
extern void operate_status_view(void);
extern void system_operate(void);
extern void guide_message_display(void);
extern void extin_ararm_deside(void);
extern void extout_ararm_deside(void);
extern void event_status_view(void);
extern void all_error_reset(void);

// Prototypes in ILMAC_Rectifier_LCD.c
//extern unsigned short sDPbuf[DPBUF_SIZE];
extern BYTE  bDpStep;
extern short sCurPos;
extern BYTE  pDpPos;
extern char  FontStyle;
extern char  DebugDP;
extern char  DumpDisplay; // 1: if col = 0, space is not display
extern char  Blink;
extern char  LineBlink;
extern short sBlinkTime;
extern char  DpUpdate;
extern char  CursorUse;
extern char  ImageMode;
extern char  ImageNo;
extern char  CodeOdd;
extern char  LCDpage;
extern unsigned char ExecMode;
extern unsigned short sExecStep;
extern unsigned short sOldStep;
extern unsigned short sExecDelay;
extern unsigned short sOldCode;
extern const unsigned char PowerOnMsg[];
extern void putchar_dp(unsigned short code);
extern unsigned char display_size(void);
extern void goto_cursor(unsigned char x,unsigned char y);
extern void putchar_pos(short pos, unsigned char code);
extern char find_cursor_vpos(void);
extern void set_cursor_h(unsigned char x);
extern void cursor_move_up(void);
extern void cursor_move_down(void);
extern void cursor_move_right(void);
extern void cursor_move_left(void);
extern void cursor_move_home(void);
extern void cursor_move_start(void);
extern void LCD_line_feed(void);
extern void LCD_carrige_return(void);
extern void display_buffer_scroll(char size);
extern void cursor_blink_control(void);
extern void DPbuf_clear(void);
extern void screen_clear(void);
extern void LCD_init(void);
extern void data_init(void);
extern void display_mode(char mode);
extern void print_byte2ascii(unsigned char code);
extern void print_short2ascii(short code);
extern void print_int2bcdascii(int hex);
extern void display_scan(void);
extern void LCD_fill(char data);
extern void LCD_test(void);
//extern void display_scan(void);
extern void LCD_GM12321_scan(void);
extern void control_code_display(void);
extern void __write(void);
extern void DPtype_clear(void);
extern void dp_type_assign(char type, short start, short end);
extern void cursor_block(char no);

// Prototype in ILMAC_Rectifier_lib.c
//extern unsigned short HanBuf[100];
//extern unsigned char HanIndex;
extern unsigned char RunLampIs;
extern unsigned char LcdLampIs;
extern unsigned char BuzzerIs;
extern unsigned char ModemPowerIs;
extern void MODEMrts(BYTE onoff);
extern unsigned char PMresetIs;
extern unsigned char PMenableIs;
extern unsigned char PMdataoutIs;
extern unsigned char PMclockIs;
extern unsigned short sLampOnTime;
extern unsigned short sLcdOnTime;
extern unsigned short sBuzzOnTime;
extern char DAclearIs;
extern char LEDno;
extern int iExecTime;
extern int iMaxExecTime;
extern char DebugMonit;
extern char DelayStep;
extern char ExtIn[16];
extern char ViewStep;
extern unsigned short usExtInBuf;
extern unsigned short ExtOutBuf;

extern unsigned char PlusLampIs;
extern void PLUSlamp(BYTE onoff);
extern unsigned char StartLampIs;
extern void STARTlamp(BYTE onoff);
extern void running_pole_indicate(char pole);

extern int putchar(int c);    // using _CSTD putchar;
extern void putstring1(const unsigned char* ptr);
extern void putstring_modem(unsigned char* ptr);
extern void print_str(const unsigned char* ptr);
extern void debug_monit(char onoff);
extern __ramfunc void delay_us (int time );
extern char step_delay(int time);
extern void RUNlamp(BYTE onoff);
void BUZZERonoff(BYTE onoff);
extern void DC24power(BYTE onoff);
extern void PMreset(BYTE onoff);
extern void PMenable(BYTE onoff);
extern void PMclock(BYTE onoff);
extern void PMdataout(BYTE onoff);
extern void DAclear(BYTE onoff);
extern char ProfiResetIs;
extern void profi_reset(BYTE onoff);
extern void rs485_direction(BYTE dir);
extern void MODEMpower(BYTE onoff);
extern void LCDlamp(BYTE onoff);
extern char ascii2hex(char asc) ;
extern char ascii2byte(char high, char low);
extern unsigned short ascii2short(char d3, char d2, char d1, char d0);
extern char high2ascii(char byte);
extern char low2ascii(char byte);
extern unsigned char bcd2hex(unsigned char bcd);
extern int bcdtoint(char high, char low);
extern int bcd2hex_int(int bcd);
extern unsigned char bcd2hex(unsigned char bcd);
extern unsigned char hex2bcd(unsigned char hex);
extern int hex2bcd5(int hex);
extern unsigned short hex2bcd3(short hex);
extern void event_indicating(void);
extern void event_clear(void);
extern void pio_init(void);
extern void led_onoff(int no, int on);
extern void delay ( void );
extern void delay_us (int time );
extern void delay_nop (short time );
extern void exec_time_check(void);
extern void print_MaxExecTime(void);
extern void print_ExecTime(void);
extern void exec_time_display(void);
extern void execmode_change(char mode);
extern void printf_volt(float val);
extern void float_jog_plus(float ref);
extern void float_jog_minus(float ref);
extern void extout_onoff(char no, char onoff);
extern short sOneSecEventTime;
extern short sSecretPassTime;
extern void execute_per_sec(void);
extern int  usart_baudrate_check(int rate, int base);
extern unsigned short usScanCount;

// Prototype in interrupt_xxx.c
extern int iCom0Speed;
extern int iCom1Speed;
extern int iCom2Speed;
extern BYTE Scan2ms;
extern int count_timer0_interrupt;
extern int count_timer1_interrupt;
extern int count_timer2_interrupt;
extern short rx_wr_index0,rx_rd_index0,rx_counter0;
extern short rx_wr_index1,rx_rd_index1,rx_counter1;
extern short rx_wr_index2,rx_rd_index2,rx_counter2;
extern char ErrUsart;
extern char ErrUsart1;
extern char ErrUsart2;
extern void timer_init (void);
extern void Usart_init(void);
extern void Usart1_init(void);
extern void dbg_init(void);
extern char dbg_reserve(short n);
extern void dbg_putc(char c);
extern void dbg_str(const char *s);
extern void dbg_uint(unsigned int v);
extern void dbg_int(int v);
extern char dbg_puts(const char *s);
extern int  dbg_printf(const char *fmt, ...);
extern void dbg_status_log(void);
// 2026-10-08 추가: 칸 단위 줄 출력 (가변 인자 없음 → 스택 일정, CSLab_SAM7_Usart1.c)
extern void dbg_lb_begin(const char *head);
extern void dbg_lb_txt(const char *s);
extern void dbg_lb_u(unsigned int v);
extern void dbg_lb_i(int v);
extern void dbg_lb_f(float v, char prec);
extern char dbg_lb_end(void);
// 2026-10-08 추가: 리셋 원인 보고 / 웜 리스타트 (CSLab_Rectifier_reset.c)
extern void reset_capture(void);
extern void warm_restart_early(void);
extern void warm_restart_late(void);
extern void reset_report_boot(void);
extern void reset_ctx_update(void);
extern void reset_mark(unsigned char stage);
extern void reset_scan_tick(void);
extern char WarmStart;
extern unsigned short WarmHoldScan;
extern unsigned short WarmSpHoldScan;
extern unsigned int uiRstScan;          // 메인루프 시작 후 스캔 수 (2ms)
// 2026-10-08 추가: PLC 명령 상태 UART 보고 (CSLab_Rectifier_profi.c)
extern void plc_cmd_report(void);
extern void plc_cmd_force(void);
// 2026-10-08 추가: 리모트 재기동 조건 (CSLab_Rectifier_profi.c)
extern char RemArmWait;
extern void remote_rearm_set(void);
extern void remote_rearm_boot(void);
extern char DbgPanelKey;                // 2026-10-08 추가: DBG_KEYLOG (CSLab_Rectifier_main.c)
// 2026-10-08 추가: 측정 노이즈 통계 / UART 튜닝 (CSLab_Rectifier_ADC.c)
typedef struct
{
  int n;                  // 창 안 ADC 샘플 수
  float amin, amax;       // 필터 전 원시 전류 최소/최대 [A]
  float anz, vnz;         // 노이즈 표준편차 [A] [V]
  unsigned short spka, spkv;  // 스파이크 대체 수
  float res;              // |fResult| 최소 (-1 = PID 구간 아님)
} ADC_NOISE;
extern void adc_noise_take(ADC_NOISE *o);
extern float fAmpAvrInput, fVoltAvrInput;
extern void adc_tune_print(void);
extern char adc_tune_command(const char *s, char len);
// 2026-10-08 추가: 웜 리스타트 복원 (CSLab_Rectifier_ADC.c / CSLab_Rectifier_main.c)
extern int iDacCodeAmp, iDacCodeVolt;
extern void ctrl_warm_resume(float outamp, float outvolt, float ampin, float voltin, int dacamp, int dacvolt);
extern void op_warm_resume(char user, char mode, char pole, int runtime);
extern void Usart2_init(void);
extern char getchar0(void);
extern char getchar1(void);
extern char getchar2(void);
extern __ramfunc void putchar0(char c);
extern void putchar1(char c);
extern __ramfunc void putchar2(char c);
extern void AT91F_US_Put( char *buffer); // \arg pointer to a string ending by \0
extern void com0_mode_set( int rate, char parity, char bit, char stop);
extern void com1_mode_set( int rate, char parity, char bit, char stop);
extern void com2_mode_set( int rate);
extern short CheckTx2Space(void);
extern void com0_buffer_clear(void);
extern void com1_buffer_clear(void);
extern void com2_buffer_clear(void);

// Prototype in ILMAC_Rectifier_key.c
//extern char  KeyCount[10];
extern char  EmegStop, EmegStop0; 
extern unsigned char KeyStatus;
extern char PushKey, PullKey;
extern char KeyDigit;
extern char KeyStep;
extern unsigned int KeyValue;
extern unsigned char JogFw, JogRev, JogTime, JogSpeed;
extern char MenuNo, MenuStart, MenuEnd, MenuSize;
extern void panel_key_scan(void);
extern void touch_status_print(void);
extern void ext_remote_scan(void);
extern void rotary_key_scan(void);
extern void scroll_key_generate(void);
extern void rotary_key_test(void);
extern char wheel_input(int min, int max);
extern char wheel_input_speed(int min, int max, char speed);
extern void running_wheel_input(int min, int max);
extern void keyin_start(char digit);
extern char key_input_hex(void);
extern char key_input_bcd(void);
extern char keyin_dp_hex(void);
extern char keyin_dp_bcd(void);
extern void popup_menu_update(char menu, char dir);

// Prototype in ILMAC_Rectifier.c
extern const unsigned char*	pImagePtr[];
extern const unsigned char ASCII_FONT6x7[128][6];
extern const unsigned char ASCII_FONT8x16[128][16];
extern const unsigned short HANGUL_FONT16x16[][16];

// Prototype in ext.irq.c
extern void FIQ_init_handler(void);
extern void pio_c_irq_handler (void);
extern void aic_software_interrupt(void);
extern void sys_c_irq_handler(void);
extern void etc_int_init ( void );

// Prototype in ILMAC_Rectifier_profi.c
extern char SystemLive; // 시스템 작동 알림 플래그
extern short RemoteStep;
extern char ProfiTestOut;
extern short sLineSpeed;
extern char ProfiFind;
extern char ProfiDebug;
extern char UnExecuteCMD;
extern char RemoteStatus;
extern char RemoteScanNo;
extern char RemoteScanSpeed;
extern void profi_set_function(void);
extern void menu_display_profi(char no);
extern void modbus_message_exchange(void);
extern void AnyBusDP_transive(void);
extern void remote_status_display(void);
extern void operate_by_local_data(void);

// 2009/03/26 Remote 4 word 추가
extern char RemoteLive, RemoteLive0, RemoteLiveError;
extern void remote_live_check(void);

// Prototype in ILMAC_Rectifier_remote.c
extern void remote_test_function(void);
extern void menu_display_remote(char no);

// Prototype in ILMAC_Rectifier_debug.c
extern char  DebugMode;
extern char  DebugFuncNo;
extern char  AutoZigBee;
extern int iTempSet;
extern void debug_key_function(char key);
extern void system_test_function(void);
extern void menu_display_test(char no);


// Prototype in ILMAC_Rectifier_DS1302.c
extern uchar Year, Month, Date, Hour, Minute, Sec, Day;
//extern void	I2C_start();
//extern void	I2C_stop();
//extern void	I2C_write(unsigned char d);
//extern uchar	I2C_read(uchar);
extern void 	DS1302_writebyte(uchar Add, uchar Data);
extern uchar	DS1302_readbyte(uchar Add);
extern char Hour0;
extern short sRtcTime; 
extern void rtc_time_read(void);
//extern void rtc_date_read(void);
//extern void rtc_clock_read(void);
extern void rtc_initialize();
extern void rtc_init_set();
extern void initialize();
extern void rtc_time_set(void);
extern void rtc_clock_set(void);
extern void rtc_date_set(void);
extern void RTC_test_function(void);
extern void RTC_set_function(void);
extern void date_time_update(void);
extern void menu_display_rtc_set(char no);
extern void menu_display_rtc_test(char no);

// Prototype in ILMAC_Rectifier_ADE7758.c
extern char AcOverErr;
extern char AcLowVoltErr;
extern char AcOverCount;
extern char PwMeterError;
extern char MeterSumErr;
extern char AdjEnable;
extern char MeasureStep;
extern int iAcLowVolt;
extern int iIa, iIb, iIc;
extern int iIa1, iIb1, iIc1;
extern int iVrs, iVst, iVtr;
extern int iPw, iPvar, iPva;
extern int iVrms, iVrms1, iVrms0;
extern int iIrms, iIrms1, iIrms0;
extern int iAMPrms, iAMPrms1, iAMPrms0;
extern int iVOLTrms, iVOLTrms1, iVOLTrms0;
extern int iActivePower, iActivePower1, iActivePower0;
extern int iIrmsDivider, iIrmsOffset; ;
extern int iVrmsDivider, iVrmsOffset;
extern int iActiveDivider, iActiveOffset;
extern int iADEregistor[72];
extern int iPa, iPa1, iPa0;
extern int iPva, iPva1, iPva0;
extern char PanelMode;
extern char ADE7758_init(void);
extern void menu_display_adetest(char no);
extern void ade_test_function(void);
extern void menu_display_adeiotest(char no);
extern void adeio_test_function(void);
extern void power_measure_ADE7753(void);
extern void AV_measure_ADE7758(void);
extern void power_simulate(void);
extern void print_title_korean(void);
extern void print_title_english(void);
extern void power_status_view(void);
extern void POWER_METER_function(void);
extern void meter_adjust_function(void);
extern void menu_display_adjust(char no);
extern void meter_data_read(void);
extern int meter_data_save(void);

// Prototype in ILMAC_Rectifier_FLASH.c
extern char OperSumErr;
extern unsigned int *pFlashPtr;
extern unsigned int uiFlashAdd, uiFlashData;
extern unsigned int uiBackupData[64];
extern short  sBackupAdd;
extern unsigned int uiBackupSum;
extern unsigned int uiResetControl;
extern unsigned int uiResetStatus;
extern unsigned int uiResetMode;
extern unsigned int uiWatchdogMode;
extern unsigned int uiWatchdogStatus;
extern unsigned int uiWdtModeSet;
extern int iSecretCode;
extern unsigned int  iVoltSum, iAmpSum, iActivSum;
extern short sVACount, sActiveCount;;
extern short sStorePage;
extern short sStoreStep;
extern short sStoreTime, sInterVal;
extern char DataMonit;
extern unsigned short SeverTxPage;
extern unsigned int iCurTime, iOldTime;

extern __ramfunc int AT91F_Flash_Write( unsigned int Flash_Address ,int size ,unsigned int * buff, unsigned char Memset);
extern void backup_flash_write(unsigned int data);
extern unsigned int backup_flash_read(unsigned int defaultdata);
extern short find_transmit_start_page(void);
extern unsigned short usTransmitBuf[256];
extern void AT91F_Flash_Init (void);
extern void flash_edit_function(void);
extern void flash_test_function(void);
extern void backup_data_read(void);
extern int backup_data_save(void);
extern void measure_data_backup(void);
extern int get_time_count(uchar hour, uchar min, uchar sec);
extern char reset_status_view(void);
extern void wdt_register_read(void);
extern char reset_status_check(void);
extern void all_flash_lock(void);
extern void unlock_flash_page(int page);
extern void lock_flash_page(int page);

// Prototype in ILMAC_Rectifier_FND.c
extern short sOutVolt;
extern int iOutCurrent;
extern char FndTestStep;
extern void all_FND_test(void);
extern void FND_display_update(void);
extern void FND_display_exectime(void);
extern void putchar_fnd(unsigned short code);

// Prototype in ILMAC_Rectifier_EXTIO.c
extern char ExtOutTestMode;
extern unsigned short TestOutBuf;
extern void extin_display(char no);
extern void extin_display1(char no);
extern void menu_display_extout(char no);
extern void extout_test_function(void);
extern void EXTOUTonoff(char no, char onoff);
extern void ext_in_read(void);
extern void ext_out_update(void);

// Prototype in ILMAC_Rectifier_ADC.c
extern void serieal_out_ad5663(int data);
extern void DAout_ad5663(char cmd, char ch, int val);
extern void control_amp_out(float amp, float max);
extern void control_volt_out(float volt, float max);
extern void menu_display_adctest(char no);
extern void menu_display_dactest(char no);
extern void adc_test_function(void);
extern void dac_test_function(void);
extern char dacout_verify(void);
extern char PidStable;
extern char PidStatus;
extern short sConStep;
extern int iTolerance;
extern int iTargetAmp;
extern int iDifferent; 
extern int iProport;
extern int iIntegral;
extern int iOutAmp0;
extern int iAmpInOld;
extern char ControlDebug;
extern char AdcStep;
extern char AdcReady[2];
extern char AdcDebug;
extern int iAdcVolt[2];
extern int iAdcRead[2];
extern int iAmpInput;
extern int iVoltInput;
extern float fAmpInput;
extern float fVoltInput;
extern float fAmpAvrInput, fVoltAvrInput;
extern short sVoltDiv;
extern int iAmpInput0;
extern int iVoltInput0;
extern int iOperAmp0;
extern int iOperVolt0;
extern int iAdcAvrAmp;
extern int iAdcAvrVolt;
extern int iAmpOffset;
extern int iVoltOffset;
extern float fOutAmp;
extern float fOutVolt;
extern int iDacOutVolt;
extern int iDacOutAmp;
extern int iVoltGain;
extern int iAmpGain;
extern void input_signal_calc(void);
extern void control_signal_out(void);
extern void adc_read_ad7705(void);
extern void output_level_control_SCR(void);
extern void output_level_control_IGBT(void);
extern void control_status_view(void);
extern char DacTestMode;

// Prototype in Dunma_SAM7_ADC.c
extern int iVccVolt;
extern int iDacMonVolt;
extern int iDacMonAmp;
extern int iSamAdcRead[8];
extern char SamAdcStep;
extern void sam_adc_converting(void);
