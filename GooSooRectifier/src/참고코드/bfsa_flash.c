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
// File Name           : bfsa_flash.c
// Object              : bfsa_flash routine
// Creation            : FB  10/feb/2005
// Modif               : JCB 15/apr/2005
//----------------------------------------------------------------------------
#include "board.h"
#include "po_types.h"
#include "trace.h"
#include "po_kernel.h"
#include "flash.h"
#include "bfsa_flash.h"
#include "dbgu.h"


//----------------------------------------------------------------------------
// \fn    AT91F_ReadMedia
// \brief Read in Flash size in byte
// \input Start address (base=AT91C_IFLASH) size (in byte ) and buff address
//----------------------------------------------------------------------------
#ifdef ADS_COMPIL
void AT91F_ReadMedia(unsigned int Flash_Address, int size, unsigned char *buff)
#else
__ramfunc void AT91F_ReadMedia(unsigned int Flash_Address, int size, unsigned char *buff)
#endif
{
#ifdef USE_LED
  AT91F_LED_blinkSpeed(0);
#endif
  po_memcpy((void *)buff,(void *)Flash_Address, size);
}

//----------------------------------------------------------------------------
// \fn    AT91F_Flash_Write_Address
// \brief Write in Flash size in byte
// \input Start address (base=AT91C_IFLASH) size (in byte ) and buff address
//----------------------------------------------------------------------------
#ifdef ADS_COMPIL
int AT91F_Flash_Write_Address( unsigned int Flash_Address, int size, unsigned char *buff, unsigned char MemSet)
#else
__ramfunc int AT91F_Flash_Write_Address( unsigned int Flash_Address, int size ,unsigned char *buff, unsigned char MemSet)
#endif
{
  int status;
  unsigned int dest;
  unsigned char *src;
  unsigned char pPage[FLASH_PAGE_SIZE_BYTE];
  unsigned int page;
  unsigned int pageStartAdd;
  unsigned int count;

  dest = Flash_Address;
  src = buff;
  status = true;

  while( (status == true) & (size > 0) )
  {
    // Compute current page
    page = (dest - (unsigned int)BASE_IFLASH ) /FLASH_PAGE_SIZE_BYTE;
    pageStartAdd = (page * FLASH_PAGE_SIZE_BYTE) + (unsigned int)BASE_IFLASH;
    // Check if we start to write on start of the page
    if (dest != pageStartAdd)
    {
      unsigned int test = (dest-pageStartAdd);
      unsigned int test2 = size+test;
      unsigned int test3 = min(FLASH_PAGE_SIZE_BYTE,test2);
      // We need to load all the page and made the modification in and finally copy it on flash
      for(count = 0; count<test; count++)
      {
        pPage[count] = *((unsigned char*)(pageStartAdd + count));
      }
      while(count<test3)
      {
        pPage[count] = *src;
        if( FALSE == MemSet )
        {
          src++;
        }
        count++;
        if (count>=test3)
          break;
      }
      while(count<FLASH_PAGE_SIZE_BYTE)
      {
        pPage[count] = *((unsigned char*)(pageStartAdd + count));
        count++;
      }
      // Write page and get status
      TRACE_DEBUG_H( "ADD1(0x%X)    ", pageStartAdd);
      TRACE_DEBUG_H( "Lg(%d)\n\r", FLASH_PAGE_SIZE_BYTE);

      // Unlock current sector base address - current address by sector size
      AT91F_Flash_Unlock(page);


      //jcb pPage ...
      status = AT91F_Flash_Write( pageStartAdd, FLASH_PAGE_SIZE_BYTE, (unsigned int*)pPage, FALSE);

      // get next page param
      size -= min(size,FLASH_PAGE_SIZE_BYTE);
      dest = ((page+1) * FLASH_PAGE_SIZE_BYTE) + (unsigned int)BASE_IFLASH; // point to the next page boundary
    }
    else
    {
      // Check if end of block is a page boundary
      if (size < FLASH_PAGE_SIZE_BYTE)
      {
        // it is not, we need to load the page, change and copy it to flash
        for (count=0;count<size;count++)
        {
          pPage[count] = *src;
          if( FALSE == MemSet )
          {
            src++;
          }
        }
        for (;count<FLASH_PAGE_SIZE_BYTE;count++)
        {
          pPage[count] = *((unsigned char*)(pageStartAdd + count));
        }
        // Write page and get status
        TRACE_DEBUG_H( "ADD2(0x%X)    ", pageStartAdd);
        TRACE_DEBUG_H( "Lg(%d)\n\r", FLASH_PAGE_SIZE_BYTE);

        // Unlock current sector base address - current address by sector size
        AT91F_Flash_Unlock(page);

        status = AT91F_Flash_Write( pageStartAdd, FLASH_PAGE_SIZE_BYTE, (unsigned int*)pPage, FALSE);

        // end of writing
        break;
      }
      else
      {
        // copy one page full!

        // Write page and get status
        TRACE_DEBUG_H( "ADD3(0x%X)    ", dest);
        TRACE_DEBUG_H( "Lg(%d)    ", FLASH_PAGE_SIZE_BYTE);

        // Unlock current sector base address - current address by sector size
        AT91F_Flash_Unlock(page);

        status = AT91F_Flash_Write( dest, FLASH_PAGE_SIZE_BYTE, (unsigned int*)src, MemSet);

        TRACE_DEBUG_H( "status(0x%X)\n\r", status);
        // get next page param
        size -= FLASH_PAGE_SIZE_BYTE;
        if( FALSE == MemSet )
        {
          src += FLASH_PAGE_SIZE_BYTE;
        }
        dest +=  FLASH_PAGE_SIZE_BYTE;
      }
    }
  }
  return status;
}


