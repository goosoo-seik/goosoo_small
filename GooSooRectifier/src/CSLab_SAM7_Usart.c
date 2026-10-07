

#define BYTE  unsigned short
// Include Standard LIB  files
#include "project.h"

#define USART_INTERRUPT_LEVEL		7

//* \fn    AT91F_US_Baudrate
//* \brief Calculate the baudrate
//* Standard Asynchronous Mode : 8 bits , 1 stop , no parity
#define AT91C_US_ASYNC_MODE ( AT91C_US_USMODE_NORMAL + \
                        AT91C_US_NBSTOP_2_BIT + \
                        AT91C_US_PAR_NONE + \
                        AT91C_US_CHRL_8_BITS + \
                        AT91C_US_CLKS_CLOCK )

int iCom0Speed;
// USART0 Receiver buffer
#define RX_BUFFER_SIZE0 256
BYTE  rx_buffer0[RX_BUFFER_SIZE0];
short rx_wr_index0,rx_rd_index0,rx_counter0;
// This flag is set on USART0 Receiver buffer overflow
char ErrUsart;

// USART0 Transmitter buffer
#define TX_BUFFER_SIZE0 256
BYTE  tx_buffer0[TX_BUFFER_SIZE0];
short tx_wr_index0,tx_rd_index0,tx_counter0;

//*------------------------- Internal Function --------------------------------

//*----------------------------------------------------------------------------
//* Function Name       : Usart_c_irq_handler
//* Object              : C handler interrupt function calAT91B_LED by the interrupts
//*                       assembling routine
//* Input Parameters    : <RTC_pt> time rtc descriptor
//* Output Parameters   : increment count_timer0_interrupt
//*----------------------------------------------------------------------------

//__ramfunc void Usart_c_irq_handler(void)
void Usart_c_irq_handler(void)
{
unsigned char  data;
        //GRNlamp(ON);
	AT91PS_USART USART_pt = AT91C_BASE_US0;
	unsigned int status;
	//* get Usart status register
	status = USART_pt->US_CSR;
	if ( status & AT91C_US_RXRDY)   // (DBGU) RXRDY Interrupt
         {    
          data = AT91F_US_GetChar(USART_pt);
          //AT91F_US_PutChar (USART_pt, data);  // ehco return
          rx_buffer0[rx_wr_index0]=data;
          ++rx_wr_index0;
          rx_wr_index0 &= 0xFF;
          //if (rx_wr_index0 == rx_rd_index0) ErrUsart = 'B';
	 }
        
        if ( status & AT91C_US_TXRDY)
         {
          if ( tx_rd_index0 != tx_wr_index0 ) 
           {
            AT91F_US_PutChar (USART_pt, tx_buffer0[tx_rd_index0]);
            if (++tx_rd_index0 == TX_BUFFER_SIZE0) tx_rd_index0=0;
           }
          else 
           AT91F_US_DisableIt(AT91C_BASE_US0, AT91C_US_TXRDY);
        }
	if ( status & AT91C_US_OVRE) {
		//* clear US_RXRDY
		 AT91F_US_GetChar(USART_pt);
                 ErrUsart = 'O';
		 //AT91F_US_PutChar (USART_pt, 'O');
	}

	//* Check error
	if ( status & AT91C_US_PARE) {
		 ErrUsart = 'P';
                 //AT91F_US_PutChar (USART_pt, 'P');
	}

	if ( status & AT91C_US_FRAME) {
		 ErrUsart = 'F';
                 //AT91F_US_PutChar (USART_pt, 'F');
	}

	if ( status & AT91C_US_TIMEOUT){
		USART_pt->US_CR = AT91C_US_STTTO;
		ErrUsart = 'T'; 
                //AT91F_US_PutChar (USART_pt, 'T');
	}
	//* Reset the satus bit
	 USART_pt->US_CR = AT91C_US_RSTSTA;
}

// Get a character from the USART0 Receiver buffer
char getchar0(void)
{
 char data;
      while (rx_rd_index0 == rx_wr_index0);
      data=rx_buffer0[rx_rd_index0];
      if (++rx_rd_index0 == RX_BUFFER_SIZE0) rx_rd_index0=0;
return data;
}

//__ramfunc void putchar0(char c)
void putchar0(char c)
{
      AT91PS_USART USART_pt = AT91C_BASE_US0;
      unsigned int status;
      //* get Usart status register
      status = USART_pt->US_CSR;
      if ((tx_rd_index0 != tx_wr_index0) | ((status & AT91C_US_TXRDY) == 0 ))
       {
        tx_buffer0[tx_wr_index0]=c;
        if (++tx_wr_index0 == TX_BUFFER_SIZE0) tx_wr_index0=0;
        AT91F_US_EnableIt(AT91C_BASE_US0, AT91C_US_TXRDY);
         // AT91C_US_TIMEOUT | AT91C_US_FRAME | AT91C_US_OVRE |AT91C_US_TXRDY);
       }
      else
      {
       AT91F_US_PutChar (USART_pt, c); 
      }
}


