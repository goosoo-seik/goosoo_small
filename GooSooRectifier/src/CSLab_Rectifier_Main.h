
// Sysyem Lanuage Mode
#define HANGUL
// Program 개발중인 상태 알림
//#define DEVELOPE_MODE 

// System Type
#define MONO_POLE 
#define PROFI_WORD_4  // Remote PLC와의 데이터 교환 길이: 4 word 

//---------------------------------------------------------------
// 2026-10-08 추가: 리셋 원인 보고 / 웜 리스타트  (CSLab_Rectifier_reset.c)
//---------------------------------------------------------------
// [리셋 원인 보고] 항상 동작
//   - 메인루프가 매 스캔 리셋 직전 상태를 __no_init RAM(ResetCtx)에 갱신
//   - 다음 부팅 때 RSTC_SR(리셋 종류) + RAM 의 직전 상태를 UART(COM1)로 출력
//       [RST] cold reset : 원인     또는     [RST] warm reset : 원인
//       [RST] info ... / [RST] prev ...  (상세)
//   - 웜 리스타트 후 WARM_REPORT_SEC 초 뒤 [RST] warm stable : 복귀 결과
// [웜 리스타트] 운전 중 DCS CPU 가 순간 리셋되면 소프트스타트/PLC 재기동 없이 직전 출력으로 복귀
//   조건: 리셋 종류 워치독(2) 또는 NRST(4), RAM 직전 상태 유효, 직전 REMOTE 운전 중,
//         고장 없음, 메인루프 실행 중 리셋(부팅 초기화 중 아님), 연속 횟수 < WARM_MAX_COUNT
//   ※ 리셋 펄스 동안(DS1233 이 늘이면 약 350ms)은 출력 래치 OE(DC24_RDY 풀업)가 꺼져
//     PLC 출력과 DA_EN 이 끊김 -> 끊김을 없애는 기능이 아니라 끊긴 뒤 바로 이어서 운전하는 기능
//   주석 처리하면 리셋 원인 보고만 하고 항상 일반(cold) 부팅
#define WARM_RESTART
#define WARM_MAX_COUNT     3     // 연속 웜 리스타트 최대 횟수 (넘으면 cold 부팅)
#define WARM_CLEAR_SEC     600   // 웜 리스타트 후 10분 계속 운전하면 연속 횟수 0 으로
#define WARM_HOLD_MS       300   // 웜 복귀 직후 측정값이 다시 들어올 때까지 제어 계산 보류 (DAC 는 직전 값 유지)
#define WARM_SP_HOLD_SEC   10    // 웜 복귀 후 PLC 설정전류 0 수신(Anybus 재초기화 중 0 클리어)을 무시하는 최대 시간
#define WARM_REPORT_SEC    3     // 웜 복귀 후 [RST] warm stable 보고 시점
// 웜 복귀 보완 스위치 (보드 시험에서 문제가 되면 해당 줄만 주석 처리 -> 그 보완만 빠짐)
#define WARM_FIX_DACCLR          // (1) pio_init() 전에 DA_CLR(PA14)을 High 로 미리 설정 + 복원 전 DAclear(OFF)
                                 //     없으면 pio_init() 이 DA_CLR 을 Low 로 내보내 AD5663 이 0 으로 클리어되고,
                                 //     Low 동안은 DAC 를 다시 써도 출력이 바뀌지 않음
#define WARM_FIX_INPUT           // (2) 비상정지/선택스위치/외부입력 디바운스를 실제 입력으로 미리 채움
                                 //     없으면 부팅 직후 약 90ms 동안 '비상정지 눌림'으로 판정되어 system_stop()
#define WARM_FIX_PLC_SP0         // (3) 웜 후 WARM_SP_HOLD_SEC 이내 PLC 설정전류 0 수신 무시 (CSLab_Rectifier_profi.c)
                                 //     없으면 Anybus 재초기화 중 0 클리어 데이터로 출력이 0 이 됨
#define WARM_FIX_INOLD           // (4) 제어 미분항의 직전 측정값(fAmpInOld/fVoltInOld) 복원 (CSLab_Rectifier_ADC.c)
                                 //     없으면 0 에서 시작해 첫 계산의 미분항이 측정값 전체만큼 튐
