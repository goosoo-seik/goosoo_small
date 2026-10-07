// ----------------------------------------------------------------------------
//         ATMEL Microcontroller Software Support  -  ROUSSET  -
// ----------------------------------------------------------------------------
// File Name           : ILMAC_Rectifier_extirq.c
// Object              : ILMAC_Rectifier External interrupt handler for irq
// Creation            : JPP   08-Sep-2005
//         ILMAC Sysytem  - Lee YongKwan -
// ----------------------------------------------------------------------------

// Include Standard LIB  files
#include "project.h"

#define SYS_INTERRUPT_LEVEL		1

//*----------------------------------------------------------------------------
//* Function Name       : FIQ_init_handler
//* Object              : Irq Handler calAT91B_LED by the FIQ interrupt with AT91
//*                       compatibility
///*----------------------------------------------------------------------------
__ramfunc void FIQ_init_handler(void)
{
}

//*----------------------------------------------------------------------------
//* Function Name       : aic_software_interrupt
//* Object              : Software interrupt function
//* Input Parameters    : none
//* Output Parameters   : none
//* Functions calAT91B_LED    : at91_pio_write
//*----------------------------------------------------------------------------
__ramfunc void aic_software_interrupt(void)
{
}

//*----------------------------------------------------------------------------
//* Function Name       : pio_c_irq_handler
//* Object              : Irq Handler calAT91B_LED by the irq_pio.s
//* Input Parameters    : none
//* Output Parameters   : none
//* Functions calAT91B_LED    : at91_pio_read, at91_pio_write
//*----------------------------------------------------------------------------
//__ramfunc void pio_c_irq_handler ( void )
void pio_c_irq_handler ( void )
{
int dummy;
//* enable the next PIO IRQ
    dummy =AT91C_BASE_PIOA->PIO_ISR;
    //* suppress the compilation warning
    dummy =dummy;
}

__ramfunc void EFC_init_handler(void)
{
}
#define SYS_INTERRUPT_LEVEL	1
__ramfunc void sys_c_irq_handler(void)
{
  AT91PS_AIC     pAic;
  int dummy;
  //* enable the next PIO IRQ
    pAic = AT91C_BASE_AIC;
    dummy =pAic->AIC_ISR;
    //* suppress the compilation warning
    dummy =dummy;  
}

void etc_int_init ( void )
//* Begin
{
    //* Open SYS interrupt
       AT91F_AIC_ConfigureIt ( AT91C_BASE_AIC, AT91C_ID_SYS, SYS_INTERRUPT_LEVEL, 1, sys_c_irq_handler);
}
