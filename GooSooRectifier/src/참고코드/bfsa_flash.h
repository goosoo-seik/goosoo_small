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
//  File Name           : bfsa_flash.h
//  Object              :
//  Creation            : FB  10/feb/2005
//  Modif               : JCB 15/apr/2005
//  ----------------------------------------------------------------------------
#ifndef bfsa_flash_h
#define bfsa_flash_h


#define BFSA_ERROR(X)       ((ULONG)(X)|0x80000000)
#define BFSA_FAILURE(X)     ((ULONG)(X)&0x80000000)       // this macro returns TRUE if X is error status

// **************************************************
//     Error Code
// **************************************************
#define BFSA_SUCCESS                       0x00000000
#define NO_MEDIA_FOUND          BFSA_ERROR(0x00000001)
#define NOT_A_FAT_FILESYSTEM    BFSA_ERROR(0x00000002)
#define FILE_NOT_EXIST          BFSA_ERROR(0x00000004)
#define FILE_ALREADY_EXIST      BFSA_ERROR(0x00000008)
#define ERROR_READING_FAT_ENTRY BFSA_ERROR(0x00000010)
#define DISK_FULL               BFSA_ERROR(0x00000020)
#define NOT_A_VALID_DIRECTORY   BFSA_ERROR(0x00000040)
#define MEMORY_ERROR            BFSA_ERROR(0x00000080)
#define FLASH_ACCESS_ERROR      BFSA_ERROR(0x00000100)
#define INTERNAL_ERROR          BFSA_ERROR(0x00000200)
#define BAD_ARGUMENT            BFSA_ERROR(0x00000400)
#define DIRECTORY_ALREADY_EXIST BFSA_ERROR(0x00000800)
#define DIRECTORY_NOT_EMPTY     BFSA_ERROR(0x00001000)
#define OFFSET_OVERFLOW         BFSA_ERROR(0x00002000)
#define NOT_IMPLEMENTED         BFSA_ERROR(0x00004000)

#if defined(AT91SAM7A3) ||defined(AT91SAM7S32) || defined(AT91SAM7S64) || defined(AT91SAM7S128) || defined(AT91SAM7S256) || defined(AT91SAM9265) || defined(NANDFLASH) || defined (AT91SAM7X256) || defined (AT91SAM7X128)
#else
#error PROBLEM DEFINE !!!
#endif

//                Size     Size      Lost     Size                Size
//                SRAM     Flash     FAT      code                Drive
// AT91SAM7S32     8Kb      32Kb     18Kb     10Kb(IAR Thumb)       4kb
// AT91SAM7S64    16Kb      64Kb     18Kb     18Kb(IAR Thumb)      28Kb
// AT91SAM7S128   32Kb     128Kb     18Kb     18Kb(IAR Thumb)      92Kb
// AT91SAM7S256   64Kb     256Kb     18Kb     18Kb(IAR Thumb)     219Kb
// AT91SAM7A3     32Kb     256Kb     18Kb     18Kb(IAR Thumb)     219Kb



// AT91SAM7S32 / AT91SAM7S64 / AT91SAM7S128 / AT91SAM7S256
#if defined(AT91SAM7A3) || defined(AT91SAM7S32) || defined(AT91SAM7S64) || defined(AT91SAM7S128) || defined(AT91SAM7S256)
  #define BASE_IFLASH            AT91C_IFLASH
  #define BASE_IFLASH_SIZE       AT91C_IFLASH_SIZE
  #ifndef NDEBUG
    #define CODE_SIZE            (ULONG)(22*1024) // MASSSTORAGE and FILESYSTEM and traces
  #else
    #ifndef AT91SAM7S32
      #define CODE_SIZE          (ULONG)(20*1024) // MASSSTORAGE and FILESYSTEM
    #else
      #define CODE_SIZE          (ULONG)(10*1024) // FILESYSTEM
    #endif
  #endif
  #define FILE_MGR_BEGIN_ADDRESS ((UCHAR*)(BASE_IFLASH + CODE_SIZE))
  #define HD_SIZE                (ULONG)(BASE_IFLASH_SIZE - CODE_SIZE)
  #define SIZEONESECTOR          512
  #define NB_BLOCK               (USHORT)(HD_SIZE / SIZEONESECTOR)
#endif

#ifdef AT91SAM7X256
  #define BASE_IFLASH             AT91C_IFLASH
  #define BASE_IFLASH_SIZE        AT91C_IFLASH_SIZE
  #define CODE_SIZE              (ULONG)(20*1024)
  #define FILE_MGR_BEGIN_ADDRESS ((UCHAR*)(BASE_IFLASH + CODE_SIZE))
  #define HD_SIZE                (ULONG)(BASE_IFLASH_SIZE - CODE_SIZE)
  #define SIZEONESECTOR          512
  #define NB_BLOCK               (USHORT)(HD_SIZE / SIZEONESECTOR)
#endif

#ifdef AT91SAM7X128
  #define BASE_IFLASH             AT91C_IFLASH
  #define BASE_IFLASH_SIZE        AT91C_IFLASH_SIZE
  #define CODE_SIZE              (ULONG)(15*1024)
  #define FILE_MGR_BEGIN_ADDRESS ((UCHAR*)(BASE_IFLASH + CODE_SIZE))
  #define HD_SIZE                (ULONG)(BASE_IFLASH_SIZE - CODE_SIZE)
  #define SIZEONESECTOR          512
  #define NB_BLOCK               (USHORT)(HD_SIZE / SIZEONESECTOR)
#endif

#ifdef AT91SAM9265
  #define BASE_IFLASH             AT91C_EBI0_SDRAM
//  #define BASE_IFLASH_SIZE        AT91C_EBI0_SDRAM_SIZE
  #define FILE_MGR_BEGIN_ADDRESS  (UCHAR*)(BASE_IFLASH)
  #define HD_SIZE                 (ULONG)(220*1024)
  #define SIZEONESECTOR           512
  #define NB_BLOCK                (USHORT)(HD_SIZE / SIZEONESECTOR)
#endif

#ifdef NANDFLASH
  #include "NandFlash.h"
    #define BASE_IFLASH          AT91_SMARTMEDIA_BASE
  #define FILE_MGR_BEGIN_ADDRESS ((UCHAR*) (AT91_SMARTMEDIA_BASE))
  #ifdef AT91SAM9261
    /* 64Mo = 16k x 4096, 4086 clusters max for FAT12 = 0x0FF6 */
    #define SIZEONESECTOR        16*1024
    #define HD_SIZE              (ULONG)(0x0FF6*SIZEONESECTOR)
    #define NB_BLOCK             (USHORT)(0x0FF6)
  #endif
#endif



#define MBR_SIZE  512


// ------------------------------
// External function Definition
// ------------------------------

// Flash function
#ifdef ADS_COMPIL
extern int AT91F_Flash_Write_Address( unsigned int Flash_Address ,int size ,unsigned char * buff, unsigned char MemSet);
extern void AT91F_ReadMedia(unsigned int Flash_Address, int size, unsigned char *buff);
#else
extern __ramfunc int AT91F_Flash_Write_Address( unsigned int Flash_Address ,int size ,unsigned char * buff, unsigned char MemSet);
extern __ramfunc void AT91F_ReadMedia(unsigned int Flash_Address, int size, unsigned char *buff);
#endif

#endif // bfsa_flash_h

