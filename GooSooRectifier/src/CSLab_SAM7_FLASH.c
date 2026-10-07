
// Include Standard LIB  files
#include "project.h"

/*****************************************************************************/
/*          FLASH MEMORY MAPPING DEFINITIONS FOR AT91SAM7X256                */
/*****************************************************************************/
/*
AT91C_IFLASH              	EQU (0x00100000) ;- Internal FLASH base address
AT91C_IFLASH_SIZE         	EQU (0x00040000) ;- Internal FLASH size in byte (256 Kbytes)
AT91C_IFLASH_PAGE_SIZE    	EQU (256) 	;- Internal FLASH Page Size: 256 bytes
AT91C_IFLASH_LOCK_REGION_SIZE	EQU (16384) 	;- Internal FLASH Lock Region Size: 16 Kbytes
AT91C_IFLASH_NB_OF_PAGES  	EQU (1024) 	;- Internal FLASH Number of Pages: 1024 bytes
AT91C_IFLASH_NB_OF_LOCK_BITS 	EQU (16) 	;- Internal FLASH Number of Lock Bits: 16 bytes

typedef struct _AT91S_MC {
	AT91_REG	 MC_RCR; 	// MC Remap Control Register
	AT91_REG	 MC_ASR; 	// MC Abort Status Register
	AT91_REG	 MC_AASR; 	// MC Abort Address Status Register
	AT91_REG	 Reserved0[21]; 	// 
	AT91_REG	 MC_FMR; 	// MC Flash Mode Register
	AT91_REG	 MC_FCR; 	// MC Flash Command Register
	AT91_REG	 MC_FSR; 	// MC Flash Status Register
} AT91S_MC, *AT91PS_MC;
*/

#define	SAMPLE_ID	0xAA55
#define	SAMPLE_PER_RECORD	40

#define	FLASH_START_ADD		0x00100000
#define	FDATA_START_ADD		0x00120000

#define	FDATA_START_PAGE	0x0200
#define	FDATA_SIZE		0x00020000
#define	EFC_KEY			0x5A << 24
#define	EFC_WAIT		4 << 8

// Flash Mode
#define	EFC_FRDY	1		// Flash ready int enable
#define	EFC_LOCKE_IE	4		// Flash lock error int enable
#define	EFC_PROGE_IE	8		// Programming error int enable
#define	EFC_NO_ERASE	0x80		// No erase before programming
#define	EFC_FLASH_WATE	0x03 << 8       // Flash wait state(0-3)
#define	EFC_CYCLE_NO	72 << 16	// Flash uSec cycle no
#define	EFC_MODE_DEFAULT	EFC_CYCLE_NO|EFC_FLASH_WATE	

// Flash Command
#define	EFC_WRITE_PAGE	1		// Write Page
#define	EFC_SET_LOCK    2               // Set lock bit
#define	EFC_WRITE_LOCK  3               // Write page & lock 
#define	EFC_CLEAR_LOCK  4               // Clear lock bit
#define	EFC_ERASE_ALL   8               // Erase all
#define	EFC_SET_NVM     0x0B            // Set GP NVM bit
#define	EFC_CLEAR_NVM	0x0D		// Clear GP NVM bit
#define	EFC_SECURITY    0x0F            // Set Security bit

//Flash Status
#define	EFC_READY	0x00000001	// Flash ready
#define	EFC_LOCK_ERR	0x00000002      // Lock error
#define	EFC_PROG_ERR    0x00000004      // Programming er_saveror
#define	EFC_SECU_STS	0x00000008      // Security bit status
#define	EFC_GPNVM0	0x00000100      // GP NVM bit 0
#define	EFC_GPNVM1	0x00000200      // GP NVM bit 1
#define	EFC_GPNVM2	0x00000400      // GP NVM bit 2
#define	EFC_LOCK	0x00010000      // Lock region(x) lock status

#define	FLASH_MAIN	10
#define	FLASH_ERASE	100
#define	FLASH_READ   	200
#define	FLASH_WRITE	300
#define	FLASH_LOCK      400
#define	FLASH_UNLOCK    500

AT91PS_MC FLASH_pt = AT91C_BASE_MC;
#define	EFC_CMD		FLASH_pt->MC_FCR
#define	EFC_MODE	FLASH_pt->MC_FMR 
#define	EFC_STATUS	FLASH_pt->MC_FSR 

