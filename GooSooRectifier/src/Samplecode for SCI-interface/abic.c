/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** ABIC.C
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
** This file contains functions for interfacing the AnyBus-IC
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

#include "fd.h"
#include "sd.h"
#include "mb.h"
#include "abic.h"

/*******************************************************************************
**
** Public Services
**
********************************************************************************
*/


/*------------------------------------------------------------------------------
** ABIC_AutoBaud()
**------------------------------------------------------------------------------
*/

UINT8 ABIC_AutoBaud( void )
{

   UINT8 abResponse[ 10 ];
   UINT8 abRequest[ 6 ] = { 0x01, 0x03, 0x50, 0x01, 0x00, 0x01 } ;
   UINT8 bRetryCount;

   /*
   ** Do 15 tries to make the AnyBus-IC Autobaud.
   ** If no succes return FALSE
   */

   for( bRetryCount = 0; bRetryCount < 15 ; bRetryCount++ )
   {

      /*
      ** We send a request to read modbus address 0x5001 ( Parameter #1 )
      ** The response should be 6 byte long.
      */

      if( MB_SendRecModbusMessage( abRequest, 6, abResponse) == 5 )
      {

         /*
         ** We got an response on the AutoBaud Message,
         ** the AutoBaud is complete. Return TRUE,
         */

         return( TRUE );

      }/* end if Response received */

   }/* end for */

   return( FALSE );

}/* end AutoBaud */


/*------------------------------------------------------------------------------
** ABIC_NormalMode()
**------------------------------------------------------------------------------
*/

BOOL ABIC_NormalMode( void )
{

   UINT8 abResponse[ 10 ];
   UINT8 abSetRequest[ 6 ] = { 0x01, 0x06, 0x50, 0x01, 0x00, 0x01 } ;
   UINT8 abCheckRequest[ 6 ] = { 0x01, 0x04, 0x50, 0x01, 0x00, 0x01 } ;


   /*
   ** Try to AnyBus-IC in Normal mode
   */

   if( MB_SendRecModbusMessage( abSetRequest, 6, abResponse ) != 6 )
   {

      return( FALSE );

   }/* end if */


   /*
   ** Check if the AnyBus-IC is in Normal mode.
   ** For different reasons the attemt to set it in normal mode can
   ** have failed. For example faulty IO configuration.
   */

   if( MB_SendRecModbusMessage( abCheckRequest, 6, abResponse ) == 6 )
   {

      /*
	  ** Byte 4 and 5 contains the parameter value and it should be 0x0001
	  */

      if( abResponse[ 4 ] == 0 && abResponse[ 5 ] == 1 )
      {

         return( TRUE );

      }/* end if */

   }/* end if */


   /*
   ** We faild to read the current mode
   */

   return( FALSE );


}/* end ABIC_NormalMode */


/*------------------------------------------------------------------------------
** ABIC_ReadOutData()
**------------------------------------------------------------------------------
*/

BOOL ABIC_ReadOutData( UINT8 bOffset, UINT8 bSize, UINT8* pData )
{

   UINT8 bCount;
   UINT8 bReadSize;
   UINT8 abRequest[ 6 ];
   UINT8 abResponse[ 10 ];

   /*
   ** Set up Read Input registers request
   */

   abRequest[ 0 ] = 0x01;           /* Modbus Address           */
   abRequest[ 1 ] = 0x04;           /* Modbus Function Code     */
   abRequest[ 2 ] = 0x10;           /* Modbus Address High Byte */
   abRequest[ 3 ] = bOffset;        /* Modbus Address Low Byte  */
   abRequest[ 4 ] = 0x00;           /* No. of Points High       */
   abRequest[ 5 ] = bSize;          /* No. of Points Low        */

   /*
   ** Send the Request
   */

   bReadSize = MB_SendRecModbusMessage( abRequest, 6, abResponse ) - 3;

   /*
   ** Check if we received the amount of data we requested.
   ** If we did, copy it to data buffer
   */

   if( bReadSize == ( bSize * 2 ) )
   {

      for( bCount = 0; bCount < ( bSize * 2 ) ; bCount++ )
      {

         pData[ bCount ] = abResponse[ bCount + 3 ];

      }/* end for */


      return TRUE;

   }/* end if right size read */


   return FALSE;

}/* end ABIC_ReadOutData */



/*------------------------------------------------------------------------------
** ABIC_WriteInData()
**------------------------------------------------------------------------------
*/

BOOL ABIC_WriteInData( UINT8 bOffset, UINT8 bSize, UINT8* pData )
{

   UINT8 bCount;
   UINT8 abRequest[ 11 ];
   UINT8 abResponse[ 10 ];

   /*
   ** Set up Preset Multiple Registers request
   */

   abRequest[ 0 ] = 0x01;           /* Modbus Address           */
   abRequest[ 1 ] = 0x10;           /* Modbus Function Code     */
   abRequest[ 2 ] = 0x00;           /* Starting Address High    */
   abRequest[ 3 ] = bOffset;        /* Starting Address Low     */
   abRequest[ 4 ] = 0x00;           /* No. of Registers High    */
   abRequest[ 5 ] = bSize;          /* No. of Registers Low     */
   abRequest[ 6 ] = bSize * 2;      /* Byte Count               */

   /*
   ** Copy Data to Request
   */

   for( bCount = 0; bCount < ( bSize * 2 ) ; bCount++ )
   {

      abRequest[ bCount + 7 ] = pData[ bCount ];

   }/* end for */

   /*
   ** Send the Modbus Request and check if the response size is
   ** the length of a Preset Multiple Register Response ( 6 )
   */

   if( MB_SendRecModbusMessage( abRequest, 7 + ( 2 * bSize ), abResponse ) == 6 )
   {


      return( TRUE );

   }/* end if command succeded */


   return( FALSE );


}/* end ABIC_WriteInData */


/*******************************************************************************
**
** End of ABIC.C
**
********************************************************************************
*/
