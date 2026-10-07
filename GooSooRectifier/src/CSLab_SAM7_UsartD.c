

#define BYTE  unsigned short
// Include Standard LIB  files
#include "project.h"

#define DBGU_INTERRUPT_LEVEL		7
#define PC_BAUD_RATE			38400	//115200          

//* \fn    AT91F_DBGU_Baudrate
//* Standard Asynchronous Mode : 8 bits , 1 stop , no parity

// USART_D Receiver buffer
#define RX_BUFFER_SIZE2 256
BYTE  rx_buffer2[RX_BUFFER_SIZE2];
short rx_wr_index2,rx_rd_index2,rx_counter2;
// This is USART2 error
char ErrUsart2;


// USART_D Transmitter buffer
#define TX_BUFFER_SIZE2 1024
unsigned char  tx_buffer2[TX_BUFFER_SIZE2];
short tx_wr_index2,tx_rd_index2,tx_counter2;
// This flag is set on USART2 Receiver buffer overflow
//char tx_buffer_overflow2;

//*----------------------------------------------------------------------------
//* \fn    AT91F_DBGU_SetBaudrate
//* \brief Set the baudrate according to the CPU clock
//*----------------------------------------------------------------------------
__inline void AT91F_DBGU_SetBaudrate (
        AT91PS_DBGU pUSART,
	unsigned int speed)     // \arg UART baudrate
{
  unsigned int baud_value;
        baud_value = (AT91B_MCK/(speed * 16));
	//* Define the baud rate divisor register
	pUSART->DBGU_BRGR =  baud_value;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_DBGU_GetChar
//* \brief Receive a character,does not check if a character is available
//*----------------------------------------------------------------------------
__inline int AT91F_DBGU_GetChar (
	const AT91PS_DBGU pUSART)
{
    return((pUSART->DBGU_RHR) & 0x1FF);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_DBGU_PutChar
//* \brief Send a character,does not check if ready to send
//*----------------------------------------------------------------------------
__inline void AT91F_DBGU_PutChar (
	AT91PS_DBGU pUSART,
	int character )
{
    pUSART->DBGU_THR = (character & 0x1FF);
}

//*------------------------- Internal Function --------------------------------

//*----------------------------------------------------------------------------
//* Function Name       : Usart2_c_irq_handler
//* Object              : C handler interrupt function calAT91B_LED by the interrupts
//*                       assembling routine
//* Input Parameters    : <RTC_pt> time rtc descriptor
//* Output Parameters   : increment count_timer1_interrupt
//*----------------------------------------------------------------------------

//__ramfunc void Usart2_c_irq_handler(void)
void Usart2_c_irq_handler(void)
{
unsigned char  data;
        //GRNlamp(ON);
	AT91PS_DBGU USART_pt = AT91C_BASE_DBGU;
	unsigned int status;
	//* get Usart status register
	status = USART_pt->DBGU_CSR;
        
	if ( status & AT91C_US_RXRDY)   // (DBGU) RXRDY Interrupt
         {    
          data = AT91F_DBGU_GetChar(USART_pt);
          //AT91F_DBGU_PutChar (USART_pt, data);    // Echo
          rx_buffer2[rx_wr_index2]=data;
          ++rx_wr_index2;
          rx_wr_index2 &= 0xFF;
          //if (rx_wr_index2 == rx_rd_index2) ErrUsart2 = 'B';
	 }
        
        if ( status & AT91C_US_TXRDY)
         {
          if ( tx_rd_index2 != tx_wr_index2 ) 
           {
            AT91F_DBGU_PutChar (USART_pt, tx_buffer2[tx_rd_index2]);
            if (++tx_rd_index2 == TX_BUFFER_SIZE2) tx_rd_index2=0;
           }
          else 
           AT91F_DBGU_InterruptDisable(AT91C_BASE_DBGU,AT91C_US_TXRDY);    
         }
	if ( status & AT91C_US_OVRE) {
		//* clear DBGU_RXRDY
		 AT91F_DBGU_GetChar(USART_pt);
                 ErrUsart2 = 'O';
		 //AT91F_DBGU_PutChar (USART_pt, 'O');
	}

	//* Check error
	if ( status & AT91C_US_PARE) {
                ErrUsart2 = 'P';
		//AT91F_DBGU_PutChar (USART_pt, 'P');
	}

	if ( status & AT91C_US_FRAME) {
                ErrUsart2 = 'F';
		//AT91F_DBGU_PutChar (USART_pt, 'F');
	}
	//* Reset the satus bit
	 USART_pt->DBGU_CR = AT91C_US_RSTSTA;
}

// Get a character from the USART1 Receiver buffer
char getchar2(void)
{
 char data;
      while (rx_rd_index2 == rx_wr_index2);
      data=rx_buffer2[rx_rd_index2];
      if (++rx_rd_index2 == RX_BUFFER_SIZE2) rx_rd_index2=0;
return data;
}

//__ramfunc void putchar2(char c)
void putchar2(char c)
{
      AT91PS_DBGU USART_pt = AT91C_BASE_DBGU;
      unsigned int status;
      //* get Usart status register
      status = USART_pt->DBGU_CSR;
      if ((tx_rd_index2 != tx_wr_index2) | ((status & AT91C_US_TXRDY) == 0 ))
       {
        tx_buffer2[tx_wr_index2]=c;
        if (++tx_wr_index2 == TX_BUFFER_SIZE2) tx_wr_index2=0;
        AT91F_DBGU_InterruptEnable(AT91C_BASE_DBGU,AT91C_US_TXRDY);
       }
      else
      {
       AT91F_DBGU_PutChar (USART_pt, c); 
      }
}


//*-------------------------- External Function -------------------------------

//*----------------------------------------------------------------------------
//* Function Name       : Usart_init
//* Object              : USART initialization
//* Input Parameters    : none
//* Output Parameters   : TRUE
//*----------------------------------------------------------------------------
void Usart2_init ( void )
{
	AT91PS_DBGU COM2 = AT91C_BASE_DBGU;

        //* Configure PIO controllers to drive DBGU signals
        AT91F_DBGU_CfgPIO();
        
   	// First, Enable Peripheral clock in PMC for  DBGU
    	AT91F_DBGU_CfgPMC();
        
        // UART baudrate
        AT91F_DBGU_SetBaudrate (COM2, PC_BAUD_RATE);
       
	// Enable usart
	COM2->DBGU_CR = AT91C_US_RXEN | AT91C_US_TXEN;

        //* AT91F_DBGU_InterruptEnable
        AT91F_DBGU_InterruptEnable(COM2,AT91C_US_TIMEOUT | AT91C_US_FRAME | AT91C_US_OVRE |AT91C_US_RXRDY);
          
        //* open Usart 2 interrupt
	AT91F_AIC_ConfigureIt ( AT91C_BASE_AIC, AT91C_ID_SYS, DBGU_INTERRUPT_LEVEL,AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL, Usart2_c_irq_handler);
	AT91F_AIC_EnableIt (AT91C_BASE_AIC, AT91C_ID_SYS);

	//AT91F_DBGU_PutChar (COM2,'X');
}

/***********************************/
/*   Calculate Tx2 buffer space    */
/***********************************/
short CheckTx2Space(void)
{
  short space;
  if (tx_wr_index2 == tx_rd_index2) space = TX_BUFFER_SIZE2;
   else if (tx_wr_index2 < tx_rd_index2) space = tx_rd_index2 - rx_wr_index2;
   else space = TX_BUFFER_SIZE2 - (tx_wr_index2 - tx_rd_index2);
   return space;
}

void com2_mode_set( int rate)
{
  AT91PS_DBGU COM2 = AT91C_BASE_DBGU;
  
	// UART baudrate set
        // com2(USART_DBG) is cannot change Parity, Bit, StopBit
        AT91F_DBGU_SetBaudrate (COM2, rate);  
        
        // Enable usart
	COM2->DBGU_CR = AT91C_US_RXEN | AT91C_US_TXEN;
        
        //* AT91F_DBGU_InterruptEnable
        AT91F_DBGU_InterruptEnable(COM2,AT91C_US_TIMEOUT | AT91C_US_FRAME | AT91C_US_OVRE |AT91C_US_RXRDY);
  	
        //* open Usart 2 interrupt
	AT91F_AIC_ConfigureIt ( AT91C_BASE_AIC, AT91C_ID_SYS, DBGU_INTERRUPT_LEVEL,AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL, Usart2_c_irq_handler);
	AT91F_AIC_EnableIt (AT91C_BASE_AIC, AT91C_ID_SYS);
        
        // Setting status indicate
	printf("\nCOM2:%6d", rate); 
}

void com2_buffer_clear(void)
{
  rx_rd_index2 = 0;
  rx_wr_index2 = 0;
  tx_rd_index2 = 0;
  tx_wr_index2 = 0;
}