#define WARM_FIX_DACREF          // (5) DAC 출력 확인 기준값(iDacOutAmp/iDacOutVolt) 복원 (CSLab_Rectifier_ADC.c)
                                 //     없으면 dac_error_check() 가 DAC 오류로 오판해 DA_soft_reset() (출력 0)
#define WARM_FIX_HOLD            // (6) 웜 후 WARM_HOLD_MS 동안 제어 계산 보류, DAC 직전 값 유지 (CSLab_Rectifier_ADC.c)
#define RST_REPEAT_SEC     60    // [RST] last 요약을 이 주기로 반복 (뷰어를 늦게 연결해도 확인). 0 이면 안 함
// 시험용 UART 명령 (COM1 RxD1 PA5 로 수신, 줄 끝 CR 또는 LF)
//   RST?  : 마지막 리셋 보고 다시 출력
//   !WDT  : 메인루프를 멈춰 워치독 리셋을 일으킴 (웜 리스타트 시험용)
//   PLC?  : PLC 명령 상태 [PLCCMD] 다시 출력 (뷰어가 연결할 때 자동으로 보냄)
//   현장 납품 펌웨어에서는 주석 처리 권장
#define RST_TEST_CMD

//---------------------------------------------------------------
// 2026-10-08 추가: PLC 명령 상태 UART 보고 [PLCCMD]  (CSLab_Rectifier_profi.c plc_cmd_report)
//   PLC(Profibus/Anybus)에서 받은 명령이 바뀌었을 때만 한 줄 출력 (매 프레임 출력 안 함)
//   [PLCCMD] t=ms com=1 user=REMOTE start=1 stop=0 dir=0 emeg=0 clear=0 live=ok arm=ok set=70000 rev=0 word=0x0001
//   비교 대상: 통신(RemoteReady), LOCAL/REMOTE, 제어워드(생존 비트 0x8000 제외), 설정전류(정/역), 생존 확인, 재입력 대기(arm)
//   주석 처리하면 출력 안 함
//---------------------------------------------------------------
#define DBG_PLCCMD
#define DBG_KEYLOG               // 2026-10-08 추가: 패널 RUN/STOP 키 판정 이유 [KEY], 운전 시작/정지 원인 [RUN] (CSLab_Rectifier_main.c)
                                 //   RUN 키가 무시되는 이유(REMOTE / 운전 중 / 비상정지 / 고장 / 키가 다른 값으로 덮임) 확인용
#define PLCCMD_MIN_GAP_MS  100   // 연속으로 바뀌어도 이 간격보다 자주 보내지 않음 (마지막 상태는 반드시 보냄)

//---------------------------------------------------------------
// 2026-10-08 추가: 리모트 기동/정지, PLC 설정전류 0 처리 (노션 5.3 처리 과정)
//   PLC 생존 확인 : 제어워드 생존 비트(0x8000)가 바뀐 횟수(RemoteLiveToggle)로 판단.
//     Anybus 재초기화 중 0 으로 클리어된 데이터는 생존 비트가 바뀌지 않으므로 '정상 통신'으로 보지 않음
//   각 항목은 주석 처리하면 원래 동작
//---------------------------------------------------------------
#define REMOTE_START_REARM       // (9)(10) 정지 후에는 PLC 시작 비트 0 -> 1 재입력이 있어야 재기동 (CSLab_Rectifier_profi.c)
                                 //   정지 = PLC 정지 비트, 패널 모드 STOP 키. 0 은 생존 비트가 2번 바뀌는 동안 유지되어야 인정
                                 //   전원 투입 후에는 기존처럼 자동 재기동(소프트스타트). 워치독 등 리셋은 정지 상태 유지
#define REARM_FAULT_STOP         //   고장/비상정지로 정지한 뒤에도 재입력 필요 (REMOTE_START_REARM 일 때)
#define REMOTE_SP0_HOLD          // (3)(5)(7)(8) 운전 중 PLC 설정전류 0 은 바로 적용하지 않고 마지막 설정값으로 계속 운전
                                 //   REMOTE_SP0_HOLD_SEC 동안 0 이 유지되고 생존 비트가 2번 이상 바뀌면 조작자 설정으로 보고 적용
                                 //   생존 비트가 끊기면(통신 이상) 계속 보류. UART [SP0] request/accept/cancel 로 뷰어에 알림
