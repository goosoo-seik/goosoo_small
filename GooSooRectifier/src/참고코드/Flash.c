//  ----------------------------------------------------------------------------
//          ATMEL Microcontroller Software Support  -  ROUSSET  -
//  ----------------------------------------------------------------------------
//  DISCLAIMER:  THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
//  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
//  DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
//  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
//  OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
//  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//  ----------------------------------------------------------------------------
//* File Name           : Flash.h
//* Object              : Flash constan description
//* Creation            : JPP  30/Jun/2004
//*
//*----------------------------------------------------------------------------

/*-------------------------------*/
/* Flash Status Field Definition */
/*-------------------------------*/

#define AT91C_MC_FSR_MVM 	((unsigned int) 0xFF << 8)	// (MC) Status Register GPNVMx: General-purpose NVM Bit Status
#define AT91C_MC_FSR_LOCK 	((unsigned int) 0xFFFF << 16)	// (MC) Status Register LOCKSx: Lock Region x Lock Status
#define	 ERASE_VALUE 		0xFFFFFFFF

/*------------------------------*/
/* External function Definition */
/*------------------------------*/

/* Flash function */
extern void AT91F_Flash_Init(void);

extern __ramfunc int AT91F_Flash_Check_Erase(unsigned int * start, unsigned int size);
extern __ramfunc int AT91F_Flash_Erase_All(void);
extern __ramfunc int AT91F_Flash_Write( unsigned int Flash_Address ,int size ,unsigned int * buff, unsigned char MemSet);
extern __ramfunc int AT91F_Flash_Write_all( unsigned int Flash_Address ,int size ,unsigned int * buff);
/* Lock Bits functions */
extern __ramfunc int AT91F_Flash_Lock_Status(void);
extern __ramfunc int AT91F_Flash_Lock (unsigned int Flash_Lock);
extern __ramfunc int AT91F_Flash_Unlock(unsigned int Flash_Lock);
/* NVM bits functions */
extern __ramfunc int AT91F_NVM_Status(void);
extern __ramfunc int AT91F_NVM_Set (unsigned char NVM_Number);
extern __ramfunc int AT91F_NVM_Clear(unsigned char NVM_Number);


/* Security bit function */
extern int AT91F_SET_Security_Status (void);
extern int AT91F_SET_Security (void);

//  ----------------------------------------------------------------------------
//* File Name           : Flash.c
//* Object              : Flash routine
//* Creation            : JPP   30/Jun/2004
//* Modif               : JPM   16/Nov/2004 Flash write status
//*----------------------------------------------------------------------------
//#include "Board.h"
//#include "trace.h"
//#include "po_types.h"
//#include "po_kernel.h"
//#include "dbgu.h"

// Prototype:
__ramfunc int AT91F_Flash_Ready (void);

