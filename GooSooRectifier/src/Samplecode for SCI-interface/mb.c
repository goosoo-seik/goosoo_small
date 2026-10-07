/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** MB.C
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
** This file contains routines for sending Modbus messages
** on the serial interface
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

#include "fd.h"
#include "mb.h"
#include "sd.h"
#include "tm.h"
#include "crc.h"


/*******************************************************************************
**
** Public Globals
**
********************************************************************************
*/

UINT8 MB_bCRCCounter;
UINT8 MB_bTimeOutCounter;


/*******************************************************************************
**
** Private Globals
**
********************************************************************************
*/

UINT16 MB_iTimeOutTime = MB_DEFAULT_TIMEOUT;


/*******************************************************************************
**
** Public Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
** MB_Init()
**------------------------------------------------------------------------------
*/

void MB_SetTimeout( UINT16 iTime )
{

   /*
   ** Set Timeout time
   */

   MB_iTimeOutTime = iTime;


}/* end of MB_Init() */


/*------------------------------------------------------------------------------
** MB_SendModbusMessage()
**------------------------------------------------------------------------------
*/

UINT8 MB_SendRecModbusMessage( UINT8* pbData, UINT8 bSize, UINT8* pbResponse )
{

   UINT8 abSendBuffer[ 30 ];
   UINT16 iCrc;
   UINT8 bCharPos = 0;


   /*
   ** Check if the message is to long
   */

   if( bSize > 249 )
   {

      return( 0 );

   }/* end if message to long */

   /*
   ** Copy Data To send buffer
   */

   for( bCharPos = 0 ; bCharPos < bSize ; bCharPos++ )
   {

      abSendBuffer[ bCharPos ] = pbData[ bCharPos ];

   }/* end for */

   /*
   ** Generate CRC for the message to send
   */

   iCrc = GenerateCrc( abSendBuffer, bCharPos );

   /*
   ** Add CRC to the end of the message to send
   */

   abSendBuffer[ bCharPos ] = ( iCrc >> 8 ) & 0x00FF;
   abSendBuffer[ bCharPos + 1] = iCrc & 0x00FF;

   /*
   ** Send Modbus Request
   */

   for( bCharPos = 0; bCharPos < ( bSize + 2 ) ; bCharPos++ )
   {

      SD_PutChar( abSendBuffer[ bCharPos ] );

   }/* end for */


   TM_SetTimer( MB_iTimeOutTime );
   TM_StartTimer();


   /*
   ** Wait for repsonse to arrive or timeout
   */

   while( !TM_TimeOut() )
   ;


   TM_StopTimer();

   /*
   ** Read response
   */

   bCharPos = 0;

   while( SD_CharReceived() )
   {

      pbResponse[ bCharPos ] = SD_GetChar();

      bCharPos++;

   }/* end while rx buffer not emty */


   if( bCharPos == 0 )
   {

      MB_bTimeOutCounter++;

   }
   else if( bCharPos == 1 )
   {

      MB_bCRCCounter++;

   }/* end if */


   if( bCharPos > 1 )
   {

      /*
      ** Generate CRC for the response message
      */

      iCrc = GenerateCrc( pbResponse, bCharPos - 2 );


      /*
      ** Check CRC
      */

      if( ( ( (UINT8)( iCrc >> 8 ) ) == pbResponse[ bCharPos - 2 ] ) &&
          ( ( (UINT8)iCrc ) == pbResponse[ bCharPos - 1 ] ) )
      {

         /*
         ** Correct CRC return received length
         */

         return( bCharPos - 2 );

      }
      else
      {


         /*
         ** CRC error
         */

         MB_bCRCCounter++;

      }/* end if right crc */

   }/* end if response received */


   /*
   ** We failed to get a error free response
   */

   return( 0 );


}/* end MB_SendRecModbusMessage */


/*******************************************************************************
**
** End of MB.C
**
********************************************************************************
*/