#define REMOTE_SP0_HOLD_SEC 10
#define CTRL_FIX_ZERO_SP         // (6)(8) 설정전류 0 이면 amp_out/volt_out(fOutAmp/fOutVolt)도 0 (실제 DAC 출력과 맞춤),
                                 //   다시 설정되면 소프트스타트부터 (CSLab_Rectifier_ADC.c CONTROL_CC+1)

//---------------------------------------------------------------
// 2026-10-08 추가: 측정(AD7705) 노이즈 확인 / 스파이크 필터 / UART 튜닝 (CSLab_Rectifier_ADC.c)
//   주석 처리하면 원래 동작
//---------------------------------------------------------------
#define ADC_NOISE_LOG            // CSV 에 노이즈 열 추가 (0.5초 창): amp_avg amp_min amp_max amp_nz volt_nz spk_a spk_v pid_res adc_n
                                 //   amp_min/max : 창 안 필터 전 원시 샘플 최소/최대 [A]
                                 //   amp_nz/volt_nz : 연속 샘플 차이로 구한 노이즈 표준편차 [A][V] (느린 변화는 거의 안 들어감)
                                 //   spk_a/spk_v : 스파이크로 판정해 대체한 샘플 수, pid_res : 창 안 |fResult| 최소 (-1 = PID 구간 아님)
                                 //   adc_n : 창 안 ADC 샘플 수 (x2 = 샘플/초)
#define ADC_SPIKE_FILTER         // 직전 샘플에서 정격의 spk_pct% 넘게 튄 샘플은 직전 spk_n(3~4)개 평균으로 대체
                                 //   spk_hold 번 넘게 계속 벗어나면 실제 변화로 인정. PidStable == 0 일 때 제어 입력에 적용,
                                 //   spk_buf = 1 이면 24개 이동평균 버퍼에도 대체값을 넣음 (표시/PLC/안정 후 제어도 보호)
#define DBG_SOFT_LOG             // CSV 에 soft[%] (소프트스타트 진행률 iRiseTime/iSoft), pid (PidStatus: 0 램프/사전 구간, 1 PID) 열 추가
#define CTRL_SOFT_PID_RAMP       // 소프트스타트 도중 PID 로 넘어가도 (amp_in 이 설정의 90% 도달, CONTROL_CC+2)
                                 //   PID 목표를 설정값 전체로 바로 올리지 않고, 그때 측정값에서 시작해 소프트스타트 기울기
                                 //   (설정값 / 시동시간)로 올림. 시동시간이 끝나거나 설정값에 닿으면 원래대로. CC 모드만
                                 //   주석 처리하면 원래 동작 (PID 전환 즉시 목표 = 설정값 전체)
#define FIX_REACT_MENU_CMP       // 원본 버그: 시스템 메뉴 '반응감도 설정'에서 바뀌었는지를 iReactRate 가 아닌 iSoftTime 과 비교
                                 //   (값이 시동시간과 같으면 저장 안 됨, 안 바꿔도 저장됨) -> iReactRate 와 비교 (CSLab_Rectifier_debug.c)
#define ADC_TUNE_CMD             // UART 'TUN?' / 'TUN 이름=값' / 'TUN DEF' 로 필터/안정 판정 값 변경 (RAM, 리셋하면 기본값)
                                 //   기본값은 원래 동작과 같음 (avg_n=24 stb_cnt=100 stb_err=0.5 stb_res=1 bump=0)

// System Version Infomation 
#define	VERSION		9	// Version NO  2007-09-22
#define	RELEASE		30	// Release NO  2009-04-02
#define	UPDATE_YEAR     2022 
#define UPDATE_MONTH	9
#define UPDATE_DATE	10