//*-------------------------- External Function -------------------------------
//----------------------------------------------------------------------------
// \fn    AT91F_US_Printk
// \brief This function is used to send a string through the US channel
//----------------------------------------------------------------------------
void AT91F_US_Put( char *buffer) // \arg pointer to a string ending by \0
{
	AT91PS_USART COM0 = AT91C_BASE_US0;
        
        while(*buffer != '\0') {
		while (!AT91F_US_TxReady(COM0));
		AT91F_US_PutChar(COM0, *buffer++);
	}
}

//*----------------------------------------------------------------------------
//* Function Name       : Usart_init
//* Object              : USART initialization
//* Input Parameters    : none
//* Output Parameters   : TRUE
//*----------------------------------------------------------------------------
void Usart_init ( void )
{
	AT91PS_USART COM0 = AT91C_BASE_US0;

        //* Configure PIO controllers to periph mode
 	AT91F_PIO_CfgPeriph( AT91C_BASE_PIOA,
 		((unsigned int) AT91C_PA0_RXD0    ) |
 		((unsigned int) AT91C_PA1_TXD0    ),  // Peripheral A
                0); // Peripheral B  
 	//	((unsigned int) AT91C_PA3_RTS0    ) |
 	//      ((unsigned int) AT91C_PA4_CTS0    ), // Peripheral A
 	//      0); // Peripheral B

   	// First, enable the clock of the USART
    	AT91F_PMC_EnablePeriphClock ( AT91C_BASE_PMC, 1 << AT91C_ID_US0 ) ;
	// Usart Configure
        AT91F_US_Configure (COM0, AT91B_MCK, AT91C_US_ASYNC_MODE, iCom0Speed, 0);

	// Enable usart
	COM0->US_CR = AT91C_US_RXEN | AT91C_US_TXEN;

    	//* Enable USART IT error and RXRDY
    	AT91F_US_EnableIt(COM0,AT91C_US_TIMEOUT | AT91C_US_FRAME | AT91C_US_OVRE |AT91C_US_RXRDY);

    	//* open Usart 1 interrupt
	AT91F_AIC_ConfigureIt ( AT91C_BASE_AIC, AT91C_ID_US0, USART_INTERRUPT_LEVEL,AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL, Usart_c_irq_handler);
	AT91F_AIC_EnableIt (AT91C_BASE_AIC, AT91C_ID_US0);
        
}

void com0_mode_set( int rate, char parity, char bit, char stop)
{
  AT91PS_USART COM0 = AT91C_BASE_US0;
  unsigned int mode, bit_mode, stop_mode, p_mode;

  if (bit == 8) bit_mode = AT91C_US_CHRL_8_BITS;
    else if (bit == 7) bit_mode = AT91C_US_CHRL_7_BITS;
    
  if (stop == 1) stop_mode = AT91C_US_NBSTOP_1_BIT;
    else if (stop == 2) stop_mode = AT91C_US_NBSTOP_2_BIT;
    
  if ((parity == 'N')|(parity == 'n')) p_mode = AT91C_US_PAR_NONE;
    else if ((parity == 'E')|(parity == 'e')) p_mode = AT91C_US_PAR_EVEN;
    else if ((parity == 'O')|(parity == 'o')) p_mode = AT91C_US_PAR_ODD;
 
  mode = AT91C_US_USMODE_NORMAL + stop_mode + p_mode + bit_mode + AT91C_US_CLKS_CLOCK;

        // Usart Configure
        AT91F_US_Configure (COM0, AT91B_MCK, mode, rate, 0);
	// Enable usart
	COM0->US_CR = AT91C_US_RXEN | AT91C_US_TXEN;
        //* Enable USART IT error and RXRDY
    	AT91F_US_EnableIt(COM0,AT91C_US_TIMEOUT | AT91C_US_FRAME | AT91C_US_OVRE |AT91C_US_RXRDY);
    	//* open Usart 0 interrupt
	AT91F_AIC_ConfigureIt ( AT91C_BASE_AIC, AT91C_ID_US0, USART_INTERRUPT_LEVEL,AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL, Usart_c_irq_handler);
	AT91F_AIC_EnableIt (AT91C_BASE_AIC, AT91C_ID_US0);
        //printf("\nCOM0:%6d,%1d,%1d,", rate, bit, stop); 
        //putchar_dp(parity);
}

void com0_buffer_clear(void)
{
  rx_rd_index0 = 0;
  rx_wr_index0 = 0;
  tx_rd_index0 = 0;
  tx_wr_index0 = 0;
}

