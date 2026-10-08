
// Include Standard LIB  files
#include "project.h"

/********************************************/
/* key_function() offer user key processing */
/* This function must locate in main loop   */ 
/********************************************/
void key_function(void)
{
 switch (ExecMode) 
 {
 case MAIN_MENU:	MAIN_MENU_function();		return;
 case RUN_STATUS:       operate_status_view();          return;
 case RTC_SET:		RTC_set_function();		return;
 case REMOTE_SET:	remote_test_function();		return;
 case PROFI_SET:	profi_set_function();		return;
 case METER_ADJUST:	meter_adjust_function();        return;
 case GUIDE_MESSAGE:	guide_message_display();	return;
 case SYSTEM_TEST:	system_test_function();		return;
 case ADC_TEST:         adc_test_function();            return;
 case DAC_TEST:         dac_test_function();            return;
 case ADE_TEST:         ade_test_function();            return;
 case ADEIO_TEST:       adeio_test_function();          return;
 case EXTOUT_TEST:	extout_test_function();		return;
 default: sExecStep = 1; ExecMode = MAIN_MENU;		return;
 };
}

unsigned char Test_out;
void pm_port_test(void)
{
  pio_set(PIOA,Test_out<<12);
  pio_clear(PIOA,(Test_out ^ 0xFF)<<12);
  PMreset(TOGLE);
  Test_out++;
}

AT91PS_WDTC WDT_BASE;
unsigned int	WdtCount;
#define	WDV		0x0FFF		// 12bit WDT counter
#define	WDFIEN		1<<12		// WDT int enable
#define	WDRSTEN		1<<13		// WDT reset enable
#define	WDRPROC		1<<14		// 1:WDT All reset 0:processor reset
#define	WDDIS		1<<15		// 0: WDT enable 1: disable
#define	WDDBGHLT	1<<28		// When debug state, 0: WDT run 1: stop  
#define	WDIDLEHLT	1<<29		// When idle mode, 0: WDT run 1: stop  
#define	watchdog_disable()	AT91C_BASE_WDTC->WDTC_WDMR = AT91C_WDTC_WDDIS;
#define	watchdog_reset()	AT91C_BASE_WDTC->WDTC_WDCR  = 0xA5000001;

//**********************************************************************

void watchdog_enable(int count)
{
  unsigned int wdtmode;
  //Enable Peripheral clock in PMC for  WDTC
  AT91F_WDTC_CfgPMC();			
  
  count = AT91F_WDTGetPeriod(count);  // mS : 유일하게 제대로 사용 가능 
  wdtmode = count<<16|WDRSTEN|count;
  uiWdtModeSet = wdtmode;
  AT91C_BASE_WDTC->WDTC_WDMR = wdtmode;
}

/*****************************************/
/*    전원투입후 각종 data값을 초기화    */
/*****************************************/
void data_init(void)
{
  Scan2ms = 0;
  sLampOnTime = 0; 
  RUNlamp(OFF);
  display_mode(0);
  debug_monit(MONOUT);
  ImageMode = 0;
  ImageNo = 19;
  DumpDisplay = 0;
  KeyStep = 0;
  
  DelayStep = 0;
  Hour0 = Hour ^ 0xFF;
  sRtcTime = SEC_1; 
  
  sStorePage = 0;
  sStoreStep = 1;
  DataMonit = 1;
  
  CursorUse = 1;
  CodeOdd = 0;
  sOldCode = 0;
  DpUpdate = 1;
  
  LCDpage = 0;
  ExecMode = GUIDE_MESSAGE;
  sExecStep = 0;
  sOldStep = 0;
  
  FndTestStep = 0;
  sOutVolt = 218;
  
  AdcStep = 0;
  AdcDebug = 0;
  sConStep = 0;
  PidStable = 0;
  
  EmegStop = EmegStop0;
  
  //RemoteRun = OFF;
  system_stop();
  RunHour = 0;
  RunMinute = 0; 
  RunSec = 0;
  
  ExtOutBuf = 0;
  
  iAmpInput0 = iAmpInput + 1;
  iVoltInput0 = iVoltInput + 1;
  
  //ViewPage = 0;
  
  ExtOutTestMode= 0;
  RemoteReady = OFF;
  
  sOneSecEventTime = 110;
  sSecretPassTime = 0;
  
  all_error_reset();
  resetcount=0;
  STARTlamp(OFF);
  ProfiDebug = 0;
  ProfiTestOut = 0;
  
  // 2009-01-09 추가
  DacTestMode = 0;
  // 2009-02-20 추가
  iPoleTurnWait = 0;
}