// code save define
#define BYTE          unsigned short
#define	uchar	      unsigned char
#define PIOA          AT91C_BASE_PIOA
#define PIOB          AT91C_BASE_PIOB
#define pio_set       AT91F_PIO_SetOutput 
#define pio_clear     AT91F_PIO_ClearOutput
#define pio_read      AT91F_PIO_GetInput
#define pio_is_in     AT91F_PIO_CfgInput
#define pio_is_out    AT91F_PIO_CfgOutput
#define pio_pullup    AT91F_PIO_CfgPullup
#define pio_open      AT91F_PIO_CfgOpendrain
#define pio_is_direct AT91F_PIO_CfgDirectDrive
#define pio_write     AT91F_PIO_ForceOutput

#define RxUSART0      rx_rd_index0 != rx_wr_index0
#define RxUSART1      rx_rd_index1 != rx_wr_index1
#define RxUSARTD      rx_rd_index2 != rx_wr_index2
#define	DEC_KEY	      (PushKey >= '0')&(PushKey <= '9')
#define	ENTER_KEY     PushKey == KEY_CR
#define	MENU_KEY      PushKey == 'a'
#define	UP_KEY        PushKey == 'b'
#define	DN_KEY        PushKey == 'c'
#define	RUN_KEY       PushKey == 'r'
#define	STOP_KEY      PushKey == 's'
#define	CLEAR_KEY     PushKey == 'l'
//#define	MONIT_KEY     PushKey == 'm'
#define	JOG_KEY       PushKey == 't'
#define	SP_KEY        PushKey == '#'		
#define	PP_KEY        PushKey == '*'
#define ROLL_UP       PushKey == 'u'
#define ROLL_DN       PushKey == 'd'
#define MENU_UP       (PushKey == 'g')|(PushKey == 'b') 
#define MENU_DN       (PushKey == 'h')|(PushKey == 'c') 
#define COUNT_DN      (ROLL_UP)|(MENU_UP)
#define COUNT_UP      (ROLL_DN)|(MENU_DN)
#define EMEG_ON       PushKey == 'x'
#define EMEG_OFF      PushKey == 'y'
#define EXT_RESET     PushKey == 'C'
#define EXT_RUN       PushKey == 'R'
#define	ANY_KEY       PushKey != 0
#define NO_KEY        PushKey == 0  

#define	ifMODEMctrl   if ((ExecMode == MODEM_CONTROL)|(ExecMode == CONNECT_SELECT))

/*--------------*/
/* Clocks       */
/*--------------*/
#define AT91B_MAIN_OSC        	18432000               		// Main Oscillator MAINCK
#define AT91B_MCK             	((18432000*73/14)/2)   		// Output PLL Clock
#define SPEED 			( AT91B_MAIN_OSC /1000)      	// 18432 Hz

// AT91B_MCK = ((18432000*73/14)/2) = 48,054,857
#define CLOCK_1MS     24027      //AT91B_MCK / 2 / 1000     // 24,027
#define CLOCK_2MS     48055      //AT91B_MCK / 2 / 500 + 1  // 48,055

//*   Waiting time between AT91B_LED1 and AT91B_LED2
#define WAIT_TIME       AT91B_MCK
#define SEC_1           500

// Interrupt level
#define PIO_INTERRUPT_LEVEL     6
#define SOFT_INTERRUPT_LEVEL	2
#define FIQ_INTERRUPT_LEVEL     7  // Always high

// IAR compiler ASCii control codes
#define ASC_NUL '\0'  // 0x00 Null
#define ASC_SOH '\1'  // 0x01 Start of Header
#define ASC_STX '\2'  // 0x02 Start of Text
#define ASC_ETX '\3'  // 0x03 End of Text
#define ASC_EOT '\4'  // 0x04 End of Transmission
#define ASC_ENQ '\5'  // 0x05 Enquiry
#define ASC_ACK '\6'  // 0x06 Acknowledgment
#define ASC_BEL '\a'  // 0x07 Bell
#define ASC_BS  '\b'  // 0x08 Back Space
#define ASC_FF  '\f'  // 0x0C Form Feed
#define ASC_LF  '\n'  // 0x0A Line Feed
#define ASC_CR  '\r'  // 0x0D Carriage Return
#define ASC_HT  '\t'  // 0x09 Harizontal Tab
#define ASC_VT  '\v'  // 0x0B Vertical Tab
 
