// ----------------------------------------------------------------------------
//         Epiatech co,. LTD  -  Lee Y k  -
// ----------------------------------------------------------------------------
// File Name           : main.c
// Object              : EpiaGate Main Unit MAIN File
// Creation            : 2006-11-01
// ----------------------------------------------------------------------------

#define	VERSION		0	// Version NO  2006.11.07
#define	RELEASE		1	// Release NO  2006.11.07

// Include Standard LIB  files
#include "project.h"

//static const char atmel_header[]=
const unsigned char PowerOnMsg[] = 
{
"   EpiaCAFE v0.94\n"
"  E-piaTech Co,LTD\n"
"Program-> LeeYK ^_^\n"
"Copyright2007/05\23"};
/*
"--------------------\r"
"Project Name :\n"
"EpiaGate Test Kit   \r"
"--------------------\r"
"E-pia Reserch Center\r"
"Reserch 2 Team\n"
"Manager:Lee YongKwan\r"
"Program:Lee JungHwan\r"
"Design :Han SangYun \r"
"H/W,RF :Lee SangHo  \r"
"MAIN File Creation: \r"
"   2006/11/01       \r"
"--------------------\r"
"Main Processor :    \r"
"ATmel AT91SAM7X256  \r"
"Copyright (C) 2005  \r"
"ATMEL Corporations  \r"
"   Version: 1.0     \r"
"--------------------\r"
"  << Good Luck!!>>  \r"
"  << Thanks You >>  \r"
"---------END--------\r\3"
};
*/

char  ScanExec;
/********************************************/
/* key_function() offer user key processing */
/* This function must locate in main loop   */ 
/********************************************/
void key_function(void)
{
if (DebugMode) debug_key_function(bPushKey);
//switch (ExecMode) {
// case CLOCK:	CLOCK_key_function();	return;
// case TIMESET:	TIMESET_key_function();	return;
// };
}

//*----------------------------------------------------------------------------
//* Function Name       : main
//* Object              : Main function
//*----------------------------------------------------------------------------
int main( void )
//* Begin
{
 //   unsigned int   loop_count ;
    AT91PS_AIC     pAic;
   // char loop;
   
    //* Load System pAic Base address
        pAic = AT91C_BASE_AIC;

    //AT91F_LowLevelInit();
    //* Enable User Reset and set its minimal assertion to 960 us
        AT91C_BASE_RSTC->RSTC_RMR = AT91C_RSTC_URSTEN | (0x4<<8) | (unsigned int)(0xA5<<24);

    //* Init
     	//loop_count = 0 ;
   // First, enable the clock of the PIOB
        AT91F_PMC_EnablePeriphClock ( AT91C_BASE_PMC, 1 << AT91C_ID_PIOB ) ; 
	
   //* Init PIO 
       pio_init();
   	
    //* open external PIO interrupt
        //* define switch SW5 at PIO input for interrupt IRQ loop
   	AT91F_PMC_EnablePeriphClock ( AT91C_BASE_PMC, 1 << AT91C_ID_PIOA ) ;

	AT91F_AIC_ConfigureIt ( pAic, AT91C_ID_PIOA, PIO_INTERRUPT_LEVEL,AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL, pio_c_irq_handler);
	//AT91F_PIO_InterruptEnable(AT91C_BASE_PIOA, EMEG_IN);
	//* set the interrupt by software
	AT91F_AIC_EnableIt (pAic, AT91C_ID_PIOA);

    //* Open the software interrupt on the AIC
    //    AT91F_AIC_ConfigureIt ( pAic, AT91C_ID_SYS, SOFT_INTERRUPT_LEVEL, AT91C_AIC_SRCTYPE_INT_POSITIVE_EDGE,  aic_software_interrupt);
    //    AT91F_AIC_EnableIt (pAic, AT91C_ID_SYS);

    //* open  FIQ interrupt
    //    AT91F_PIO_CfgPeriph(AT91C_BASE_PIOA,AT91B_SW1,0);
    //	AT91F_AIC_ConfigureIt ( pAic, AT91C_ID_FIQ, FIQ_INTERRUPT_LEVEL,AT91C_AIC_SRCTYPE_EXT_NEGATIVE_EDGE, FIQ_init_handler);
    //	AT91F_AIC_EnableIt (pAic, AT91C_ID_FIQ);
        //* generate FIQ interrupt by software
    //	AT91F_AIC_Trig (pAic,AT91C_ID_FIQ) ;

    //* Init timer interrupt
        timer_init();

    //* Init Usart
        Usart_init();
        Usart1_init();
        Usart2_init();

    //* generate software interrupt
    //    AT91F_AIC_Trig (pAic,AT91C_ID_SYS) ;
    
    
    data_init();  // data init
    //ZCM_reset();  // ZigBee Module Reset
    LCD_init();   // Init LCD display GM123210 & Start Massage display
    //bDpStep = 1;
    DebugMode = 1;
    CursorUse = 1;
    CodeOdd = 0;
    sOldCode = 0;
    //StepETS = 1;
    //StepPCrx = 0;
    //sStepZBrx = ZCM_POWERON;
    //AutoZigBee = 1;
    ImageNo = 19;
       //ZCM_reset();
    
for (;;)

    {
        RUNlamp(ON);
	 RUNlamp(OFF);
	//if (Scan2ms)
        {
	  /**********************************************/
	  /*                  Main Loop                 */
	  /* All function in loop executed per ScanTime */
	  /* All function must programmed callback type */
	  /* NOTE: Recommand no have delay(>1ms)        */ 
	  /* and execcute time is short.                */
	  /**********************************************/
         // pio_set( PIOB, loop_count);
	  //pio_clear( PIOB, loop_count ^ 0xFFFF);
	  //loop_count++;
	  // RUNlamp(ON);
          //Scan2ms = 0;
          //touch_key_scan();
          //if (rx_rd_index0 != rx_wr_index0) bPushKey = getchar0();         
	  //key_function();
          //if (bPushKey) putchar_dp(bPushKey);
          //link_receive_PC();
          //tx_process_manage_PC();
          //display_scan();
          //event_lighting();
          //event_clear();
          //exec_time_display();
          //exec_time_check();
          //RUNlamp(OFF);
          //ScanExec = 0;
        }
    }
}
