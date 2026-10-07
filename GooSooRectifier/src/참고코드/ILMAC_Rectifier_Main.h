// ----------------------------------------------------------------------------
//         ILMAC Sysytem  - Lee YongKwan -
// ----------------------------------------------------------------------------
// File Name           : ILMAC_Rectifier_Main.h
// Object              : ILMAC_Rectifier main Definition File
// Creation            : 2007/09/22
// Last update         : 2008/11/25
// ----------------------------------------------------------------------------
// Sysyem Lanuage Mode
#define HANGUL

// System Type
//#define MONO_POLE 

// System Version Infomation 
#define	VERSION		2	// Version NO  2007-09-22
#define	RELEASE		40	// Release NO  2008-11-25
#define	UPDATE_YEAR     2008 
#define UPDATE_MONTH	11
#define UPDATE_DATE	26

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
#define REMOTE_DR       (AT91C_PIO_PA7)        	//(OUT) REMOTE com DIRECTION
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

#define RTC_CK          (AT91C_PIO_PB27)        //(OUT) RTC DS1302 clock
#define RTC_DT          (AT91C_PIO_PB28)        //(IN)  RTC DS1302 data
#define RTC_RST         (AT91C_PIO_PB29)        //(OUT) RTC DS1302 reset
#define LP_RUN          (AT91C_PIO_PB30)        //(OUT) RUN lamp
#define	LP_GRN		SPOUT
#define AT91A_OUT_MASK  (FND_CK|FND_DT|FND_LD|BUZZER|REMOTE_DR|AD_RST|AD_CS|PM_CE|DA_CLR|PROFI_RST|PM_CLK|PM_DIN|DA_LDAC|DA_DOUT|DA_CK|DA_SYNC|DC24_RDY)
#define AT91B_OUT_MASK  (LCD_DATA|LCD_A0|LCD_CS1|LCD_CS2|LCD_RW|LCD_EN|LCD_LP|LCD_RST|LP_PLUS|LP_START|EXT_E1|EXT_E2|RTC_CK|RTC_RST|LP_RUN)
#define AT91A_IN_MASK   AT91A_OUT_MASK ^ 0xFFFFFFFF
#define AT91B_IN_MASK   AT91B_OUT_MASK ^ 0xFFFFFFFF

/*--------------------*/
/* Extend I/O Define  */
/*--------------------*/
// EXT INPUT define
#define	EXTIN_EMEG_STOP		0
#define	EXTIN_FUSE_CUT          1
#define	EXTIN_VCS_TRIP          2
#define	EXTIN_MANUAL_OP         3
#define	EXTIN_TEMP_TR           4
#define	EXTIN_TEMP_SCR          5
#define	EXTIN_WATER_FAULT       6
#define	EXTIN_COOLER_FAULT      7

// EXT OUTPUT define
//#define EXTOUT_BUZZER           0
#define EXTOUT_REMOTE           0
#define EXTOUT_READY            1 
#define EXTOUT_RUNNING          2 
#define EXTOUT_REVERSE          3 
#define EXTOUT_ALARM            4 
#define EXTOUT_FAULT            5
#define EXTOUT_VCSTRIP          6 
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
#define	DEFAULT_SPEED_COM2      115200
#define	SECRET_CODE		9494
#define DEFAULT_MAX_HOUR        1
#define DEFAULT_MAX_MINUTE      0
#define DEFAULT_MAX_SEC         0
#define DEFAULT_OPMODE          CC_MODE
#define DEFAULT_OPPOLE          PLUS
#define DEFAULT_VIEWPAGE        0
#define DEFAULT_OP_AMP          1000000
#define DEFAULT_MAX_AMP         2000000
#define DEFAULT_OVER_AMP        DEFAULT_MAX_AMP * 1.1
#define DEFAULT_OP_VOLT         3000     //25V
#define DEFAULT_MAX_VOLT        3000     //50.00V
#define DEFAULT_OVER_VOLT       DEFAULT_MAX_VOLT * 1.2
#define DEFAULT_AC_OVER_AMP     200
#define DEFAULT_AC_LOW_VOLT     0
#define DEFAULT_VOLT_OFFSET     0
#define DEFAULT_AMP_OFFSET      0
#define DEFAULT_SOFT_TIME       5
#define DEFAULT_REACT_RATE      20
#define MAX_REACT_RATE          100
#define MIN_REACT_RATE          5
#define DEFAULT_REACT_RANGE     1000
#ifdef MONO_POLE
  #define MAX_OFFSET_RANGE        400
#else
  #define MAX_OFFSET_RANGE        200
#endif
#define MAX_DC_GAIN               1200
#define MIN_DC_GAIN               800
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