#define STX_PC            ASC_STX
#define ETX_PC            ASC_ETX
#define MAX_TX_PROCESS    10    // Max Tx Process no for PC
#define PACKET_BUF_SIZE   64    // packet buffer size per Tx Process

/*-----------------*/
/* IN/OUT Definition */
/*-----------------*/

#define PROFI_Rx	(AT91C_PIO_PA0)		// PROFI-BUS RxD
#define PROFI_Tx  	(AT91C_PIO_PA1)         // PROFI-BUS TxD
#define FND_CK          (AT91C_PIO_PA2)		//(OUT)FND UNIT CLOCK
#define FND_DT          (AT91C_PIO_PA3)		//(OUT)FND UNIT DATA
#define FND_LD          (AT91C_PIO_PA4)		//(OUT)FND UNIT LOAD
//#define REMOTE_Rx     (AT91C_PIO_PA5)         // REMOTE com Rx
//#define REMOTE_Tx     (AT91C_PIO_PA6)         // REMOTE com Tx
// #define REMOTE_DR       (AT91C_PIO_PA7)        	//(OUT) REMOTE com DIRECTION

#define JOG_FW          (AT91C_PIO_PA8)         //(IN) JOG-DIAL FORWARD pulse
#define JOG_REV         (AT91C_PIO_PA9)         //(IN) JOG-DIAL REVERSE pulse
#define AD_RST          (AT91C_PIO_PA10)        //(OUT)A/D Converter AD7705 Reset
#define AD_RDY          (AT91C_PIO_PA11)        //(IN) A/D Converter AD7705 Ready
#define AD_CS           (AT91C_PIO_PA12)        //(OUT)A/D Converter AD7705 chip select(0: select)
#define PM_CE           (AT91C_PIO_PA13)        //(OUT)ADE7758 chip enable(1: select)
#define DA_CLR          (AT91C_PIO_PA14)        //(OUT) D/A Converter AD5663 Clear
#define PROFI_RST       (AT91C_PIO_PA15)        //(OUT)PROFI-BUS DP Reset out
#define AD_DOUT         (AT91C_PIO_PA16)        //(IN) A/D Converter AD7705 Data IN
#define PM_DIN          (AT91C_PIO_PA17)        //(OUT) SPI bus data in
#define PM_CLK          (AT91C_PIO_PA18)        //(OUT)SPI bus clock
#define BT_RUN          (AT91C_PIO_PA19)        //(IN) RUN Botton in
#define BT_STOP         (AT91C_PIO_PA20)        //(IN) STOP Botton in
#define SW_REMOT        (AT91C_PIO_PA21)        //(IN) Remote Select SW in(0:REMOT)
#define DA_LDAC         (AT91C_PIO_PA22)        //(OUT)D/A Converter AD5663 load
#define DA_DOUT   	(AT91C_PIO_PA23)        //(OUT)D/A Converter AD5663 data
#define DA_CK           (AT91C_PIO_PA24)        //(OUT)D/A Converter AD5663 clock
#define DA_SYNC         (AT91C_PIO_PA25)        //(OUT)D/A Converter AD5663 sync
#define DC24_RDY        (AT91C_PIO_PA26)        //(OUT)External 24V power ON
#define BUZZER          (AT91C_PIO_PA27)	//(OUT)Internal Piazo Buzzer
#define PM_DOUT         (AT91C_PIO_PA28)        //(IN) Power Meter Data IN
#define PM_IRQ          (AT91C_PIO_PA29)        //(IN) ADE7753 IRQ
#define JOG_BT          (AT91C_PIO_PA30)        //(IN) JOG-DIAL Button pulse
#define AD_DIN          PM_DIN
#define AD_CLK          PM_CLK

