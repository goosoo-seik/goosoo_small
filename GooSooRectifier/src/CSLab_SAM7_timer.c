

#define BYTE  unsigned short
// Include Standard LIB  files
#include "project.h"

//* Global variable
int count_timer0_interrupt;
int count_timer1_interrupt;
BYTE Scan2ms;

#define TIMER0_INTERRUPT_LEVEL		1
#define TIMER1_INTERRUPT_LEVEL		7

/*-----------------*/
/* Clock Selection */
/*-----------------*/
#define TC_CLKS                  0x7
#define TC_CLKS_MCK2             0x0
#define TC_CLKS_MCK8             0x1
#define TC_CLKS_MCK32            0x2
#define TC_CLKS_MCK128           0x3
#define TC_CLKS_MCK1024          0x4
/*-----------------*/
/* WAVE Selection  */
/*-----------------*/
#define UP_NON_TRIG   0x0<<13    // UP mode without auto-trigger on RC compare
#define UP_AUTO_TRIG  0x2<<13    // UP mode with auto-trigger on RC compare
#define UD_NON_TRIG   0x1<<13    // UPDOWN mode without auto-trigger on RC compare
#define UD_AUTO_TRIG  0x3<<13    // UPDOWN mode with auto-trigger on RC compare


//*------------------------- Internal Function --------------------------------
//*----------------------------------------------------------------------------
//* Function Name       : AT91F_TC_Open
//* Object              : Initialize Timer Counter Channel and enable is clock
//* Input Parameters    : <tc_pt> = TC Channel Descriptor Pointer
//*                       <mode> = Timer Counter Mode
//*                     : <TimerId> = Timer peripheral ID definitions
//* Output Parameters   : None
//*----------------------------------------------------------------------------
void AT91F_TC_Open ( AT91PS_TC TC_pt, unsigned int Mode, unsigned int TimerId)
//* Begin
{
    unsigned int dummy;

    //* First, enable the clock of the TIMER
    	AT91F_PMC_EnablePeriphClock ( AT91C_BASE_PMC, 1<< TimerId ) ;

    //* Disable the clock and the interrupts
	TC_pt->TC_CCR = AT91C_TC_CLKDIS ;
	TC_pt->TC_IDR = 0xFFFFFFFF ;

    //* Clear status bit
        dummy = TC_pt->TC_SR;
    //* Suppress warning variable "dummy" was set but never used
        dummy = dummy;
    //* Set the Mode of the Timer Counter
	TC_pt->TC_CMR = Mode ;

    //* Enable the clock
	TC_pt->TC_CCR = AT91C_TC_CLKEN ;
//* End
}
//*------------------------- Interrupt Function -------------------------------

//*----------------------------------------------------------------------------
//* Function Name       : timer0_c_irq_handler
//* Object              : C handler interrupt function calAT91B_LED by the interrupts
//*                       assembling routine
//* Output Parameters   : increment count_timer0_interrupt
//*----------------------------------------------------------------------------

__ramfunc void timer0_c_irq_handler(void)
{
	AT91PS_TC TC_pt = AT91C_BASE_TC0;
    unsigned int dummy;
    //* AcknowAT91B_LEDge interrupt status
    dummy = TC_pt->TC_SR;
    dummy = dummy;
    Scan2ms++;
    count_timer0_interrupt++;
}
//*----------------------------------------------------------------------------
//* Function Name       : timer1_c_irq_handler
//* Object              : C handler interrupt function calAT91B_LED by the interrupts
//*                       assembling routine
//* Output Parameters   : increment count_timer1_interrupt
//*----------------------------------------------------------------------------
__ramfunc void timer1_c_irq_handler(void)
{
	AT91PS_TC TC_pt = AT91C_BASE_TC1;
    unsigned int dummy;
    //* AcknowAT91B_LEDge interrupt status
    dummy = TC_pt->TC_SR;
    //* Suppress warning variable "dummy" was set but never used
    dummy = dummy;
    count_timer1_interrupt++;
    //LEDon ^= 1;
    //if (LEDon) AT91F_PIO_ClearOutput( AT91C_BASE_PIOB, AT91B_LED4 );
    // else AT91F_PIO_SetOutput( AT91C_BASE_PIOB, AT91B_LED4 );
    
    //* Read the output state
    /*
    if ( (AT91F_PIO_GetInput(AT91C_BASE_PIOB) & LP_RFIN ) == LP_RFIN )
    {
        AT91F_PIO_ClearOutput( AT91C_BASE_PIOB, LP_RFIN );
    }
    else
    {
        AT91F_PIO_SetOutput( AT91C_BASE_PIOB, LP_RFIN );
    }
    */
}
//*-------------------------- External Function -------------------------------

//*----------------------------------------------------------------------------
//* Function Name       : timer_init
//* Object              : Init timer counter
//* Input Parameters    : none
//* Output Parameters   : TRUE
//*----------------------------------------------------------------------------
void timer_init ( void )
//* Begin
{
    //init the timer interrupt counter
    count_timer0_interrupt=0;
    count_timer1_interrupt=0;
    
    //* Open timer0
	AT91F_TC_Open(AT91C_BASE_TC0,TC_CLKS_MCK2|UP_AUTO_TRIG,AT91C_ID_TC0);
    //* timer0 interval = 2.0 mS
        AT91C_BASE_TC0->TC_RC = CLOCK_2MS;
    //* Open Timer 0 interrupt
	AT91F_AIC_ConfigureIt ( AT91C_BASE_AIC, AT91C_ID_TC0, TIMER0_INTERRUPT_LEVEL,AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL, timer0_c_irq_handler);
        AT91C_BASE_TC0->TC_IER = AT91C_TC_CPCS;  //  IRQ enable CPC
	AT91F_AIC_EnableIt (AT91C_BASE_AIC, AT91C_ID_TC0);

    //* Open timer1
	AT91F_TC_Open(AT91C_BASE_TC1,TC_CLKS_MCK128,AT91C_ID_TC1);

    //* Open Timer 1 interrupt
	AT91F_AIC_ConfigureIt ( AT91C_BASE_AIC, AT91C_ID_TC1, TIMER1_INTERRUPT_LEVEL,AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL, timer1_c_irq_handler);
	AT91C_BASE_TC1->TC_IER  = AT91C_TC_CPCS;  //  IRQ enable CPC
	AT91F_AIC_EnableIt (AT91C_BASE_AIC, AT91C_ID_TC1);
	
     //   AT91C_BASE_TC0->TC_RC = 0x0666;
    //* Start timer0
        AT91C_BASE_TC0->TC_CCR = AT91C_TC_SWTRG ;

    //* Start timer1
        AT91C_BASE_TC1->TC_RC = 0x2000;
        AT91C_BASE_TC1->TC_CCR = AT91C_TC_SWTRG ;

//* End
}