unsigned int uiFlashMode, uiFlashCmd, uiFlashSts, uiFlashSts0;
unsigned int uiFlashAdd, uiFlashData;
unsigned int iCurTime, iOldTime;
unsigned int uiRegisterAdd;
unsigned int uiRegisterData;
unsigned int * uiRegisterPtr;

unsigned int uiFlashBuffer[64];
unsigned int uiBackupData[64];
unsigned int *pFlashPtr;
unsigned int *pFptr;

unsigned int  iVoltSum, iAmpSum, iActivSum;
short sVACount, sActiveCount; 
short sStoreTime, iInterVal;
short sStoreStep, sStoreDelay;
short sFlashPage, sEraseQty;
short sStorePage;
short sViewPage;
unsigned short usStoreNo;
unsigned short SeverTxPage;

unsigned short sSampleNo;
unsigned short usMeasureBuf[128];
char Result;
char FlashCmd;
char DataMonit;
char FlashMask;
unsigned char MeasureNo;

/*-------------------------------*/
/* Flash Status Field Definition */
/*-------------------------------*/

#define AT91C_MC_FSR_MVM	((unsigned int) 0xFF << 8)		// (MC) Status Register GPNVMx: General-purpose NVM Bit Status
#define AT91C_MC_FSR_LOCK	((unsigned int) 0xFFFF << 16)	// (MC) Status Register LOCKSx: Lock Region x Lock Status
#define	ERASE_VALUE 		0xFFFFFFFF
#define FLASH_PAGE_SIZE_BYTE	256

//*----------------------------------------------------------------------------
//* \fn    AT91F_Flash_Ready
//* \brief Wait the flash ready
//*----------------------------------------------------------------------------
__ramfunc int AT91F_Flash_Ready (void)
//int AT91F_Flash_Ready (void)
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
  AT91F_MC_EFC_CfgModeReg( AT91C_BASE_MC, ((AT91C_MC_FMCN)&(72 <<16)) | AT91C_MC_FWS_3FWS );
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_Flash_Lock_Status
//* \brief Get the Lock bits field status
//*----------------------------------------------------------------------------
__ramfunc int AT91F_Flash_Lock_Status(void)
//int AT91F_Flash_Lock_Status(void)
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
//int AT91F_Flash_Lock (unsigned int Flash_Lock_Page)
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
//int AT91F_Flash_Erase_All(void)
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
//int AT91F_Flash_Unlock(unsigned int Flash_Lock_Page)
{
  //* Write the Clear Lock Bit command
  AT91F_MC_EFC_PerformCmd( AT91C_BASE_MC, AT91C_MC_CORRECT_KEY | AT91C_MC_FCMD_UNLOCK | (AT91C_MC_PAGEN & (Flash_Lock_Page << 8) ) );

  //* Wait the end of command
  AT91F_Flash_Ready();
  return( AT91F_Flash_Lock_Status() );
}

unsigned long CPT_LOCK = 0;
/******************************************************************
*
* SUB-ROUTINE  po_unlock
*
*------------------------------------------------------------------
*
* purpose : releases a spin lock and restores the original IRQL at
*      which the caller was running.
*
******************************************************************/
__ramfunc void po_unlock()
//void po_unlock()
{
  CPT_LOCK--;
  if( 0 == CPT_LOCK )
  {
    AT91F_AIC_EnableIt (AT91C_BASE_AIC, AT91C_ID_TC0);
    AT91F_AIC_EnableIt (AT91C_BASE_AIC, AT91C_ID_TC1);
    AT91F_AIC_EnableIt (AT91C_BASE_AIC, AT91C_ID_US0);
    AT91F_AIC_EnableIt (AT91C_BASE_AIC, AT91C_ID_US1);
    AT91F_AIC_EnableIt (AT91C_BASE_AIC, AT91C_ID_SYS);
    
    // FIQ & IRQ mask,  필요한 코드인지 확실치 않음
    uiRegisterPtr = (unsigned int *)0xFFFFF138;
    *uiRegisterPtr = 0;
    
    //AT91F_enable_interrupt();
  }
}