#define LCD_DATA        ((unsigned int) 0x00FF) //(I/O) LCD Module Data Port(0-7)
#define EXT_DATA        ((unsigned int) 0xFF00) //(I/O) Extended Bus Data Port(0-7)
#define LCD_A0          (AT91C_PIO_PB16)        //(OUT) LCD Module Data/Command
#define LCD_CS1         (AT91C_PIO_PB17)        //(OUT) LCD Module chip enable 1
#define LCD_CS2         (AT91C_PIO_PB18)        //(OUT) LCD Module chip enable 2
#define LCD_RW          (AT91C_PIO_PB19)        //(OUT) LCD Module Read/Write Select(0:Write)
#define LCD_EN          (AT91C_PIO_PB20)        //(OUT) LCD enable
#define LCD_LP          (AT91C_PIO_PB21)        //(OUT) LCD Back Light(1: ON)
#define LCD_RST         (AT91C_PIO_PB22)        //(OUT) LCD reset(0: reset)

#define LP_START        (AT91C_PIO_PB23)        //(OUT)0: Stop Lamp,  1: Start Lamp
#define LP_PLUS         (AT91C_PIO_PB24)        //(OUT)0: Minus Lamp, 1: Plus Lamp
#define EXT_E1          (AT91C_PIO_PB25)        //(OUT)Extended Bus enable 1
#define EXT_E2          (AT91C_PIO_PB26)        //(OUT)Extended Bus enable 2
#define EXT_E3           (AT91C_PIO_PA7)        //(OUT) REMOTE com DIRECTION

#define RTC_CK          (AT91C_PIO_PB27)        //(OUT) RTC DS1302 clock
#define RTC_DT          (AT91C_PIO_PB28)        //(IN)  RTC DS1302 data
#define RTC_RST         (AT91C_PIO_PB29)        //(OUT) RTC DS1302 reset
#define LP_RUN          (AT91C_PIO_PB30)        //(OUT) RUN lamp
#define	LP_GRN		SPOUT
#define AT91A_OUT_MASK  (FND_CK|FND_DT|FND_LD|BUZZER|EXT_E3|AD_RST|AD_CS|PM_CE|DA_CLR|PROFI_RST|PM_CLK|PM_DIN|DA_LDAC|DA_DOUT|DA_CK|DA_SYNC|DC24_RDY)
#define AT91B_OUT_MASK  (LCD_DATA|LCD_A0|LCD_CS1|LCD_CS2|LCD_RW|LCD_EN|LCD_LP|LCD_RST|LP_PLUS|LP_START|EXT_E1|EXT_E2|RTC_CK|RTC_RST|LP_RUN)
#define AT91A_IN_MASK   AT91A_OUT_MASK ^ 0xFFFFFFFF
#define AT91B_IN_MASK   AT91B_OUT_MASK ^ 0xFFFFFFFF

/*--------------------*/
/* Extend I/O Define  */
/*--------------------*/
// EXT INPUT define
// 2009-02-13 수정
#define	EXTIN_EMEG_STOP		0
#define	EXTIN_ERROR_1           1
#define	EXTIN_ERROR_2           2
#define	EXTIN_MANUAL_OP         3
#define	EXTIN_ERROR_3           4
#define	EXTIN_ERROR_4           5
#define	EXTIN_ERROR_5           6
#define	EXTIN_ERROR_6           7
#define	EXTIN_ERROR_7           8
#define	EXTIN_ERROR_8           9
#define	EXTIN_ERROR_9           10
#define	EXTIN_ERROR_10          11
#define	EXTIN_ERROR_11          12
#define	EXTIN_ERROR_12          13
#define	EXTIN_ERROR_13          14
#define	EXTIN_ERROR_14          15


// 2008-11-25 수정
/*
#define	EXTIN_EMEG_STOP		0
#define	EXTIN_FUSE_CUT          1
#define	EXTIN_VCS_TRIP          2
#define	EXTIN_MANUAL_OP         3
#define	EXTIN_REMOTE_RUN        4
#define	EXTIN_TEMP_OVER         5
#define	EXTIN_WATER_FAULT       6
#define	EXTIN_COOLER_FAULT      7
*/

// EXT OUTPUT define
// 2009-02-13 수정
#define EXTOUT_REMOTE           0
#define EXTOUT_READY            1 
#define EXTOUT_RUNNING          2 
#define EXTOUT_REVERSE          3 
#define EXTOUT_ALARM            4 
#define EXTOUT_FAULT            5
#define EXTOUT_MCCB_TRIP        6 
//#define EXTOUT_VCSOFF_REQ       6 
#define EXTOUT_DAOUT            7

