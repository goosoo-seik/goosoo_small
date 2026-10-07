/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** MAIN.C
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
** This file contains the main function for the AnyBus-IC sample code.
** This example runs on a AnyBus-IC preconfigured to have a 2 byte input and
** 2 bytes output data on the SCI interface.
** This sample code should be considured as an example on how to communicate
** with the AnyBus-IC and it is not a drive routine.
** HMS Industrial Networks AB is not responsible for any damages caused by
** this program.
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
#include "tm.h"
#include "sd.h"
#include "lcd.h"
#include "abic.h"

/*******************************************************************************
**
** Public Globals
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** ABIC_MonitorSwitch
**
**------------------------------------------------------------------------------
**
** Switch "Monitor"
**
**------------------------------------------------------------------------------
*/

sbit ABIC_MonitorSwitch = 0x91;

/*------------------------------------------------------------------------------
**
** ABIC_StepSwitch
**
**------------------------------------------------------------------------------
**
** Switch "STEP"
**
**------------------------------------------------------------------------------
*/

sbit ABIC_StepSwitch = 0x90;


/*------------------------------------------------------------------------------
**
** ABIC_IntPin
**
**------------------------------------------------------------------------------
**
** AnyBus Int signal
**
**------------------------------------------------------------------------------
*/

sbit ABIC_IntPin = 0xB3;


/*******************************************************************************
**
** Public Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
** AD_GetValue()
**------------------------------------------------------------------------------
*/

UINT8 AD_GetValue( UINT8 bChannel )
{

   if( bChannel == 0 )
   {

      ADCON1 = 0x07;

   }
   else
   {

       ADCON1 = 0x06;

   }/* end if */

   ADDATL = 0x0;               /* Wandlung Start */

   while( BSY )
   ;                           /* warten bis Wandlung fertig */

   return( ADDATH );

}/* end AD_GetValue */


/*------------------------------------------------------------------------------
** main()
**------------------------------------------------------------------------------
*/

void main( void )
{

   BOOL fResult;
   UINT8 bDisplayState = 0;
   UINT8 abInData[ 2 ];
   UINT8 abOutData[ 2 ];

   UINT8 bLoopCounter = 0;
   UINT8 bWriteErrorCounter = 0;
   UINT8 bReadErrorCounter = 0;

   /*
   ** Initiate Display
   */
   LCD_Init( );
   LCD_SelectRow( 0 );
   LCD_ClearDisplay();
   LCD_SelectRow( 1 );
   LCD_ClearDisplay();


   /*
   ** Initiate Serial Driver
   */
   SD_Init();

   /*
   ** Make the AnyBus-IC autodetect our Baud Rate
   */
   fResult = ABIC_AutoBaud();


   if( !fResult )
   {

      /*
      ** The baud rate auto detection failed.
      ** Write error message and stay here for ever
      */

      LCD_SelectRow( 0 );
      LCD_PrintString( "ERROR!");
      LCD_SelectRow( 1 );
      LCD_PrintString( "AutoBaud fault!");

      while( 1 )
      ;

   }/* end if */


   /*
   ** When the AnyBus-IC is configured and ready we will
   ** have to start the data excange by setting the Module Mode
   ** parameter ( #1 ) to 1 ( Normal Operation ).
   ** In this sample we assume that the configuration already are done
   ** and stored in flash. Then the only thing we have to do is to set
   ** the module in Normal Operational mode.
   */

   if( ABIC_NormalMode() )
   {

      /*
      ** The baud rate auto detection failed.
      ** Write error message and stay here for ever
      */

      LCD_SelectRow( 0 );
      LCD_PrintString( "ERROR!");
      LCD_SelectRow( 1 );
      LCD_PrintString( "Set Mode Fault!");

      while( 1 )
      ;

   }/* end if */


   while( 1 )
   {

      /*
      ** Get indata values from the CPU AD converter
      */
      abInData[ 0 ] = AD_GetValue( 0 );
      abInData[ 1 ] = AD_GetValue( 1 );


      /*
      ** Write the In Data values to the AnyBus-IC
      */
      ABIC_WriteInData( 0, 1, abInData );


      /*
      ** Print the In Data values on the display.
      */
      LCD_SelectRow( 0 );
      LCD_PrintString( " In Data: ");
      LCD_PrintHex( abInData[ 0 ]  );
      LCD_PrintString( " ");
      LCD_PrintHex( abInData[ 1 ]  );


      LCD_SelectRow( 1 );
      LCD_PrintString( "Out Data: ");


      /*
      ** Read the Out Data values from the AnyBus-IC
      */
      fResult = ABIC_ReadOutData( 0, 1, abOutData );

      if( fResult )
      {

         /*
         ** Print the Out Data values on the display
         */
         LCD_PrintHex( abOutData[ 0 ]  );
         LCD_PrintString( " ");
         LCD_PrintHex( abOutData[ 1 ]  );

      }
      else
      {

         /*
         ** We could not get correct Out Data
         */
         LCD_PrintString( "-- -- ");

      }/* end if */

   }/* end main loop */

}/* end main */


/*******************************************************************************
**
** End of MAIN.C
**
********************************************************************************
*/