//*----------------------------------------------------------------------------
//* Function Name       : main
//* Object              : Main function
//*----------------------------------------------------------------------------
int main( void )
//* Begin
{
//        short loop;
//unsigned short daval;
//	short time;
//	char no;
//        AT91PS_AIC     pAic;
   
    //watchdog_disable();
    //* Load System pAic Base address
    //  pAic = AT91C_BASE_AIC;
 
    // 2026-10-08 추가: 리셋 원인(RSTC_SR)과 리셋 직전 상태(__no_init RAM)를 다른 초기화보다 먼저 확보,
    //   웜 리스타트 여부 판정 (CSLab_Rectifier_reset.c)
       reset_capture();
    // 2026-10-08 추가: 정지 후 재입력 대기 상태 (전원 투입이면 해제, 그 밖의 리셋은 유지)
       remote_rearm_boot();

    //* Enable User Reset and set its minimal assertion to 960 us
       AT91C_BASE_RSTC->RSTC_RMR = AT91C_RSTC_URSTEN | (0x4<<8) | (unsigned int)(0xA5<<24);

       
    //* Init PIO 
       pio_init();
    // 2026-10-08 추가: 웜 리스타트면 DAC 코드/출력 래치를 바로 복원하고 출력 허가 (가능한 빨리 출력 복귀)
       warm_restart_early();

    //AT91F_AIC_ConfigureIt ( pAic, AT91C_ID_PIOA, PIO_INTERRUPT_LEVEL,AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL, pio_c_irq_handler);
    //* set the interrupt by software
    //	AT91F_AIC_EnableIt (pAic, AT91C_ID_PIOA);

    //* Init timer interrupt
        timer_init();	
	etc_int_init();

    // Flash mode init
	AT91F_Flash_Init();
    // Init LCD display GM123210 & Start Massage display
    	LCD_init();
    // data init
	data_init(); 	
    	backup_data_read();
        meter_data_read();
    // 2026-10-08 추가: 웜 리스타트면 운전 상태/출력 제어 상태 복원 (설정값 FLASH 로드 후)
       warm_restart_late();
        
    //* Init Usart
        //iCom0Speed = DEFAULT_SPEED_COM0;
        //iCom1Speed = DEFAULT_SPEED_COM1;
        iCom0Speed = usart_baudrate_check(iCom0Speed, DEFAULT_SPEED_COM0);
        iCom1Speed = usart_baudrate_check(iCom1Speed, DEFAULT_SPEED_COM1);
#ifdef DBG_COM1_SPEED
        iCom1Speed = DBG_COM1_SPEED;   // 2026-10-07 추가: UART 디버그용 속도 강제 (CSLab_Rectifier_Main.h)
#endif
        Usart_init();
        Usart1_init();       
        //Usart2_init(); // IO포트로 사용하므로 사용할수 없음
        com0_mode_set( iCom0Speed, 'n', 8, 1);
        com1_mode_set( iCom1Speed, 'n', 8, 1);        
        dbg_init();                    // UART debug: REMOTE_Tx(PA6), COM1 iCom1Speed(DBG_COM1_SPEED 115200) 8N1
        //com2_mode_set(iCom2Speed);
        
    // Init real time clock
	rtc_initialize();      

    // 2026-10-08 추가: 이번 부팅의 리셋 원인 보고 [RST] (UART, RTC 초기화 후)
       reset_report_boot();
        
    // watchdog timer start
       watchdog_enable(200);
    
for (;;)

    {
	if (Scan2ms)
        {
	  /**********************************************/
	  /*                  Main Loop                 */
	  /* All function in loop executed per ScanTime */
	  /* All function must programmed callback type */
	  /* NOTE: Recommand not use delay(>1ms)        */ 
	  /* and execute time is short.                 */
	  /**********************************************/
          RUNlamp(ON);
          // 2026-10-08 추가: 리셋 직전 상태 갱신 + 실행 위치 표시(stage)
          //   워치독 리셋이면 마지막 stage = 멈춘 함수 (번호는 아래 각 줄, 99 = 다음 스캔 대기)
          reset_ctx_update();
          reset_mark(1);
          ext_in_read();
          reset_mark(2);
          panel_key_scan();
#ifdef DBG_KEYLOG
          DbgPanelKey = PushKey;         // 2026-10-08 추가: 패널 키 이벤트 (운전 판단 전에 바뀌면 [KEY] lost)
#endif
          reset_mark(3);
          rotary_key_scan();
          reset_mark(4);
          scroll_key_generate();
          //ext_remote_scan();
          reset_mark(5);
          key_function();
          reset_mark(6);
          AnyBusDP_transive();
          reset_mark(7);
          operate_by_local_data();
          reset_mark(8);
          modbus_message_exchange();
         //if (ExecDebug) execstep_monit();
          reset_mark(9);
	  AV_measure_ADE7758();
          reset_mark(10);
          extin_ararm_deside();
          reset_mark(11);
          system_operate();
          
          reset_mark(12);
          adc_read_ad7705();
          reset_mark(13);
          sam_adc_converting();
          reset_mark(14);
          if ((ExecMode != ADE_TEST)&(ExecMode != ADEIO_TEST)&(ExecMode != ADC_TEST))
            output_level_control_SCR(); 
 
          reset_mark(15);
          extout_ararm_deside();
          reset_mark(16);
          running_pole_indicate(OperPole);
          reset_mark(17);
          ext_out_update();
          reset_mark(18);
          date_time_update();
          reset_mark(19);
          execute_per_sec();
          reset_mark(20);
          remote_live_check();
          reset_mark(21);
          all_FND_test();
          reset_mark(22);
          FND_display_update();
          //FND_display_exectime();
          reset_mark(23);
	  display_scan();
          reset_mark(24);
	  event_indicating();
          reset_mark(25);
          event_clear();
          //exec_time_display();
          reset_mark(26);
          exec_time_check();          
          dbg_status_log();              // UART debug status line (CSV)
          reset_scan_tick();             // 2026-10-08 추가: [RST] 웜 복귀 결과/반복 보고, 시험용 UART 명령
#ifdef DBG_PLCCMD
          plc_cmd_report();              // 2026-10-08 추가: PLC 명령이 바뀌면 [PLCCMD] 출력
#endif
	  watchdog_reset();
          RUNlamp(OFF);
          reset_mark(99);               // 2026-10-08 추가: 다음 스캔(Scan2ms) 대기
        }
    }
}