/*--------------*/
/* I/O Control  */
/*--------------*/
#define OFF     0
#define ON      1
#define READ    0
#define WRITE   1
#define TOGLE   2
#define BLINK   3
#define UP      10
#define DOWN    11
#define CC_MODE 0
#define CV_MODE 1
#define PLUS    0
#define MINUS   1
#define LOCAL   0
#define REMOTE  1
#define TRUE  (1==1)
#define FALSE (0==1)
#define true  (1==1)
#define false (0==1)

/*------------------*/
/* putchar Control  */
/*------------------*/
#define	MONOUT		0x01
#define	COM2OUT		0x02
#define	COM1OUT         0x04
#define	COM0OUT		0x08
#define	FND_OUT		0x80

/*********************/
/*  Key Code Define  */
/*********************/ 
#define KEY_0        	0x30        
#define KEY_1        	0x31
#define KEY_2        	0x32
#define KEY_3        	0x33
#define KEY_4        	0x34
#define KEY_5        	0x35
#define KEY_6        	0x36
#define KEY_8        	0x37
#define KEY_7        	0x38
#define KEY_9        	0x39
#define KEY_pp    	0x2A	// '*'
#define KEY_sp    	0x23	// '#'
#define KEY_A           0x41	// 'A'
#define KEY_B	        0x42	// 'B'
#define KEY_C       	0x43	// 'C'
#define KEY_D           0x44	// 'D'
#define KEY_E           0x45	// 'E'
#define KEY_F           0x46	// 'F'
#define KEY_G    	0x47	// 'G'	
#define KEY_H    	0x48	// 'H'		
#define KEY_I    	0x49	// 'I'		
#define KEY_J    	0x4A	// 'J'		
#define KEY_K    	0x4B	// 'K'		
#define KEY_L    	0x4C	// 'L'		
#define KEY_M    	0x4D	// 'M'		
#define KEY_N    	0x4E	// 'N'		
#define KEY_O    	0x4F	// 'O'		
#define KEY_P    	0x50	// 'P'		
#define KEY_Q    	0x51	// 'Q'		
#define KEY_R    	0x52	// 'R'		
#define KEY_S    	0x53	// 'S'		
#define KEY_T    	0x54	// 'T'		
#define KEY_U    	0x55	// 'U'		
#define KEY_V    	0x56	// 'V'		
#define KEY_W    	0x57	// 'W'
#define KEY_X    	0x58	// 'X'	
#define KEY_Y    	0x59	// 'Y'	
#define KEY_Z    	0x5A	// 'Z'
#define KEY_a           0x61	// 'a'
#define KEY_b	        0x62	// 'b'
#define KEY_c       	0x63	// 'c'
#define KEY_d           0x64	// 'd'
#define KEY_e           0x65	// 'e'
#define KEY_f           0x66	// 'f'
#define KEY_g    	0x67	// 'g'	
#define KEY_h    	0x68	// 'h'		
#define KEY_i    	0x69	// 'i'		
#define KEY_j    	0x6A	// 'j'		
#define KEY_k    	0x6B	// 'k'		
#define KEY_l    	0x6C	// 'l'		
#define KEY_m    	0x6D	// 'm'		
#define KEY_n    	0x6E	// 'n'		
#define KEY_o    	0x6F	// 'o'		
#define KEY_p    	0x70	// 'p'		
#define KEY_q    	0x71	// 'q'		
#define KEY_r    	0x72	// 'r'		
#define KEY_s    	0x73	// 's'		
#define KEY_t    	0x74	// 't'		
#define KEY_u    	0x75	// 'u'		
#define KEY_v    	0x76	// 'v'		
#define KEY_w    	0x77	// 'w'
#define KEY_x    	0x78	// 'x'	
#define KEY_y    	0x79	// 'y'	
#define KEY_z    	0x7A	// 'z'
#define KEY_tt          0x2E	// '.'
#define KEY_CR          0x0D    // return

#define KEY_RUN         KEY_r
#define KEY_STOP        KEY_s
#define KEY_CLEAR       KEY_l
#define KEY_REMOT       KEY_m

/*--------------------*/
/* Grapic Image Q'ty  */
/*--------------------*/
#define	MAX_IMAGE	5

