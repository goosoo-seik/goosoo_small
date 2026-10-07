/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** SD.C
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
** This file contains routines for the Serial Interface
**
**
**
********************************************************************************
********************************************************************************
**                                                                            **
** COPYRIGHT NOTIFICATION (c) 1995,96,97,98,99 HMS Industrial Networks AB.    **
**                                                                            **
** This program is the property of HMS Industrial Networks AB.                **
** It may not be reproduced, distributed, or used without permission          **
** of an authorised company official.                                         **
**                                                                            **
********************************************************************************
********************************************************************************
**
** Company: HMS Industrial Networks AB
**          Pilefeltsgatan 93-95
**          S-302 50  Halmstad
**          SWEDEN
**          Tel:     +46 (0)35 - 17 29 00
**          Fax:     +46 (0)35 - 17 29 09
**          e-mail:  info@hms.se
**
********************************************************************************
********************************************************************************
*/

/*******************************************************************************
********************************************************************************
**
** Change Log
** ----------
**
** Latest Revision:
**
**    Rev 0.10    12 jun 2002   Created by AnN
**    Rev 1.00    12 jul 2002   First Release
**
********************************************************************************
********************************************************************************
*/

#include "reg517a.h"
#include "fd.h"
#include "sd.h"
#include "tm.h"


/*******************************************************************************
**
** Private Globals
**
********************************************************************************
*/

sd_DataType sd_s;


/*******************************************************************************
**
** Public Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
** SD_Init()
**------------------------------------------------------------------------------
*/

void SD_Init( void )
{

   /*
   ** Disable Serial Interrupt
   */

   ES0 = 0;

   /*
   ** Setup baud generator and UART
   */

   PCON = 0x080;
   S0CON = 0x0D8;          /* Set serial interface in mode 3             */
   S0RELL = S0RELL_9600;   /* Set the baudrate to                        */
   S0RELH = S0RELH_9600;   /* 9600 bit/s                                 */
   BD = 1;                 /* Enable baud generator                      */
   TI0 = 0;                /* Reset Timer 0 interrupt flag               */
   RI0 = 0;                /* Reset receive interrupt flag               */
   TMOD = 0x01;            /* Timer0=16 Bit Timer                        */
   ET0 = 1;                /* Enable timer interrupt                     */
   REN0 = 1;               /* Enable serial interface                    */


   /*
   ** Enable Serial Interrupt
   */

   ES0 = 1;


}/* end SD_Init */


/*------------------------------------------------------------------------------
** SD_PutByte()
**------------------------------------------------------------------------------
*/

void SD_PutChar( UINT8 bByte )
{

   /*
   ** Disable Serial Interrupt
   */

   ES0 = 0;

   /*
   ** Clear transmit interrupt
   */

   TI0 = 0;

   /*
   ** Byte to send buffer
   */

   S0BUF = bByte;

   /*
   ** Wait until Byte has sent
   */

   while (!TI0)
   ;

   /*
   ** Clear transmit interrupt
   */

   TI0 = 0;

   /*
   ** Enables Serial Interrupt
   */

   ES0 = 1;

}/* end SD_PutChar */


/*------------------------------------------------------------------------------
** SD_GetBuffer()
**------------------------------------------------------------------------------
*/

UINT8 SD_GetChar( void )
{

   UINT8 bReturnChar;

   if( sd_s.iRxSize == 0 )
   {

      /*
      ** There is no characters in the buffer.
      */

      return 0;

   }/* end if */

   bReturnChar = sd_s.abRxBuffer[ sd_s.iRxGet ];


   /*
   ** Take a step so we point to the next character in the rx buffer, then
   ** decrement the usage of the buffer indicating that we have seen the
   ** character.
   */

   sd_s.iRxGet++;
   sd_s.iRxSize--;


   /*
   ** Handle the rotation of our ring buffer
   */

   if( sd_s.iRxGet == SD_RX_BUFFER_SIZE )
   {

      sd_s.iRxGet = 0;

   }/* end if */

   return bReturnChar;


}/* end SD_GetChar */


/*------------------------------------------------------------------------------
** SD_CharReceived()
**------------------------------------------------------------------------------
*/

BOOL SD_CharReceived( void )
{

   /*
   ** Check if there are any characters in the receive buffer.
   */

   if( sd_s.iRxSize == 0 )
   {

      return FALSE;

   }
   else
   {

      return TRUE;

   }/* end if */

} /* end of SD_CharReceived() */



/*******************************************************************************
**
** Private Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
** sd_RxInterrupt()
**------------------------------------------------------------------------------
*/

void sd_RxInterrupt( void ) interrupt 4 using 1
{

   /*
   ** Wait until Serial Interrupt can be handled
   */

   while(!RI0)
   ;

   /*
   ** Get the character from the input "buffer"
   */

   sd_s.abRxBuffer[ sd_s.iRxPut ] = S0BUF;

   /*
   ** go to the next possition in the rx buffer
   */

   sd_s.iRxSize++;
   sd_s.iRxPut++;

   /*
   ** Handle the rotation of our ring buffer
   */

   if( sd_s.iRxPut == SD_RX_BUFFER_SIZE )
   {

      sd_s.iRxPut = 0;

   }/* end if end of buffer */

   /*
   ** Reset serial interrupt
   */

   RI0 = 0;

   /*
   ** Set timeout to 3.5 char to know when a modbus message ends.
   */

   TM_SetTimer( MB_MESSAGE_END_9600 );

   /*
   ** Timer start
   */

   TR0=1;

}/* end sd_RxInterrupt */


/*******************************************************************************
**
** End of SD.C
**
********************************************************************************
*/