//*----------------------------------------------------------------------------
//* \fn    AT91F_Flash_Init
//* \brief Flash init
//*----------------------------------------------------------------------------
void AT91F_Flash_Init (void)
{
  //* Set Flash Waite sate
  //  Single Cycle Access at Up to 30 MHz, or 40
  //  if AT91C_MASTER_CLOCK = 47923200 I have 48 Cycle for 1 usecond ( flied MC_FMR->FMCN )
  //  48 x 1,5 = 72
  AT91F_MC_EFC_CfgModeReg( AT91C_BASE_MC, ((AT91C_MC_FMCN)&(72 <<16)) | AT91C_MC_FWS_1FWS );
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_Flash_Lock_Status
//* \brief Get the Lock bits field status
//*----------------------------------------------------------------------------
__ramfunc int AT91F_Flash_Lock_Status(void)
{
  return( AT91F_MC_EFC_GetStatus( AT91C_BASE_MC ) & AT91C_MC_FSR_LOCK );
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_Flash_Lock
//* \brief Write the lock bit and set at 0 FSR Bit = 1
//* \input page number (0-1023)
//* \output Region
//*----------------------------------------------------------------------------
__ramfunc int AT91F_Flash_Lock (unsigned int Flash_Lock_Page)
{
  //* write the flash
  //* Write the Set Lock Bit command
  AT91F_MC_EFC_PerformCmd( AT91C_BASE_MC, AT91C_MC_CORRECT_KEY | AT91C_MC_FCMD_LOCK | (AT91C_MC_PAGEN & (Flash_Lock_Page << 8) ) );

  //* Wait the end of command
  AT91F_Flash_Ready();
  return (AT91F_Flash_Lock_Status());
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_Flash_Check_Erase
//* \brief Check the memory at 0xFF in 32 bits access
//*----------------------------------------------------------------------------
__ramfunc int AT91F_Flash_Check_Erase (unsigned int * start, unsigned int size)
{
  unsigned int i;

  //* Check if flash is erased
  for (i=0; i < (size/4) ; i++ )
  {
    if ( start[i] != ERASE_VALUE )
    {
      return  false;
    }
  }
  return true ;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_Flash_Erase_All
//* \brief Send command erase all flash
//*----------------------------------------------------------------------------
__ramfunc int AT91F_Flash_Erase_All(void)
{
  //* Write the Erase All command
  AT91F_MC_EFC_PerformCmd( AT91C_BASE_MC, AT91C_MC_CORRECT_KEY | AT91C_MC_FCMD_ERASE_ALL );
  //* Wait the end of command
  AT91F_Flash_Ready();
  //* Check the result
  return( ( AT91F_MC_EFC_GetStatus( AT91C_BASE_MC ) & ( AT91C_MC_PROGE | AT91C_MC_LOCKE ))==0) ;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_Flash_Unlock
//* \brief Clear the lock bit and set at 1 FSR bit=0
//* \input page number (0-1023)
//* \output Region
//*----------------------------------------------------------------------------
__ramfunc int AT91F_Flash_Unlock(unsigned int Flash_Lock_Page)
{
  //* Write the Clear Lock Bit command
  AT91F_MC_EFC_PerformCmd( AT91C_BASE_MC, AT91C_MC_CORRECT_KEY | AT91C_MC_FCMD_UNLOCK | (AT91C_MC_PAGEN & (Flash_Lock_Page << 8) ) );

  //* Wait the end of command
  AT91F_Flash_Ready();
  return( AT91F_Flash_Lock_Status() );
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_Flash_Write
//* \brief Write in one Flash page located in AT91C_IFLASH,  size in 32 bits
//* \input Flash_Address: start at 0x0010 0000 size: in byte
//*----------------------------------------------------------------------------
__ramfunc int AT91F_Flash_Write( unsigned int Flash_Address ,int size ,unsigned int * buff, unsigned char Memset)
{
  unsigned int i, page, status;
  unsigned int * Flash;

  //* init flash pointer
  Flash = (unsigned int *) Flash_Address;
  //* Get the Flash page number
  page = ((Flash_Address - (unsigned int)AT91C_IFLASH ) /FLASH_PAGE_SIZE_BYTE);

  TRACE_DEBUG_H( "FL Ad(0x%X)    ", Flash);
  TRACE_DEBUG_H( "FL Ad(0x%X)    ", Flash_Address);
  TRACE_DEBUG_H( "Lg(%d)\n\r", size);

  //po_lock();

  //* copy the new value
  for (i=0; (i < FLASH_PAGE_SIZE_BYTE) & (size > 0); i++,Flash++,buff++,size-=4 )
  {
    //* copy the flash to the write buffer ensuring code generation
    if( FALSE == Memset )
    {
      *Flash=*buff;
    }
    else
    {
      *Flash=buff[0];
      buff--;
    }
  }
  //* Write the write page command
  AT91F_MC_EFC_PerformCmd( AT91C_BASE_MC, AT91C_MC_CORRECT_KEY | AT91C_MC_FCMD_START_PROG | (AT91C_MC_PAGEN & (page <<8)) );

  //* Wait the end of command
  status = AT91F_Flash_Ready();

  //po_unlock();

  //* Check the result
  if ( (status & ( AT91C_MC_PROGE | AT91C_MC_LOCKE )) != 0)
  {
    return false;
  }
  return true;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_Flash_Write_all
//* \brief Write in one Flash page located in AT91C_IFLASH,  size in byte
//* \input Start address (base=AT91C_IFLASH) size (in byte ) and buff address
//*----------------------------------------------------------------------------
__ramfunc int AT91F_Flash_Write_all( unsigned int Flash_Address ,int size ,unsigned int * buff)
{
  int   next, status;
  unsigned int  dest;
  unsigned int * src;

  dest = Flash_Address;
  src = buff;
  status = true;

  while( (status == true) & (size > 0) )
  {
    //* Check the size
    if (size <= FLASH_PAGE_SIZE_BYTE)
      next = size;
    else
      next = FLASH_PAGE_SIZE_BYTE;

    //* Write page and get status
    status = AT91F_Flash_Write( dest ,next ,src, FALSE);

    // * get next page param
    size -= next;
    src += FLASH_PAGE_SIZE_BYTE/4;
    dest +=  FLASH_PAGE_SIZE_BYTE;
  }
  return status;
}


//*----------------------------------------------------------------------------
//* \fn    AT91F_Flash_Ready
//* \brief Wait the flash ready
//*----------------------------------------------------------------------------
__ramfunc int AT91F_Flash_Ready (void)
{
  unsigned int status;
  status = 0;

  //* Wait the end of command
  while ((status & AT91C_MC_FRDY) != AT91C_MC_FRDY )
  {
    status = AT91F_MC_EFC_GetStatus( AT91C_BASE_MC );
  }
  return status;
}