/*--------------------*/
/* Ddfault Parameter  */
/*--------------------*/
#define	DEFAULT_SPEED_COM0      38400
#define	DEFAULT_SPEED_COM1      38400
// 2026-10-07 추가: UART 디버그 출력 속도 (COM1, REMOTE_Tx PA6 -> Nu-Link VCOM)
//   FLASH 에 저장된 iCom1Speed 와 관계없이 부팅 시 이 속도로 설정. 주석 처리하면 저장값(기본 38400) 사용
//   115200 : 보드레이트 오차 +0.27% (MCK 48.055MHz / 16 / 26), 송신은 인터럽트 방식이라 루프 지연 없음
#define	DBG_COM1_SPEED          115200
#define	DEFAULT_SPEED_COM2      115200
#define	SECRET_CODE		9494
#define DEFAULT_MAX_HOUR        1
#define DEFAULT_MAX_MINUTE      0
#define DEFAULT_MAX_SEC         0
#define DEFAULT_OPMODE          CC_MODE
#define DEFAULT_OPPOLE          PLUS
#define DEFAULT_VIEWPAGE        0
#define DEFAULT_OP_AMP          5000000
#define DEFAULT_MAX_AMP         5000000
#define DEFAULT_OVER_AMP        DEFAULT_MAX_AMP * 1.1
#define DEFAULT_OP_VOLT         5000     //25V
#define DEFAULT_MAX_VOLT        6000     //50.00V
#define DEFAULT_OVER_VOLT       DEFAULT_MAX_VOLT * 1.2
#define DEFAULT_AC_OVER_AMP     200
#define DEFAULT_AC_LOW_VOLT     0
#define DEFAULT_VOLT_OFFSET     0
#define DEFAULT_AMP_OFFSET      0
#define DEFAULT_SOFT_TIME       5
#define DEFAULT_REACT_RATE      20
#define MAX_REACT_RATE          100
#define MIN_REACT_RATE          5
#define MIN_POLE_TURN_TIME      1       // 0.1sec
#define MAX_POLE_TURN_TIME      50      // 5.0sec
#define DEFAULT_POLE_TURN_TIME  10      // 1.0sec
#define DEFAULT_REACT_RANGE     1000
#ifdef MONO_POLE
  #define MAX_OFFSET_RANGE        400
#else
  #define MAX_OFFSET_RANGE        200
#endif
#define MAX_DC_GAIN               1600
#define MIN_DC_GAIN               400
#define DEFAULT_DC_GAIN           1000
#define LCD_LIGHT_ON_TIME         180     // sec

#define IRMS_DIVIDER		16848	
#define IRMS_OFFSET		IRMS_DIVIDER
#define VRMS_DIVIDER    	8800
#define VRMS_OFFSET		0
#define PW_DIVIDER      	11//8
#define	ACTIVE_DIVIDER		1000
#define	ACTIVE_OFFSET		0
#define	SAMPLE_DURATION		60	// 측정데이터 기록 주기(Sec)
#define	DEFAULT_CONNECT		0	// 0: CDMA modem, 1:RS232C
#define	SYSTEM_LIVE_TIME        SEC_1/2 // 사스템 작동상태 알림 주기

/*--------------------*/
/* Execute Mode list  */
/*--------------------*/
#define	MAIN_MENU	0
#define	RUN_STATUS      1
#define SYSTEM_SET      2
#define	ADE7758_TEST	3
#define	FLASH_EDIT	4
#define	GUIDE_MESSAGE	5
#define	RTC_SET	        6
#define	METER_ADJUST	7
#define	FLASH_TEST	8
#define	RTC_TEST	10
#define	CONNECT_SELECT	11
#define	SYSTEM_TEST	12
#define	EXTOUT_TEST     13  
#define EXTIN_TEST      14
#define ADC_TEST        15
#define DAC_TEST        16
#define ADE_TEST        17
#define ADEIO_TEST      18
#define REMOTE_SET      19
#define PROFI_SET       20
#define	DEBUG_MODE      21
/*----------------------*/
/* Common Execute Step  */
/*----------------------*/
#define	PARAMETER_SAVE	2000