/******************************************************************
*
* SUB-ROUTINE  po_lock
*
*------------------------------------------------------------------
*
* purpose : acquires a spin lock so the caller can synchronize access
*      to shared data in a multiprocessor-safe way by raising IRQL.
*
******************************************************************/
__ramfunc void po_lock()
//void po_lock()
{
  if( 0 == CPT_LOCK )
  {
    // FIQ, IRQ mask 복구, 필요한 코드인지 확실치 않음
    //AT91C_AIC_DCR  EQU (0xFFFFF138) ;- (AIC) Debug Control Register (Protect)
    uiRegisterPtr = (unsigned int *)0xFFFFF138;
    *uiRegisterPtr = 2;
    //Warning이 발생하는 이유가 불명 
    AT91F_AIC_DisableIt (AT91C_BASE_AIC, AT91C_ID_TC0);
    AT91F_AIC_DisableIt (AT91C_BASE_AIC, AT91C_ID_TC1);
    AT91F_AIC_DisableIt (AT91C_BASE_AIC, AT91C_ID_US0);
    AT91F_AIC_DisableIt (AT91C_BASE_AIC, AT91C_ID_US1);
    AT91F_AIC_DisableIt (AT91C_BASE_AIC, AT91C_ID_SYS);
    //AT91F_disable_interrupt();
  }
  CPT_LOCK++;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_Flash_Write
//* \brief Write in one Flash page located in AT91C_IFLASH,  size in 32 bits
//* \input Flash_Address: start at 0x0010 0000 size: in byte
//*----------------------------------------------------------------------------
__ramfunc int AT91F_Flash_Write( unsigned int Flash_Address ,int size ,unsigned int * buff, unsigned char Memset)
//int AT91F_Flash_Write( unsigned int Flash_Address ,int size ,unsigned int * buff, unsigned char Memset)
{
  unsigned int i, page, status;
  unsigned int * Flash;
  
  //* init flash pointer
  Flash = (unsigned int *) Flash_Address;
  //* Get the Flash page number
  page = ((Flash_Address - (unsigned int)AT91C_IFLASH ) /FLASH_PAGE_SIZE_BYTE);

  po_lock();
  
  //* copy the new value
  for (i=0; (i < FLASH_PAGE_SIZE_BYTE) & (size > 0); i++,Flash++,buff++,size-=4 )
  {
    //* copy the flash to the write buffer ensuring code generation
    if( FALSE == Memset ) *Flash=*buff;
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

  po_unlock();

  //* Check the result
  status = 0;
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
int AT91F_Flash_Write_all( unsigned int Flash_Address ,int size ,unsigned int * buff)
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

void all_flash_lock(void)
{
  short lp;
  po_lock();
  //AT91F_Flash_Unlock(0x3FF);  
  for (lp = 0; lp < 0x400; lp++) 
  {
    AT91F_Flash_Lock(lp);
    RUNlamp(TOGLE);
  }
  //AT91F_Flash_Lock(0x3FE);
  po_unlock();  
}

void unlock_flash_page(int page)
{
  po_lock();
  AT91F_Flash_Unlock(page);  
  po_unlock();  
}

void lock_flash_page(int page)
{
  po_lock();
  AT91F_Flash_Lock(page);  
  po_unlock();  
}

/********************************************/
/* Flash 메모리에 저장된 시스템 변수를 Load */
/* 시스템 변수를  Flash 메모리에 Save       */
/********************************************/
#define	OPERATE_PARAMETER_ADD	0x0013FF00  // system parameter area
#define	OPERATE_PARAMETER_PAGE  0x03FF
#define OPERATE_SAVE_MASK       0xAA55AA55

#define SAVE_MASK	0xFFFFFFFF
#define MASK_INTERVAL  32
int iSecretCode;
short  sBackupAdd;
unsigned int uiBackupSum;
char OperSumErr;
void backup_flash_write(unsigned int data)
{
  short maskadd;
  maskadd = sBackupAdd + MASK_INTERVAL;
  uiBackupData[sBackupAdd] = data;
  uiBackupData[maskadd] = data ^ SAVE_MASK;
  uiBackupSum += data;
  sBackupAdd++;
}

unsigned int backup_flash_read(unsigned int defaultdata)
{
  unsigned int data;
  short maskadd;
  maskadd = sBackupAdd + MASK_INTERVAL;
  if (uiBackupData[sBackupAdd] == (uiBackupData[maskadd] ^ SAVE_MASK))
    data = uiBackupData[sBackupAdd]; else data = defaultdata;
  uiBackupSum += data;
  sBackupAdd++;
  //data = defaultdata;
  return data;
}

int backup_data_save(void)
{
  unsigned int flashadd, ui, result;
  sBackupAdd = 0;
  uiBackupSum = 0;
  
  unlock_flash_page(OPERATE_PARAMETER_PAGE);
// Flash Saved Mark 
  backup_flash_write(OPERATE_SAVE_MASK); 
  backup_flash_write(iSecretCode);            //iSecretCode
  ui = (MaxHour<<16)|(MaxMinute<<8)|MaxSec;   //MaxHour, MaxMinute, MaxSec                  
  backup_flash_write(ui);                           
  ui = (LocalMode<<16)|(LocalPole<<8)|ViewPage; 
  backup_flash_write(ui);                     //OperMode, OperPole, ViewPage
  ui = fOperAmp * 100;                        //fOperAmp
  backup_flash_write(ui);                     
  ui = (fOperVolt +0.1 )* 100;                       //fOperVolt
  backup_flash_write(ui);                     
  ui = fRevOperAmp * 100;                     //fRevOperAmp
  backup_flash_write(ui);                     
  ui = fRevOperVolt * 100;                    //fRevOperVolt
  backup_flash_write(ui);           
  ui = fMaxOperAmp * 100;                     //fMaxOperAmp
  backup_flash_write(ui);                     
  ui =(fMaxOperVolt+0.1)* 100;                    //fMaxOperVolt
  backup_flash_write(ui);                     
  ui = fMaxOverAmp * 100;                     //fMaxOverAmp
  backup_flash_write(ui);                     
  ui = fMaxOverVolt * 100;                    //fMaxOverVolt
  backup_flash_write(ui);                     
  backup_flash_write(iSoftTime);              //iSoftTime
  backup_flash_write(iReactRate);             //iReactDelay
  backup_flash_write(iCom1Speed);             //iCom1Speed
  backup_flash_write(iVoltOffset);            //iVoltOffset
  backup_flash_write(iAmpOffset);             //iAmpOffset
  backup_flash_write(iAcOverAmp);             //iAcOverAmp
  backup_flash_write(iAmpGain);               //iAmpGain
  backup_flash_write(iVoltGain);              //iVoltGain
  backup_flash_write(iCom0Speed);             //iCom0Speed
  backup_flash_write(iAcLowVolt);             //iAcLowVolt
  backup_flash_write(iPoleTurnTime);          //iPoleTurnTime

  // Flash Backup Check Sum
  backup_flash_write(uiBackupSum);            //Backup CheckSum  
  
// Operate data write
  flashadd = OPERATE_PARAMETER_ADD;
  result = AT91F_Flash_Write( flashadd , 256, &uiBackupData[0], 0);
  //printf("\nG:%8X",result);
  return result;
} 

void backup_data_read(void)
{
  char lp;
  unsigned int *uiptr;
  unsigned int flashadd, ui, sum;
  
  flashadd = OPERATE_PARAMETER_ADD;
  uiptr = (unsigned int*) flashadd;
  
// Flash data move to buffer
  for (lp = 0; lp < 64; lp++)
  {
    uiBackupData[lp] = *uiptr;
    uiptr++;
  }
  
  sBackupAdd = 0;
  uiBackupSum = 0;
  OperSumErr = 0;
  
  // Flash Save Mark  
  ui = backup_flash_read(0);
  if (ui != OPERATE_SAVE_MASK) OperSumErr = 1; 
   
  iSecretCode = backup_flash_read(SECRET_CODE); 
  ui = backup_flash_read(DEFAULT_MAX_HOUR<<16|DEFAULT_MAX_MINUTE<<8|DEFAULT_MAX_SEC);  
  MaxHour = (ui>>16) & 0xFF;
  MaxMinute = (ui>>8) & 0xFF;
  MaxSec = ui & 0xFF;
   
  ui = backup_flash_read(DEFAULT_OPMODE<<16|DEFAULT_OPPOLE<<8|DEFAULT_VIEWPAGE);  
  LocalMode = (ui>>16) & 0xFF;
#ifdef MONO_POLE
  LocalPole = PLUS;
#else 
  LocalPole = (ui>>8) & 0xFF;
#endif
  ViewPage = 0; //ui & 0xFF;
   
  ui = backup_flash_read(DEFAULT_OP_AMP); 
  fOperAmp = ui / 100; 
  
  ui = backup_flash_read(DEFAULT_OP_VOLT); 
  fOperVolt = ui / 100; 

  ui = backup_flash_read(DEFAULT_OP_AMP); 
  fRevOperAmp = ui / 100;
  
  ui = backup_flash_read(DEFAULT_OP_VOLT); 
  fRevOperVolt = ui /100; 
 
  ui = backup_flash_read(DEFAULT_MAX_AMP); 
  fMaxOperAmp = ui /100; 
  iMaxOperAmp = fMaxOperAmp;
  
  ui = backup_flash_read(DEFAULT_MAX_VOLT); 
  fMaxOperVolt = ui / 100; 

  ui = backup_flash_read(DEFAULT_OVER_AMP);
  fMaxOverAmp = ui / 100; 
  
  ui = backup_flash_read(DEFAULT_OVER_VOLT); 
  fMaxOverVolt = ui / 100; 
 
  iSoftTime = backup_flash_read(DEFAULT_SOFT_TIME); 
  iReactRate = backup_flash_read(DEFAULT_REACT_RATE); 
  iCom1Speed = backup_flash_read(DEFAULT_SPEED_COM1);
  iVoltOffset = backup_flash_read(DEFAULT_VOLT_OFFSET); 
  iAmpOffset = backup_flash_read(DEFAULT_AMP_OFFSET); 
  iAcOverAmp = backup_flash_read(DEFAULT_AC_OVER_AMP); 
  iAmpGain = backup_flash_read(DEFAULT_DC_GAIN); 
  iVoltGain = backup_flash_read(DEFAULT_DC_GAIN); 
  iCom0Speed = backup_flash_read(DEFAULT_SPEED_COM0);
  iAcLowVolt = backup_flash_read(DEFAULT_AC_LOW_VOLT);
  iPoleTurnTime = backup_flash_read(DEFAULT_POLE_TURN_TIME);

  // Flash Backup Check Sum
  sum = uiBackupSum;
  ui = backup_flash_read(0);
  if (ui != sum) OperSumErr = 1;
}

unsigned int uiResetControl;
unsigned int uiResetStatus;
unsigned int uiResetMode;
unsigned int uiWatchdogMode;
unsigned int uiWatchdogStatus;
unsigned int uiWdtModeSet;

void wdt_register_read(void)
{
   uiRegisterAdd = 0xFFFFFD44;
   uiRegisterPtr = (unsigned int *) uiRegisterAdd;
   uiWatchdogMode = *uiRegisterPtr; uiRegisterPtr++;
   uiWatchdogStatus = *uiRegisterPtr; 
}

void reset_register_read(unsigned int * uiPtr)
{
   uiRegisterAdd = 0xFFFFFD00;
   uiRegisterPtr = (unsigned int *) uiRegisterAdd;
   uiResetControl = *uiRegisterPtr;  uiRegisterPtr++;
   uiResetStatus = *uiRegisterPtr;  uiRegisterPtr++;
   uiResetMode = *uiRegisterPtr;
}
/***********************************/
/* AT91SAM7xxx Reset Status View   */
/***********************************/
char reset_status_view(void)
{
  char type, length;
   screen_clear();
   printf("<Reset Status>");
   reset_register_read(uiRegisterPtr);
   if ( uiResetStatus & 0x0001) printf("\nUser Reset Status: H"); else printf("\nUser Reset Status: L");
   if ( uiResetStatus & 0x0002) printf("\nBrownout Status:   H"); else printf("\nBrownout Status:   L");
   type = ( uiResetStatus & 0x0700) >> 8;
   if (type == 0)      printf("\nRESET[%1d] Power UP", type);
   else if (type == 2) printf("\nRESET[%1d] Watchdog", type);
   else if (type == 3) printf("\nRESET[%1d] Software", type);
   else if (type == 4) printf("\nRESET[%1d] User", type);
   else if (type == 5) printf("\nRESET[%1d] Brownout", type);
   if ( uiResetStatus & 0x00010000) printf("\nNRSTL Level: H");
   if ( uiResetStatus & 0x00020000) printf("\nSRCMP:Soft RESET being");
   printf("\n<Reset Mode>");
   if ( uiResetMode & 0x0001) printf("\nUserReset Enable");
   if ( uiResetMode & 0x0010) printf("\nUserReset Int Enable");
   if ( uiResetMode & 0x10000) printf("\nBrownOut Int Enable");
   length = ( uiResetMode & 0x0F00) >> 8;
   printf("\nExt Reset Length:%d", length);
   // WatchDog Register View
   wdt_register_read();
   //printf("\nWDT Set :0x%08X", uiWdtModeSet);
   printf("\nWDT Mode:0x%08X", uiWatchdogMode);
   //printf("\nWDT Stat:0x%08X", uiWatchdogStatus);
   return type;
}

char reset_status_check(void)
{
  char type;
   reset_register_read(uiRegisterPtr);
   type = ( uiResetStatus & 0x0700) >> 8;
   return type;
}




